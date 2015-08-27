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

# Common Brillo init script
PRODUCT_COPY_FILES += \
  device/generic/brillo/init.brillo.rc:root/init.brillo.rc \
  device/generic/brillo/init.firewall-setup.sh:system/etc/init.firewall-setup.sh \
  device/generic/brillo/init.set-sane-initial-date.sh:system/etc/init.set-sane-initial-date.sh

# Directory for init files.
TARGET_COPY_OUT_INITRCD := $(TARGET_COPY_OUT_ROOT)/initrc.d

# Skip API checks.
WITHOUT_CHECK_API := true
# Don't try to build and run all tests by defaults. Several tests have
# dependencies on the framework.
ANDROID_NO_TEST_CHECK := true

# Template for init files.
INITRC_TEMPLATE := device/generic/brillo/init.template.rc.in

PRODUCT_PACKAGES = \
  adbd \
  firewalld \
  init \
  init.environ.rc \
  init.rc \
  ip \
  ip6tables \
  iptables \
  libminijail \
  linker \
  logcat \
  logd \
  reboot \
  sh \
  toolbox \
  toybox \
  weaved \
  webservd \

# SELinux packages
PRODUCT_PACKAGES += \
  sepolicy \
  file_contexts \
  seapp_contexts \
  property_contexts \
  mac_permissions.xml \
  selinux_version \
  service_contexts \

# D-Bus daemon, utilities, and example programs.
PRODUCT_PACKAGES += \
  dbus-daemon \
  dbus-example-client \
  dbus-example-daemon \
  dbus-monitor \
  dbus-send \

# Connectivity packages.
PRODUCT_PACKAGES += \
  cacerts \
  dhcpcd \
  dnsmasq \
  hostapd \
  wpa_supplicant \

# Metrics daemon and metrics library
PRODUCT_PACKAGES += \
  libmetrics \
  metrics_client \
  metrics_daemon \

# Avahi packages.
PRODUCT_PACKAGES += \
  avahi-browse \
  avahi-client \
  avahi-daemon \
  libdaemon \

# For android_filesystem_config.h
# This configures filesystem capabilities.
TARGET_ANDROID_FILESYSTEM_CONFIG_H := \
device/generic/brillo/fs_config/android_filesystem_config.h
PRODUCT_PACKAGES += \
  fs_config_files \

# We must select a wpa_supplicant version, or the AOSP version won't be built.
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211

# Enable WPA supplicant D-Bus support.
CONFIG_CTRL_IFACE_DBUS=y
CONFIG_CTRL_IFACE_DBUS_NEW=y

# Wireless debugging.
PRODUCT_PACKAGES += \
  iw \
  libnl \
  ping \
  wpa_cli \

# Stop-gap connectivity solution.
PRODUCT_PACKAGES += \
  wifi_connect \

# TODO(derat): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dbus.conf:system/etc/dbus.conf \

# TODO(samueltan): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/wpa_supplicant.conf:system/etc/wpa_supplicant.conf \

BOARD_SEPOLICY_DIRS := $(BOARD_SEPOLICY_DIRS) device/generic/brillo/sepolicy

# Define that identifies Brillo targets
BRILLO := 1

# Required helpers for Brillo targets.
# TODO(wad,leecam) once stable, move into build/core.
BRILLO_BSP_PREFIX := device/generic/brillo/bsp
BRILLO_BOARD_PREFIX := $(BRILLO_BSP_PREFIX)/boards
BRILLO_PLATFORM_PREFIX := $(BRILLO_BSP_PREFIX)/platforms
BRILLO_ARCH_PREFIX := $(BRILLO_BSP_PREFIX)/arch

# TODO(wad) collect all the mks using find even though a hierarchy is faster.
define inherit-board
  $(eval board_vendor := $(strip $(1))) \
  $(eval board_name := $(strip $(2))) \
  $(eval board_make_file := $(BRILLO_BOARD_PREFIX)/$(board_vendor)/$(board_name)/board.mk) \
  $(if $(wildcard $(board_make_file)),$(eval include $(board_make_file)), \
    $(error Can't find board definition. Vendor: $(board_vendor) Board: $(board_name)))
endef

define inherit-platform
  $(eval platform_vendor := $(strip $(1))) \
  $(eval plaform_name := $(strip $(2))) \
  $(eval platform_make_file := $(BRILLO_PLATFORM_PREFIX)/$(platform_vendor)/$(plaform_name)/platform.mk) \
  $(if $(wildcard $(platform_make_file)),$(eval include $(platform_make_file)), \
    $(error Can't find platform definition. Vendor: $(platform_vendor) Board: $(plaform_name)))
endef

define inherit-arch
  $(eval arch_name := $(strip $(1))) \
  $(eval arch_make_file := $(BRILLO_ARCH_PREFIX)/$(arch_name)/arch.mk) \
  $(if $(wildcard $(arch_make_file)),$(eval include $(arch_make_file)), \
    $(error Can't find arch definition: $(arch_name)))
endef

# TODO(jorgelo): Move into main build.
define generate-initrc-file
  @echo "Generate: $< -> $@"
  @mkdir -p $(dir $@)
  $(hide) sed -e 's?%SERVICENAME%?$(1)?g' $< > $@
  $(hide) sed -i -e 's?%ARGS%?$(2)?g' $@
  $(hide) sed -i -e 's?%GROUPS%?$(3)?g' $@
endef


HARDWARE_BSP_PREFIX := hardware/bsp
# New BSP helpers - move to /build once stable.
define set_soc
  $(eval soc_vendor := $(strip $(1))) \
  $(eval soc_name := $(strip $(2))) \
  $(eval soc_make_file := $(HARDWARE_BSP_PREFIX)/$(soc_vendor)/soc/$(soc_name)/soc.mk) \
  $(if $(wildcard $(soc_make_file)),$(eval include $(soc_make_file)), \
    $(error Can't find SoC definition. Vendor: $(soc_vendor) SoC: $(soc_name)))
endef

define add_peripheral
  $(eval peripheral_vendor := $(strip $(1))) \
  $(eval peripheral_name := $(strip $(2))) \
  $(eval peripheral_make_file := $(HARDWARE_BSP_PREFIX)/$(peripheral_vendor)/peripheral/$(peripheral_name)/peripheral.mk) \
  $(if $(wildcard $(peripheral_make_file)),$(eval include $(peripheral_make_file)), \
    $(error Can't find peripheral definition. Vendor: $(peripheral_vendor) peripheral: $(peripheral_name)))
endef
