#ifndef _SVG_H
#define _SVG_H

#include "zcurve.h"

#define SVG_PATH_ELEMENT_MAX_LENGTH 23

void generate_svg_line(unsigned degree, coord_t *x, coord_t *y, unsigned offset, unsigned scale, char *filename);
void generate_svg_path(unsigned degree, coord_t *x, coord_t *y, unsigned offset, unsigned scale, char *filename);

#endif // _SVG_H
