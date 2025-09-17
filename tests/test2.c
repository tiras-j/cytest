#include <stdio.h>
#include "cytest.h"

// Copied over for unwrapping fixture
struct my_fixture {
    int data;
    char *str;
};

TEST_FN(second_file, fix_t fixture1, fix_t fix3) {
    struct my_fixture *f = unwrap_fixture(fix3, struct my_fixture *);
    printf("Second file test, using external fixtures\n");
    printf("fixture1: %s\tfix3.data: %d\tfix3.str: %s\n", 
            unwrap_fixture(fixture1, char *), f->data, f->str);
    return 0;
}
