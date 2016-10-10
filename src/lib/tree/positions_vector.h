#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct PositionsVector PositionsVector;
typedef struct PVIterator      PVIterator;

/* position */
PositionsVector*    pv_new();
void                pv_free(PositionsVector *);
void                pv_insert(PositionsVector *, long);

/* iteration */
PVIterator*         pv_iter_start(PositionsVector *);
void                pv_iter_next(PVIterator *);
long                pv_iter_cur(PVIterator *);
bool                pv_iter_done(PVIterator *);
