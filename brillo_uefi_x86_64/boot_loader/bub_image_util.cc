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

// NOTE: See avb_slot_verify_unittest.cc for orginal reference to similar
//       partition testing.

#include "bub_image_util.h"

static BubIOResult my_ops_read_from_partition(BubOps* ops,
                                              const char* partition, void* buf,
                                              int64_t offset, size_t num_bytes,
                                              size_t* out_num_read) {
  return ((MyBubOps*)ops)
      ->my_ops->read_from_partition(partition, buf, offset, num_bytes,
                                    out_num_read);
}

static BubIOResult my_ops_write_to_partition(BubOps* ops, const char* partition,
                                             const void* buf, int64_t offset,
                                             size_t num_bytes) {
  return ((MyBubOps*)ops)
      ->my_ops->write_to_partition(partition, buf, offset, num_bytes);
}


void MyOps::set_partition_dir(const base::FilePath& partition_dir) {
  partition_dir_ = partition_dir;
}

BubIOResult MyOps::read_from_partition(const char* partition, void* buf,
                                int64_t offset, size_t num_bytes,
                                size_t* out_num_read) {
  base::FilePath path =
      partition_dir_.Append(std::string(partition)).AddExtension("img");

  if (offset < 0) {
    int64_t file_size;
    if (!base::GetFileSize(path, &file_size)) {
      fprintf(stderr, "Error getting size of file '%s'\n",
              path.value().c_str());
      return BUB_IO_RESULT_ERROR_IO;
    }
    offset = file_size - (-offset);
  }

  int fd = open(path.value().c_str(), O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening file '%s': %s\n", path.value().c_str(),
                strerror(errno));
    return BUB_IO_RESULT_ERROR_IO;
  }
  if (lseek(fd, offset, SEEK_SET) != offset) {
    fprintf(stderr, "Error seeking to pos %zd in file %s: %s\n", offset,
                path.value().c_str(), strerror(errno));
    close(fd);
    return BUB_IO_RESULT_ERROR_IO;
  }
  ssize_t num_read = read(fd, buf, num_bytes);
  if (num_read < 0) {
    fprintf(stderr, "Error reading %zd bytes from pos %" PRId64
                " in file %s: %s\n",
                num_bytes, offset, path.value().c_str(), strerror(errno));
    close(fd);
    return BUB_IO_RESULT_ERROR_IO;
  }
  close(fd);

  if (out_num_read != NULL) {
    *out_num_read = num_read;
  }

  return BUB_IO_RESULT_OK;
}

BubIOResult MyOps::write_to_partition(const char* partition, const void* buf,
                               int64_t offset, size_t num_bytes) {
  base::FilePath path =
      partition_dir_.Append(std::string(partition)).AddExtension("img");

  if (offset < 0) {
    int64_t file_size;
    if (!base::GetFileSize(path, &file_size)) {
      fprintf(stderr, "Error getting size of file '%s'\n",
              path.value().c_str());
      return BUB_IO_RESULT_ERROR_IO;
    }
    offset = file_size - (-offset);
  }

  int fd = open(path.value().c_str(), O_WRONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening file '%s': %s\n", path.value().c_str(),
                strerror(errno));
    return BUB_IO_RESULT_ERROR_IO;
  }
  if (lseek(fd, offset, SEEK_SET) != offset) {
    fprintf(stderr, "Error seeking to pos %zd in file %s: %s\n", offset,
                path.value().c_str(), strerror(errno));
    close(fd);
    return BUB_IO_RESULT_ERROR_IO;
  }
  ssize_t num_written = write(fd, buf, num_bytes);
  if (num_written < 0) {
    fprintf(stderr, "Error writing %zd bytes at pos %"
                    PRId64 " in file %s: %s\n",
            num_bytes, offset, path.value().c_str(), strerror(errno));
    close(fd);
    return BUB_IO_RESULT_ERROR_IO;
  }
  close(fd);

  return BUB_IO_RESULT_OK;
}

void MyOps::write_ab_metadata(BubAbData* ab,
                              const uint8_t* magic,
                              uint8_t a_priority,
                              uint8_t a_tries_remaining,
                              uint8_t a_successful_boot,
                              uint8_t b_priority,
                              uint8_t b_tries_remaining,
                              uint8_t b_successful_boot) {
  bub_memset(ab, 0, sizeof(BubAbData));
  bub_memcpy(ab->magic, magic, sizeof(ab->magic));
  ab->major_version = BUB_MAJOR_VERSION;
  ab->minor_version = BUB_MINOR_VERSION;
  ab->slots[0].priority = a_priority;
  ab->slots[0].tries_remaining = a_tries_remaining;
  ab->slots[0].successful_boot = a_successful_boot;
  ab->slots[1].priority = b_priority;
  ab->slots[1].tries_remaining = b_tries_remaining;
  ab->slots[1].successful_boot = b_successful_boot;
}

base::FilePath MyOps::make_metadata_image(const BubAbData* ab_metadata,
                                          const char* name) {
  // Generate a 1025 KiB file with known content.
  std::vector<uint8_t> image;
  image.resize(sizeof(BubAbData));
  BubAbData ab_metadata_be;
  uint8_t* image_data = (uint8_t*)&ab_metadata_be;

  bub_memcpy(&ab_metadata_be, ab_metadata, sizeof(BubAbData));

  // Byte swap all necessary variables here.

  ab_metadata_be.crc32 = 0;
  ab_metadata_be.crc32 =
    bub_be32toh(bub_crc32((uint8_t*)&ab_metadata_be, sizeof(BubAbData)));

  for (size_t n = 0; n < sizeof(BubAbData); n++) {
    image[n] = image_data[n];
  }
  base::FilePath image_path = partition_dir_.Append(name);
  EXPECT_EQ(sizeof(BubAbData),
            static_cast<const size_t>(base::WriteFile(
                image_path, reinterpret_cast<const char*>(image.data()),
                image.size())));
  return image_path;
}

void AbTest::SetUp() {
  base::FilePath ret;
  char* buf = strdup("/tmp/bub-tests.XXXXXX");
  ASSERT_TRUE(mkdtemp(buf) != nullptr);
  testdir_ = base::FilePath(buf);
  ops_.set_partition_dir(testdir_);
  free(buf);
}

MyOps::MyOps() {
  bub_ops_ = new MyBubOps;
  bub_ops_->parent.read_from_partition = my_ops_read_from_partition;
  bub_ops_->parent.write_to_partition = my_ops_write_to_partition;
  bub_ops_->my_ops = this;
}

MyOps::~MyOps() { delete bub_ops_; }

void AbTest::GenerateMiscImage(const BubAbData* ab_metadata) {
  ops_.make_metadata_image(ab_metadata, "misc.img");
}

int AbTest::CompareMiscImage(BubAbData ab_expected) {
  const uint8_t A = 0, B = 1;
  size_t num_bytes_read;
  BubAbData ab_expected_be;
  BubAbData* ab_actual = (BubAbData*)bub_calloc(sizeof(BubAbData));

  bub_memcpy(&ab_expected_be, &ab_expected, sizeof(BubAbData));

  // Byte swap all necessary variables here.

  ab_expected_be.crc32 = 0;
  ab_expected_be.crc32 =
    bub_be32toh(bub_crc32((uint8_t*)&ab_expected_be, sizeof(BubAbData)));

  if ((ops_.bub_ops_)->parent.read_from_partition((BubOps *)ops_.bub_ops_,
                                                  "misc", ab_actual, 0,
                                                  sizeof(BubAbData),
                                                  &num_bytes_read)) {
    fprintf(stderr, "Could not read from misc partition.\n");
    bub_free(ab_actual);
    return 1;
  }
  if (num_bytes_read != sizeof(BubAbData)) {
    fprintf(stderr, "Bad misc partition read.\n");
    bub_free(ab_actual);
    return 1;
  }

  // Check magic and version numbers.
  EXPECT_EQ(0, bub_memcmp(&ab_expected_be, ab_actual, 8));

  // Check slots values.
  EXPECT_EQ(ab_expected_be.slots[A].priority,
            ab_actual->slots[A].priority);
  EXPECT_EQ(ab_expected_be.slots[A].tries_remaining,
            ab_actual->slots[A].tries_remaining);
  EXPECT_EQ(ab_expected_be.slots[A].successful_boot,
            ab_actual->slots[A].successful_boot);
  EXPECT_EQ(0, bub_memcmp(ab_expected_be.slots[A].reserved,
                          ab_actual->slots[A].reserved,
                          sizeof(ab_actual->slots[A].reserved)));
  EXPECT_EQ(ab_expected_be.slots[B].priority,
            ab_actual->slots[B].priority);
  EXPECT_EQ(ab_expected_be.slots[B].tries_remaining,
            ab_actual->slots[B].tries_remaining);
  EXPECT_EQ(ab_expected_be.slots[B].successful_boot,
            ab_actual->slots[B].successful_boot);
  EXPECT_EQ(0, bub_memcmp(&ab_expected_be.slots[B].reserved,
                          ab_actual->slots[B].reserved,
                          sizeof(ab_actual->slots[A].reserved)));

  // Check reserved and crc bytes.
  // TODO: Compute and compare crc value here.
  EXPECT_EQ(0, bub_memcmp(ab_actual->reserved2,
                          ab_expected_be.reserved2,
                          sizeof(ab_expected_be.reserved2)));

  EXPECT_EQ(ab_expected_be.crc32, ab_actual->crc32);

  bub_free(ab_actual);
  return 0;
}
