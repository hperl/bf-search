/*
 * Supplies an efficient data structure for bloom filters
 */

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include <openssl/sha.h>

#include "../../include/definitions.h"


/* masks with only one bit at one position set */
static const u_int8_t BF_BITMASKS[] = {
    1 << 0,
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7
};

typedef struct Bloomfilter Bloomfilter;

struct Bloomfilter {
    union data {
        u_int64_t limbs[BF_LIMBS];
        u_int8_t  bytes[BF_LIMBS*8];
    } data;
};

/* initialization */
void    bf_set_zero(Bloomfilter *);
void    bf_set_one(Bloomfilter *);
void    bf_copy(Bloomfilter *, Bloomfilter *);

/* marking */
void    bf_mark(Bloomfilter *);
bool    bf_is_marked(Bloomfilter *);

/* getting & setting positions */
void    bf_setpos(Bloomfilter *, int);
void    bf_clearpos(Bloomfilter *, int);
bool    bf_getpos(Bloomfilter *, int);
void    bf_hash(Bloomfilter *, char *);
void    bf_obfuscate(Bloomfilter *, int);

/* arithmetrics */
void    bf_addbf(Bloomfilter *, Bloomfilter *, Bloomfilter *);
bool    bf_included_in(Bloomfilter *, Bloomfilter *, int);
bool    bf_equal(Bloomfilter *, Bloomfilter *);

/* conversion */
char *  bf_to_str(Bloomfilter *, char *);