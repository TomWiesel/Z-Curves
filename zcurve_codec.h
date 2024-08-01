#ifndef _ZCURVE_CODEC_H
#define _ZCURVE_CODEC_H

#include "defs.h"

// taken from http://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
// and https://gist.github.com/JarkkoPFC/0e4e599320b0cc7ea92df45fb416d79a
static inline void decode(size_t idx, coord_t *x, coord_t *y)
{
    // isolate every second bit
    size_t z = ((unsigned)idx | (idx << 31)) & 0x5555555555555555;

    // shift every second bit to the right
    z = (z | (z >> 1)) & 0x3333333333333333;

    // shift every fourth bit to the right
    z = (z | (z >> 2)) & 0x0f0f0f0f0f0f0f0f;

    // shift every eighth bit to the right
    z = (z | (z >> 4)) & 0x00ff00ff00ff00ff;

    // shift every sixteenth bit to the right
    z = (z | (z >> 8));

    *x = (coord_t)z;
    *y = (coord_t)(z >> 32);

    return;
}

static inline size_t encode(coord_t x, coord_t y)
{
    // reverse of the decode function
    size_t z = (size_t)x | ((size_t)y << 32);

    z = (z | (z << 8)) & 0x00ff00ff00ff00ff;

    z = (z | (z << 4)) & 0x0f0f0f0f0f0f0f0f;

    z = (z | (z << 2)) & 0x3333333333333333;

    z = (z | (z << 1)) & 0x5555555555555555;

    return (z | (z >> 31)) & 0x00000000ffffffff;
}

#endif // _ZCURVE_CODEC_H
