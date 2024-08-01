# Detect OS & Architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Set compiler flags
CC = gcc -lpthread
WARNING_FLAGS = -Wall -Wextra -Wpedantic -Werror -msse4.2
SANIZIZE_FLAGS = -fsanitize=address -fsanitize=undefined -fdiagnostics-format=json
ADDITIONAL_FLAGS = 
CVERSION = -std=c17

# Set additional flags for Apple Silicon
ifeq ($(UNAME_S),Darwin)
	ifeq ($(UNAME_M),arm64)
		ADDITIONAL_FLAGS += -arch x86_64
	endif
else
	SANIZIZE_FLAGS += -fsanitize=leak
endif

# Set Linker flags
LDFLAGS = -lm

# Set executable name
EXECUTABLE = zcurve
EXECUTABLE_O0 = zcurve_o0
EXECUTABLE_O2 = zcurve_o2

# Set generator name
GENERATOR = generator

# Set generator sources and headers
GENERATOR_SOURCES = generate_lookuptables.c zcurve.c
GENERATOR_HEADERS = zcurve_codec.h zcurve.h

# Set lookup table options and headers
LOOKUPTABLES = 4 8 16
LOOKUPTABLE_HEADERS = $(foreach table,$(LOOKUPTABLES),lookup_table_$(table)bit.h lookup_table_simd_$(table)bit.h)

# Set main sources and headers
SOURCES = main.c zcurve.c zcurve_multithreading.c zcurve_magic.c svg.c zcurve_simd.c zcurve_lookup.c cfg.c
HEADERS = zcurve_codec.h zcurve.h zcurve_multithreading.h zcurve_magic.h svg.h zcurve_simd.h zcurve_lookup.h tables.h cfg.h $(LOOKUPTABLE_HEADERS)

# Set targets
all: zcurve

zcurve: $(SOURCES) $(HEADERS)
	$(CC) $(CVERSION) $(WARNING_FLAGS) $(SANIZIZE_FLAGS) $(ADDITIONAL_FLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS) -O3

# zcurve with O0 optimization
zcurve_o0: $(SOURCES) $(HEADERS)
	$(CC) $(CVERSION) $(WARNING_FLAGS) $(SANIZIZE_FLAGS) $(ADDITIONAL_FLAGS) $(SOURCES) -o $(EXECUTABLE_O0) $(LDFLAGS) -O0

# zcurve with O2 optimization
zcurve_o2: $(SOURCES) $(HEADERS)
	$(CC) $(CVERSION) $(WARNING_FLAGS) $(SANIZIZE_FLAGS) $(ADDITIONAL_FLAGS) $(SOURCES) -o $(EXECUTABLE_O2) $(LDFLAGS) -O2

$(LOOKUPTABLE_HEADERS): generator
	./$(GENERATOR) $(LOOKUPTABLES)

generator: $(GENERATOR_SOURCES) $(GENERATOR_HEADERS)
	$(CC) $(CVERSION) $(WARNING_FLAGS) $(SANIZIZE_FLAGS) $(ADDITIONAL_FLAGS) $(GENERATOR_SOURCES) -o $(GENERATOR) $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE) $(GENERATOR) $(LOOKUPTABLE_HEADERS)
