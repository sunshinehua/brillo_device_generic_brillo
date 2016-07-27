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
#include "bub_boot_kernel.h"
#include "bub_sysdeps.h"


EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle,
                            EFI_SYSTEM_TABLE* SystemTable) {
  int err;

  InitializeLib(ImageHandle, SystemTable);
  bub_print("Brillo UEFI A/B BOOT LOADER\n");

  err = bub_boot_kernel(ImageHandle, "boot_a");
  if (err) {
    bub_error("Error loading kernel.\n");
    uefi_call_wrapper(BS->Stall, 1, 15 * 1000 * 1000);
    return EFI_LOAD_ERROR;
  }

  return EFI_SUCCESS;
}