#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

#include "node.h"
#include "node_list.h"
#include "util.h"
#include "matches.h"
#include "../../include/definitions.h"

typedef struct Level Level;
typedef struct Tree  Tree;

Tree*        tree_new();
void         tree_free(Tree *);
void         tree_search(Tree *, char *, float *, int);
int          tree_index_file(Tree *, char *, float *);
MatchVector* tree_matches(Tree *);
Node*        tree_node_at(Tree *, unsigned long);
