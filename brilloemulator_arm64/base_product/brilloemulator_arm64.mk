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

include device/generic/brillo/brilloemulator/base.mk

PRODUCT_NAME := brilloemulator_arm64
PRODUCT_DEVICE := brilloemulator_arm64

# Install emulator-specific config file for weaved.
PRODUCT_COPY_FILES += \
  device/generic/brillo/brilloemulator_arm64/base_product/weaved.conf:system/etc/weaved/weaved.conf

PRODUCT_PACKAGES += \
  brilloemulator-arm64
