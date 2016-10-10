#include "positions_vector.h"

/* enlarge by this if we have too little space */
static const size_t SIZE_GAIN = 50;
/* initial size of vector */
static const size_t SIZE_INIT = 20;

struct PositionsVector {
    size_t size;
    size_t length;
    long *positions;
};

struct PVIterator {
    PositionsVector* pv;
    size_t current_pos;
};


PositionsVector*
pv_new()
{
    PositionsVector *pv = malloc(sizeof(PositionsVector));
    
    pv->size = SIZE_INIT;
    pv->length = 0;
    pv->positions = malloc(sizeof(fpos_t) * SIZE_INIT);
    return pv;
}

void
pv_free(PositionsVector *pv)
{
    free(pv->positions);
    free(pv);
}

void
pv_insert(PositionsVector *pv, long position)
{
    if (pv->length >= pv->size) {
        /* need to enlarge offsets vector */
        pv->size += SIZE_GAIN;
        pv->positions = realloc(pv->positions, pv->size * sizeof(fpos_t));
    }
    pv->positions[pv->length] = position;
    pv->length++;
}

PVIterator*
pv_iter_start(PositionsVector *pv)
{
    PVIterator *pv_iter = malloc(sizeof(PVIterator));
    
    pv_iter->pv = pv;
    pv_iter->current_pos = 0;
    return pv_iter;
}

void
pv_iter_next(PVIterator *pv_iter)
{
    pv_iter->current_pos++;
}

long
pv_iter_cur(PVIterator *pv_iter)
{
    return pv_iter->pv->positions[pv_iter->current_pos];
}

bool
pv_iter_done(PVIterator *pv_iter)
{
    if (pv_iter->current_pos >= pv_iter->pv->length) {
        /* iteration has ended, deallocate self */
        free(pv_iter);
        return true;
    }
    return false;
}
