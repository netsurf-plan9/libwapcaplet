/* test/testmain.c
 *
 * Core of the test suite for libwapcaplet
 *
 * Copyright 2009 The NetSurf Browser Project
 *                Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <check.h>
#include <stdlib.h>

#include "tests.h"

/* This means that assertion failures are silent in tests */
void __assert_fail(void) { abort(); }

int
main(int argc, char **argv)
{
        int number_failed = 0;
        SRunner *sr;
        
        sr = srunner_create(suite_create("Test suite for libwapcaplet"));
        
        lwc_basic_suite(sr);
        
        srunner_set_fork_status(sr, CK_FORK);
        srunner_run_all(sr, CK_ENV);
        number_failed = srunner_ntests_failed(sr);
        
        srunner_free(sr);
        
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
