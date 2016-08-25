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

#include "bub_ab_flow.h"
#include "bub_util.h"

static const uint8_t magic[] = BUB_BOOT_CTRL_MAGIC;
static const char* bub_suffixes[2] = {"_a", "_b"};

static int normalize_slot(BubSlotData *slot) {
  if (slot->priority > 0) {
    if (slot->tries_remaining == 0 && !slot->successful_boot) {
      bub_memset(slot, 0, sizeof(BubSlotData));
      return 1;
    }
    if (slot->tries_remaining > 0 && slot->successful_boot) {
      slot->successful_boot = 0;
      return 1;
    }
  } else if (slot->tries_remaining != 0 || slot->successful_boot) {
    bub_memset(slot, 0, sizeof(BubSlotData));
    return 1;
  }

  return 0;
}

static int slot_is_bootable(const BubSlotData *slot) {
  return (slot->successful_boot || slot->tries_remaining > 0);
}

static int reset_metadata(BubOps* ops) {
  BubAbData metadata;

  bub_memset(&metadata, 0, sizeof(BubAbData));
  bub_memcpy(&metadata.magic, magic, sizeof(metadata.magic));
  metadata.major_version = BUB_MAJOR_VERSION;
  metadata.minor_version = BUB_MINOR_VERSION;
  metadata.slots[0].priority = 15;
  metadata.slots[0].tries_remaining = 7;
  metadata.slots[0].successful_boot = 0;
  metadata.slots[1].priority = 15;
  metadata.slots[1].tries_remaining = 7;
  metadata.slots[1].successful_boot = 0;
  return bub_write_ab_data_to_misc(ops, &metadata);
}

BubAbFlowResult bub_ab_flow(BubOps* ops,
                            char* out_selected_suffix,
                            size_t suffix_num_bytes) {
  bub_assert(out_selected_suffix != NULL);
  bub_assert(suffix_num_bytes >= 3);

  BubAbFlowResult ab_err;
  BubAbData ab_ctl;
  int target_slot_index_to_boot = -1;
  int new_metadata = 0;

  // No selection has been made yet.
  bub_memset(out_selected_suffix, 0, 3);

  ab_err = bub_read_ab_data_from_misc(ops, &ab_ctl);
  if (ab_err != BUB_AB_FLOW_RESULT_OK) {
    if (ab_err == BUB_AB_FLOW_ERROR_INVALID_METADATA) {
      bub_warning("Reseting metadata.\n");
      if (!reset_metadata(ops)) {
        bub_warning("Unable to reset metadata.\n");
      }
    }
    return ab_err;
  }

  // Ensure only proper slot states exist.
  if (normalize_slot(&ab_ctl.slots[0])) {
    bub_warning("State of slot A was normalized.\n");
    new_metadata = 1;
  }
  if (normalize_slot(&ab_ctl.slots[1])) {
    bub_warning("State of slot B was normalized.\n");
    new_metadata = 1;
  }

  if (slot_is_bootable(&ab_ctl.slots[0]) && slot_is_bootable(&ab_ctl.slots[1]))
  {
    if (ab_ctl.slots[1].priority > ab_ctl.slots[0].priority)
      target_slot_index_to_boot = 1;
    else
      target_slot_index_to_boot = 0;
  } else if (slot_is_bootable(&ab_ctl.slots[0])) {
    // Choose slot A.
    target_slot_index_to_boot = 0;
  } else if (slot_is_bootable(&ab_ctl.slots[1])) {
    // Choose slot B.
    target_slot_index_to_boot = 1;
  } else {
    if (new_metadata && !bub_write_ab_data_to_misc(ops, &ab_ctl))
      return BUB_AB_FLOW_ERROR_WRITE_METADATA;
    // If neither was chosen, there are no valid slots.
    bub_warning("No valid slot found.\n");
    return BUB_AB_FLOW_ERROR_NO_VALID_SLOTS;
  }

  // Found usable slot to attempt boot on. Decrement tries remaining and write
  // out the new AB metadata if the slots is in "Updated" state.
  if (ab_ctl.slots[target_slot_index_to_boot].tries_remaining > 0) {
    ab_ctl.slots[target_slot_index_to_boot].tries_remaining--;
    new_metadata = 1;
  }

  if (new_metadata)
    if (!bub_write_ab_data_to_misc(ops, &ab_ctl))
      return BUB_AB_FLOW_ERROR_WRITE_METADATA;

  // Write selected suffix to caller's pointer.
  bub_memcpy(out_selected_suffix, bub_suffixes[target_slot_index_to_boot], 3);

  return BUB_AB_FLOW_RESULT_OK;
}

int bub_ab_mark_as_invalid(BubOps* ops, const char* invalid_suffix) {
  bub_assert(invalid_suffix != NULL);
  bub_assert(bub_strlen(invalid_suffix) >= 2);

  BubAbData ab_ctl;
  BubAbFlowResult ab_err;
  unsigned int i;

  ab_err = bub_read_ab_data_from_misc(ops, &ab_ctl);
  if (ab_err != BUB_AB_FLOW_RESULT_OK)
    return ab_err;

  // Find suffix in small list.
  for (i = 0; i < 2; ++i)
  {
    if (!bub_memcmp(bub_suffixes[i], invalid_suffix, 3)) {
      // Found the corresponding index to invalid_suffix. Invalidate the slot
      // and write out.
      bub_memset(&ab_ctl.slots[i], 0, sizeof(BubSlotData));
      return bub_write_ab_data_to_misc(ops, &ab_ctl);
    }
  }

  bub_warning("Could not find requested slot.\n");
  return 0;
}

BubAbFlowResult bub_read_ab_data_from_misc(BubOps* ops, BubAbData* data) {
  BubIOResult io_result;
  size_t num_bytes_read;
  uint32_t crc;

  io_result = ops->read_from_partition(ops, "misc", data, 0,
                                    sizeof(BubAbData), &num_bytes_read);
  if (io_result != BUB_IO_RESULT_OK || num_bytes_read != sizeof(BubAbData)) {
    bub_warning("Could not read metadata from misc partition.\n");
    return BUB_AB_FLOW_ERROR_READ_METADATA;
  }

  if(bub_memcmp(data->magic, magic, 4) != 0) {
    bub_warning("AB metadata magic did not match.\n");
    return BUB_AB_FLOW_ERROR_INVALID_METADATA;
  }

  crc = bub_be32toh(data->crc32);
  data->crc32 = 0;
  if (crc != bub_crc32(0, data, sizeof(BubAbData))) {
    bub_warning("AB metadata crc is invalid.\n");
    return BUB_AB_FLOW_ERROR_INVALID_METADATA;
  }

  // Assign host byte order to necessary variables needed here.
  data->crc32 = crc;

  return BUB_AB_FLOW_RESULT_OK;
}

int bub_write_ab_data_to_misc(BubOps* ops, const BubAbData* data) {
  BubIOResult io_result;
  BubAbData metadata;

  bub_memcpy(&metadata, data, sizeof(BubAbData));

  // Assign big endian order to necessary variables here.

  // Calculate crc assign back to crc field, maintaining big endianness.
  metadata.crc32 = 0;
  metadata.crc32 = bub_be32toh(bub_crc32(0, &metadata, sizeof(BubAbData)));
  io_result = ops->write_to_partition(ops,
                                      "misc",
                                      &metadata,
                                      0,
                                      sizeof(BubAbData));
  if (io_result != BUB_IO_RESULT_OK) {
    bub_warning("Could not write to misc partition.\n");
    return 0;
  }

  return 1;
}