//
//  matches.c
//  bloomfiltersearch
//
//  Created by Henning Perl on 07.02.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#include "matches.h"

struct Match {
    long pos;
    char  str[N_GRAM_LENGTH+1];
    Match *next;
};

struct MatchVector {
    Match *head;
    Match *tail;
};

MatchVector*
new_match_vector()
{
    MatchVector *mv = malloc(sizeof(MatchVector));
    
    mv->head = mv->tail = NULL;
    return mv;
}

void
free_match_vector(MatchVector *mv)
{
    clear_matches(mv);
    free(mv);
}

void
add_match(MatchVector *mv, long position)
{
    Match* new_match = (Match*)malloc(sizeof(Match));
    new_match->pos   = position;
    new_match->next  = NULL;
    
    if (mv->tail) {
        mv->tail->next = new_match;
    } else {
        mv->head = new_match;
    }
    mv->tail = new_match;
}

void
clear_matches(MatchVector *mv)
{
    Match *tmp = NULL,
          *cur = mv->head;
    
    while (cur != NULL) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    mv->head = NULL;
    mv->tail = NULL;
}

bool
mv_is_empty(MatchVector *mv)
{
    return mv->head == NULL;
}

void
print_matches(MatchVector *mv)
{
    int cnt = 0;
    for (Match* m = mv_iter_start(mv); !mv_iter_done(m); m = mv_iter_next(m)) {
        cnt++;
        printf("Match at %ld: %s\n", m->pos, m->str);
    }
    if (cnt == 1) {
        printf("Found 1 match in total\n");
    } else {
        printf("Found %d matches in total\n", cnt);
    }
}

int
mv_fill_strings_from_file(MatchVector *mv, char* filename)
{
    FILE *fp;
    
    if (mv_is_empty(mv)) {
        return EXIT_SUCCESS;
    }
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        return errno;
    }
    /* iterate over the matches */
    for (Match* m = mv_iter_start(mv); !mv_iter_done(m); m = mv_iter_next(m)) {
        fseek(fp, m->pos, SEEK_SET);
        read_ngram(m->str, fp);
    }
    fclose(fp);
    return EXIT_SUCCESS;
}

char*
match_str(Match *m)
{
    return m->str;
}

long
match_pos(Match *m)
{
    return m->pos;
}

Match*
mv_iter_start(MatchVector *mv)
{
    return mv->head;
}

Match*
mv_iter_next(Match *m)
{
    return m->next;
}

bool
mv_iter_done(Match *m)
{
    return m == NULL;
}
