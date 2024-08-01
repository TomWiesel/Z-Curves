#ifndef _COMMON_H
#define _COMMON_H

#include <stddef.h>

#define MODE_DEFAULT STANDARD
#define IMPLEMENTATION_DEFAULT 0

#define DEGREE_DEFAULT 0
#define DEGREE_MAX 16

#define THREADS_DEFAULT 3
#define THREADS_MIN 1
#define THREADS_MAX 8

#define BENCHMARK_DEFAULT false
#define BENCHMARK_ITERATIONS_DEFAULT 10
#define BENCHMARK_ITERATIONS_MAX 1000000

#define SVG_DEFAULT false
#define SVG_FILENAME_MAX_LENGTH 255
#define SVG_FILENAME_DEFAULT "zcurve.svg"

#define INDEX_DEFAULT 0
#define INDEX_MAX ((1ull << (sizeof(coord_t) << 4)) - 1)

#define COORD_MAX ((1ull << (sizeof(coord_t) << 3)) - 1)

#define X_DEFAULT 0
#define Y_DEFAULT 0

typedef unsigned short coord_t;

typedef enum
{
    STANDARD,
    INDEX,
    POSITION,
    HELP,
    VERSION_HELP,
    MAX_MODE
} mode_of_operation_t;

typedef enum 
{
    ZCURVE_MAGIC_SIMD,
    ZCURVE_LOOKUP_SIMD_16BIT,
    ZCURVE_MAGIC,
    ZCURVE_SIMD,
    ZCURVE_LOOKUP_16BIT,
    ZCURVE_LOOKUP_8BIT,
    ZCURVE_LOOKUP_4BIT,
    ZCURVE_MULTITHREADED,
    ZCURVE,
    MAX_IMPL
} standard_impl_t;

typedef enum
{
    POSITION_ZCURVE_MAGIC,
    POSITION_ZCURVE,
    POSITION_MAX_IMPL
} position_impl_t;

typedef enum
{
    INDEX_ZCURVE_MAGIC,
    INDEX_ZCURVE_LOOKUP_16BIT,
    INDEX_ZCURVE_LOOKUP_8BIT,
    INDEX_ZCURVE_LOOKUP_4BIT,
    INDEX_ZCURVE,
    INDEX_MAX_IMPL
} index_impl_t;

static inline const char *standard_impl_to_string(standard_impl_t impl)
{
    switch (impl)
    {
    case ZCURVE:
        return "ZCURVE";
    case ZCURVE_MULTITHREADED:
        return "ZCURVE_MULTITHREADED";
    case ZCURVE_SIMD:
        return "ZCURVE_SIMD";
    case ZCURVE_LOOKUP_4BIT:
        return "ZCURVE_LOOKUP_4BIT";
    case ZCURVE_LOOKUP_8BIT:
        return "ZCURVE_LOOKUP_8BIT";
    case ZCURVE_LOOKUP_16BIT:
        return "ZCURVE_LOOKUP_16BIT";
    case ZCURVE_LOOKUP_SIMD_16BIT:
        return "ZCURVE_LOOKUP_SIMD_16BIT";
    case ZCURVE_MAGIC:
        return "ZCURVE_MAGIC";
    case ZCURVE_MAGIC_SIMD:
        return "ZCURVE_MAGIC_SIMD";
    default:
        return "UNKNOWN";
    }
}

static inline const char *position_impl_to_string(position_impl_t impl)
{
    switch (impl)
    {
    case POSITION_ZCURVE:
        return "ZCURVE";
    case POSITION_ZCURVE_MAGIC:
        return "ZCURVE_MAGIC";
    default:
        return "UNKNOWN";
    }
}

static inline const char *index_impl_to_string(index_impl_t impl)
{
    switch (impl)
    {
    case INDEX_ZCURVE:
        return "ZCURVE";
    case INDEX_ZCURVE_LOOKUP_4BIT:
        return "ZCURVE_LOOKUP_4BIT";
    case INDEX_ZCURVE_LOOKUP_8BIT:
        return "ZCURVE_LOOKUP_8BIT";
    case INDEX_ZCURVE_LOOKUP_16BIT:
        return "ZCURVE_LOOKUP_16BIT";
    case INDEX_ZCURVE_MAGIC:
        return "ZCURVE_MAGIC";
    default:
        return "UNKNOWN";
    }
}

static inline const char *mode_to_string(mode_of_operation_t mode)
{
    switch (mode)
    {
    case STANDARD:
        return "STANDARD";
    case INDEX:
        return "INDEX";
    case POSITION:
        return "POSITION";
    case HELP:
        return "HELP";
    default:
        return "UNKNOWN";
    }
}

static inline const char *impl_to_string(int impl, mode_of_operation_t mode)
{
    switch (mode)
    {
    case STANDARD:
        return standard_impl_to_string((standard_impl_t)impl);
    case INDEX:
        return index_impl_to_string((index_impl_t)impl);
    case POSITION:
        return position_impl_to_string((position_impl_t)impl);
    default:
        return "UNKNOWN";
    }
}

#endif // _COMMON_H
