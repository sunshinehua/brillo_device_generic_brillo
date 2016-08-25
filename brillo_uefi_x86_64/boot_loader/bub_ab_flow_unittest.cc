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

#include "bub_image_util.h"

#define ab_init(magic,                                                        \
                a_priority, a_tries_remaining, a_successful_boot,             \
                b_priority, b_tries_remaining, b_successful_boot)             \
  do {                                                                        \
    BubAbData init;                                                           \
    ops_.write_ab_metadata(&init, (uint8_t[4])magic,                          \
                           a_priority, a_tries_remaining, a_successful_boot,  \
                           b_priority, b_tries_remaining, b_successful_boot); \
    GenerateMiscImage(&init);                                                 \
  } while(0)

#define test_ab_flow(a_priority, a_tries_remaining, a_successful_boot,        \
                     b_priority, b_tries_remaining, b_successful_boot,        \
                     expected_result, expected_suffix)                        \
  do {                                                                        \
    char suffix[BUB_SUFFIX_SIZE] = {0};                                       \
    BubAbData ab_result;                                                      \
    ops_.write_ab_metadata(&ab_result, (uint8_t[4])BUB_BOOT_CTRL_MAGIC,       \
                           a_priority, a_tries_remaining, a_successful_boot,  \
                           b_priority, b_tries_remaining, b_successful_boot); \
    EXPECT_EQ(expected_result,                                                \
              bub_ab_flow((BubOps*)ops_.bub_ops(), suffix, BUB_SUFFIX_SIZE)); \
    EXPECT_EQ(0, bub_memcmp(expected_suffix, suffix, BUB_SUFFIX_SIZE));       \
    EXPECT_EQ(0, CompareMiscImage(ab_result));                                \
  } while (0)

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

TEST_F(AbTest, NoValidSlots) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 0, 0, 0, 0, 0, 0);

  test_ab_flow(
    0, 0, 0, 0, 0, 0,                   // Expected A/B state.
    BUB_AB_FLOW_ERROR_NO_VALID_SLOTS,   // Expected A/B result.
    "\0\0");                            // Expected A/B suffix.
}

TEST_F(AbTest, InvalidMetadataMagicInvalidSlots) {
  char suffix[BUB_SUFFIX_SIZE] = {0};
  BubAbData ab_init;
  BubAbData ab_result;

  ops_.write_ab_metadata(&ab_init, (uint8_t[4]){'N','O','P','E'},
                         0, 0, 0, 0, 0, 0);
  ops_.write_ab_metadata(&ab_result, (uint8_t[4])BUB_BOOT_CTRL_MAGIC,
                         15, 7, 0, 15, 7, 0);
  GenerateMiscImage(&ab_init);

  // Invalid metadata should be found by ab flow at this point. It should reset
  // each slot to an 'updating' state on disk so that we may reattempt boot.
  EXPECT_EQ(BUB_AB_FLOW_ERROR_INVALID_METADATA,
            bub_ab_flow((BubOps*)ops_.bub_ops(), suffix, BUB_SUFFIX_SIZE));
  EXPECT_EQ(0, bub_memcmp((char[BUB_SUFFIX_SIZE]){0,0,0},
                          suffix,
                          BUB_SUFFIX_SIZE));
  EXPECT_EQ(0, CompareMiscImage(ab_result));

  // Run again to check slot A is selected with one less try remaining.
  test_ab_flow(
    15, 6, 0, 15, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");
}

TEST_F(AbTest, InvalidMetadataMagicValidSlots) {
  char suffix[BUB_SUFFIX_SIZE] = {0};
  BubAbData ab_init;
  BubAbData ab_result;

  ops_.write_ab_metadata(&ab_init, (uint8_t[4]){'N','O','P','E'},
                         15, 0, 1, 14, 0, 1);
  ops_.write_ab_metadata(&ab_result, (uint8_t[4])BUB_BOOT_CTRL_MAGIC,
                         15, 7, 0, 15, 7, 0);
  GenerateMiscImage(&ab_init);

  // Invalid metadata should be found by ab flow at this point. It should reset
  // each slot to an 'updating' state on disk so that we may reattempt boot.
  EXPECT_EQ(BUB_AB_FLOW_ERROR_INVALID_METADATA,
            bub_ab_flow((BubOps*)ops_.bub_ops(), suffix, BUB_SUFFIX_SIZE));
  EXPECT_EQ(0, bub_memcmp((char[BUB_SUFFIX_SIZE]){0,0,0},
                          suffix,
                          BUB_SUFFIX_SIZE));
  EXPECT_EQ(0, CompareMiscImage(ab_result));

  // Run again to check slot A is selected with one less try remaining.
  test_ab_flow(
    15, 6, 0, 15, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, SingleSuccessfulSlot) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 14, 0, 1, 0, 0, 0);

  test_ab_flow(
    14, 0, 1, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, SingleTryingSlot) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 14, 3, 0, 0, 0, 0);

  test_ab_flow(
    14, 2, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, TwoValidSlotsA) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 0, 1, 14, 0, 1);

  test_ab_flow(
    15, 0, 1, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, TwoValidSlotsB) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 14, 0, 1, 15, 0, 1);

  test_ab_flow(
    14, 0, 1, 15, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, TryingFallback) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 7, 0, 14, 0, 1);

  // Decremented our expected tries_remaining for slot a as we run ab_flow
  test_ab_flow(
    15, 6, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 5, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 4, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 3, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 2, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 1, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 0, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  // Should revert to slot b.
  test_ab_flow(
    0, 0, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, TryingNoFallbackRecovery) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 7, 0, 0, 0, 0);

  // Decremented our expected tries_remaining for slot a as we run ab_flow
  test_ab_flow(
    15, 6, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 5, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 4, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 3, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 2, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 1, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 0, 0, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 0, 0, 0,                // Expected A/B state.
    BUB_AB_FLOW_ERROR_NO_VALID_SLOTS, // Expected A/B result.
    "\0\0");                          // Expected A/B suffix.
}

TEST_F(AbTest, SingleTryingSuccess) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 7, 0, 14, 0, 1);

  // Decremented our expected tries_remaining for slot a. Boot was a success
  // on our 6th try and we reboot 2 more times to make sure we stick to the
  // same slot.

  test_ab_flow(
    15, 6, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 5, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 4, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 3, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 2, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 1, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 0, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  // Boot Control HAL should do this.
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 0, 1, 14, 0, 1);

  test_ab_flow(
    15, 0, 1, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  // Should not have changed still.
  test_ab_flow(
    15, 0, 1, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, TwoTryingRecovery) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 7, 0, 14, 7, 0);

  // Decremented our expected tries_remaining for slot a.
  test_ab_flow(
    15, 6, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 5, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 4, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 3, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 2, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 1, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  test_ab_flow(
    15, 0, 0, 14, 7, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  // At this point a should have run out of tries, so we expect the other
  // updating slot to be chosen.

  // Decremented our expected tries_remaining for slot b.
  test_ab_flow(
    0, 0, 0, 14, 6, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 5, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 4, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 3, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 2, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 1, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 14, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.

  test_ab_flow(
    0, 0, 0, 0, 0, 0,                 // Expected A/B state.
    BUB_AB_FLOW_ERROR_NO_VALID_SLOTS, // Expected A/B result.
    "\0\0");                          // Expected A/B suffix.
}

TEST_F(AbTest, MarkedInvalidFallback) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 15, 0, 1, 14, 0, 1);

  // Initially selects slot a.
  test_ab_flow(
    15, 0, 1, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.

  // Invalidate slot a. We expect this slot to be all zero values with slot b
  // unchanged.
  EXPECT_TRUE(bub_ab_mark_as_invalid((BubOps*)ops_.bub_ops(), "_a"));

  // Should select slot b now.
  test_ab_flow(
     0, 0, 0, 14, 0, 1,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, ValidAndInvalidHigherPriority) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 14, 0, 1, 15, 0, 0);

  // Normalizes and selects slot a.
  test_ab_flow(
    14, 0, 1, 0, 0, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_a");                 // Expected A/B suffix.
}

TEST_F(AbTest, ValidAndUpdatingBadSuccessfulBoot) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 14, 0, 1, 15, 7, 1);

  // Normalizes and selects slot b.
  test_ab_flow(
    14, 0, 1, 15, 6, 0,    // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, InvalidBadTriesRemainingAndValid) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 0, 7, 0, 14, 0, 1);

  // Normalizes and selects slot b.
  test_ab_flow(
    0, 0, 0, 14, 0, 1,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, InvalidBadSuccessfulBootandValid) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 0, 0, 1, 14, 0, 1);

  // Normalizes and selects slot b.
  test_ab_flow(
    0, 0, 0, 14, 0, 1,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}

TEST_F(AbTest, InvalidTriesBootAndUpdatingBadSuccessfulBoot) {
  ab_init(BUB_BOOT_CTRL_MAGIC, 0, 7, 1, 15, 7, 1);

  // Normalizes and selects slot b.
  test_ab_flow(
    0, 0, 0, 15, 6, 0,     // Expected A/B state.
    BUB_AB_FLOW_RESULT_OK, // Expected A/B result.
    "_b");                 // Expected A/B suffix.
}