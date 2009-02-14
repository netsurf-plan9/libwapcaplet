/* libwapcaplet.h
 *
 * String internment and management tools.
 *
 * Copyright 2009 The NetSurf Browser Project.
 *		  Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#ifndef libwapcaplet_h_
#define libwapcaplet_h_

#include <sys/types.h>
#include <stdbool.h>

/**
 * Memory allocator type
 */
typedef void *(*lwc_allocator_fn)(void *ptr, size_t size, void *pw);

/**
 * A string internment context.
 */
typedef struct lwc_context_s lwc_context;

/**
 * An interned string.
 */
typedef struct lwc_string_s lwc_string;

/**
 * Error codes which libwapcaplet might return.
 */
typedef enum lwc_error_e {
	lwc_error_ok		= 0,
	lwc_error_oom		= 1,
        lwc_error_range		= 2,
} lwc_error;

/**
 * Create an internment context.
 *
 * This creates an internment context with a weak ref of one.
 *
 * You should reference the context with lwc_context_ref to convert
 * that to a strong reference when you decide where to store it.
 */
extern lwc_error lwc_create_context(lwc_allocator_fn alloc, void *pw,
				    lwc_context **ret);

/**
 * Increment the reference count for an internment context.
 */
extern lwc_context *lwc_context_ref(lwc_context *ctx);

/**
 * Decrement the reference count for an internment context.
 *
 * @note If the reference count reaches zero, the context will be freed.
 */
extern void lwc_context_unref(lwc_context *ctx);

/**
 * Intern a string.
 *
 * If the string was already present, its reference count is incremented.
 */
extern lwc_error lwc_context_intern(lwc_context *ctx,
				    const char *s, size_t slen,
				    lwc_string **ret);

/**
 * Intern a substring.
 */
extern lwc_error lwc_context_intern_substring(lwc_context *ctx,
					      lwc_string *str,
					      size_t ssoffset, size_t sslen,
					      lwc_string **ret);

/**
 * Increment the reference count on an lwc_string.
 *
 * Use this if copying the string and intending both sides to retain
 * ownership.
 */
extern lwc_string *lwc_context_string_ref(lwc_context *ctx, lwc_string *str);

/**
 * Release a reference on an lwc_string.
 *
 * If the reference count reaches zero then the string will be freed.
 */
extern void lwc_context_string_unref(lwc_context *ctx, lwc_string *str);

/**
 * Check if two interned strings are equal.
 *
 * @note The strings must be from the same intern context and that
 *	 must be the context passed in.
 */
#define lwc_context_string_isequal(ctx, str1, str2, ret) \
	(*ret = (str1 == str2)), lwc_error_ok

/**
 * Check if two interned strings are case-insensitively equal.
 *
 * @note The strings must be from the same intern context and that
 *	 must be the context passed in.
 */
extern lwc_error lwc_context_string_caseless_isequal(lwc_context *ctx,
                                                     lwc_string *str1,
                                                     lwc_string *str2,
                                                     bool *ret);

#endif /* libwapcaplet_h_ */
