#include "test_driver.h"

void
test_set_zero()
{
    Bloomfilter bf;
    bf_set_zero(&bf);
    for (int i = 0; i < BF_SIZE; i++) {
        test_true(bf_getpos(&bf, i) == 0);
    }
}

void
test_get_set_pos()
{
    Bloomfilter bf;
    for (int i = 0; i < BF_SIZE; i++) {
        bf_setpos(&bf, i);
        test_true(bf_getpos(&bf, i) == 1);
        bf_clearpos(&bf, i);
        test_true(bf_getpos(&bf, i) == 0);
    }
}

void
test_addbf_and_included()
{
    Bloomfilter a, b, c;
    int IND_SIZE = 16;
    int indices[IND_SIZE];
    for (int run = 0; run < RUNS; run++) {
        bf_set_zero(&a);
        bf_set_zero(&b);
        bf_set_zero(&c);
        for (int i = 0; i < IND_SIZE; i+=2) {
            indices[i]   = rand() % BF_SIZE;
            indices[i+1] = rand() % BF_SIZE;
            
            bf_setpos(&a, indices[i]);
            bf_setpos(&b, indices[i+1]);
        }
        bf_addbf(&c, &a, &b);
        for (int i = 0; i < IND_SIZE; i++) {
            if (!(bf_getpos(&c, indices[i]))) {
                char* str = NULL;
                str = bf_to_str(&a, str); puts(str);
                str = bf_to_str(&b, str); puts(str);
                str = bf_to_str(&c, str); puts(str);
                free(str);
                test_failure();
            }
        }
        // a should be in c
        test_true(bf_included_in(&a, &c, 0));
        // b should be in c
        test_true(bf_included_in(&b, &c, 0));
        // c should not be in a or b
        test_true(!bf_included_in(&c, &a, 0));
        test_true(!bf_included_in(&c, &b, 0));
        // however with tolerance IND_SIZE/2 c should be in a or b
        test_true(bf_included_in(&c, &a, IND_SIZE/2));
        test_true(bf_included_in(&c, &b, IND_SIZE/2));
    }

}


int
bloomfilter()
{
    test_setup();
    
    test_set_zero();
    test_get_set_pos();
    test_addbf_and_included();
    
    test_exit();
}
