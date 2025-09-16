#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "cytest.h"

#define MAX_TEST_FIXT 6

struct test {
    char *name;
    char *args;
    test_fn fn;
    int pipe[2];
    pid_t child;
};

struct fixture {
    char *name;
    fix_t (*init)(void);
    void (*fini)(fix_t);
};

static struct test tests[128] = { 0 };
static struct fixture fixtures[128] = { 0 };

static inline void _install_test_fn(test_fn f, char *name, char *args)
{
    struct test *p = &tests[0];
    while(p->name) p++;
    printf("installing test, name: %s\nargs: %s\n", name, args);
    p->name = name;
    p->args = args;
    p->fn = f;
}

static inline void _install_fixture(char *name, fix_t (*init)(void), void(*fini)(fix_t))
{
    printf("installing fixture: %s\n", name);
    struct fixture *p = &fixtures[0];
    while(p->name) p++;
    p->name = name;
    p->init = init;
    p->fini = fini;
}

FIXTURE(fixture2, printf("FIX2 CLEANING\n"))
{
    fix_t f = { "Boom Shakalaka" };
    return f;
}

FIXTURE(fixture1, printf("CLEANING\n"))
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

struct fixture _load_fixt(const char *name)
{
    printf("Loading fixture: %s\n", name);
    struct fixture *p = &fixtures[0];
    while(p->name) {
        if(!strcmp(name, p->name))
            return *p;
        p++;
    }
    fprintf(stderr, "[ERROR] could not load fixture: %s\n", name);
    exit(1);
}

#include <ctype.h>
static int _load_fixtures(struct fixture *fxs, const char *arg_str)
{
    int argc = 0;
    size_t len = strlen(arg_str);
    char namebuf[1024];
    const char *p = arg_str, *a;
    while((p = strstr(p, "fix_t")) != NULL) {
        p += strlen("fix_t");
        while(isspace(*p)) p++;
        a = memchr(p, ',', len - (p - arg_str));
        if(!a) {
            fxs[argc++] = _load_fixt(p);
        } else {
            memcpy(namebuf, p, a - p);
            namebuf[a - p] = 0;
            fxs[argc++] = _load_fixt(namebuf);
        }
    }

    return argc;
}

static int _do_test_call(test_fn fn, fix_t *args, int argc)
{
    switch(argc) {
        case 0: return fn.fn0();
        case 1: return fn.fn1(args[0]);
        case 2: return fn.fn2(args[0], args[1]);
        case 3: return fn.fn3(args[0], args[1], args[2]);
        case 4: return fn.fn4(args[0], args[1], args[2], args[3]);
        case 5: return fn.fn5(args[0], args[1], args[2], args[3], args[4]);
        case 6: return fn.fn6(args[0], args[1], args[2], args[3], args[4], args[5]);
        default:
            fprintf(stderr, "[ERROR] invalid arg count to do_test_call: %d\n", argc);
            exit(1);
    }
}

static int do_test(struct test *t, fix_t *args, int argc)
{
    pid_t pid;
    int so_pipe[2], si_pipe[2], se_pipe[2];
    pipe(so_pipe); pipe(si_pipe); pipe(se_pipe);
    pid = fork();
    if(pid < 0) {
        perror("fork");
        exit(1);
    }
    if(pid == 0) {
        // child
        close(2); dup2(se_pipe[1], 2);
        close(1); dup2(so_pipe[1], 1);
        close(0); dup2(si_pipe[0], 0);
        exit(_do_test_call(t->fn, args, argc));
    } else {
        char buf[1024] = {0};
        int status = 0;
        waitpid(pid, &status, 0);
        read(so_pipe[0], buf, 1024);
        printf("status: %d\n--------%s: stdout-------\n%s\n", status, t->name, buf);
        return status;
    } 
}

int main(int argc, char *argv[])
{
    fix_t fix_args[MAX_TEST_FIXT] = { 0 };
    struct fixture fix_objs[MAX_TEST_FIXT] = { 0 };

    struct test *runner = &tests[0];
    while(runner->name) {
        int cnt = _load_fixtures(fix_objs, runner->args);
        for(int i = 0; i < cnt; i++)
            fix_args[i] = fix_objs[i].init();
        
        //_do_test_call(runner->fn, fix_args, cnt);
        do_test(runner, fix_args, cnt);

        for(int i = 0; i < cnt; i++)
            fix_objs[i].fini(fix_args[i]);

        runner++;
    }
}
