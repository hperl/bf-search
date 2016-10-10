#include "tree.h"
#include <unistd.h>

int NUM_THREADS = 6;
//const long int NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN) + 1;

struct Tree {
    Node        *root;
    char        *filename;
    MatchVector *matches;
};

typedef struct ThreadArgs ThreadArgs;
struct ThreadArgs {
    char *filename; /* struct does not own this! */
    long start_pos;
    long end_pos;
    int  threadnum;
};


Tree*
tree_new()
{
    Tree *t = malloc(sizeof(Tree));
    
    t->matches = new_match_vector();
    t->filename = NULL;
    return t;
}

void
tree_free(Tree* t)
{
    free_match_vector(t->matches);
    free(t->filename);
    node_free(t->root);
    free(t);
}

void
tree_set_filename(Tree* t, char* filename)
{
    int len = strlen(filename);
    if (t->filename == NULL) {
        t->filename = malloc(len+1);
    } else {
        t->filename = realloc(t->filename, len+1);
    }
    if (t->filename == NULL) {
        perror("malloc failed");
    }
    strncpy(t->filename, filename, len+1);
}

void
tree_search_subtree(Tree* t, Node* node, QueryNode* query)
{
    if (node_matches_query(node, query)) {
        if (node_is_leaf(node)) {
            PVIterator *iter;
            long pos;
            for (iter = pv_iter_start(node_pv(node)); !pv_iter_done(iter); pv_iter_next(iter)) {
                pos = pv_iter_cur(iter);
                add_match(t->matches, pos);
            }
        } else {
            tree_search_subtree(t, node_left_child(node), query);
            tree_search_subtree(t, node_right_child(node), query);
        }
    }
}

void
tree_search(Tree* t, char* q, float *time, int obfuscations)
{
    clock_t clock_diff = clock();
    /*unsigned int q_len = strlen((char*)q);*/
    QueryNode *q_node = query_node_new(1 /*q_len+1 - N_GRAM_LENGTH*/);
    
    clear_matches(t->matches);
    query_node_set_data(q_node, q, obfuscations);
    tree_search_subtree(t, t->root, q_node);
    mv_fill_strings_from_file(t->matches, t->filename);
    query_node_free(q_node);
    clock_diff = clock() - clock_diff;
    *time = (float)(clock_diff)/CLOCKS_PER_SEC;
}

MatchVector*
tree_matches(Tree *tree)
{
    return tree->matches;
}

void *
read_ngrams_from_file(void *_args)
{
    ThreadArgs *args = (ThreadArgs*)_args;
    FILE *fp;
    char ngram[N_GRAM_LENGTH+1];
    Bloomfilter bf;
    NodeList *nl = nl_new();
    bool appended;
    
    printf("STARTING thread %d\n", args->threadnum);
    
    fp = fopen(args->filename, "r");
    if (fp == NULL) {
        puts("IO error");
        exit(EXIT_FAILURE);
    }
    fseek(fp, args->start_pos, SEEK_SET);
    while (read_ngram(ngram, fp) && args->start_pos < args->end_pos) {
        /* OK, we have a full n_gram */
        /* build new Bloom filter here */
        bf_hash(&bf, ngram);
        /* check if this bloom filter already exists */
        appended = false;
        for (NodeListItem *i = nl_iter_start(nl); i != NULL; i = i->next) {
            if (bf_equal(&bf, node_bf(i->node))) {
                node_add_position(i->node, args->start_pos);
                appended = true;
                break;
            }
        }
        if (!appended) {
            /* nothing found, insert node */
            Node *node = node_new();
            node_set_bf(node, &bf);
            node_add_position(node, args->start_pos);
            nl_add_node(nl, node);
        }
        /* bookkeeping */
        args->start_pos = ftell(fp);
    }
    
    fclose(fp);
    printf("ENDING thread %d\n", args->threadnum);
    return nl;
}

void *
read_line_from_file(void *_args)
{
    ThreadArgs *args = (ThreadArgs*)_args;
    FILE *fp;
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t bytes_read;
    Bloomfilter bf;
    NodeList *nl = nl_new();
    bool appended;
    
//    printf("STARTING thread %d with START %ld and END %ld\n", args->threadnum, args->start_pos, args->end_pos);
    
    fp = fopen(args->filename, "r");
    if (fp == NULL) {
        puts("IO error");
        exit(EXIT_FAILURE);
    }
    fseek(fp, args->start_pos, SEEK_SET);
    while ((bytes_read = getline(&buffer, &buffer_size, fp)) != -1 &&
           args->start_pos < args->end_pos) {
        if (buffer[bytes_read-1] == '\n') buffer[bytes_read-1] = 0;
        bf_hash(&bf, buffer);
        /* check if this bloom filter already exists */
        appended = false;
        /*
        for (NodeListItem *i = nl_iter_start(nl); i != NULL; i = i->next) {
            if (bf_equal(&bf, node_bf(i->node))) {
                node_add_position(i->node, args->start_pos);
                appended = true;
                break;
            }
        }*/
        if (!appended) {
            /* nothing found, insert node */
            Node *node = node_new();
            node_set_bf(node, &bf);
            node_add_position(node, args->start_pos);
            nl_add_node(nl, node);
        }
        /* bookkeeping */
        args->start_pos = ftell(fp);
    }
    free(buffer);
    
    fclose(fp);
//    printf("ENDING thread %d\n", args->threadnum);
    return nl;
}

int
tree_index_file(Tree* tree, char* filename, float *elapsed_time)
{
    /* find out number of processors and adjust number of threads */
    NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN);

    FILE *fp;
    Node **node_buffer;
    long bytes_in_file;
    pthread_t workers[NUM_THREADS];
    struct timeval start_time, end_time;
    NodeList *nl = nl_new();
    
    /* time measurement */
    gettimeofday(&start_time, NULL);
    
    /* terminate n-gram to always make it a proper c string */
    /* open file */
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return errno;
    }
    /* how many bytes are there? */
    fseek(fp, 0, SEEK_END);
    bytes_in_file = ftell(fp);
    /* init tree */
    tree_set_filename(tree, filename);
        
    /* init threads */
    for (long t = 0; t < NUM_THREADS; t++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->filename  = filename;              /* struct does not own this! */
        args->start_pos = bytes_in_file * t/NUM_THREADS;
        args->end_pos   = bytes_in_file * (t+1)/NUM_THREADS;
        args->threadnum = t;
        pthread_create(workers+t, NULL, read_line_from_file, args);
    }
    /* join threads and their nodelists */
    for (int t = 0; t < NUM_THREADS; t++) {
        NodeList *thread_nl;
        
        if (pthread_join(workers[t], (void **)&thread_nl) != 0) {
            puts("Error while joining a thread");
        }
        nl_merge(nl, thread_nl);
    }
    /* Do one step of the tree building by filling the node buffer with the
     * parents of the node list.
     */
    node_buffer = malloc(sizeof(Node*) * (nl->cnt/2 + 3));
    int idx = 0;
    NodeListItem *item;
    for (item = nl_iter_start(nl); item && item->next; item = item->next->next) {
        Node* node = node_new();
        node_set_children(node, item->node, item->next->node);
        node_buffer[idx] = node;
        idx++;
    }
    if (item) {
        node_buffer[idx] = item->node;
        idx++;
    }
    node_buffer[idx]   = NULL;
    node_buffer[idx+1] = NULL;
    /* 
     * Now that the file has been read and node_buffer has been filled, we need
     * to construct a tree over the nodes.
     */
    Node **parent, **left, **right;
    for (parent = left = &node_buffer[0], right = left+1; *right != NULL;
         parent = left = &node_buffer[0], right = left+1) {
        for (; *right != NULL; parent++, left += 2, right += 2) {
            Node* node = node_new();
            node_set_children(node, *left, *right);
            *parent = node;
        }
        if (*left) {
            (*parent) = (*left);
            parent++;
        }
        /* zero-terminate remaining buffer */
        parent[0] = NULL;
        parent[1] = NULL;
    }
    /* root is now at the beginning of the node_buffer */
    tree->root = node_buffer[0];
    free(node_buffer);
    nl_free(nl);
    
    /* end time measurement */
    gettimeofday(&end_time, NULL);
    *elapsed_time = end_time.tv_sec   + (end_time.tv_usec/1000000.0)
                  - start_time.tv_sec - (start_time.tv_usec/1000000.0);

    /* close file */
    fclose(fp);
    return EXIT_SUCCESS;
}
