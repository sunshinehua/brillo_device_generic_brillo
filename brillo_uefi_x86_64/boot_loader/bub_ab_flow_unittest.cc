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

#include <gtest/gtest.h>
#include "bub_sysdeps.h"
#include "bub_ab_flow.h"
#include "bub_util.h"

static int converted_utf8_ucs2(const char* data,
                               const char* raw_bytes,
                               size_t utf8_num_bytes) {
  int ret;
  size_t ucs2_num_bytes = sizeof(uint16_t) * utf8_num_bytes;
  uint16_t* test_str = (uint16_t*)bub_calloc(ucs2_num_bytes);
  if (test_str == NULL) {
    fprintf(stderr, "Bad bub_calloc.\n");
    return 1;
  }
  if (utf8_to_ucs2(reinterpret_cast<const uint8_t*>(data),
                   utf8_num_bytes, test_str, ucs2_num_bytes)) {
    bub_free(test_str);
    return 1;
  }
  ret = bub_memcmp(reinterpret_cast<const uint16_t*>(raw_bytes),
                   test_str, bub_strlen(raw_bytes) + 1);
  bub_free(test_str);
  return ret;
}

TEST(UtilTest, Utf8toUcs2) {
  // UTF-8 3 bytes encoding case.
  EXPECT_EQ(0, converted_utf8_ucs2("æ", "\xE6\x00", 3));
  // UTF-8 4 bytes encoding case.
  EXPECT_EQ(0, converted_utf8_ucs2("€", "\xAC\x20", 4));
  // UTF-8 multiple character case.
  EXPECT_EQ(0, converted_utf8_ucs2("AB", "\x41\x00\x42\x00", 6));

  // These should fail.
  // UTF-8 5 bytes encoding case.
  EXPECT_NE(0, converted_utf8_ucs2("👦", "\x66\xF4", 5));
}
