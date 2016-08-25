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

#include <efi.h>
#include <efilib.h>
#include "bub_sysdeps.h"
#include "bub_boot_kernel.h"

BubIOResult bub_read_from_partition(BubOps* ops,
                                    const char* partition_name,
                                    void* buf,
                                    int64_t offset_from_partition,
                                    size_t num_bytes,
                                    size_t* out_num_read) {
  bub_assert(partition_name != NULL);
  bub_assert(buf != NULL);
  bub_assert(out_num_read != NULL);

  EFI_STATUS err;
  GPTEntry *partition_entry;
  UINT64 partition_size;
  MyBubOps* bub = (MyBubOps*)ops;

  err = bub_partition_entry_by_name(bub->block_io,
                                    partition_name,
                                    &partition_entry);
  if (EFI_ERROR(err))
    return BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

  partition_size = IMG_SIZE(partition_entry, bub->block_io);

  if (offset_from_partition < 0) {
    if ((-offset_from_partition) > partition_size) {
      bub_warning("Offset outside range.\n");
      bub_free(partition_entry);
      return BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
    }
    offset_from_partition = partition_size - (-offset_from_partition);
  }

  // Check if num_bytes goes beyond partition end. If so, don't read beyond
  // this boundary -- do a partial I/O instead.
  if (num_bytes > partition_size - offset_from_partition)
    *out_num_read = partition_size - offset_from_partition;
  else
    *out_num_read = num_bytes;

  err = uefi_call_wrapper(bub->disk_io->ReadDisk, 5,
                          bub->disk_io,
                          bub->block_io->Media->MediaId,
                          (partition_entry->first_lba *
                            bub->block_io->Media->BlockSize) +
                            offset_from_partition,
                          *out_num_read,
                          buf);
  if (EFI_ERROR(err)) {
    bub_warning("Could not read from Disk.\n");
    *out_num_read = 0;
    bub_free(partition_entry);
    return BUB_IO_RESULT_ERROR_IO;
  }

  bub_free(partition_entry);
  return BUB_IO_RESULT_OK;
}

BubIOResult bub_write_to_partition(BubOps* ops,
                                   const char* partition_name,
                                   const void* buf,
                                   int64_t offset_from_partition,
                                   size_t num_bytes) {
  bub_assert(partition_name != NULL);
  bub_assert(buf != NULL);

  EFI_STATUS err;
  GPTEntry *partition_entry;
  UINT64 partition_size;
  MyBubOps* bub = (MyBubOps*)ops;

  err = bub_partition_entry_by_name(bub->block_io,
                                    partition_name,
                                    &partition_entry);
  if (EFI_ERROR(err))
    return BUB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

  partition_size = IMG_SIZE(partition_entry, bub->block_io);

  if (offset_from_partition < 0) {
    if ((-offset_from_partition) > partition_size) {
      bub_warning("Offset outside range.\n");
      bub_free(partition_entry);
      return BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
    }
    offset_from_partition = partition_size - (-offset_from_partition);
  }

  // Check if num_bytes goes beyond partition end. If so, error out -- no
  // partial I/O.
  if (num_bytes > partition_size - offset_from_partition) {
    bub_warning("Cannot write beyond partition boundary.\n");
    bub_free(partition_entry);
    return BUB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
  }

  err = uefi_call_wrapper(bub->disk_io->WriteDisk, 5,
                          bub->disk_io,
                          bub->block_io->Media->MediaId,
                          (partition_entry->first_lba *
                            bub->block_io->Media->BlockSize) +
                            offset_from_partition,
                          num_bytes,
                          buf);

  if (EFI_ERROR(err)) {
    bub_warning("Could not write to Disk.\n");
    bub_free(partition_entry);
    return BUB_IO_RESULT_ERROR_IO;
  }

  bub_free(partition_entry);
  return BUB_IO_RESULT_OK;
}