//
//  check_node.c
//  bloomfiltersearch
//
//  Created by Henning Perl on 25.01.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#include "check_check.h"

START_TEST(test_node_init_and_clear)
{
    Node n, *node = &n;
    node_init(node);
    fail_unless(node->left == NULL  &&
                node->right == NULL &&
                node->line == 0, "failed to initialize node");
    node_clear(node);
}
END_TEST

START_TEST(test_node_init_with_children)
{
    Node _p, _c1, _c2,
         *p = &_p, *c1 = &_c1, *c2 = &_c2;
    node_init(c1);
    node_init(c2);
    bf_setpos(&c1->bloomfilter, 0);
    bf_setpos(&c1->bloomfilter, 1);
    
    fail_if(bf_getpos(&c1->bloomfilter, 0) != 1, "first pos should be 1");
    fail_if(bf_getpos(&c1->bloomfilter, 1) != 1, "second pos should be 1");
    
    node_init_with_children(p, c1, c2);
//    fail_if(bf_getpos(&p->bloomfilter, 0) != 1, "first pos should be 1");
//    fail_if(bf_getpos(&p->bloomfilter, 1) != 1, "second pos should be 1");
    
    node_clear(p);
    node_clear(c1);
    node_clear(c2);
}
END_TEST

Suite*
node_suite(void)
{
    Suite* s = suite_create("Node");
    
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_node_init_and_clear);
    tcase_add_test(tc_core, test_node_init_with_children);
    suite_add_tcase(s, tc_core);
    
    return s;
}

void hello(void)
{
    printf("Hello\n");
}