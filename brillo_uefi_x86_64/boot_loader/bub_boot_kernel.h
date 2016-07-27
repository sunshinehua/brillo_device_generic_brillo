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

// For printing debug statements.
#define BUB_ENABLE_DEBUG

/* Boots a UEFI kernel image given a |boot_partition_name| string belonging to a
 * bootable partition entry. The partition must be on the same block device as
 * the current UEFI application, |app_image|. |app_image| is given at the entry
 * point, efi_main(), of the UEFI application.
 *
 * @return int 1 upon failure. 0 on success.
 */
int bub_boot_kernel(EFI_HANDLE app_image, const char* boot_partition_name);

#endif /* BUB_BOOT_KERNEL_H_ */