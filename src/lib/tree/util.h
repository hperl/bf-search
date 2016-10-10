//
//  util.h
//  bloomfiltersearch
//
//  Created by Henning Perl on 24.01.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#ifndef bloomfiltersearch_util_h
#define bloomfiltersearch_util_h

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../include/definitions.h"

typedef struct {
    double step;
    unsigned int width;
    unsigned long old_current;
} ProgressBarInfo;


void            strchomp(char* str);
unsigned long   characters_in_file(FILE* fp);
void            start_progressbar(unsigned long total, unsigned int width);
void            refresh_progressbar(unsigned long current);
void            finish_progressbar();

bool            read_ngram(char *ngram, FILE *fp);

#endif
