/* test/basictests.c
 *
 * Basic tests for the test suite for libwapcaplet
 *
 * Copyright 2009 The NetSurf Browser Project
 *                Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <check.h>
#include <stdlib.h>

#include "tests.h"

START_TEST (test_lwc_context_creation_bad_alloc)
{
        lwc_context *ctx = NULL;
        lwc_error err;
        
        err = lwc_create_context(NULL, NULL, &ctx);
}
END_TEST



void
lwc_basic_suite(SRunner *sr)
{
        Suite *s = suite_create("libwapcaplet: Basic tests");
        TCase *tc_basic = tcase_create("Creation/Destruction");
        
        tcase_add_test_raise_signal(tc_basic, test_lwc_context_creation_bad_alloc, SIGABRT);
        
        suite_add_tcase(s, tc_basic);
        srunner_add_suite(sr, s);
}
