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

# Common Brillo init scripts.
PRODUCT_COPY_FILES += \
  device/generic/brillo/brillo.rc:system/etc/init/brillo.rc \
  device/generic/brillo/sensorservice.rc:system/etc/init/sensorservice.rc \
  device/generic/brillo/init.firewall-setup.sh:system/etc/init.firewall-setup.sh \

# Directory for init files.
TARGET_COPY_OUT_INITRCD := $(TARGET_COPY_OUT_SYSTEM)/etc/init

# Directory for Brillo build time properties.
OSRELEASED_DIRECTORY := os-release.d

# Install the BDK version.
PRODUCT_COPY_FILES += \
  tools/bdk/VERSION:system/etc/$(OSRELEASED_DIRECTORY)/bdk_version

# Skip API checks.
WITHOUT_CHECK_API := true
# Don't try to build and run all tests by default. Several tests have
# dependencies on the framework.
ANDROID_NO_TEST_CHECK := true

# Template for init files.
INITRC_TEMPLATE := device/generic/brillo/init.template.rc.in

PRODUCT_PACKAGES = \
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
  nativepowerman \
  power_example \
  reboot \
  rootdev \
  servicemanager \
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

ifneq ($(TARGET_BUILD_VARIANT),eng)
# eng builds don't include the official payload key so developers can test
# providing their own testing key.
PRODUCT_PACKAGES += \
  brillo-update-payload-key
endif

# TODO(deymo): Remove the example postinst once payload v2 is used.
PRODUCT_PACKAGES += \
  postinst_example \

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
PRODUCT_PACKAGES += \
  dbus-daemon \
  dbus-example-client \
  dbus-example-daemon \
  dbus-monitor \
  dbus-send \

# Audio example program.
PRODUCT_PACKAGES += \
  audio_test \

# Audio example program dependencies.
PRODUCT_PACKAGES += \
  libtinycompress \
  libtinyalsa \
  libaudioroute \
  mediaserver \
  local_time.default \

# Sensor packages and example programs.
PRODUCT_PACKAGES += \
  libsensor \
  sensorservice \
  sensors-hal-example-app \
  sensors-ndk-example-app \

# Connectivity packages.
PRODUCT_PACKAGES += \
  apmanager \
  cacerts \
  dhcpcd \
  dhcpcd-6.8.2 \
  dnsmasq \
  hostapd \
  shill \
  wpa_supplicant \

# Metrics daemon and metrics library.
PRODUCT_PACKAGES += \
  libmetrics \
  metrics_client \
  metrics_daemon \

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
UPDATE_AB_PARTITIONS := \
  boot \
  system

# We must select a wpa_supplicant version, or the AOSP version won't be built.
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211

# Enable WPA supplicant D-Bus support.
CONFIG_CTRL_IFACE_DBUS=y
CONFIG_CTRL_IFACE_DBUS_NEW=y

# Enable disable_vht option in WPA supplicant.
CONFIG_VHT_OVERRIDES=y

# WPA supplicant basic authentication methods.
CONFIG_DYNAMIC_EAP_METHODS=y
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_MD5=y
CONFIG_EAP_MSCHAPV2=y
CONFIG_EAP_TLS=y
CONFIG_EAP_PEAP=y
CONFIG_EAP_TTLS=y
CONFIG_EAP_GTC=y
CONFIG_EAP_OTP=y
CONFIG_EAP_LEAP=y
CONFIG_PKCS12=y
CONFIG_PEERKEY=y
CONFIG_BGSCAN_SIMPLE=y
CONFIG_BGSCAN_LEARN=y
CONFIG_IEEE80211W=y

# Settings for dhcpcd-6.8.2.
DHCPCD_USE_IPV6=yes
DHCPCD_USE_DBUS=yes

# Wireless debugging.
PRODUCT_PACKAGES += \
  iw \
  libnl \
  ping \
  wpa_cli \

# Bluetooth.
PRODUCT_PACKAGES += \
  bluetoothtbd \
  bluetooth-cli \

# TODO(derat): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dbus.conf:system/etc/dbus.conf \

# TODO(samueltan): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/wpa_supplicant.conf:/system/lib/shill/shims/wpa_supplicant.conf \

# TODO(zqiu): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dhcpcd-6.8.2.conf:/system/etc/dhcpcd-6.8.2/dhcpcd.conf \

# TODO(arihc): Move this whitelist file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/tests.txt:data/nativetest/tests.txt

BOARD_SEPOLICY_DIRS := $(BOARD_SEPOLICY_DIRS) device/generic/brillo/sepolicy

# Define that identifies Brillo targets.
BRILLO := 1

# Generate Breakpad symbols.
BREAKPAD_GENERATE_SYMBOLS := true

# TODO(jorgelo): Move into main build.
define generate-initrc-file
  @echo "Generate: $< -> $@"
  @mkdir -p $(dir $@)
  $(hide) sed -e 's?%SERVICENAME%?$(1)?g' $< > $@
  $(hide) sed -i -e 's?%ARGS%?$(2)?g' $@
  $(hide) sed -i -e 's?%GROUPS%?$(3)?g' $@
endef

# Called from the product makefile, it sets any derived defaults.
define set-product-defaults
  $(eval PRODUCT_NAME := $$(basename $$(notdir \
    $$(filter $$(LOCAL_PATH)/%.mk,$$(MAKEFILE_LIST)))))
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
  $(if $(wildcard $(peripheral_make_file)),$(eval include $(peripheral_make_file)), \
    $(if $(wildcard $(peripheral_prebuilts_make_file)),$(eval include $(peripheral_prebuilts_make_file)), \
      $(error Can't find peripheral definition. Vendor: $(peripheral_vendor) peripheral: $(peripheral_name))))
endef

define add_device_packages
  $(eval CUSTOM_MODULES += $(DEVICE_PACKAGES))
endef
