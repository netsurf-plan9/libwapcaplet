/* test/basictests.c
 *
 * Basic tests for the test suite for libwapcaplet
 *
 * Copyright 2009 The NetSurf Browser Project
 *                Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "tests.h"

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

static void *last_pw = NULL;
static void *
trivial_alloc_fn(void *p, size_t s, void *pw)
{
        last_pw = pw;
        return realloc(p, s);
}

#ifndef NDEBUG
/* All the basic assert() tests */
START_TEST (test_lwc_intern_string_aborts1)
{
        lwc_intern_string(NULL, 0, NULL);
}
END_TEST

START_TEST (test_lwc_intern_string_aborts2)
{
        lwc_intern_string("A", 1, NULL);
}
END_TEST

START_TEST (test_lwc_intern_substring_aborts1)
{
        lwc_intern_substring(NULL, 0, 0, NULL);
}
END_TEST

START_TEST (test_lwc_intern_substring_aborts2)
{
        lwc_string *str;
        fail_unless(lwc_initialise(trivial_alloc_fn, NULL, 0) == lwc_error_ok,
                    "unable to initialise the library");
        fail_unless(lwc_intern_string("Jam", 3, &str) == lwc_error_ok,
                    "unable to intern 'Jam'");
        
        lwc_intern_substring(str, 88, 77, NULL);
}
END_TEST

START_TEST (test_lwc_string_ref_aborts)
{
        lwc_string_ref(NULL);
}
END_TEST

START_TEST (test_lwc_string_unref_aborts)
{
        lwc_string_unref(NULL);
}
END_TEST

START_TEST (test_lwc_string_data_aborts)
{
        lwc_string_data(NULL);
}
END_TEST

START_TEST (test_lwc_string_length_aborts)
{
        lwc_string_length(NULL);
}
END_TEST

START_TEST (test_lwc_string_hash_value_aborts)
{
        lwc_string_hash_value(NULL);
}
END_TEST

#endif

START_TEST (test_lwc_double_initialise_fails)
{
        fail_unless(lwc_initialise(trivial_alloc_fn, NULL, 0) == lwc_error_ok,
                    "Unable to initialise library");
        fail_unless(lwc_initialise(trivial_alloc_fn, NULL, 0) == lwc_error_initialised,
                    "Able to initialise library a second time");
}
END_TEST

static void *enomem_allocator(void *ptr, size_t n, void *pw)
{
        int *pi = (int*)pw;
        UNUSED(ptr);
        UNUSED(n);
        
        if (*pi > 0) {
                *pi -= 1;
                return realloc(ptr, n);
        }
        
        return NULL;
}

START_TEST (test_lwc_initialise_fails_with_no_memory)
{
        int permitted = 0;
        fail_unless(lwc_initialise(enomem_allocator, &permitted, 0) == lwc_error_oom,
                    "Able to initialise library with no memory available?!");
}
END_TEST

START_TEST (test_lwc_initialise_fails_with_low_memory)
{
        int permitted = 1;
        fail_unless(lwc_initialise(enomem_allocator, &permitted, 0) == lwc_error_oom,
                    "Able to initialise library with no memory available?!");
}
END_TEST

START_TEST (test_lwc_intern_fails_with_no_memory)
{
        int permitted = 2; /* context and buckets */
        lwc_string *str;
        
        fail_unless(lwc_initialise(enomem_allocator, &permitted, 0) == lwc_error_ok,
                    "Unable to initialise library");
        fail_unless(lwc_intern_string("Hello", 5, &str) == lwc_error_oom,
                    "Able to allocate string despite enomem.");
        
}
END_TEST

START_TEST (test_lwc_caseless_compare_fails_with_no_memory1)
{
        int permitted = 3; /* ctx, buckets, 1 string */
        lwc_string *str;
        bool result = true;
        
        fail_unless(lwc_initialise(enomem_allocator, &permitted, 0) == lwc_error_ok,
                    "Unable to initialise library");
        fail_unless(lwc_intern_string("Hello", 5, &str) == lwc_error_ok,
                    "Unable to allocate string.");
        fail_unless(lwc_string_caseless_isequal(str, str, &result) == lwc_error_oom,
                    "Able to caselessly compare despite no memory");
        
}
END_TEST

START_TEST (test_lwc_caseless_compare_fails_with_no_memory2)
{
        int permitted = 5; /* ctx, buckets, 3 strings */
        lwc_string *str1, *str2;
        bool result = true;
        
        fail_unless(lwc_initialise(enomem_allocator, &permitted, 0) == lwc_error_ok,
                    "Unable to initialise library");
        fail_unless(lwc_intern_string("Hello", 5, &str1) == lwc_error_ok,
                    "Unable to allocate string.");
        fail_unless(lwc_intern_string("World", 5, &str2) == lwc_error_ok,
                    "Unable to allocate string.");
        fail_unless(lwc_string_caseless_isequal(str1, str2, &result) == lwc_error_oom,
                    "Able to caselessly compare despite no memory");
        
}
END_TEST

/**** The next set of tests need a fixture set ****/

static void
with_simple_context_setup(void)
{
        fail_unless(lwc_initialise(trivial_alloc_fn, NULL, 0) == lwc_error_ok,
                    "Unable to initialise library");
}

static void
with_simple_context_teardown(void)
{
        /* Nothing to do to tear down */
}

START_TEST (test_lwc_intern_string_ok)
{
        lwc_string *str = NULL;
        fail_unless(lwc_intern_string("A", 1, &str) == lwc_error_ok,
                    "Unable to intern a simple string");
        fail_unless(str != NULL,
                    "Returned OK but str was still NULL");
}
END_TEST

START_TEST (test_lwc_intern_string_twice_ok)
{
        lwc_string *str1 = NULL, *str2 = NULL;
        fail_unless(lwc_intern_string("A", 1, &str1) == lwc_error_ok,
                    "Unable to intern a simple string");
        fail_unless(str1 != NULL,
                    "Returned OK but str was still NULL");
        fail_unless(lwc_intern_string("B", 1, &str2) == lwc_error_ok,
                    "Unable to intern a simple string");
        fail_unless(str2 != NULL,
                    "Returned OK but str was still NULL");
}
END_TEST

START_TEST (test_lwc_intern_string_twice_same_ok)
{
        lwc_string *str1 = NULL, *str2 = NULL;
        fail_unless(lwc_intern_string("A", 1, &str1) == lwc_error_ok,
                    "Unable to intern a simple string");
        fail_unless(str1 != NULL,
                    "Returned OK but str was still NULL");
        fail_unless(lwc_intern_string("A", 1, &str2) == lwc_error_ok,
                    "Unable to intern a simple string");
        fail_unless(str2 != NULL,
                    "Returned OK but str was still NULL");
}
END_TEST

/**** The next set of tests need a fixture set with some strings ****/

static lwc_string *intern_one = NULL, *intern_two = NULL, *intern_three = NULL, *intern_YAY = NULL;

static void
with_filled_context_setup(void)
{
        fail_unless(lwc_initialise(trivial_alloc_fn, NULL, 2) == lwc_error_ok,
                    "Unable to initialise library");
        
        fail_unless(lwc_intern_string("one", 3, &intern_one) == lwc_error_ok,
                    "Unable to intern 'one'");
        fail_unless(lwc_intern_string("two", 3, &intern_two) == lwc_error_ok,
                    "Unable to intern 'two'");
        fail_unless(lwc_intern_string("three", 5, &intern_three) == lwc_error_ok,
                    "Unable to intern 'three'");
        fail_unless(lwc_intern_string("YAY", 3, &intern_YAY) == lwc_error_ok,
                    "Unable to intern 'YAY'");
        
        fail_unless(intern_one != intern_two, "'one' == 'two'");
        fail_unless(intern_one != intern_three, "'one' == 'three'");
        fail_unless(intern_two != intern_three, "'two' == 'three'");
}

static void
with_filled_context_teardown(void)
{
        lwc_string_unref(intern_one);
        lwc_string_unref(intern_two);
        lwc_string_unref(intern_three);
        lwc_string_unref(intern_YAY);
}

START_TEST (test_lwc_interning_works)
{
        lwc_string *new_one = NULL;
        
        fail_unless(lwc_intern_string("one", 3, &new_one) == lwc_error_ok,
                    "Unable to re-intern 'one'");
        
        fail_unless(new_one == intern_one,
                    "Internalising of the string failed");
}
END_TEST

START_TEST (test_lwc_intern_substring)
{
        lwc_string *new_hre = NULL, *sub_hre = NULL;
        
        fail_unless(lwc_intern_string("hre", 3, &new_hre) == lwc_error_ok,
                    "Unable to intern 'hre'");
        fail_unless(lwc_intern_substring(intern_three,
                                         1, 3, &sub_hre) == lwc_error_ok,
                    "Unable to re-intern 'hre' by substring");
        fail_unless(new_hre == sub_hre,
                    "'hre' != 'hre' -- wow!");
}
END_TEST

START_TEST (test_lwc_intern_substring_bad_offset)
{
        lwc_string *str;
        
        fail_unless(lwc_intern_substring(intern_three, 100, 1, &str) == lwc_error_range,
                    "Able to intern substring starting out of range");
}
END_TEST

START_TEST (test_lwc_intern_substring_bad_size)
{
        lwc_string *str;
        
        fail_unless(lwc_intern_substring(intern_three, 1, 100, &str) == lwc_error_range,
                    "Able to intern substring ending out of range");
}
END_TEST

START_TEST (test_lwc_string_ref_ok)
{
        fail_unless(lwc_string_ref(intern_one) == intern_one,
                    "Oddly, reffing a string didn't return it");
}
END_TEST

START_TEST (test_lwc_string_unref_ok)
{
        lwc_string_unref(intern_one);
}
END_TEST

START_TEST (test_lwc_string_ref_unref_ok)
{
        lwc_string_ref(intern_one);
        lwc_string_unref(intern_one);
}
END_TEST

START_TEST (test_lwc_string_isequal_ok)
{
        bool result = true;
        fail_unless((lwc_string_isequal(intern_one, intern_two, &result)) == lwc_error_ok,
                    "Failure comparing 'one' and 'two'");
        fail_unless(result == false,
                    "'one' == 'two' ?!");
}
END_TEST

START_TEST (test_lwc_string_caseless_isequal_ok1)
{
        bool result = true;
        lwc_string *new_ONE;
        
        fail_unless(lwc_intern_string("ONE", 3, &new_ONE) == lwc_error_ok,
                    "Failure interning 'ONE'");
        
        fail_unless((lwc_string_isequal(intern_one, new_ONE, &result)) == lwc_error_ok);
        fail_unless(result == false,
                    "'one' == 'ONE' ?!");
        
        fail_unless((lwc_string_caseless_isequal(intern_one, new_ONE, &result)) == lwc_error_ok,
                    "Failure comparing 'one' and 'ONE' caselessly");
        fail_unless(result == true,
                    "'one' !~= 'ONE' ?!");
}
END_TEST

START_TEST (test_lwc_string_caseless_isequal_ok2)
{
        bool result = true;
        lwc_string *new_yay;
        
        fail_unless(lwc_intern_string("yay", 3, &new_yay) == lwc_error_ok,
                    "Failure interning 'yay'");
        
        fail_unless((lwc_string_isequal(intern_YAY, new_yay, &result)) == lwc_error_ok);
        fail_unless(result == false,
                    "'yay' == 'YAY' ?!");
        
        fail_unless((lwc_string_caseless_isequal(intern_YAY, new_yay, &result)) == lwc_error_ok,
                    "Failure comparing 'yay' and 'YAY' caselessly");
        fail_unless(result == true,
                    "'yay' !~= 'YAY' ?!");
}
END_TEST

START_TEST (test_lwc_string_caseless_isequal_bad)
{
        bool result = true;
        
        fail_unless(lwc_string_caseless_isequal(intern_YAY, intern_one, &result) == lwc_error_ok,
                    "Failure comparing 'YAY' and 'one' caselessly");
        fail_unless(result == false,
                    "'YAY' ~= 'one' ?!");
}
END_TEST

START_TEST (test_lwc_extract_data_ok)
{
        fail_unless(memcmp("one",
                           lwc_string_data(intern_one),
                           lwc_string_length(intern_one)) == 0,
                    "Extracting data ptr etc failed");
}
END_TEST

START_TEST (test_lwc_string_hash_value_ok)
{
        lwc_string_hash_value(intern_one);
}
END_TEST

START_TEST (test_lwc_string_is_nul_terminated)
{
        lwc_string *new_ONE;

        fail_unless(lwc_intern_string("ONE", 3, &new_ONE) == lwc_error_ok,
                    "Failure interning 'ONE'");

        fail_unless(lwc_string_data(new_ONE)[lwc_string_length(new_ONE)] == '\0',
                    "Interned string isn't NUL terminated");
}
END_TEST

START_TEST (test_lwc_substring_is_nul_terminated)
{
        lwc_string *new_ONE;
        lwc_string *new_O;

        fail_unless(lwc_intern_string("ONE", 3, &new_ONE) == lwc_error_ok,
                    "Failure interning 'ONE'");

        fail_unless(lwc_intern_substring(new_ONE, 0, 1, &new_O) == lwc_error_ok,
                    "Failure interning substring 'O'");

        fail_unless(lwc_string_data(new_O)[lwc_string_length(new_O)] == '\0',
                    "Interned substring isn't NUL terminated");
}
END_TEST

static void
counting_cb(lwc_string *str, void *pw)
{
        UNUSED(str);
        
        *((int *)pw) += 1;
}

START_TEST (test_lwc_string_iteration)
{
        int counter = 0;
        lwc_iterate_strings(counting_cb, (void*)&counter);
        fail_unless(counter == 4, "Incorrect string count");
}
END_TEST

/**** And the suites are set up here ****/

void
lwc_basic_suite(SRunner *sr)
{
        Suite *s = suite_create("libwapcaplet: Basic tests");
        TCase *tc_basic = tcase_create("Creation/Destruction");
        
#ifndef NDEBUG
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_intern_string_aborts1,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_intern_string_aborts2,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_intern_substring_aborts1,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_intern_substring_aborts2,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_string_ref_aborts,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_string_unref_aborts,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_string_data_aborts,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_string_length_aborts,
                                    SIGABRT);
        tcase_add_test_raise_signal(tc_basic,
                                    test_lwc_string_hash_value_aborts,
                                    SIGABRT);
#endif
        
        tcase_add_test(tc_basic, test_lwc_double_initialise_fails);
        tcase_add_test(tc_basic, test_lwc_initialise_fails_with_no_memory);
        tcase_add_test(tc_basic, test_lwc_initialise_fails_with_low_memory);
        tcase_add_test(tc_basic, test_lwc_intern_fails_with_no_memory);
        tcase_add_test(tc_basic, test_lwc_caseless_compare_fails_with_no_memory1);
        tcase_add_test(tc_basic, test_lwc_caseless_compare_fails_with_no_memory2);
        
        suite_add_tcase(s, tc_basic);
        
        tc_basic = tcase_create("Ops with a context");
        
        tcase_add_checked_fixture(tc_basic, with_simple_context_setup,
                                  with_simple_context_teardown);
        tcase_add_test(tc_basic, test_lwc_intern_string_ok);
        tcase_add_test(tc_basic, test_lwc_intern_string_twice_ok);
        tcase_add_test(tc_basic, test_lwc_intern_string_twice_same_ok);
        suite_add_tcase(s, tc_basic);
        
        tc_basic = tcase_create("Ops with a filled context");
        
        tcase_add_checked_fixture(tc_basic, with_filled_context_setup,
                                  with_filled_context_teardown);
        tcase_add_test(tc_basic, test_lwc_interning_works);
        tcase_add_test(tc_basic, test_lwc_intern_substring);
        tcase_add_test(tc_basic, test_lwc_string_ref_ok);
        tcase_add_test(tc_basic, test_lwc_string_ref_unref_ok);
        tcase_add_test(tc_basic, test_lwc_string_unref_ok);
        tcase_add_test(tc_basic, test_lwc_string_isequal_ok);
        tcase_add_test(tc_basic, test_lwc_string_caseless_isequal_ok1);
        tcase_add_test(tc_basic, test_lwc_string_caseless_isequal_ok2);
        tcase_add_test(tc_basic, test_lwc_string_caseless_isequal_bad);
        tcase_add_test(tc_basic, test_lwc_extract_data_ok);
        tcase_add_test(tc_basic, test_lwc_string_hash_value_ok);
        tcase_add_test(tc_basic, test_lwc_string_is_nul_terminated);
        tcase_add_test(tc_basic, test_lwc_substring_is_nul_terminated);
        tcase_add_test(tc_basic, test_lwc_intern_substring_bad_size);
        tcase_add_test(tc_basic, test_lwc_intern_substring_bad_offset);
        tcase_add_test(tc_basic, test_lwc_string_iteration);
        suite_add_tcase(s, tc_basic);
        
        srunner_add_suite(sr, s);
}
