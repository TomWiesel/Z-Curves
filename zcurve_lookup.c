#include "zcurve_lookup.h"
#include <immintrin.h>

void z_curve_lookup_4bit(unsigned degree, coord_t *x, coord_t *y)
{
    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    // round up degree to next multiple of 2 and divide by 2
    unsigned iterations = ((degree + 1) & ~1) >> 1;

    for (size_t i = 0; i < max; ++i)
    {
        size_t idx = i;
        x[i] = 0;
        y[i] = 0;

        for (unsigned j = 0; j < iterations && idx > 0; ++j)
        {
            // extract the last 4 bits
            unsigned char lookup_index = idx & 0xf;

            // lookup the x and y values
            unsigned x_value = lookup_table_4bit[lookup_index].x;
            unsigned y_value = lookup_table_4bit[lookup_index].y;

            // store the x and y values
            x[i] |= x_value << (j * 2);
            y[i] |= y_value << (j * 2);

            // shift the bits to the right
            idx >>= 4;
        }
    }
}

void z_curve_lookup_4bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y)
{
    if (degree > DEGREE_MAX)
    {
        degree = DEGREE_MAX;
    }

    *x = 0;
    *y = 0;

    // round up degree to next multiple of 2 and divide by 2
    unsigned iterations = ((degree + 1) & ~1) >> 1;

    for (unsigned i = 0; i < iterations && idx > 0; ++i)
    {
        // extract the last 4 bits
        unsigned char lookup_index = idx & 0xf;

        // lookup the x and y values
        unsigned x_value = lookup_table_4bit[lookup_index].x;
        unsigned y_value = lookup_table_4bit[lookup_index].y;

        // store the x and y values
        *x |= x_value << (i * 2);
        *y |= y_value << (i * 2);

        // shift the bits to the right
        idx >>= 4;
    }
}

void z_curve_lookup_8bit(unsigned degree, coord_t *x, coord_t *y)
{
    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    // round up degree to next multiple of 4 and divide by 4
    unsigned iterations = ((degree + 3) & ~3) >> 2;

    for (size_t i = 0; i < max; ++i)
    {
        size_t idx = i;
        x[i] = 0;
        y[i] = 0;

        for (unsigned j = 0; j < iterations && idx > 0; ++j)
        {
            // extract the last 8 bits
            unsigned char lookup_index = idx & 0xff;

            // lookup the x and y values
            unsigned x_value = lookup_table_8bit[lookup_index].x;
            unsigned y_value = lookup_table_8bit[lookup_index].y;

            // store the x and y values
            x[i] |= x_value << (j * 4);
            y[i] |= y_value << (j * 4);

            // shift the bits to the right
            idx >>= 8;
        }
    }
}

void z_curve_lookup_8bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y)
{
    if (degree > DEGREE_MAX)
    {
        degree = DEGREE_MAX;
    }

    *x = 0;
    *y = 0;

    // round up degree to next multiple of 4 and divide by 4
    unsigned iterations = ((degree + 3) & ~3) >> 2;

    for (unsigned i = 0; i < iterations && idx > 0; ++i)
    {
        // extract the last 8 bits
        unsigned lookup_index = idx & 0xff;

        // lookup the x and y values
        unsigned x_value = lookup_table_8bit[lookup_index].x;
        unsigned y_value = lookup_table_8bit[lookup_index].y;

        // store the x and y values
        *x |= x_value << (i * 4);
        *y |= y_value << (i * 4);

        // shift the bits to the right
        idx >>= 8;
    }
}

void z_curve_lookup_16bit(unsigned degree, coord_t *x, coord_t *y)
{
    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    // round up degree to next multiple of 8 and divide by 8
    unsigned iterations = ((degree + 7) & ~7) >> 3;

    for (size_t i = 0; i < max; ++i)
    {
        size_t idx = i;
        x[i] = 0;
        y[i] = 0;

        for (unsigned j = 0; j < iterations && idx > 0; ++j)
        {
            // extract the last 16 bits
            unsigned short lookup_index = idx & 0xffff;

            // lookup the x and y values
            unsigned x_value = lookup_table_16bit[lookup_index].x;
            unsigned y_value = lookup_table_16bit[lookup_index].y;

            // store the x and y values
            x[i] |= x_value << (j * 8);
            y[i] |= y_value << (j * 8);

            // shift the bits to the right
            idx >>= 16;
        }
    }
}

void z_curve_lookup_16bit_at(unsigned degree, size_t idx, coord_t *x, coord_t *y)
{
    if (degree > DEGREE_MAX)
    {
        degree = DEGREE_MAX;
    }

    *x = 0;
    *y = 0;

    // round up degree to next multiple of 8 and divide by 8
    unsigned iterations = ((degree + 7) & ~7) >> 3;

    for (unsigned i = 0; i < iterations && idx > 0; ++i)
    {
        // extract the last 16 bits
        unsigned lookup_index = idx & 0xffff;

        // lookup the x and y values
        unsigned x_value = lookup_table_16bit[lookup_index].x;
        unsigned y_value = lookup_table_16bit[lookup_index].y;

        // store the x and y values
        *x |= x_value << (i * 8);
        *y |= y_value << (i * 8);

        // shift the bits to the right
        idx >>= 16;
    }
}

void z_curve_simd_lookup_16bit(unsigned degree, coord_t *x, coord_t *y)
{
    // base case
    if (degree == 1)
    {
        // base case
        *(size_t *)x = 0x100000001ull;
        *(size_t *)y = 0x10001ull;

        return;
    }

    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    // round up degree to next multiple of 8 and divide by 8
    unsigned iterations = ((degree + 7) & ~7) >> 3;

    // divide by 8 because we generate 8 points at a time
    size_t simd_iterations = max >> 3;

    for (size_t i = 0; i < simd_iterations; ++i)
    {
        size_t idx = i << 3;

        __m128i x_vec = _mm_setzero_si128();
        __m128i y_vec = _mm_setzero_si128();

        unsigned short lookup_start_idx = idx & 0xffff;

        __m128i x_lookup_values = _mm_load_si128((__m128i *)&x_16bit[lookup_start_idx]);
        __m128i y_lookup_values = _mm_load_si128((__m128i *)&y_16bit[lookup_start_idx]);

        // or the x and y values into the x_vec and y_vec vectors
        x_vec = _mm_or_si128(x_vec, x_lookup_values);
        y_vec = _mm_or_si128(y_vec, y_lookup_values);

        idx >>= 16;

        if (idx == 0)
        {
            goto store;
        }

        for (unsigned j = 1; j < iterations; ++j)
        {
            unsigned short lookup_start_idx = idx & 0xffff;

            coord_t x_val = x_16bit[lookup_start_idx];
            coord_t y_val = y_16bit[lookup_start_idx];

            // shift the x and y values left by j * 8 bits
            x_val <<= j * 8;
            y_val <<= j * 8;

            // create a vector 4 times the x and y values
            __m128i x_val_vec = _mm_set_epi16(x_val, x_val, x_val, x_val, x_val, x_val, x_val, x_val);
            __m128i y_val_vec = _mm_set_epi16(y_val, y_val, y_val, y_val, y_val, y_val, y_val, y_val);

            // or the x and y values into the x_vec and y_vec vectors
            x_vec = _mm_or_si128(x_vec, x_val_vec);
            y_vec = _mm_or_si128(y_vec, y_val_vec);

            idx >>= 16;

            if (idx == 0)
            {
                goto store;
            }
        }

    store:

        // store the x_vec and y_vec vectors to the x and y arrays
        _mm_storeu_si128((__m128i *)&x[i << 3], x_vec);
        _mm_storeu_si128((__m128i *)&y[i << 3], y_vec);
    }
}
