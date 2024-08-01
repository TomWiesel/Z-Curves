#ifndef _ZCURVE_MULTITHREADING_H
#define _ZCURVE_MULTITHREADING_H

#include "defs.h"
#include <pthread.h>
#include <stdlib.h>

typedef struct
{
    unsigned thread_id;
    unsigned degree;
    coord_t *x;
    coord_t *y;
    size_t start;
    size_t end;
} thread_data_t;

int z_curve_multithreaded(unsigned degree, coord_t *x, coord_t *y, unsigned num_threads);

#endif // _ZCURVE_MULTITHREADING_H
