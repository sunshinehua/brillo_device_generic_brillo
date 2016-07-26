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

# This is a build configuration for the base of a Brillo system.
# It contains the mandatory targets required to boot a Brillo device.

# Define a make variable that identifies Brillo targets.
BRILLO := 1

# Common Brillo init scripts.
PRODUCT_COPY_FILES += \
  device/generic/brillo/brillo.rc:system/etc/init/brillo.rc \
  device/generic/brillo/sensorservice.rc:system/etc/init/sensorservice.rc \
  device/generic/brillo/init.firewall-adbd-setup.sh:system/etc/init.firewall-adbd-setup.sh \
  device/generic/brillo/init.firewall-setup.sh:system/etc/init.firewall-setup.sh \
  device/generic/brillo/init.wifi-setup.sh:system/etc/init.wifi-setup.sh \

# Directory for init files.
TARGET_COPY_OUT_INITRCD := $(TARGET_COPY_OUT_SYSTEM)/etc/init

# Template for init files.
INITRC_TEMPLATE := device/generic/brillo/init.template.rc.in

# Directory for Brillo build time properties.
OSRELEASED_DIRECTORY := os-release.d

# Install the BDK version.
PRODUCT_COPY_FILES += \
  tools/bdk/VERSION:system/etc/$(OSRELEASED_DIRECTORY)/bdk_version

# Include the cfgtree helpers for loading config values from disk.
include device/generic/brillo/cfgtree.mk

BOARD_BPT_INPUT_FILES := device/generic/brillo/brillo_partitions.bpt

# Global Brillo USE flags.
BRILLO_USE_BINDER := 1
BRILLO_USE_DBUS := 1
BRILLO_USE_OMAHA := 1
BRILLO_USE_WEAVE := 1

# Java is not used or required for Brillo.
JAVA_NOT_REQUIRED := true

# Brillo does not use the Android recovery image.
TARGET_NO_RECOVERY := true

# Brillo does not run a surface flinger.
TARGET_BOARD_HAS_NO_SURFACE_FLINGER := true

# Generate Breakpad symbols.
BREAKPAD_GENERATE_SYMBOLS := true

# Skip API checks.
WITHOUT_CHECK_API := true
# Don't try to build and run all tests by default. Several tests have
# dependencies on the framework.
ANDROID_NO_TEST_CHECK := true

PRODUCT_PACKAGES := \
  adbd \
  bootctl \
  firewalld \
  init \
  init.environ.rc \
  init.rc \
  ip \
  ip6tables \
  iptables \
  keymaster \
  keystore \
  libminijail \
  linker \
  logcat \
  logd \
  nativeperms \
  nativepowerman \
  peripheralman \
  power_example \
  reboot \
  rootdev \
  service \
  servicemanager \
  softkeymaster \
  sh \
  toolbox \
  toybox \
  update_engine \
  update_engine_client \
  weaved \
  webservd \

# Android build adds libstdc++ dependencies to some modules. Normally Android
# devices inherit embedded.mk which brings in libstdc++, but we don't, so we
# need to explicitly add it. See http://b/24499744.
PRODUCT_PACKAGES += \
  libstdc++ \

# Packages included only for eng or userdebug builds.
PRODUCT_PACKAGES_DEBUG += \
  dumpsys \
  webservd_test_client

# eng builds don't include the official payload key so developers can test
# providing their own testing key.
PRODUCT_PACKAGES_ENG += \
  brillo-update-payload-key

# Packages for eng and userdebug images
PRODUCT_PACKAGES_DEBUG += \
  webservd_testc

# Packages for eng images
PRODUCT_PACKAGES_ENG += \
  apmanager_test \
  audio_hal_playback_test \
  audio_hal_record_test \
  brillo_audio_test \
  brillo_camera_client \
  crash_reporter_tests \
  firewalld_unittest \
  libbrillo_test \
  libchrome_test \
  libminijail_test \
  libminijail_unittest \
  libnativepower_tests \
  libperipheralman_tests \
  metrics_collector_tests \
  metricsd_tests \
  nativepowerman_tests \
  seccomp_bpf_test \
  sensors-hal-example-app \
  sensors-ndk-example-app \
  shill_setup_wifi \
  shill_test \
  syscall_filter_unittest \
  update_engine_unittests \
  weaved_test \

# SELinux packages.
PRODUCT_PACKAGES += \
  sepolicy \
  file_contexts.bin \
  seapp_contexts \
  property_contexts \
  mac_permissions.xml \
  selinux_version \
  service_contexts \

# Build time parameters.
PRODUCT_PACKAGES += \
  product_version \
  product_id \

# D-Bus daemon, utilities, and example programs.
ifeq ($(BRILLO_USE_DBUS),1)
PRODUCT_PACKAGES += \
  dbus-daemon \
  dbus-example-client \
  dbus-example-daemon \
  dbus-monitor \
  dbus-send
endif

# Audio NDK tests.
PRODUCT_PACKAGES += \
  slesTest_playFdPath \
  slesTest_recBuffQueue \
  slesTest_sawtoothBufferQueue \

# Audio dependencies.
PRODUCT_PACKAGES += \
  brilloaudioservice \
  libaudioroute \
  libtinyalsa \
  libtinycompress \
  local_time.default \
  mediaserver \

# Audio libraries.
PRODUCT_PACKAGES += \
  libmedia \
  libstagefright \
  libOpenSLES \
  libOpenMAXAL \

# OpenMAX audio codecs.
PRODUCT_PACKAGES += \
  libstagefright_soft_aacdec \
  libstagefright_soft_aacenc \
  libstagefright_soft_amrdec \
  libstagefright_soft_amrnbenc \
  libstagefright_soft_amrwbenc \
  libstagefright_soft_flacenc \
  libstagefright_soft_g711dec \
  libstagefright_soft_gsmdec \
  libstagefright_soft_mp3dec \
  libstagefright_soft_opusdec \
  libstagefright_soft_rawdec \
  libstagefright_soft_vorbisdec \

# Sensor packages and example programs.
PRODUCT_PACKAGES += \
  libsensor \
  sensorservice \

# Connectivity packages.
PRODUCT_PACKAGES += \
  cacerts \
  cacerts_google \
  dhcpcd \
  dhcpcd-6.8.2 \
  dnsmasq \
  hostapd \
  shill \
  wifi_init \
  wpa_supplicant \

# Check for targets that do not support WiFi.
WIFI_SUPPORTED := true
ifeq ($(TARGET_PRODUCT),brilloemulator_arm64)
WIFI_SUPPORTED := false
endif
ifeq ($(TARGET_PRODUCT),brilloemulator_arm)
WIFI_SUPPORTED := false
endif
ifeq ($(TARGET_PRODUCT),brilloemulator_x86_64)
WIFI_SUPPORTED := false
endif
ifeq ($(TARGET_PRODUCT),brilloemulator_x86)
WIFI_SUPPORTED := false
endif

ifeq ($(WIFI_SUPPORTED),true)
# It only makes sense to include apmanager if WiFi is supported.
PRODUCT_PACKAGES += apmanager
SHILL_USE_WIFI := true
endif

# Metrics daemons and metrics library.
PRODUCT_PACKAGES += \
  libmetrics \
  metrics_client \
  metricsd \

# Crash reporter package.
PRODUCT_PACKAGES += \
  crash_reporter \

# Avahi packages.
PRODUCT_PACKAGES += \
  avahi-browse \
  avahi-client \
  avahi-daemon \
  libdaemon \

# tlsdate binaries.
PRODUCT_PACKAGES += \
  tlsdate \
  tlsdate-helper \
  tlsdated \

# Make 3rd party packages available by default.
PRODUCT_3RD_PARTY_PACKAGES :=
PRODUCT_PACKAGES += \
  3rd-party-packages \

# For android_filesystem_config.h
# This configures filesystem capabilities.
TARGET_ANDROID_FILESYSTEM_CONFIG_H := \
device/generic/brillo/fs_config/android_filesystem_config.h
PRODUCT_PACKAGES += \
  fs_config_files \

# Brillo targets use the A/B updater.
AB_OTA_UPDATER := true

# Do not build Android OTA package.
TARGET_SKIP_OTA_PACKAGE := true

# This is the list of partitions the A/B updater will update. These need to have
# two partitions each in the partition table, with the right suffix used by the
# bootloader, for example "system_a" and "system_b".
AB_OTA_PARTITIONS := \
  boot \
  system

include device/generic/brillo/net_packages_config.mk

# Wireless debugging.
PRODUCT_PACKAGES += \
  iw \
  libnl \
  ping \
  wpa_cli \

# Bluetooth.
# Don't compile for targets without WiFi support until b/25083459 is fixed.
ifeq ($(WIFI_SUPPORTED),true)
PRODUCT_PACKAGES += \
  bluetoothtbd \
  bluetooth-cli \

endif

# Access-Controlled NVRAM
PRODUCT_PACKAGES += nvram-client

# TODO(derat): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dbus.conf:system/etc/dbus.conf \

# TODO(samueltan): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/wpa_supplicant.conf:/system/lib/shill/shims/wpa_supplicant.conf \

# TODO(zqiu): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dhcpcd-6.8.2.conf:/system/etc/dhcpcd-6.8.2/dhcpcd.conf \

BOARD_SEPOLICY_DIRS := $(BOARD_SEPOLICY_DIRS) device/generic/brillo/sepolicy

# Ensure that this property is always defined so that bionic_systrace.cpp
# can rely on it being initially set by init.
# On Android this is defined in embedded.mk, see
# https://android-review.googlesource.com/#/c/106470.
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    debug.atrace.tags.enableflags=0 \
    ro.build.shutdown_timeout=5 \

# TODO(jorgelo): Move into main build.
define generate-initrc-file
  @echo "Generate: $< -> $@"
  @mkdir -p $(dir $@)
  $(hide) sed -e 's?%SERVICENAME%?$(1)?g' $< > $@
  $(hide) sed -i -e 's?%ARGS%?$(2)?g' $@
  $(hide) sed -i -e 's?%GROUPS%?$(3)?g' $@
endef

# Called from the product makefile, it sets any derived defaults.
# DEPRECATED
define set-product-defaults
  $(eval PRODUCT_NAME := $$(basename $$(notdir \
    $$(filter $$(LOCAL_PATH)/%.mk,$$(MAKEFILE_LIST)))))
endef
# Returns the product name as guessed from the calling file.
define get_product_name_from_file
$(firstword $(basename $(notdir \
  $(filter $(LOCAL_PATH)/%.mk,$(MAKEFILE_LIST)))))
endef

HARDWARE_BSP_PREFIX := hardware/bsp
HARDWARE_BSP_PREBUILTS_PREFIX := vendor/bsp
# New BSP helpers - move to /build once stable.
define set_soc
  $(eval soc_vendor := $(strip $(1))) \
  $(eval soc_name := $(strip $(2))) \
  $(eval soc_make_file := $(HARDWARE_BSP_PREFIX)/$(soc_vendor)/soc/$(soc_name)/soc.mk) \
  $(eval soc_prebuilts_make_file := $(HARDWARE_BSP_PREBUILTS_PREFIX)/$(soc_vendor)/hardware/soc/$(soc_name)/soc.mk) \
  $(if $(wildcard $(soc_make_file)),$(eval include $(soc_make_file)), \
    $(if $(wildcard $(soc_prebuilts_make_file)),$(eval include $(soc_prebuilts_make_file)), \
       $(error Can't find SoC definition. Vendor: $(soc_vendor) SoC: $(soc_name))))
endef

define add_peripheral
  $(eval peripheral_vendor := $(strip $(1))) \
  $(eval peripheral_name := $(strip $(2))) \
  $(eval peripheral_make_file := $(HARDWARE_BSP_PREFIX)/$(peripheral_vendor)/peripheral/$(peripheral_name)/peripheral.mk) \
  $(eval peripheral_prebuilts_make_file := $(HARDWARE_BSP_PREBUILTS_PREFIX)/$(peripheral_vendor)/hardware/peripheral/$(peripheral_name)/peripheral.mk) \
  $(eval HAL_MAKEFILES += $(HARDWARE_BSP_PREFIX)/$(peripheral_vendor)/peripheral/$(peripheral_name)/hal.mk) \
  $(if $(wildcard $(peripheral_make_file)),$(eval include $(peripheral_make_file)), \
    $(if $(wildcard $(peripheral_prebuilts_make_file)),$(eval include $(peripheral_prebuilts_make_file)), \
      $(error Can't find peripheral definition. Vendor: $(peripheral_vendor) peripheral: $(peripheral_name))))
endef

define add_device_packages
  $(eval CUSTOM_MODULES += $(DEVICE_PACKAGES))
endef

define add_kernel_configs
  $(eval TARGET_KERNEL_CONFIGS := $(TARGET_KERNEL_CONFIGS) $(1))
endef

# Configures adbd to listen for connections on the given TCP port on userdebug
# and eng builds. The host can then use `adb connect <ip>:<port>` to connect.
# Usage: $(call enable_adb_over_tcp, <port>[, <start_on_boot>])
#   port: TCP port for adbd to listen on.
#   start_on_boot: set to true to auto-start adbd on userdebug and eng builds.
#       Normally init.usb.rc handles this, but boards that do not include it
#       (e.g. boards without USB) may want to use this option.
define enable_adb_over_tcp
  $(if $(filter userdebug eng,$(TARGET_BUILD_VARIANT)), \
    $(eval PRODUCT_DEFAULT_PROPERTY_OVERRIDES += persist.adb.tcp.port=$(strip $(1))) \
    $(if $(filter true,$(2)), \
      $(eval PRODUCT_DEFAULT_PROPERTY_OVERRIDES += adbd-setup.autostart=1)))
endef
