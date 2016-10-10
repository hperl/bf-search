#include "test_driver.h"

#define add_test(test_name)                                                   \
        if (strcmp(argv[1], #test_name) == 0) {                               \
            return test_name();                                               \
        }


int
main(int argc, char** argv)
{
    if (argc != 2) {
        puts("run_test takes exactly one argument - the test you want to run.");
        return EXIT_FAILURE;
    }
    
    add_test(bloomfilter);
    add_test(tree);
    
    printf("Test %s not found.\n", argv[0]);
    return EXIT_FAILURE;
}
