#include "bloomfilter.h"


#pragma mark initialization
void
bf_set_zero(Bloomfilter* bf)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        bf->data.limbs[limb] = 0L;
    }
}

void
bf_set_one(Bloomfilter* bf)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        bf->data.limbs[limb] = -1;
    }
}

void
bf_copy(Bloomfilter *bf, Bloomfilter *other)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        bf->data.limbs[limb] = other->data.limbs[limb];
    }
}

#pragma mark marking
void
bf_mark(Bloomfilter *bf)
{
    bf->data.bytes[BF_LIMBS*8 - 1] |= BF_BITMASKS[7];
}

bool
bf_is_marked(Bloomfilter *bf)
{
    return bf->data.bytes[BF_LIMBS*8 - 1] & BF_BITMASKS[7];
}

#pragma mark getting + setting positions
void
bf_setpos(Bloomfilter* bf, int pos)
{
    assert(pos < BF_SIZE);
    int bit = pos % 8;
    bf->data.bytes[pos / 8] |= BF_BITMASKS[bit];
}

void
bf_clearpos(Bloomfilter* bf, int pos)
{
    assert(pos < BF_SIZE);
    int bit = pos % 8;
    bf->data.bytes[pos / 8] &= ~BF_BITMASKS[bit];
}

bool
bf_getpos(Bloomfilter* bf, int pos)
{
    assert(pos < BF_SIZE);
    int bit = pos % 8;
    return bf->data.bytes[pos / 8] & BF_BITMASKS[bit];
}

/* In the current implementation the data is hashed once using SHA-512,
 * resulting in a a digest of size 64 bytes. The first BLOOM_NUM_HASHES bytes
 * are then casted as an integer and taken modulo BLOOM_SIZE to calculate the
 * bloom filter.
 * This is a balance between the strong one-way property of SHA-512 and the
 * performance of just hashing once.
 */
void
bf_hash(Bloomfilter *bf, char *str)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    SHA_CTX c;
    SHA_Init(&c);
    SHA_Update(&c, str, strlen(str));
    SHA_Final(digest, &c);
#pragma clang diagnostic pop
    bf_set_zero(bf);
    for (int i = 0; i < BLOOM_NUM_HASHES; i++) {
        bf_setpos(bf, (int)digest[i] % BF_SIZE);
    }   
}

void
bf_obfuscate(Bloomfilter *bf, int obfuscation_parameter)
{
    int pos;

    /* Assume that there are enough zeros left. Otherwise loop to infinity. */
    while (obfuscation_parameter != 0) {
        pos = rand() % BF_SIZE;
        if (bf_getpos(bf, pos) == 0) {
            bf_setpos(bf, pos);
            obfuscation_parameter--;
        }
    }
}

#pragma arithmetrics
void
bf_addbf(Bloomfilter* bf, Bloomfilter* a, Bloomfilter* b)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        bf->data.limbs[limb] = a->data.limbs[limb] | b->data.limbs[limb];
    }
}

bool
bf_included_in(Bloomfilter* el, Bloomfilter* set, int tolerance)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        u_int64_t conflicts = el->data.limbs[limb] & ~set->data.limbs[limb];
        tolerance -= __builtin_popcountll(conflicts);
    }
    return (tolerance >= 0);
}

bool
bf_equal(Bloomfilter *a, Bloomfilter *b)
{
    for (int limb = 0; limb < BF_LIMBS; limb++) {
        if (a->data.limbs[limb] != b->data.limbs[limb]) return false;
    }
    return true;
}


#pragma mark output
char*
bf_to_str(Bloomfilter* bf, char* str)
{
    if (str == NULL) {
        /* allocate enough space for the string representation
         * enough -> one char per entry + one char delimiter + start + \0
         */
        str = malloc(BF_SIZE*2 + 2);
    }
    char *c = str;
    c[0] = '|';
    c++;
    for (int pos = 0; pos < BF_SIZE; pos++) {
        sprintf(c, "%d|", bf_getpos(bf, pos));
        c += 2;
    }
    return str;
}