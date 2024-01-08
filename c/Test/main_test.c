/* main_test.c */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"


extern int bouncer_run_tests(void);


int main() {
    // Exec. suites de tests.
    bouncer_run_tests();
}