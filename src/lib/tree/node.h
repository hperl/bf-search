#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <openssl/sha.h>

#include "positions_vector.h"
#include "../../include/definitions.h"
#include "../bloomfilter/bloomfilter.h"

typedef struct Node      Node;
typedef struct QueryNode QueryNode;

struct QueryNode {
    Bloomfilter    bloomfilter;
    Bloomfilter*   bloomfilters;
    int            bloomfilter_count;
    int            tolerance;
};

QueryNode*      query_node_new(int);
void            query_node_free(QueryNode* n);
void            query_node_set_data(QueryNode *, char *, int);

Node*           node_new();
void            node_free(Node *);

void            node_set_children(Node *n, Node *left, Node *right);
void            node_set_data(Node* n, char* data);
void            node_add_position(Node *n, long position);
unsigned char   _add_filter_entries(unsigned char a, unsigned char b);
Node*           node_left_child(Node *n);
Node*           node_right_child(Node *n);
PositionsVector* node_pv(Node *n);
Bloomfilter*    node_bf(Node *n);
void            node_set_bf(Node *n, Bloomfilter *bf);

// printing
void            query_node_print(QueryNode* n);
void            node_print_tree(Node* n);

// node searching
bool            node_matches_node(Node* a, Node* b);
bool            node_matches_query(Node* node, QueryNode* query);
bool            node_matches_query_bf(Node* node, QueryNode* query);
bool            node_matches_data_with_tolerance(Node* node, unsigned char* data, int tol);
bool            node_is_leaf(Node* n);
