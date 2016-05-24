#
# Copyright (C) 2015 The Android Open Source Project
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

$(call inherit-product, device/generic/brillo/brillo_base.mk)

PRODUCT_BRAND := Brillo

BOARD_SEPOLICY_DIRS += device/generic/brillo/brilloemulator/sepolicy

BRILLOEMULATOR := true

# This string eventually gets put into the system property ro.board.platform.
# libhardware uses it to look up HALs that we should load on emulators.
TARGET_BOARD_PLATFORM := brilloemulator

# Enable fake OpenGL graphics HALs from goldfish.
BUILD_EMULATOR_OPENGL := true
USE_CAMERA_STUB := true

# For the TPM simulator.
PRODUCT_PACKAGES += libtpm2

# For the TPM software stack.
PRODUCT_PACKAGES += \
  libtrunks \
  trunks_client \
  trunksd \
  libtpm_manager \
  tpm_manager_client \
  tpm_managerd \

# Typically, BSPs define the set of HALs included for a board.
# However, emulators all share this fake camera HAL and there is
# currently no concept for a hierarchical BSP.
PRODUCT_PACKAGES += \
    camera.$(TARGET_BOARD_PLATFORM) \
    gralloc.$(TARGET_BOARD_PLATFORM) \
    gralloc.default

# Software NVRAM HAL.
PRODUCT_PACKAGES += \
    fake-nvram \
    nvram.testing
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hardware.nvram=testing
