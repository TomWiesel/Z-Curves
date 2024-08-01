#ifndef _ZCURVE_LOOKUP_H
#define _ZCURVE_LOOKUP_H

#include "tables.h"

void z_curve_lookup_4bit(unsigned degree, coord_t *x, coord_t *y);
void z_curve_lookup_4bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y);

void z_curve_lookup_8bit(unsigned degree, coord_t *x, coord_t *y);
void z_curve_lookup_8bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y);

void z_curve_lookup_16bit(unsigned degree, coord_t *x, coord_t *y);
void z_curve_lookup_16bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y);

// simd
void z_curve_simd_lookup_16bit(unsigned degree, coord_t *x, coord_t *y);

#endif // _ZCURVE_LOOKUP_H
