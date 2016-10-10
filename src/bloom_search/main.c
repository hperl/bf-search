//
//  bloom_search.c
//  bloomfiltersearch
//
//  Created by Henning Perl on 20.01.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "../lib/tree/tree.h"

static int index_only_flag_g = 0;
static int obfuscations_g = 0;

void
interactive_search(Tree *bloom_tree)
{
    char* query = NULL;
    size_t len = 0;
    float time;

    do {
        printf("\nsearch for: ");
        getline(&query, &len, stdin);
        strchomp(query);
        
        if (strcmp(query, "quit") == 0) {
            free(query);
            query = NULL;
            printf("Exiting ...\n");
            return;
        }
        tree_search(bloom_tree, query, &time, obfuscations_g);
        print_matches(tree_matches(bloom_tree));
        printf("Search took %f s.\n", time);
    } while (1);
}

void
usage()
{
    printf("COMMAND:    bloom_search [OPTIONS] FILES...\n\n");
    printf("OPTIONS:    -h, --help                Show this message.\n");
    printf("            -i, --index-only          Only index the files and exit right away.\n");
    printf("            -o, --obfuscations        The obfuscation parameter.\n");
}

void
parse_args(int argc, char** argv)
{
    int c;
    int option_index = 0;
        
    static struct option long_options[] = {
        {"index-only",          no_argument,        &index_only_flag_g,  1},
        {"help",                no_argument,        0,                'h'},
        {"obfuscations",        required_argument,  0,                'o'},
        {0,                     0,                  0,                 0}
    };
    
    while ((c = getopt_long(argc, argv, "iho:", long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                break;
                
            case 'i':
                index_only_flag_g = 1;
                break;
            
            case 'o':
                obfuscations_g = atoi(optarg);
                break;
            
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
                
            case '?':
                usage();
                exit(EXIT_FAILURE);
                
            default:
                abort();
        }
    }

    if (!argv[optind]) { // we expect at least one filename to be here
        usage();
        exit(EXIT_FAILURE);
    }
    // check if all assumptions are met
    if (BLOOM_NUM_HASHES > SHA_DIGEST_LENGTH) {
        printf("ERROR: BLOOM_NUM_HASHES (%d) too large (must not be larger than %d).\n",
               BLOOM_NUM_HASHES, SHA_DIGEST_LENGTH);
        abort();
    }
}

int
main(int argc, char** argv)
{
    int errno;
    Tree *bloom_tree = tree_new();
    float time;
    
    parse_args(argc, argv);
    if ((errno = tree_index_file(bloom_tree, argv[optind], &time))) {
        /* error handling */
        printf("Indexing failed (%s)\n", argv[optind]);
        puts(strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Obfuscation parameter: %d\n", obfuscations_g);
    printf("Index took %f s.\n", time);
    if (!index_only_flag_g) {
        interactive_search(bloom_tree);
    }
    tree_free(bloom_tree);
    exit(EXIT_SUCCESS);
}