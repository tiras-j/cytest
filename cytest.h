
/* TEST_FN
 *
 * USE: TEST_FN(<test_name>, fix_t fix1name, fix_t fix2name...) {
 *      <test code>
 *      return <integer exit code>
 * }
 *
 * This macro creates a new test and allows providing test fixtures
 * as arguments (current max: 6). Each fixture name must match a
 * define FIXTURE() declaration. When the test is invoked, each fixture
 * is created and passed in as arguments to the function.
 *
 * RETURN(int): 0 on success, or error code on failure
 */
#define TEST_FN(_test, _fixtures...)                        \
int _test(_fixtures);                                       \
__attribute__((constructor))                                \
void testcons_##_test(void) {                               \
    _install_test_fn((test_fn)_test, __FILE__, #_test, #_fixtures); } \
int _test(_fixtures)

/* FIXTURE
 *
 * USE: FIXTURE(<fixture_name>, <cleanup expression>) {
 *      fix_t f = { <my initialized pointer or data> }
 *      return f;
 * }
 *
 * This macro creates a fixture object named <fixture_name>
 * which may be provided to TEST_FN macro to pass as arguments
 * to a test function. The cleanup expression is any expression
 * on the object `fix_t _F` which is provided to the cleanup scope.
 *
 * RETURN(fix_t): The constructed fix_t object
 *
 * EXAMPLE:
 * struct my_data { int num; char *str };
 * void my_free(struct my_data *md) { free(md); }
 * FIXTURE(my_fix, my_free(unwrap_fixture(_F, struct my_data *)) {
 *      my_data *m = malloc(sizeof(*m));
 *      m->data = 10; m->str = "Hello world!";
 *      fix_t f = { m };
 *      return f;
 * }
 */
#define FIXTURE(_name, _cleanup)                            \
fix_t _name(void);                                          \
void fixclean_##_name(fix_t _F) {  _cleanup; }              \
__attribute__((constructor))                                \
void fixcons_##_name(void) {                                \
    _install_fixture(#_name, _name, fixclean_##_name); }    \
fix_t _name(void)

// Helper to convert fix_t to your pointer type of choice
#define unwrap_fixture(_f, _type) (_type)(_f.__inner)

// Necessary shared defs
typedef struct { void *__inner; } fix_t;
typedef union test_fn {
    int (*fn0)(void);
    int (*fn1)(fix_t);
    int (*fn2)(fix_t, fix_t);
    int (*fn3)(fix_t, fix_t, fix_t);
    int (*fn4)(fix_t, fix_t, fix_t, fix_t);
    int (*fn5)(fix_t, fix_t, fix_t, fix_t, fix_t);
    int (*fn6)(fix_t, fix_t, fix_t, fix_t, fix_t, fix_t);
} test_fn;
// Forward decls of private structs
struct test;
struct fixture;
extern void _install_test_fn(test_fn, char *, char *, char *);
extern void _install_fixture(char *, fix_t (*)(void), void(*)(fix_t));

