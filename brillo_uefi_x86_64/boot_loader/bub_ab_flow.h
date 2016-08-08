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

#ifndef BUB_AB_FLOW_H_
#define BUB_AB_FLOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bub_ops.h"

/* Magic for the Brillo Uefi metadata header */
#define BUB_BOOT_CTRL_MAGIC {'B', 'U', 'E', 'F'}

/* The current major and minor versions used for AB metadata structs */
#define BUB_MAJOR_VERSION 1
#define BUB_MINOR_VERSION 0

#define BUB_BLOCK_SIZE 512
#define BUB_AB_DATA_SIZE 64
#define BUB_SUFFIX_SIZE 3

typedef enum {
  BUB_AB_FLOW_RESULT_OK,
  BUB_AB_FLOW_ERROR_INPUT,
  BUB_AB_FLOW_ERROR_INVALID_METADATA,
  BUB_AB_FLOW_ERROR_NO_VALID_SLOTS,
  BUB_AB_FLOW_ERROR_READ_METADATA,
  BUB_AB_FLOW_ERROR_WRITE_METADATA
} BubAbFlowResult;

typedef struct {
    // Slot priority with 15 meaning highest priority, 1 lowest
    // priority and 0 the slot is unbootable.
    uint8_t priority: 4;
    // Number of times left attempting to boot this slot.
    uint8_t tries_remaining: 3;
    // 1 if this slot has booted successfully, 0 otherwise.
    uint8_t successful_boot: 1;
    // Reserved for further use.
    uint8_t reserved[7];
} __attribute__((__packed__)) BubSlotData;

/* Bootloader Control BubAbData
 *
 * This struct is used to manage A/B metadata. Big-endian order is used.
 */
typedef struct {
    // Bootloader Control AB magic number (see BUB_BOOT_CTRL_MAGIC).
    uint8_t magic[4];
    // Major version number.
    uint8_t major_version;
    // Minor version number.
    uint8_t minor_version;
    // Reserved for slot alignment.
    uint8_t reserved1[2];
    // Per-slot information.
    BubSlotData slots[2];
    // Reserved for further use.
    uint8_t reserved2[36];
    // CRC32 of all 60 bytes preceding this field.
    uint32_t crc32;
} __attribute__((__packed__)) BubAbData;


#if defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 6
 _Static_assert(sizeof(BubAbData) == BUB_AB_DATA_SIZE,
                "BubAbData has wrong size!");
#endif

/* A/B flow logic for Brillo booting. Reads A/B metadata from the 'misc'
 * partition and validates it. Chooses a bootable slot based on its state. Upon
 * finding the bootable slot, its "tries remaining" attribute is decremented
 * and thus A/B metadata may be modified in the "misc" partition. The suffix of
 * a bootable slot, including a terminating NUL-byte, will be written in
 * |out_selected_suffix| on success. Caller must specify the size of the
 * |out_selected_suffix| in |suffix_num_bytes| which must be at least 3
 * otherwise aborting the program. If BUB_AB_FLOW_INVALID_AB_METADATA is
 * returned, metadata on disk will be reset to an 'updating' state where both
 * slots have tries remaining to reattempt booting.
 *
 * @return: BUB_AB_FLOW_RESULT_OK on success. BUB_AB_FLOW_INVALID_AB_METADATA
 *          if AB metadata is invalid. BUB_AB_FLOW_RESULT_ERROR if no available
 *          memory for allocationor no valid slot found.
 *
 */
BubAbFlowResult bub_ab_flow(BubOps* ops,
                            char* out_selected_suffix,
                            size_t suffix_num_bytes);

/* Marks a boot slot with |invalid_suffix| invalid by assigning zero to its
 * priority, tries_remaining, and successful_boot member variables. Caller
 * must pass |invalid_suffix| as a NUL_terminated string with length 2.
 *
 * @return: 0 on success. 1 on failure.
 */
int bub_ab_mark_as_invalid(BubOps* ops, const char *invalid_suffix);

/* Helper function to read and check validity of AB metadata using |ops|
 * read_from_partition method.  Will read from "misc" partition and assign
 * fields to |data| in host byte order. Checks magic field matches expected
 * value and calculates crc32.
 *
 * @return: BUB_AB_FLOW_ERROR_READ_METADATA on i/o error.
 *          BUB_AB_FLOW_ERROR_INVALID_METADATA if magic or crc are not
 *          correct. BUB_AB_FLOW_RESULT_OK on success.
 *
 */
BubAbFlowResult bub_read_ab_data_from_misc(BubOps* ops, BubAbData* data);

/* Helper function to write AB metadata using |ops| write_to_partition method.
 * Will write to "misc" partition and assign fields to |data| in big-endian byte
 * order. Calculates crc32 as well.
 *
 * @return: 0 on i/o error, 1 on success.
 *
 */
int bub_write_ab_data_to_misc(BubOps* ops, const BubAbData* data);


#ifdef __cplusplus
}
#endif

#endif /* BUB_AB_FLOW_H_ */