#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cytest.h"

FIXTURE(fixture2, (void)_F; printf("FIX2 CLEANING\n"))
{
    fix_t f = { "Boom Shakalaka" };
    return f;
}

FIXTURE(fixture1, (void)_F; printf("CLEANING\n"))
{
    fix_t f = { "Hello World!" };
    return f;
}


struct my_fixture {
    int data;
    char *str;
};

void my_fixture_free(struct my_fixture *mf)
{
    printf("Cleaning up my_fixture\n");
    if(mf->str)
        free(mf->str);
    free(mf);
}

FIXTURE(fix3, my_fixture_free(unwrap_fixture(_F, struct my_fixture *)))
{
    struct my_fixture *mf = malloc(sizeof(*mf));
    mf->str = strdup("blah, blah, blah");
    mf->data = 42;
    fix_t f = { mf };
    return f;
}

TEST_FN(mytest, fix_t fixture1, fix_t fixture2)
{
    printf("hello from mytest\n");
    printf("fix1: %s\tfix2: %s\n", unwrap_fixture(fixture1, char *), unwrap_fixture(fixture2, char *));
    return 0;
}

TEST_FN(test2, fix_t fix3)
{
    struct my_fixture *mf = unwrap_fixture(fix3, struct my_fixture *);
    printf("Hello from test2 (fix3.data: %d fix3.str: %s)\n", mf->data, mf->str);
    return 0;
}

