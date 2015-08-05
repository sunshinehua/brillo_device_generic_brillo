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

# The qemu board uses qualcomms msm8916 soc
$(call inherit-platform, qcom, msm8916)

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 268435456
BOARD_USERDATAIMAGE_PARTITION_SIZE := 134217728
BOARD_CACHEIMAGE_PARTITION_SIZE := 268435456

PRODUCT_COPY_FILES += \
  device/generic/brillo/bsp/boards/qcom/dragon_410c/fstab.brillo:root/fstab.brillo

WLAN_FIRMWARE_SRC = device/generic/brillo/bsp/boards/qcom/firmware/wlan
FIRMWARE_DST = system/vendor/firmware

# Wireless peripheral firmware -- loaded by drivers/net/wireless/wcnss.
WCNSS_FILES = mdt b00 b01 b02 b04 b06 b09 b10 b11
WCNSS_COPY_FILES += \
  $(join $(patsubst %, $(WLAN_FIRMWARE_SRC)/wcnss.%, $(WCNSS_FILES)), $(patsubst %, :$(FIRMWARE_DST)/wcnss.%, $(WCNSS_FILES)))

PRODUCT_COPY_FILES += $(WCNSS_COPY_FILES)

# WiFi driver -- loaded by drivers/staging/prima.
WLAN_FIRMWARE_DST = $(FIRMWARE_DST)/wlan/prima
PRIMA_FILES = \
  WCNSS_cfg.dat \
  WCNSS_qcom_cfg.ini \
  WCNSS_qcom_wlan_nv.bin \
  WCNSS_wlan_dictionary.dat
PRIMA_COPY_FILES += \
  $(join $(patsubst %, $(WLAN_FIRMWARE_SRC)/%, $(PRIMA_FILES)), $(patsubst %, :$(WLAN_FIRMWARE_DST)/%, $(PRIMA_FILES)))

PRODUCT_COPY_FILES += $(PRIMA_COPY_FILES)
