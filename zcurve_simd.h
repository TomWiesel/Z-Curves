#ifndef _ZCURVE_SIMD_H
#define _ZCURVE_SIMD_H

#include "defs.h"
#include <immintrin.h>

void z_curve_simd(unsigned degree, coord_t *x, coord_t *y);

#endif // _ZCURVE_SIMD_H
