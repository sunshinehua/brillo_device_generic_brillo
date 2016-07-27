/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// NOTE: See avb_sysdeps.h

// #if !defined(BUB_INSIDE_LIBBUB_H) && !defined(BUB_COMPILATION)
// #error "Never include this file directly, include libbub.h instead."
// #endif

#ifndef BUB_SYSDEPS_H_
#define BUB_SYSDEPS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Change these includes to match your platform to bring in the
 * equivalent types available in a normal C runtime, as well as
 * printf()-format specifiers such as PRIx64.
 */
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include "bub_util.h"

/* If you don't have gcc or clang, these attribute macros may need to
 * be adjusted.
 */
#define BUB_ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define BUB_ATTR_PACKED __attribute__((packed))
#define BUB_ATTR_FORMAT_PRINTF(format_idx, arg_idx) \
  __attribute__((format(printf, format_idx, arg_idx)))
#define BUB_ATTR_NO_RETURN __attribute__((noreturn))

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define WFILE WIDEN(__FILE__)

#ifdef BUB_ENABLE_DEBUG
/* Aborts the program if |expr| is false.
 *
 * This has no effect unless BUB_ENABLE_DEBUG is defined.
 */
#define bub_assert(expr)            \
  do {                              \
    if (!(expr)) {                  \
      bub_error("assert fail: \n"); \
    }                               \
  } while (0)
#else
#define bub_assert(expr)
#endif /* BUB_ENABLE_DEBUG */

/* Size in bytes used for word-alignment.
 *
 * Change this to match your architecture - must be a power of two.
 */
#define BUB_WORD_ALIGNMENT_SIZE 8

/* Aborts the program if |addr| is not word-aligned.
 *
 * This has no effect unless BUB_ENABLE_DEBUG is defined.
 */
#define bub_assert_word_aligned(addr) \
//   bub_assert((((uintptr_t)addr) & (BUB_WORD_ALIGNMENT_SIZE - 1)) == 0)

/* Compare |n| bytes in |src1| and |src2|.
 *
 * Returns an integer less than, equal to, or greater than zero if the
 * first |n| bytes of |src1| is found, respectively, to be less than,
 * to match, or be greater than the first |n| bytes of |src2|. */
int bub_memcmp(const void* src1, const void* src2,
               size_t n) BUB_ATTR_WARN_UNUSED_RESULT;

/* Compare two strings.
 *
 * Return an integer less than, equal to, or greater than zero if |s1|
 * is found, respectively, to be less than, to match, or be greater
 * than |s2|.
 */
int bub_strcmp(const char* s1, const char* s2);

/* Copy |n| bytes from |src| to |dest|. */
void* bub_memcpy(void* dest, const void* src, size_t n);

/* Set |n| bytes starting at |s| to |c|.  Returns |dest|. */
void* bub_memset(void* dest, const int c, size_t n);

/* Compare |n| bytes starting at |s1| with |s2| and return 0 if they
 * match, 1 if they don't.  Returns 0 if |n|==0, since no bytes
 * mismatched.
 *
 * Time taken to perform the comparison is only dependent on |n| and
 * not on the relationship of the match between |s1| and |s2|.
 *
 * Note that unlike bub_memcmp(), this only indicates inequality, not
 * whether |s1| is less than or greater than |s2|.
 */
int bub_safe_memcmp(const void* s1, const void* s2,
                    size_t n) BUB_ATTR_WARN_UNUSED_RESULT;

#ifdef BUB_ENABLE_DEBUG
/* printf()-style function, used for diagnostics.
 *
 * This has no effect unless BUB_ENABLE_DEBUG is defined.
 */
#define bub_debug(format)        \
  do {                           \
    bub_print("DEBUG: " format); \
  } while (0)
#else
#define bub_debug(format)
#endif /* BUB_ENABLE_DEBUG */

/* Prints out a message (defined by |format|, printf()-style).
 */
void bub_print(const char* format);

/* Prints out a message (defined by |format|, printf()-style). This is
 * typically used if a runtime-error occurs.
 */
#define bub_warning(format)        \
  do {                             \
    bub_print("WARNING: " format); \
  } while (0)


/* Prints out a message (defined by |format|, printf()-style) and
 * calls bub_abort().
 */
#define bub_error(format)        \
  do {                           \
    bub_print("ERROR: " format); \
    bub_abort();                 \
  } while (0)

/* Aborts the program or reboots the device. */
void bub_abort(void) BUB_ATTR_NO_RETURN;

/* Allocates |size| bytes. Returns NULL if no memory is available,
 * otherwise a pointer to the allocated memory.
 *
 * The memory is not initialized.
 *
 * The pointer returned is guaranteed to be word-aligned.
 *
 * The memory should be freed with bub_free() when you are done with it.
 */
void* bub_malloc_(size_t size) BUB_ATTR_WARN_UNUSED_RESULT;

/* Frees memory previously allocated with bub_malloc(). */
void bub_free(void* ptr);

/* Returns the lenght of |str|, excluding the terminating NUL-byte. */
size_t bub_strlen(const char* str) BUB_ATTR_WARN_UNUSED_RESULT;

#ifdef __cplusplus
}
#endif

#endif /* BUB_SYSDEPS_H_ */