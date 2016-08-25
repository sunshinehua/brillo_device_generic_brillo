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
#ifndef BUB_BOOT_KERNEL_H_
#define BUB_BOOT_KERNEL_H_

#include <efi.h>
#include <efilib.h>
#include "bub_ops.h"

// For printing debug statements.
#define BUB_ENABLE_DEBUG

typedef enum {
  BUB_BOOT_RESULT_OK,
  BUB_BOOT_ERROR_OOM,
  BUB_BOOT_ERROR_IO,
  BUB_BOOT_ERROR_PARTITION_INVALID_FORMAT,
  BUB_BOOT_ERROR_LOAD_KERNEL,
  BUB_BOOT_ERROR_PARAMETER_LOAD,
  BUB_BOOT_ERROR_START_KERNEL,
} BubBootResult;

// GPT related constants
#define GPT_REVISION 0x00010000
#define GPT_MAGIC "EFI PART"
#define GPT_MIN_SIZE 92
#define GPT_ENTRIES_LBA 2
#define BUB_BLOCK_SIZE 512
#define ENTRIES_PER_BLOCK 4
#define ENTRY_NAME_LEN 36
#define MAX_GPT_ENTRIES 128

typedef struct {
  UINT8   signature[8];
  UINT32  revision;
  UINT32  header_size;
  UINT32  header_crc32;
  UINT32  reserved;
  UINT64  header_lba;
  UINT64  alternate_header_lba;
  UINT64  first_usable_lba;
  UINT64  last_usable_lba;
  UINT8   disk_guid[16];
  UINT64  entry_lba;
  UINT32  entry_count;
  UINT32  entry_size;
  UINT32  entry_crc32;
  UINT8   reserved2[420];
} GPTHeader;

typedef struct {
  UINT8   type_GUID[16];
  UINT8   unique_GUID[16];
  UINT64  first_lba;
  UINT64  last_lba;
  UINT64  flags;
  CHAR16  name[ENTRY_NAME_LEN];
} GPTEntry;


#define IMG_SIZE(entry, block) \
  (entry->last_lba - entry->first_lba) * block->Media->BlockSize

#define SIZE_BLOCK_ALIGN(bytes, BUB_BLOCK_SIZE) \
  ((bytes + BUB_BLOCK_SIZE - 1) / BUB_BLOCK_SIZE) * BUB_BLOCK_SIZE

#define OFFSET_BLOCK_ALIGN(bytes, BUB_BLOCK_SIZE) \
  (bytes / BUB_BLOCK_SIZE) * BUB_BLOCK_SIZE

typedef struct {
  BubOps parent;
  EFI_HANDLE efi_image_handle;
  EFI_DEVICE_PATH* path;
  EFI_BLOCK_IO* block_io;
  EFI_DISK_IO* disk_io;
  // EFI_STATUS (*PopulateMiscPartition)(MyBubOps* self);
} MyBubOps;

BubIOResult bub_read_from_partition(BubOps* ops,
                                    const char* partition_name,
                                    void* buf,
                                    int64_t offset_from_partition,
                                    size_t num_bytes,
                                    size_t* out_num_read);

BubIOResult bub_write_to_partition(BubOps* ops,
                                   const char* partition_name,
                                   const void* buf,
                                   int64_t offset_from_partition,
                                   size_t num_bytes);

/* Allocates memory for and assigns to member variables of |bub|. Also assigns
 * the Brillo Uefi-specific read_from_partition and write_to_partition
 * functions to its BubOps parent. |app_image| must be the EFI main-specific
 * (the current currently running program) handle.
 *
 * @return int 0 on failure. non-zero on success.
 */
int bub_init(MyBubOps* bub, EFI_HANDLE app_image);

/* Boots a UEFI kernel image given a |boot_partition_name| string belonging to a
 * bootable partition entry. The partition must be on the same block device as
 * the current UEFI application, |app_image|. |app_image| is given at the entry
 * point, efi_main(), of the UEFI application.
 *
 * @return BUB_BOOT_ERROR_OOM on allocation,
 *         BUB_BOOT_ERROR_IO on read/write error,
 *         BUB_BOOT_ERROR_PARTITION_INVALID_FORMAT on bad magic or bad size
 *           boundaries,
 *         BUB_BOOT_ERROR_LOAD_KERNEL if unable to load kernel into memory
 *         BUB_BOOT_ERROR_PARAMETER_LOAD if unable to load kernel parameters to
 *          the EFI_STUB,
 *         BUB_BOOT_ERROR_START_KERNEL if unable to execute kernel,
 *         BUB_BOOT_RESULT_OK on success.
 */
BubBootResult bub_boot_kernel(MyBubOps* bub, const char* boot_partition_name);

/* Looks through |block_io| to search for a GPT entry named |partition_name|, a
 * NULL-terminated string. Allocates a GPTEntry struct for |entry_buf|. Caller
 * is responsible for freeing |entry_buf|.
 *
 * @return EFI_NOT_FOUND on error, EFI SUCCESS on success.
 */
EFI_STATUS bub_partition_entry_by_name(IN EFI_BLOCK_IO *block_io,
                                       const char* partition_name,
                                       GPTEntry** entry_buf);

#endif /* BUB_BOOT_KERNEL_H_ */