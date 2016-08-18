#
# Copyright 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Note: This is not invoked by Android build system and is intended to aid
#       users to locally build the boot loader application.
# TODO: Port this as an Android.mk and add gnu-efi to external/.

# EFI make flags and arguments
EFI_CC          = gcc
EFI_LD          = ld
EFI_OBJCOPY     = objcopy

EFI_SRC_FILES   = bub_ab_flow.c \
                  bub_boot_kernel.c \
		  bub_crc32.c \
                  bub_main.c \
                  bub_ops_uefi.c \
                  bub_sysdeps_uefi.c \
                  bub_util.c
EFI_OBJ_FILES   = $(patsubst %.c,%.o,$(EFI_SRC_FILES))
EFI_TARGET      = brillo_boot_loader.efi
EFI_SHARED_OBJ  = $(patsubst %.efi,%.so,$(EFI_TARGET))

EFI_CFLAGS  = -I/usr/include/efi \
                -I/usr/include/efi/x86_64 \
                -I/usr/include/efi/protocol \
                -fno-stack-protector -fpic \
                -fshort-wchar -mno-red-zone -Wall \
                -DEFI_FUNCTION_WRAPPER

EFI_LDFLAGS = -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared \
                -Bsymbolic -L /usr/lib/ /usr/lib/crt0-efi-x86_64.o \
                /usr/lib/elf_x86_64_efi.lds
EFI_LDLIBS  = -lefi -lgnuefi

EFI_OBJCOPY_SECTIONS    = -j .text -j .sdata -j .data -j .dynamic \
                            -j .dynsym  -j .rel -j .rela -j .reloc
EFI_OBJCOPY_TARGET      = --target=efi-app-x86_64


all: $(EFI_TARGET)

$(EFI_OBJ_FILES): $(EFI_SRC_FILES)
	$(EFI_CC) $(EFI_CFLAGS) -c $(EFI_SRC_FILES)

$(EFI_SHARED_OBJ): $(EFI_OBJ_FILES)
	$(EFI_LD) $(EFI_LDFLAGS) $(EFI_OBJ_FILES) -o $@ $(EFI_LDLIBS)

$(EFI_TARGET): $(EFI_SHARED_OBJ)
	$(EFI_OBJCOPY) $(EFI_OBJCOPY_SECTIONS) $(EFI_OBJCOPY_TARGET) $^ $@

clean:
	rm  $(EFI_OBJ_FILES) $(EFI_SHARED_OBJ) $(EFI_TARGET)

keep:
	rm  $(EFI_OBJ_FILES) $(EFI_SHARED_OBJ)
