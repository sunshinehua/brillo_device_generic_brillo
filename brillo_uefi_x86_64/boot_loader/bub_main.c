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

#include <efi.h>
#include <efilib.h>
#include "bub_ab_flow.h"
#include "bub_boot_kernel.h"
#include "bub_sysdeps.h"


EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle,
                            EFI_SYSTEM_TABLE* SystemTable) {
  MyBubOps ops;
  BubAbFlowResult ab_result;
  BubBootResult boot_result;
  char slot_suffix[BUB_SUFFIX_SIZE] = {0};
  char boot_name[7] = "boot\0\0\0";

  InitializeLib(ImageHandle, SystemTable);
  bub_print("Brillo UEFI A/B BOOT LOADER\n");

  if (!bub_init(&ops, ImageHandle))
    bub_error("Could not initialize Brillo Uefi object.");

  // Attempt AB flow and boot.  Invalidate metadata for slots having bad
  // partition format.
  do {
    ab_result = bub_ab_flow((BubOps *)&ops, slot_suffix, BUB_SUFFIX_SIZE);
    if (ab_result != BUB_AB_FLOW_RESULT_OK)
      bub_error("Could not choose A/B slot.\n");

    bub_memcpy(boot_name + 4, slot_suffix, BUB_SUFFIX_SIZE);

    boot_result = bub_boot_kernel(&ops, boot_name);
    if (boot_result == BUB_BOOT_ERROR_PARTITION_INVALID_FORMAT) {
      bub_warning("Marking slot as invalid.\n");

      if (bub_ab_mark_as_invalid((MyBubOps *)&ops, slot_suffix))
        bub_error("Could not mark slot invalid.");

    }
    else if (boot_result != BUB_BOOT_RESULT_OK)
      bub_error("Error loading kernel.\n");

  } while (boot_result == BUB_BOOT_ERROR_PARTITION_INVALID_FORMAT);

  return EFI_SUCCESS;
}