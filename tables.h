#ifndef _TABLES_H
#define _TABLES_H

#include "defs.h"

typedef struct
{
    coord_t x;
    coord_t y;
} lookup_t;

#include "lookup_table_simd_4bit.h"
#include "lookup_table_simd_8bit.h"
#include "lookup_table_simd_16bit.h"

#include "lookup_table_4bit.h"
#include "lookup_table_8bit.h"
#include "lookup_table_16bit.h"

#endif // _TABLES_H
