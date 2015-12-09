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

# Include common emulator settings.
include device/generic/brillo/brilloemulator/CommonBoardConfig.mk

# Standard devices would usually define an SoC. As the emulator
# has no SoC definition we pull in a local QEMU BSP.
include device/generic/brillo/brilloemulator_arm/bsp/qemu_arm.mk

TARGET_RECOVERY_FSTAB = device/generic/brillo/brilloemulator_arm/fstab.device

PRODUCT_COPY_FILES += \
    device/generic/brillo/brilloemulator_arm/fstab.device:root/fstab.device \
    device/generic/brillo/brilloemulator_arm/fstab.device:root/fstab.qemu
