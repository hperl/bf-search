#include "node.h"

/* file-local declarations */
static void node_print(Node* n, int indent);
static void _node_print_tree(Node* n, int indent);

struct Node {
    Bloomfilter bloomfilter;
    union node_data {
        struct children {
            Node *left;
            Node *right;
        } children;
        struct positions {
            intptr_t zero_if_leaf;  /* field should be one if a leaf */
            PositionsVector *pv;    /* array of file positions */
        } positions;
    } node_data;
};

Node*
node_new()
{
    Node* n = calloc(1, sizeof(Node));

    n->node_data.positions.pv = NULL;
    n->node_data.positions.zero_if_leaf = 1L;
    return n;
}

void
node_free(Node *n)
{
    if (node_is_leaf(n)) {
        pv_free(n->node_data.positions.pv);
    } else {
        node_free(n->node_data.children.left);
        node_free(n->node_data.children.right);
    }
    free(n);
}

void
node_set_children(Node* n, Node* left, Node* right)
{
    n->node_data.children.left  = left;
    n->node_data.children.right = right;
    bf_mark(&n->bloomfilter);
    bf_addbf(&n->bloomfilter, &left->bloomfilter, &right->bloomfilter);
}

void
node_set_data(Node* n, char *data)
{
    bf_hash(&n->bloomfilter, data);
}

Bloomfilter*
node_bf(Node *n)
{
    return &n->bloomfilter;
}

void
node_set_bf(Node *n, Bloomfilter *bf)
{
    bf_copy(&n->bloomfilter, bf);
}

void
node_add_position(Node *n, long position)
{
    if (!n->node_data.positions.pv) {
        n->node_data.positions.pv = pv_new();
    }
    pv_insert(n->node_data.positions.pv, position);
}

PositionsVector*
node_pv(Node *n)
{
    return n->node_data.positions.pv;
}

Node*
node_left_child(Node *n)
{
    return n->node_data.children.left;
}

Node*
node_right_child(Node *n)
{
    return n->node_data.children.right;
}

#pragma mark node printing
static void
node_print(Node* n, int indent)
{    
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
    printf("data: ");
    for (int i = 0; i < MAX_INDENT - indent; i++) {
        printf(" ");
    }
    char *buf = NULL;
    bf_to_str(&n->bloomfilter, buf);
    printf("Bloomfilter:   %s\n", buf);
    free(buf);
}

static void
_node_print_tree(Node* n, int indent)
{
    if (!n) {
        return;
    }
    node_print(n, indent);
    _node_print_tree(n->node_data.children.left,  indent + 2);
    _node_print_tree(n->node_data.children.right, indent + 2);
}

void
node_print_tree(Node* n)
{
    _node_print_tree(n, 0);
}

#pragma mark node searching and matching
bool
node_matches_node(Node* a, Node* b)
{
    return bf_included_in(&a->bloomfilter, &b->bloomfilter, 0);
}

bool
node_matches_query(Node* node, QueryNode* query)
{
    return bf_included_in(&query->bloomfilter, &node->bloomfilter, query->tolerance);
}

bool
node_matches_query_bf(Node* node, QueryNode* query)
{
    for (int i = 0; i < query->bloomfilter_count; i++) {
        if (bf_included_in(&query->bloomfilters[i], &node->bloomfilter, 0)) {
            return true;
        }
    }
    return false;
}

bool
node_is_leaf(Node* n)
{
    return n->node_data.positions.zero_if_leaf == 1L;
}

unsigned char
_add_filter_entries(unsigned char a, unsigned char b)
{
    if (a >= BLOOM_ENTRY_MAX || b >= BLOOM_ENTRY_MAX) {
        return BLOOM_ENTRY_MAX;
    }
    return a + b;
}
