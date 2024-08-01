#include "zcurve_magic.h"
#include "zcurve_simd.h"

void z_curve_magic(unsigned degree, coord_t *x, coord_t *y)
{
    // number of max points is 4^degree
    size_t max = 1ull << (degree * 2);

    for (size_t i = 0; i < max; ++i)
    {
        decode(i, &x[i], &y[i]);
    }

    return;
}

void z_curve_magic_at(unsigned degree, size_t idx, coord_t *x, coord_t *y)
{
    (void)degree;
    decode(idx, x, y);
}

size_t z_curve_magic_pos(unsigned degree, coord_t x, coord_t y)
{
    (void)degree;
    return encode(x, y);
}

void z_curve_simd_magic(unsigned degree, coord_t *x, coord_t *y)
{
    if (degree == 1)
    {
        // base case
        *(size_t *)x = 0x100000001ull;
        *(size_t *)y = 0x10001ull;

        return;
    }

    // number of quad'Z's in the curve
    size_t num_blocks = (1ull << (degree * 2)) >> 4;

    __m128i m0 = _mm_set_epi64x(0x5555555555555555, 0x5555555555555555);
    __m128i m1 = _mm_set_epi64x(0x3333333333333333, 0x3333333333333333);
    __m128i m2 = _mm_set_epi64x(0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f);
    __m128i m3 = _mm_set_epi64x(0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff);

    for (size_t i = 0; i < num_blocks; ++i)
    {
        size_t idx0 = i << 4;
        size_t idx1 = idx0 + (1 << 3);

        coord_t x0 = 0;
        coord_t y0 = 0;
        coord_t x1 = 0;
        coord_t y1 = 0;

        __m128i idx_vec = _mm_set_epi64x(idx1, idx0);

        __m128i z = _mm_or_si128(idx_vec, _mm_slli_epi64(idx_vec, 31));
        z = _mm_and_si128(z, m0);

        z = _mm_or_si128(z, _mm_srli_epi64(z, 1));
        z = _mm_and_si128(z, m1);

        z = _mm_or_si128(z, _mm_srli_epi64(z, 2));
        z = _mm_and_si128(z, m2);

        z = _mm_or_si128(z, _mm_srli_epi64(z, 4));
        z = _mm_and_si128(z, m3);

        z = _mm_or_si128(z, _mm_srli_epi64(z, 8));

        x0 = (coord_t)_mm_extract_epi32(z, 0);
        y0 = (coord_t)_mm_extract_epi32(z, 1);
        x1 = (coord_t)_mm_extract_epi32(z, 2);
        y1 = (coord_t)_mm_extract_epi32(z, 3);

        /*
        x and y represent the top left corner of the 'Z' block
        and the next block is located at x+2 and y

        x,y     x+1,y   x+2,y   x+3,y
         _______          _______
        |       |        |       |
        |       |        |       |
        |       |        |       |
        |_______|        |_______|

        x,y+1   x+1,y+1  x+2,y+1 x+3,y+1
        */

        __m128i x0_vec = _mm_set_epi16(x0 + 3, x0 + 2, x0 + 3, x0 + 2, x0 + 1, x0, x0 + 1, x0);
        __m128i y0_vec = _mm_set_epi16(y0 + 1, y0 + 1, y0, y0, y0 + 1, y0 + 1, y0, y0);

        __m128i x1_vec = _mm_set_epi16(x1 + 3, x1 + 2, x1 + 3, x1 + 2, x1 + 1, x1, x1 + 1, x1);
        __m128i y1_vec = _mm_set_epi16(y1 + 1, y1 + 1, y1, y1, y1 + 1, y1 + 1, y1, y1);

        // store the values
        _mm_storeu_si128((__m128i *)&x[idx0], x0_vec);
        _mm_storeu_si128((__m128i *)&y[idx0], y0_vec);

        _mm_storeu_si128((__m128i *)&x[idx1], x1_vec);
        _mm_storeu_si128((__m128i *)&y[idx1], y1_vec);
    }
}
