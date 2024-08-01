#include "svg.h"
#include "zcurve.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SVG_HEAD "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"            \
                 "<svg width=\"%zu\" height=\"%zu\" xmlns=\"http://www.w3.org/2000/svg\">\n" \
                 "<g>\n"

#define SVG_TAIL "</g>\n" \
                 "</svg>\n"

void generate_svg_line(unsigned degree, coord_t *x, coord_t *y, unsigned offset, unsigned scale, char *filename)
{
    size_t size = 1ull << degree;
    size_t max = 1ull << (degree * 2);
    size_t dim = (size - 1 + offset * 2) * scale;

    // create an svg file to visualize the z curve
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open %s!\n", filename);
        return;
    }

    fprintf(fp, SVG_HEAD, dim, dim);

    // draw lines between the points
    for (size_t i = 0; i < max - 1; ++i)
    {
        fprintf(fp, "<line x1=\"%u\" y1=\"%u\" x2=\"%u\" y2=\"%u\" stroke=\"black\" stroke-width=\"%f\"/>\n", (x[i] + offset) * scale, (y[i] + offset) * scale, (x[(i + 1)] + offset) * scale, (y[(i + 1)] + offset) * scale, 0.1 * scale);
    }

    fprintf(fp, SVG_TAIL);

    fclose(fp);

    return;
}

void generate_svg_path(unsigned degree, coord_t *x, coord_t *y, unsigned offset, unsigned scale, char *filename)
{
    size_t size = 1ull << degree;
    size_t max = 1ull << (degree * 2);
    size_t dim = (size - 1 + offset * 2) * scale;

    // create an svg file to visualize the z curve
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open %s!\n", filename);
        return;
    }

    fprintf(fp, SVG_HEAD, dim, dim);

    fprintf(fp, "<rect x=\"0\" y=\"0\" width=\"100%%\" height=\"100%%\" fill=\"none\" stroke=\"black\" stroke-width=\"%f\"/>\n", 0.1 * scale);

    fprintf(fp, "<path d=\"M%u,%u ", (x[0] + offset) * scale, (y[0] + offset) * scale);

    for (size_t i = 1; i < max; ++i)
    {
        fprintf(fp, "L%u,%u ", (x[i] + offset) * scale, (y[i] + offset) * scale);
    }

    fprintf(fp, "\" fill=\"none\" stroke=\"black\" stroke-width=\"%f\"/>\n", 0.05 * scale);

    fprintf(fp, SVG_TAIL);

    fclose(fp);

    return;
}
