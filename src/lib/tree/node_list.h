#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "node.h"

typedef struct NodeList NodeList;
typedef struct NodeListItem NodeListItem;

struct NodeListItem {
    NodeListItem *next;
    Node         *node;
};
struct NodeList {
    NodeListItem *head;
    NodeListItem *tail;
    long         cnt;
};

NodeList *      nl_new();
void            nl_free(NodeList *);

bool            nl_is_empty(NodeList *);
void            nl_add_node(NodeList *, Node *);
void            nl_merge(NodeList *restrict, NodeList *restrict);

/* iteration */
NodeListItem *  nl_iter_start(NodeList *);
