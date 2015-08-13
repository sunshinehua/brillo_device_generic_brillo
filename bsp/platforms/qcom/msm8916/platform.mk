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

# Arm
$(call inherit-arch, arm)

TARGET_NO_BOOTLOADER := false
TARGET_NO_KERNEL := false

BOARD_KERNEL_CMDLINE := console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=brillo msm_rtb.filter=0x237 ehci-hcd.park=3 androidboot.bootdevice=7824900.sdhci lpm_levels.sleep_disabled=1 earlyprintk androidboot.selinux=permissive

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 131072
# TARGET_USERIMAGES_SPARSE_EXT_DISABLED := true

PRODUCT_COPY_FILES += \
  device/generic/brillo/bsp/platforms/qcom/msm8916/init.platform.rc:root/init.platform.rc \
  system/core/rootdir/init.usb.rc:root/init.usb.rc \
  system/core/rootdir/ueventd.rc:root/ueventd.rc \

PRODUCT_COPY_FILES += \
    device/generic/brillo/bsp/platforms/qcom/msm8916/kernel/zImage-dtb:kernel

BOARD_SEPOLICY_DIRS += device/generic/brillo/bsp/platforms/qcom/msm8916/sepolicy
