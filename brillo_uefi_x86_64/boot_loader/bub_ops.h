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

#ifndef BUB_OPS_H_
#define BUB_OPS_H_

#include "bub_sysdeps.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes used for I/O operations.
 *
 * BUB_IO_RESULT_OK is returned if the requested operation was
 * successful.
 *
 * BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION is returned if the requested
 * partition does not exist.
 *
 * BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION is returned if the
 * range of bytes requested to be read or written is outside the range
 * of the partition.
 *
 * BUB_IO_RESULT_ERROR_IO is returned if the underlying disk
 * encountered an I/O error.
 */
typedef enum {
  BUB_IO_RESULT_OK,
  BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION,
  BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION,
  BUB_IO_RESULT_ERROR_IO,
} BubIOResult;

struct BubOps;
typedef struct BubOps BubOps;

/* High-level operations/functions/methods that are platform
 * dependent.
 */
struct BubOps {
  /* Reads |num_bytes| from offset |offset| from partition with name
   * |partition| (NUL-terminated UTF-8 string). If |offset| is
   * negative, its absolute value should be interpreted as the number
   * of bytes from the end of the partition.
   *
   * This function returns BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION if
   * there is no partition with the given name,
   * BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION if the requested
   * |offset| is outside the partition, and BUB_IO_RESULT_ERROR_IO if
   * there was an I/O error from the underlying I/O subsystem.  If the
   * operation succeeds as requested BUB_IO_RESULT_OK is returned and
   * the data is available in |buf|.
   *
   * The only time partial I/O may occur is if reading beyond the end
   * of the partition. In this case the value returned in
   * |out_num_read| may be smaller than |num_bytes|.
   */
  BubIOResult (*read_from_partition)(BubOps* ops, const char* partition,
                                     void* buf, int64_t offset,
                                     size_t num_bytes, size_t* out_num_read);

  /* Writes |num_bytes| at offset |offset| from partition with name
   * |partition| (NUL-terminated UTF-8 string). If |offset| is
   * negative, its absolute value should be interpreted as the number
   * of bytes from the end of the partition.
   *
   * This function returns BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION if
   * there is no partition with the given name,
   * BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION if the requested
   * byterange goes outside the partition, and BUB_IO_RESULT_ERROR_IO
   * if there was an I/O error from the underlying I/O subsystem.  If
   * the operation succeeds as requested BUB_IO_RESULT_OK is
   * returned.
   *
   * This function never does any partial I/O, it either transfers all
   * of the requested bytes or returns an error.
   */
  BubIOResult (*write_to_partition)(BubOps* ops, const char* partition,
                                    const void* buf, int64_t offset,
                                    size_t num_bytes);

  /* Checks if the given public key is trusted. Return non-zero if
   * trusted or zero if untrusted.
   */
  int (*validate_public_key)(BubOps* ops, const uint8_t* public_key_data,
                             size_t public_key_length);

  /* Gets the rollback index corresponding to the slot given by
   * |rollback_index_slot|. The value is returned in
   * |out_rollback_index|. Returns non-zero if the rollback index was
   * retrieved, zero on error.
   *
   * A device may have a limited amount of rollback index slots (say,
   * one or four) so may error out if |rollback_index_slot| exceeds
   * this number.
   */
  int (*read_rollback_index)(BubOps* ops, size_t rollback_index_slot,
                             uint64_t* out_rollback_index);

  /* Sets the rollback index corresponding to the slot given by
   * |rollback_index_slot| to |rollback_index|. Returns non-zero if
   * the rollback index was set, zero on error.
   *
   * A device may have a limited amount of rollback index slots (say,
   * one or four) so may error out if |rollback_index_slot| exceeds
   * this number.
   */
  int (*write_rollback_index)(BubOps* ops, size_t rollback_index_slot,
                              uint64_t rollback_index);

  /* Gets whether the device is unlocked. The value is returned in
   * |out_is_unlocked| (non-zero if unlocked, zero otherwise). Returns
   * non-zero if the state was retrieved, zero on error.
   */
  int (*read_is_unlocked)(BubOps* ops, int* out_is_unlocked);

  /* Gets the unique partition GUID for a partition with name in
   * |partition| (NUL-terminated UTF-8 string). The GUID is copied as
   * a string into |guid_buf| of size |guid_buf_size| and will be NUL
   * terminated. The string must be lower-case and properly
   * hyphenated. For example:
   *
   *  527c1c6d-6361-4593-8842-3c78fcd39219
   *
   * Returns zero if the operation fails (no such partition or
   * |buf_size| is too small), non-zero if it succeeds.
   */
  int (*get_unique_guid_for_partition)(BubOps* ops, const char* partition,
                                       char* guid_buf, size_t guid_buf_size);
};

#ifdef __cplusplus
}
#endif

#endif /* BUB_OPS_H_ */