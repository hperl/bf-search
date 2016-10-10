#include "node.h"

QueryNode*
query_node_new(int bloomfilter_count)
{
    QueryNode *n = malloc(sizeof(QueryNode));
    
    bf_set_zero(&n->bloomfilter);
    n->bloomfilter_count = bloomfilter_count;
    n->tolerance = (bloomfilter_count - 1) * BLOOM_NUM_HASHES;
    n->bloomfilters = malloc(bloomfilter_count * sizeof(Bloomfilter));
    for (int i = 0; i < bloomfilter_count; i++) {
        bf_set_zero(&n->bloomfilters[i]);
    }
    return n;
}

void
query_node_free(QueryNode* n)
{
    free(n->bloomfilters);
    free(n);
}

void
query_node_set_data(QueryNode* n, char *data, int obfuscations)
{
    char *ngram;
    
    n->tolerance = obfuscations;
    for (int i = 0; i < n->bloomfilter_count; i++) {
        ngram = data + i;
        bf_hash(&n->bloomfilters[i], ngram);
        bf_obfuscate(&n->bloomfilters[i], obfuscations);
        bf_addbf(&n->bloomfilter, &n->bloomfilter, &n->bloomfilters[i]);
    }
}

#pragma mark node printing
void
query_node_print(QueryNode* n)
{
    char *buf = NULL;
    bf_to_str(&n->bloomfilter, buf);
    printf("Bloomfilter:   %s\n", buf);
    
    for (int j = 0; j < n->bloomfilter_count; j++) {
        bf_to_str(&n->bloomfilters[j], buf);
        printf("Bloomfilter %d: %s\n", j, buf);
    }
    free(buf);
}
