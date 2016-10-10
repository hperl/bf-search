//
//  util.c
//  bloomfiltersearch
//
//  Created by Henning Perl on 24.01.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#include "util.h"

static ProgressBarInfo _progressbar_info;

void
strchomp(char* str)
{
    int end = strlen(str)-1;
    if (str[end] == '\n') {
        str[end] = 0;
    }
}

unsigned long
characters_in_file(FILE* fp)
{
    long int characters, lines = 0;
    size_t size = 0;
    char* line = NULL;
    
    fseek(fp, 0, SEEK_END);
    characters = ftell(fp);
    
    rewind(fp);
    while (getline(&line, &size, fp) != -1) {
        lines++;
    }
    free(line);
    rewind(fp);
    return characters - lines;
}

bool
read_ngram(char *ngram, FILE *fp)
{
    char *p = ngram;
    int chars_needed = N_GRAM_LENGTH;
    
    ngram[N_GRAM_LENGTH-1] = 0;
    while (fgets(p, chars_needed+1, fp) != NULL) {
        /* check if stopped on newline */
        if (ngram[N_GRAM_LENGTH-1] == '\0' || ngram[N_GRAM_LENGTH-1] == '\n') {
            int l = strlen(ngram) - 1; /* -1 for newline */
            chars_needed -= l;
            p = ngram + l;
        } else {
            /* OK, we have a full n_gram, return successfull */
            return true;
        }
    }
    /* EOF reached (or error) */
    return false;
}

void
start_progressbar(unsigned long total, unsigned int width)
{
    _progressbar_info.old_current = 42;
    _progressbar_info.width = width;
    _progressbar_info.step = (double)width/total;
    printf("Progress: |");
    for (int i = 0; i < _progressbar_info.width; i++) {
        printf(" ");
    }
    printf("|");
}

void
refresh_progressbar(unsigned long current)
{
    int i;
    int c = (int)(current * _progressbar_info.step);
    if (_progressbar_info.old_current == c) {
        return;
    }
    fputc('\r', stdout);
    _progressbar_info.old_current = c;
    printf("Progress: |");
    for (i = 0; i < c; i++) {
        printf("=");
    }
    for (; i < _progressbar_info.width; i++) {
        printf(" ");
    }
    printf("|");
    fflush(stdout);
}

void
finish_progressbar()
{
    printf("\n");
}