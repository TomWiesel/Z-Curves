#ifndef _CFG_H
#define _CFG_H

#include <stdbool.h>
#include <stdint.h>
#include "defs.h"

typedef struct
{
    const char *path;
    char *svg_filename;
    mode_of_operation_t mode;
    int32_t implementation;
    size_t index;
    unsigned degree;
    unsigned num_threads;
    unsigned benchmark_iterations;
    coord_t x;
    coord_t y;
    bool should_benchmark;
    bool save_svg;
} config_t;

void config_init(config_t *cfg);
int config_parse(int argc, char **argv, config_t *cfg);

#endif // _CFG_H
