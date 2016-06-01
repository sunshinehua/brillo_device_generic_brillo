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

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 786432000
# No ramdisk.
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
BOARD_USERDATAIMAGE_PARTITION_SIZE := 576716800

# No ramdisk.
BOARD_USES_FULL_RECOVERY_IMAGE := true

# Additional bpt file specifying EFI
BOARD_BPT_INPUT_FILES += device/generic/brillo/brillo_uefi_x86_64/device-partitions.bpt

# Standard devices would usually define an SoC. Here we define
# a local BSP.
include device/generic/brillo/brillo_uefi_x86_64/bsp/brillo_uefi_x86_64.mk

TARGET_RECOVERY_FSTAB = \
    device/generic/brillo/brillo_uefi_x86_64/fstab.brillo_uefi_x86_64

PRODUCT_COPY_FILES += \
    device/generic/brillo/brillo_uefi_x86_64/fstab.brillo_uefi_x86_64:root/fstab.brillo_uefi_x86_64 \
