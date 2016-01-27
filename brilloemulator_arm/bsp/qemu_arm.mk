#
# Copyright 2015 The Android Open Source Project
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

# TODO(leecam): Make this automatic.
LOCAL_PATH := device/generic/brillo/brilloemulator_arm/bsp

# Arm32 device.
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a
TARGET_CPU_VARIANT := generic
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_KERNEL_ARCH := $(TARGET_ARCH)

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := false

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_FLASH_BLOCK_SIZE := 512
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := true

PRODUCT_COPY_FILES += \
  $(LOCAL_PATH)/init.qemu.rc:root/init.qemu.rc \
  $(LOCAL_PATH)/initnetwork.sh:system/bin/initnetwork.sh \
  system/core/rootdir/init.usb.rc:root/init.usb.rc \
  system/core/rootdir/ueventd.rc:root/ueventd.rc \

# Set up the local kernel.
TARGET_KERNEL_SRC := hardware/bsp/kernel/common/v4.4
TARGET_KERNEL_DEFCONFIG := vexpress_defconfig
$(call add_kernel_configs,$(realpath $(LOCAL_PATH)/brillo.kconf))
TARGET_KERNEL_DTB := vexpress-v2p-ca9.dtb

BOARD_SEPOLICY_DIRS := $(BOARD_SEPOLICY_DIRS) $(LOCAL_PATH)/sepolicy
