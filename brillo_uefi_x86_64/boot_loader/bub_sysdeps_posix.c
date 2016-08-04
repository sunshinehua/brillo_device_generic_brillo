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

// NOTE: See avb_sysdeps_posix.c

#include <endian.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bub_sysdeps.h"

int bub_memcmp(const void* src1, const void* src2, size_t n) {
  return memcmp(src1, src2, n);
}

void* bub_memcpy(void* dest, const void* src, size_t n) {
  return memcpy(dest, src, n);
}

void* bub_memset(void* dest, const int c, size_t n) {
  return memset(dest, c, n);
}

int bub_strcmp(const char* s1, const char* s2) { return strcmp(s1, s2); }

size_t bub_strlen(const char* str) { return strlen(str); }

int bub_safe_memcmp(const void* s1, const void* s2, size_t n) {
  const unsigned char* us1 = s1;
  const unsigned char* us2 = s2;
  int result = 0;

  if (0 == n) return 0;

  /*
   * Code snippet without data-dependent branch due to Nate Lawson
   * (nate@root.org) of Root Labs.
   */
  while (n--) result |= *us1++ ^ *us2++;

  return result != 0;
}

void bub_abort(void) { abort(); }

void bub_print(const char* format) { fprintf(stderr, format); }

void* bub_malloc_(size_t size) { return malloc(size); }

void bub_free(void* ptr) { free(ptr); }