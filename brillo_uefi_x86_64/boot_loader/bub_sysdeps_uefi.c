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

int bub_memcmp(const void* src1, const void* src2, size_t n) {
  return (int)CompareMem(src1, src2, (UINTN)n);
}

int bub_strcmp(const char* s1, const char* s2) {
  return (int)StrCmp(s1, s2);
}

void* bub_memcpy(void* dest, const void* src, size_t n) {
  CopyMem(dest, src, (UINTN)n);
}

void* bub_memset(void* dest, const int c, size_t n) {
  SetMem(dest, (UINTN)n, (UINT8)c);
}

int bub_safe_memcmp(const void* s1, const void* s2, size_t n) {
  const unsigned char* us1 = s1;
  const unsigned char* us2 = s2;
  int result = 0;

  if (0 == n) return 0;

  /*
   * Code snippet without data-dependent branch due to Nate Lawson
   * (nate@root.org) of Root Labs.
   */
  while (n--) result |= *us1++ ^ *us2++;

  return result != 0;
}

void bub_print(const char* format) {
   size_t utf8_bytes = bub_strlen(format) + 1;
   size_t max_ucs2_bytes = utf8_bytes * 2;
   uint16_t* format_ucs2 = (uint16_t*)bub_calloc(max_ucs2_bytes);
   if (format_ucs2 == NULL)
     return;
   if (!utf8_to_ucs2(format, utf8_bytes, format_ucs2, max_ucs2_bytes))
     Print(format_ucs2);
   bub_free(format_ucs2);
}

void bub_abort(void) {
  bub_print("\nABORTING...\n");
  uefi_call_wrapper(BS->Stall, 1, 5 * 1000 * 1000);
  uefi_call_wrapper(BS->Exit, 4,
                    NULL,
                    EFI_NOT_FOUND,
                    0,
                    NULL);
}

void* bub_malloc_(size_t size) {
  EFI_STATUS err;
  void *x;

  err = uefi_call_wrapper(BS->AllocatePool, 3,
                          EfiBootServicesData,
                          (UINTN)size,
                          &x);
  if (EFI_ERROR(err)) {
    return NULL;
  }

  return x;

}

void bub_free(void* ptr) {
  EFI_STATUS err;
  err = uefi_call_wrapper(BS->FreePool, 1, ptr);

  if (EFI_ERROR(err)) {
    Print(L"Warning: Bad bub_free: %r\n", err);
    uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
  }
}

size_t bub_strlen(const char* str) {
  return strlena(str);
}