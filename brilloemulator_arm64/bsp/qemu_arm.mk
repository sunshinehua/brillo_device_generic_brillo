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
LOCAL_PATH := device/generic/brillo/brilloemulator_arm64/bsp

# Arm64 device.
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_VARIANT := generic
TARGET_CPU_ABI := arm64-v8a

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_VARIANT := cortex-a15
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi

TARGET_USES_64_BIT_BINDER := true

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
TARGET_KERNEL_SRC := hardware/bsp/kernel/common/android-3.18
TARGET_KERNEL_DEFCONFIG := defconfig
TARGET_KERNEL_CONFIGS := $(realpath $(LOCAL_PATH)/brillo.kconf)

BOARD_SEPOLICY_DIRS := $(BOARD_SEPOLICY_DIRS) $(LOCAL_PATH)/sepolicy
