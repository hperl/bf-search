#include <ruby.h>
#include <stdlib.h>
#include <errno.h>

#include "../lib/tree/tree.h"

VALUE tree_class;
VALUE match_class;

static VALUE
rb_tree_new(VALUE class)
{
    Tree *tree = tree_new();
    
    VALUE tree_data = Data_Wrap_Struct(class, 0, tree_free, tree);
    rb_obj_call_init(tree_data, 0, NULL);
    return tree_data;
}

static VALUE
rb_tree_init(VALUE self)
{
    /* do nothing */
    return self;
}


static VALUE
rb_tree_index_file(VALUE self, VALUE filename)
{
    Tree *tree;
    float time;
    
    Data_Get_Struct(self, Tree, tree);
    if (tree_index_file(tree, StringValuePtr(filename), &time)) {
        /* error occurred */
        rb_sys_fail(StringValuePtr(filename));
    }
    rb_iv_set(self, "@operation_time", rb_float_new(time));
    return rb_float_new(time);
}

static VALUE
rb_tree_search(VALUE self, VALUE query, VALUE obfuscation)
{
    Tree *tree;
    float time;
    MatchVector *mv;
    Match *m;
    VALUE matches   = rb_ary_new();
    VALUE match_key = rb_str_new2("match");
    VALUE pos_key   = rb_str_new2("position");
    
    Data_Get_Struct(self, Tree, tree);
    tree_search(tree, StringValuePtr(query), &time, FIX2INT(obfuscation));
    rb_iv_set(self, "@operation_time", rb_float_new(time));
    mv = tree_matches(tree);
    for (m = mv_iter_start(mv); !mv_iter_done(m); m = mv_iter_next(m)) {
        VALUE match = rb_hash_new();
        
        rb_hash_aset(match, match_key, rb_str_new2(match_str(m)));
        rb_hash_aset(match, pos_key,   INT2FIX(match_pos(m)));
        rb_ary_push(matches, match);
    }
    return matches;
}

static VALUE
rb_operation_time(VALUE self)
{
    return rb_iv_get(self, "@operation_time");
}

void
Init_bfruby()
{
    /* init tree class */
    tree_class = rb_define_class("BFTree", rb_cObject);
    rb_define_singleton_method(tree_class, "new", rb_tree_new, 0);
    rb_define_method(tree_class, "initialize",    rb_tree_init, 0);
    rb_define_method(tree_class, "index_file",    rb_tree_index_file, 1);
    rb_define_method(tree_class, "search",        rb_tree_search, 2);
    rb_define_method(tree_class, "operation_time",rb_operation_time, 0);
}
