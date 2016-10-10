#pragma once
#ifndef TEST_H
#define TEST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../lib/bloomfilter/bloomfilter.h"
#include "../lib/tree/tree.h"

int bloomfilter();
int tree();

#define RUNS 1000

/*
 * Test helper macros.
 */

static char _exit_status __attribute((unused)); /* xcode can't figure this out */

#define test_setup()                                                           \
_exit_status = EXIT_SUCCESS

#define test_exit()                                                            \
    if (_exit_status == EXIT_SUCCESS) {                                        \
        printf(" PASSED.\n");                                                  \
    } else {                                                                   \
        printf(" FAILED.\n");                                                  \
    }                                                                          \
    return _exit_status

#define test_success() printf("."); fflush(stdout)

#define test_failure() printf("f"); fflush(stdout); _exit_status = EXIT_FAILURE

#define test_true(cond) if (!(cond)) _exit_status = EXIT_FAILURE

#endif
