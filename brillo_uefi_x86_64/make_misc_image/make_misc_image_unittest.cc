/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "make_misc_image.h"

#define test_args(expected_ret, dat)                                           \
  do {                                                                         \
    BubSlotData metadata[2];                                                   \
    base::FilePath misc_name;                                                  \
    EXPECT_EQ(expected_ret,                                                    \
              parse_command_line_args(3,                                       \
                (const char *[]){"make_misc_image", dat, "--output=misc.img"}, \
                metadata, &misc_name));                                        \
  } while (0)


TEST_F(AbTest, ActiveAndBackup) {
  // EXPECT_EQ(1, init_misc(15, 0, 1, 14, 0, 1));

  test_args(
  1,                                  // Expected return value.
  "--ab_metadata=15,0,1,9,0,1");      // Metadata arg.
}

TEST_F(AbTest, TwoUpdating) {
  test_args(
  1,                                  // Expected return value.
  "--ab_metadata=15,7,0,15,7,0");     // Metadata arg.
}

TEST_F(AbTest, ActiveAndInvalid) {
  test_args(
  1,                                  // Expected return value.
  "--ab_metadata=15,0,1,0,0,0");      // Metadata arg.
}

TEST_F(AbTest, TwoInvalid) {
  test_args(
  1,                                  // Expected return value.
  "--ab_metadata=0,0,0,0,0,0");       // Metadata arg.
}

TEST_F(AbTest, BadPriority) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=20,0,1,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=20,0,1,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,20,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=20,0,1,20,0,1");     // Metadata arg.
}

TEST_F(AbTest, BadTriesRemaining) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=20,0,1,20,0,1");     // Metadata arg.

  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,8,1,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,8,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,8,1,14,8,1");     // Metadata arg.
}

TEST_F(AbTest, BadSuccessfulBoot) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,2,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,0,2");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,2,14,0,2");     // Metadata arg.
}

TEST_F(AbTest, OutOfRangePositiveAttribute) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=100,0,1,14,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,100,1,14,0,1");   // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,100,14,0,1");   // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,100,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,100,1");   // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,0,100");   // Metadata arg.
}

TEST_F(AbTest, OutOfRangeNegativeAttribute) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=-1,0,1,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,-1,1,14,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,-1,14,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,-1,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,-1,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,0,-1");    // Metadata arg.
}

TEST_F(AbTest, NonNumericAtrribute) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=1A,0,1,14,0,1");     // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,A1,1,14,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,AA,14,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,a1a,0,1");    // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,3.14,1");  // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,0,.14");   // Metadata arg.
}

TEST_F(AbTest, MissingAttributes) {
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,,1,14,0,1");      // Metadata arg.
  test_args(
  0,                                  // Expected return value.
  "--ab_metadata=15,0,1,14,0");       // Metadata arg.
}

TEST_F(AbTest, MissingOutput) {
  BubSlotData metadata[2];
  base::FilePath misc_name;
  EXPECT_EQ(0,  //Expected return value.
            parse_command_line_args(2,
              (const char *[]){"make_misc_image",
                               "--ab_metadata=15,0,1,14,0,1"},
              metadata, &misc_name));
  EXPECT_EQ(0,  //Expected return value.
            parse_command_line_args(2,
              (const char *[]){"make_misc_image",
                               "--ab_metadata=15,0,1,14,0,1",
                               "--out=misc.img"},
              metadata, &misc_name));
}

