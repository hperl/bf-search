//
//  check_layer.c
//  bloomfiltersearch
//
//  Created by Henning Perl on 25.01.12.
//  Copyright (c) 2012 DCSec – Uni Hannover. All rights reserved.
//

#include "check_check.h"

START_TEST(test_level_length)
{
    Level l;
    l.start = 10;
    l.end = 14;
    fail_unless(level_length(&l) == 4, "Length is not correct");
}
END_TEST

START_TEST(test_level_parent_level)
{
    Level l;
    l.start = 0;
    l.end = 11;
    parent_level(&l, &l);
    fail_unless(l.start == 11 && l.end == 16, "Level parent incorrect");
    
    l.start = 15;
    l.end = 16;
    parent_level(&l, &l);
    fail_unless(l.start == 16 && l.end == 16, "Level should just move one up");
    parent_level(&l, &l);
    fail_unless(l.start == 16 && l.end == 16, "Level should stay the same");
}
END_TEST

Suite*
tree_util_suite(void)
{
    Suite* s = suite_create("Tree");
    
    TCase *tc_core = tcase_create("Levels");
    tcase_add_test(tc_core, test_level_length);
    tcase_add_test(tc_core, test_level_parent_level);
    suite_add_tcase(s, tc_core);
    
    return s;
}
