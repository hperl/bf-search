#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "util.h"
#include "../../include/definitions.h"

typedef struct Match       Match;
typedef struct MatchVector MatchVector;


MatchVector* new_match_vector();
void         free_match_vector(MatchVector *mv);

void         add_match(MatchVector *mv, long position);
void         clear_matches(MatchVector *mv);
void         print_matches(MatchVector *mv);
int          mv_fill_strings_from_file(MatchVector *mv, char* filename);
bool         mv_is_empty(MatchVector *mv);

char*        match_str(Match *);
long         match_pos(Match *);


/* iteration */
Match*       mv_iter_start(MatchVector *mv);
Match*       mv_iter_next(Match *m);
bool         mv_iter_done(Match *m);
