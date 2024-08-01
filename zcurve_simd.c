#include "zcurve_simd.h"
#include "zcurve_lookup.h"

#include <stdio.h>

void z_curve_simd(unsigned degree, coord_t *x, coord_t *y)
{
    if (degree == 1)
    {
        // base case
        *(size_t *)x = 0x100000001ull;
        *(size_t *)y = 0x10001ull;

        return;
    }

    // number of max points is 4^degree
    size_t max = (1ull << (degree * 2)) >> 3;

    for (size_t i = 0; i < max; ++i)
    {
        __m128i x_vec = _mm_setzero_si128();
        __m128i y_vec = _mm_setzero_si128();

        size_t idx = i << 3;

        for (unsigned j = 0; j < degree; ++j)
        {
            x_vec = _mm_or_si128(x_vec, _mm_slli_epi16(_mm_and_si128(_mm_set_epi16(1, 1, 1, 1, 1, 1, 1, 1), _mm_srli_epi16(_mm_set_epi16(idx + 7, idx + 6, idx + 5, idx + 4, idx + 3, idx + 2, idx + 1, idx), j << 1)), j));
            y_vec = _mm_or_si128(y_vec, _mm_slli_epi16(_mm_and_si128(_mm_set_epi16(1, 1, 1, 1, 1, 1, 1, 1), _mm_srli_epi16(_mm_set_epi16(idx + 7, idx + 6, idx + 5, idx + 4, idx + 3, idx + 2, idx + 1, idx), (j << 1) + 1)), j));
        }
        _mm_storeu_si128((__m128i *)&x[idx], x_vec);
        _mm_storeu_si128((__m128i *)&y[idx], y_vec);
    }
}
