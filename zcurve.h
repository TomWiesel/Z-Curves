#ifndef _ZCURVE_H
#define _ZCURVE_H

#include "defs.h"

void z_curve(unsigned degree, coord_t *x, coord_t *y);
void z_curve_at(unsigned degree, size_t idx, coord_t *x, coord_t *y);
size_t z_curve_pos(unsigned degree, coord_t x, coord_t y);

#endif
