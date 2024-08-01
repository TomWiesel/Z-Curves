
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

#include "zcurve_simd.h"
#include "zcurve_lookup.h"
#include "zcurve_multithreading.h"
#include "zcurve_magic.h"
#include "zcurve.h"
#include "svg.h"
#include "cfg.h"
#include "util.h"

#define USAGE "Usage: %s [options]\n"                                                                 \
              "Options:\n"                                                                            \
              "  -V <opt:number>    The implementation to use\n"                                      \
              "                     If this option is not passed 0 is used\n"                        \
              "                     To list available options pass no argument\n"                   \
              "  -B <opt:number>    Measure runtime of specified implementation (default: false)\n"   \
              "                     Optional argument specifies number of repetitions (default: 1)\n" \
              "  -d <number>        Degree of the Z-curve to be constructed\n"                        \
              "  -t <number>        Number of threads to use for multithreaded impl (default: 3)\n"   \
              "  -p                 Calculates the index of the specified coordiantes\n"              \
              "  <number>           Positional argument specifying x-coordinate\n"                    \
              "  <number>           Positional argument specifying y-coordinate\n"                    \
              "  -i <number>        Calculates the coordinates of the point at the specified index\n" \
              "                     Please note: This option is mutually exclusive with -p\n"         \
              "  -s <opt:filename>  Save generated z-curve as SVG (defualt: false)\n"                 \
              "                     Optional argument specifies filename (default: zcurve.svg)\n"     \
              "  -h                 Prints this help text\n"                                          \
              "  --help             Prints this help text\n"                                          \
              "Examples:\n"                                                                           \
              "  %s -d 5 -s         Generates a zcurve of degree 5 and saves it to zcurve.svg\n"      \
              "  %s -d 5 -B 1 -V 1  Measures the runtime of the SIMD implementation 1 time\n"         \
              "  %s -d 9 -i 91186   Calculates the coordinates of the point at index 91186\n"         \
              "  %s -d 9 -p 53 6    Calculates the index of the point at coordinates (53, 6)\n"

static inline int run_index(const config_t *cfg)
{
    if (cfg->degree < DEGREE_MAX)
    {
        // bounds check
        if (cfg->index >= (1ull << (cfg->degree * 2)))
        {
            fprintf(stderr, "%s: argument for option -- 'i' is out of bounds for degree %u\n", get_filename(cfg->path), cfg->degree);
            return -1;
        }
    }

    coord_t x = 0, y = 0;
    char *version;
    switch (cfg->implementation)
    {
    case INDEX_ZCURVE:
        version = "ZCURVE";
        z_curve_at(cfg->degree, cfg->index, &x, &y);
        break;
    case INDEX_ZCURVE_LOOKUP_4BIT:
        version = "ZCURVE_LOOKUP_4BIT";
        z_curve_lookup_4bit_at(cfg->degree, cfg->index, &x, &y);
        break;
    case INDEX_ZCURVE_LOOKUP_8BIT:
        version = "ZCURVE_LOOKUP_8BIT";
        z_curve_lookup_8bit_at(cfg->degree, cfg->index, &x, &y);
        break;
    case INDEX_ZCURVE_LOOKUP_16BIT:
        version = "ZCURVE_LOOKUP_16BIT";
        z_curve_lookup_16bit_at(cfg->degree, cfg->index, &x, &y);
        break;
    case INDEX_ZCURVE_MAGIC:
        version = "ZCURVE_MAGIC";
        z_curve_magic_at(cfg->degree, cfg->index, &x, &y);
        break;
    default:
        fprintf(stderr, "%s: invalid implementation %u specified - No SIMD or Multithreaded-Version allowed!\n", get_filename(cfg->path), cfg->implementation);
        return -1;
    }
    printf("%s: Index %zu for degree %u at: (%u, %u)\n", version, cfg->index, cfg->degree, x, y);
    return 0;
}

static inline int benchmark_index(const config_t *cfg)
{
    if (cfg->degree < DEGREE_MAX)
    {
        if (cfg->index >= (1ull << (cfg->degree * 2)))
        {
            fprintf(stderr, "%s: argument for option -- 'i' is out of bounds for degree %u\n", get_filename(cfg->path), cfg->degree);
            return -1;
        }
    }

    struct timespec start, end;
    double time_total = 0.0;
    coord_t x = 0, y = 0;
    for (unsigned i = 0; i < cfg->benchmark_iterations; ++i)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        switch (cfg->implementation)
        {
        case INDEX_ZCURVE:
            z_curve_at(cfg->degree, cfg->index, &x, &y);
            break;
        case INDEX_ZCURVE_LOOKUP_4BIT:
            z_curve_lookup_4bit_at(cfg->degree, cfg->index, &x, &y);
            break;
        case INDEX_ZCURVE_LOOKUP_8BIT:
            z_curve_lookup_8bit_at(cfg->degree, cfg->index, &x, &y);
            break;
        case INDEX_ZCURVE_LOOKUP_16BIT:
            z_curve_lookup_16bit_at(cfg->degree, cfg->index, &x, &y);
            break;
        case INDEX_ZCURVE_MAGIC:
            z_curve_magic_at(cfg->degree, cfg->index, &x, &y);
            break;
        default:
            fprintf(stderr, "%s: invalid implementation %u specified - No SIMD or Multithreaded-Version allowed!\n", get_filename(cfg->path), cfg->implementation);
            return -1;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total += (end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec));
        sleep(1);
    }
    printf("Index %zu for degree %u at: (%u, %u)\n", cfg->index, cfg->degree, x, y);
    printf("Benchmarking implementation %s for %u iterations took %f seconds on average\n",
           impl_to_string(cfg->implementation, cfg->mode), cfg->benchmark_iterations,
           time_total / cfg->benchmark_iterations);
    return 0;
}

static inline int run_position(const config_t *cfg)
{
    if (cfg->degree < DEGREE_MAX)
    {
        if (cfg->x >= (1u << cfg->degree) || cfg->y >= (1u << cfg->degree))
        {
            fprintf(stderr, "%s: arguments for option -- 'p' (%u, %u) are out of bounds for degree %u\n", get_filename(cfg->path), cfg->x, cfg->y, cfg->degree);
            return -1;
        }
    }

    size_t index = 0;
    char *version;
    switch (cfg->implementation)
    {
    case POSITION_ZCURVE:
        version = "ZCURVE";
        index = z_curve_pos(cfg->degree, cfg->x, cfg->y);
        break;
    case POSITION_ZCURVE_MAGIC:
        version = "ZCURVE_MAGIC";
        index = z_curve_magic_pos(cfg->degree, cfg->x, cfg->y);
        break;
    default:
        fprintf(stderr, "%s: invalid implementation specified - No SIMD, LUT or Multithreaded-Version allowed\n", get_filename(cfg->path));
        return -1;
    }
    printf("%s: Position (%u, %u) for degree %u at index: %zu\n", version, cfg->x, cfg->y, cfg->degree, index);
    return 0;
}

static inline int benchmark_position(const config_t *cfg)
{
    if (cfg->degree < DEGREE_MAX)
    {
        if (cfg->x >= (1u << cfg->degree) || cfg->y >= (1u << cfg->degree))
        {
            fprintf(stderr, "%s: arguments for option -- 'p' (%u, %u) are out of bounds for degree %u\n", get_filename(cfg->path), cfg->x, cfg->y, cfg->degree);
            return -1;
        }
    }

    struct timespec start, end;
    double time_total = 0.0;
    size_t index = 0;
    for (unsigned i = 0; i < cfg->benchmark_iterations; ++i)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        switch (cfg->implementation)
        {
        case POSITION_ZCURVE:
            index = z_curve_pos(cfg->degree, cfg->x, cfg->y);
            break;
        case POSITION_ZCURVE_MAGIC:
            index = z_curve_magic_pos(cfg->degree, cfg->x, cfg->y);
            break;
        default:
            fprintf(stderr, "%s: invalid implementation specified - No SIMD, LUT or Multithreaded-Version allowed\n", get_filename(cfg->path));
            return -1;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total += (end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec));
        sleep(1);
    }
    printf("Position (%u, %u) for degree %u at index: %zu\n", cfg->x, cfg->y, cfg->degree, index);
    printf("Benchmarking implementation %s for %u iterations took %f seconds on average\n",
           impl_to_string(cfg->implementation, cfg->mode), cfg->benchmark_iterations,
           time_total / cfg->benchmark_iterations);
    return 0;
}

static inline int run_standard_impl(const config_t *cfg, coord_t *x, coord_t *y)
{
    switch (cfg->implementation)
    {
    case ZCURVE:
        z_curve(cfg->degree, x, y);
        break;
    case ZCURVE_MULTITHREADED:
        if (z_curve_multithreaded(cfg->degree, x, y, cfg->num_threads))
        {
            fprintf(stderr, "%s: failed to run multithreaded implementation\n", get_filename(cfg->path));
            return -1;
        }
        break;
    case ZCURVE_LOOKUP_4BIT:
        z_curve_lookup_4bit(cfg->degree, x, y);
        break;
    case ZCURVE_LOOKUP_8BIT:
        z_curve_lookup_8bit(cfg->degree, x, y);
        break;
    case ZCURVE_LOOKUP_16BIT:
        z_curve_lookup_16bit(cfg->degree, x, y);
        break;
    case ZCURVE_MAGIC:
        z_curve_magic(cfg->degree, x, y);
        break;
    case ZCURVE_SIMD:
        z_curve_simd(cfg->degree, x, y);
        break;
    case ZCURVE_LOOKUP_SIMD_16BIT:
        z_curve_simd_lookup_16bit(cfg->degree, x, y);
        break;
    case ZCURVE_MAGIC_SIMD:
        z_curve_simd_magic(cfg->degree, x, y);
        break;
    default:
        fprintf(stderr, "%s: unknown implementation\n", get_filename(cfg->path));
        return -1;
    }

    return 0;
}

static inline int benchmark_standard(const config_t *cfg)
{
    size_t max = 1ull << (cfg->degree * 2);
    coord_t *x = (coord_t *)malloc(sizeof(coord_t) * max);
    if (x == NULL)
    {
        fprintf(stderr, "%s: error in benchmark_standard: failed to allocate memory for x\n", get_filename(cfg->path));
        return -1;
    }

    coord_t *y = (coord_t *)malloc(sizeof(coord_t) * max);
    if (y == NULL)
    {
        free(x);
        fprintf(stderr, "%s: error in benchmark_standard: failed to allocate memory for y\n", get_filename(cfg->path));
        return -1;
    }

    unsigned long n = cfg->benchmark_iterations;
    struct timespec start, end;
    double time_total = 0.0;

    while (n--)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        if (run_standard_impl(cfg, x, y))
        {
            free(x);
            free(y);
            return -1;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total += (end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec));
        sleep(1);
    }

    if (cfg->benchmark_iterations == 1)
    {
        printf("One repetition takes %lf seconds\n", time_total);
    }
    else
    {
        printf("%u repetitions took %lf seconds on average\n", cfg->benchmark_iterations, time_total / cfg->benchmark_iterations);
    }

    free(x);
    free(y);

    return 0;
}

static inline int run_standard(const config_t *cfg)
{
    size_t max = 1ull << (cfg->degree * 2);
    coord_t *x = (coord_t *)malloc(sizeof(coord_t) * max);
    if (x == NULL)
    {
        fprintf(stderr, "%s: error in run_standard: failed to allocate memory for x\n", get_filename(cfg->path));
        return -1;
    }

    coord_t *y = (coord_t *)malloc(sizeof(coord_t) * max);
    if (y == NULL)
    {
        free(x);
        fprintf(stderr, "%s: error in run_standard: failed to allocate memory for y\n", get_filename(cfg->path));
        return -1;
    }

    if (run_standard_impl(cfg, x, y))
    {
        free(x);
        free(y);
        return -1;
    }

    printf("Finished generating zcurve!\n");

    if (cfg->save_svg)
    {
        printf("Saving data to svg...\n");
        generate_svg_path(cfg->degree, x, y, 2, 10, cfg->svg_filename);
        printf("Done!\n");
    }

    free(x);
    free(y);

    return 0;
}

static inline int run_benchmark(const config_t *cfg)
{
    switch (cfg->mode)
    {
    case STANDARD:
        printf("Running implementation: %s\n", impl_to_string(cfg->implementation, cfg->mode));
        return benchmark_standard(cfg);
    case INDEX:
        return benchmark_index(cfg);
    case POSITION:
        return benchmark_position(cfg);
    default:
        fprintf(stderr, "%s: argument error: invalid mode\n", get_filename(cfg->path));
        return -1;
    }

    return 0;
}

static inline int run_default(const config_t *cfg)
{
    switch (cfg->mode)
    {
    case STANDARD:
        printf("You have chosen version: %s\n", impl_to_string(cfg->implementation, cfg->mode));
        return run_standard(cfg);
    case INDEX:
        return run_index(cfg);
    case POSITION:
        return run_position(cfg);
    default:
        fprintf(stderr, "%s: argument error: invalid mode\n", get_filename(cfg->path));
        return -1;
    }

    return 0;
}

void print_help(const char *path)
{
    const char *program_name = get_filename(path);
    printf(USAGE, program_name, program_name, program_name, program_name, program_name);
}

void print_available_implementations_for_mode(mode_of_operation_t mode)
{
    switch (mode)
    {
    case STANDARD:
        for (int i = 0; i < MAX_IMPL; i++)
        {
            printf("\t%s : %d\n", standard_impl_to_string((standard_impl_t)i), i);
        }
        break;
    case INDEX:
        for (int i = 0; i < INDEX_MAX_IMPL; i++)
        {
            printf("\t%s : %d\n", index_impl_to_string((index_impl_t)i), i);
        }
        break;
    case POSITION:
        for (int i = 0; i < POSITION_MAX_IMPL; i++)
        {
            printf("\t%s : %d\n", position_impl_to_string((position_impl_t)i), i);
        }
        break;
    default:
        break;
    }
}

void print_available_implementations()
{
    printf("Available implementations:\n");
    for (int i = 0; i < 3; i++)
    {
        printf("Mode: %s\n", mode_to_string((mode_of_operation_t)i));
        print_available_implementations_for_mode((mode_of_operation_t)i);
    }
}

int run(const config_t *cfg)
{
    if (cfg->mode == HELP)
    {
        print_help(cfg->path);
        return 0;
    }

    if (cfg->mode == VERSION_HELP)
    {
        print_available_implementations();
        return 0;
    }

    if (cfg->should_benchmark)
    {
        return run_benchmark(cfg);
    }
    else
    {
        return run_default(cfg);
    }
}

int main(int argc, char **argv)
{
    config_t config = {0};

    config_init(&config);

    if (config_parse(argc, argv, &config))
    {
        return EXIT_FAILURE;
    }

    if (run(&config))
    {
        return EXIT_FAILURE;
    }

    printf("See you next time!\n");

    return EXIT_SUCCESS;
}
