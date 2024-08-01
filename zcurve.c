#include "zcurve.h"

void z_curve(unsigned degree, coord_t *x, coord_t *y)
{
    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    for (size_t i = 0; i < max; ++i)
    {
        x[i] = 0;
        y[i] = 0;

        for (unsigned j = 0; j < degree; ++j)
        {
            x[i] |= ((i >> (j * 2)) & 1ull) << j;
            y[i] |= ((i >> (j * 2 + 1)) & 1ull) << j;
        }
    }

    return;
}

void z_curve_at(unsigned degree, size_t idx, coord_t *x, coord_t *y)
{
    if (degree > DEGREE_MAX)
    {
        degree = DEGREE_MAX;
    }

    *x = 0;
    *y = 0;

    for (unsigned i = 0; i < degree; ++i)
    {
        *x |= ((idx >> (i * 2)) & 1ull) << i;
        *y |= ((idx >> (i * 2 + 1)) & 1ull) << i;
    }

    return;
}

size_t z_curve_pos(unsigned degree, coord_t x, coord_t y)
{
    if (degree > DEGREE_MAX)
    {
        degree = DEGREE_MAX;
    }

    /*
    the index is the bitwise interlace of the x and y coordinates

    idx = 0010 0101
          |||| ||||
          yxyx yxyx

    x =   0011
    y =   0100
    */

    size_t idx = 0;

    for (unsigned i = 0; i < degree; ++i)
    {
        idx |= ((x >> i) & 1ull) << (i * 2);
        idx |= ((y >> i) & 1ull) << (i * 2 + 1);
    }

    return idx;
}
