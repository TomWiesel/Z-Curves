#include "zcurve.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define LOOKUP_TABLE_MAX_SIZE 16

typedef struct
{
    unsigned short x;
    unsigned short y;
} lookup_t;

int generate_simd_lookup_table(unsigned short table_size)
{
    if (!table_size || table_size & 1)
    {
        fprintf(stderr, "Error: Table size must be a positive even number larger than 0.\n");
        return 1;
    }

    if (table_size > LOOKUP_TABLE_MAX_SIZE)
    {
        fprintf(stderr, "Error: Table size is too large. Maximum size is %d.\n", LOOKUP_TABLE_MAX_SIZE);
        return 1;
    }

    unsigned degree = table_size >> 1;
    size_t size = 1ull << (degree * 2);

    printf("Generating SIMD lookup table for %zu points...\n", size);

    coord_t *x = (coord_t *)malloc(sizeof(coord_t) * size);
    if (x == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for x-coordinates.\n");
        return 1;
    }

    coord_t *y = (coord_t *)malloc(sizeof(coord_t) * size);
    if (y == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for y-coordinates.\n");
        free(x);
        return 1;
    }

    for (size_t i = 0; i < size; i++)
    {
        coord_t _x, _y = {0};
        z_curve_at(degree, i, &_x, &_y);

        x[i] = _x;
        y[i] = _y;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "lookup_table_simd_%ubit.h", table_size);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open %s for writing.\n", filename);
        free(x);
        free(y);
        return 1;
    }

    fprintf(file, "#ifndef _SIMD_LOOKUP_TABLE_%ubit_H\n", table_size);
    fprintf(file, "#define _SIMD_LOOKUP_TABLE_%ubit_H\n\n", table_size);

    fprintf(file, "#include \"zcurve.h\"\n\n");

    fprintf(file, "static const coord_t x_%ubit[%zu] = {\n", table_size, size);
    for (size_t i = 0; i < size; i++)
    {
        fprintf(file, "    0x%x", x[i]);
        if (i < size - 1)
        {
            fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "};\n\n");

    fprintf(file, "static const coord_t y_%ubit[%zu] = {\n", table_size, size);
    for (size_t i = 0; i < size; i++)
    {
        fprintf(file, "    0x%x", y[i]);
        if (i < size - 1)
        {
            fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "};\n\n");

    fprintf(file, "#endif // _SIMD_LOOKUP_TABLE_%ubit_H\n", table_size);

    fclose(file);

    free(x);
    free(y);

    return 0;
}

int generate_lookup_table(unsigned short table_size)
{
    if (!table_size || table_size & 1)
    {
        fprintf(stderr, "Error: Table size must be a positive even number larger than 0.\n");
        return 1;
    }

    if (table_size > LOOKUP_TABLE_MAX_SIZE)
    {
        fprintf(stderr, "Error: Table size is too large. Maximum size is %d.\n", LOOKUP_TABLE_MAX_SIZE);
        return 1;
    }

    unsigned degree = table_size >> 1;
    size_t size = 1ull << (degree * 2);

    printf("Generating lookup table for %zu points...\n", size);

    lookup_t *lookup = (lookup_t *)malloc(sizeof(lookup_t) * size);
    if (lookup == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for lookup table.\n");
        return 1;
    }

    for (size_t i = 0; i < size; i++)
    {
        coord_t x, y = {0};
        z_curve_at(degree, i, &x, &y);

        lookup[i].x = x;
        lookup[i].y = y;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "lookup_table_%ubit.h", table_size);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", filename);
        free(lookup);
        return 1;
    }

    fprintf(file, "#ifndef _LOOKUP_TABLE_%uBIT_H\n", table_size);
    fprintf(file, "#define _LOOKUP_TABLE_%uBIT_H\n\n", table_size);
    fprintf(file, "#include \"tables.h\"\n\n");

    fprintf(file, "static const lookup_t lookup_table_%ubit[%zu] = {\n", table_size, size);

    for (size_t i = 0; i < size; i++)
    {
        fprintf(file, "    { %u, %u }", lookup[i].x, lookup[i].y);
        if (i < size - 1)
        {
            fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fprintf(file, "};\n");
    fprintf(file, "#endif // _LOOKUP_TABLE_%uBIT_H\n", table_size);

    fclose(file);

    free(lookup);

    return 0;
}

int main(int argc, char *argv[])
{
    // we get an array of table sizes, e.g. 4, 8, 16, 24
    if (argc < 2)
    {
        printf("Usage: %s <table_size> [<table_size> ...]\n", argv[0]);
        return 1;
    }

    if (argc > 8)
    {
        printf("Error: Too many table sizes.\n");
        return 1;
    }

    unsigned char tables[16] = {0};

    for (int i = 1; i < argc; i++)
    {
        unsigned char table_size = atoi(argv[i]);
        if (table_size > 0 && table_size <= 16 && (table_size & 1) == 0)
        {
            tables[i - 1] = table_size;
        }
        else
        {
            printf("Error: Invalid table size '%s'.\n", argv[i]);
            return 1;
        }
    }

    printf("Generating lookup tables...\n");

    for (int i = 0; i < 16 && tables[i] != 0; i++)
    {
        if (generate_lookup_table(tables[i]) != 0)
        {
            printf("Error: Could not generate lookup table for %ubit.\n", tables[i]);
            return 1;
        }

        if (generate_simd_lookup_table(tables[i]) != 0)
        {
            printf("Error: Could not generate SIMD lookup table for %ubit.\n", tables[i]);
            return 1;
        }
    }

    printf("Done.\n");

    return 0;
}
