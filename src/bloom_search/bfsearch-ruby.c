#include <Ruby/ruby.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <gcrypt.h>

#include "../lib/tree/tree.h"

VALUE cBFTree;

int
main(int argc, char** argv)
{
    Tree bloom_tree;
    
    tree_init_from_file(&bloom_tree, argv[optind]);
    
    tree_clear(&bloom_tree);

    exit(EXIT_SUCCESS);
}

static VALUE
bftree_new(VALUE class)
{
    Tree tree;
    VALUE tree_data = Data_Wrap_Struct(class, 0, tree_clear, &tree);
    rb_obj_call_init(tree_data, 0, NULL);
    return tree_data;
}

void
Init_BFSearch()
{
    cBFTree = rb_define_class("BFTree", rb_cObject);
    rb_define_singleton_method(cBFTree, "new", bftree_new, 1);
}