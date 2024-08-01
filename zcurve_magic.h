#ifndef _ZCURVE_MAGIC_H
#define _ZCURVE_MAGIC_H

#include "defs.h"
#include "zcurve_codec.h"

// Magic
void z_curve_magic(unsigned degree, coord_t *x, coord_t *y);
void z_curve_magic_at(unsigned degree, size_t idx, coord_t *x, coord_t *y);
size_t z_curve_magic_pos(unsigned degree, coord_t x, coord_t y);

// SIMD Magic
void z_curve_simd_magic(unsigned degree, coord_t *x, coord_t *y);

#endif // _ZCURVE_MAGIC_H
