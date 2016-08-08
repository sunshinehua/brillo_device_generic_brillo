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

#ifndef BUB_IMAGE_UTIL_H_
#define BUB_IMAGE_UTIL_H_

#include <fcntl.h>
#include <gtest/gtest.h>
#include <base/files/file_util.h>

#include "bub_sysdeps.h"
#include "bub_ab_flow.h"
#include "bub_util.h"

struct MyBubOps;
typedef struct MyBubOps MyBubOps;

class MyOps {
 public:
  MyOps();
  ~MyOps();

  BubOps* bub_ops() { return (BubOps*)bub_ops_; }
  void set_partition_dir(const base::FilePath& partition_dir);
  BubIOResult read_from_partition(const char* partition, void* buf,
                                  int64_t offset, size_t num_bytes,
                                  size_t* out_num_read);
  BubIOResult write_to_partition(const char* partition, const void* buf,
                                 int64_t offset, size_t num_bytes);

  /* Assigns to |ab| metadata using |magic| and [a,b]_*| parameters. This
   * function does not swap byte order nor does it calculate the crc.
   */
  void write_ab_metadata(BubAbData* ab,
                         const uint8_t* magic,
                         uint8_t a_priority,
                         uint8_t a_tries_remaining,
                         uint8_t a_successful_boot,
                         uint8_t b_priority,
                         uint8_t b_tries_remaining,
                         uint8_t b_successful_boot);

  /* Writes out a misc.img file in a temp directory using |ab_metadata|.
   * Byte swapping is done prior to writing to ensure the big endianness
   * expected in the Misc partition.
   */
  base::FilePath make_metadata_image(const BubAbData* ab_metadata,
                                   	 const char* name);

  MyBubOps* bub_ops_;
  base::FilePath partition_dir_;
};

struct MyBubOps {
  BubOps parent;
  MyOps* my_ops;
};

class AbTest : public ::testing::Test {
  public:
    AbTest() {}

    // Create temporary directory to stash images in.
    void SetUp() override;

    /* Wrapper function to generate test misc image by calling MyOps
     * make_metadata_image.
     */
    void GenerateMiscImage(const BubAbData* ab_metadata);

    /* Tests expected vs actual contents of ab metadata found in the Misc
     * partition. Byte swapping to big endianness and crc for |ab_expected|
     * is done prior to test comparisons.
     */
    int CompareMiscImage(BubAbData ab_expected);

    // Temporary directory created in SetUp().
    base::FilePath testdir_;

    MyOps ops_;
};

#endif /* BUB_IMAGE_UTIL_H_ */