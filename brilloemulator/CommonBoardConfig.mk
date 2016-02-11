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

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 786432000
# No ramdisk.
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
BOARD_USERDATAIMAGE_PARTITION_SIZE := 576716800

# No ramdisk.
BOARD_USES_FULL_RECOVERY_IMAGE := true

# This is an emulator build.
TARGET_SKIP_OTA_PACKAGE := true

PRODUCT_COPY_FILES += \
    device/generic/brillo/brilloemulator/ueventd.rc:root/ueventd.qemu.rc \

# Set up common kernel settings.
$(call add_kernel_configs,$(realpath device/generic/brillo/brilloemulator/brillo.kconf))
