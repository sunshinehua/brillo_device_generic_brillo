#
# Copyright (C) 2015 The Android Open-Source Project
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

# This is a build configuration for the base of a brillo system.
# It contains the mandatory targets required to boot a brillo device.

PRODUCT_PACKAGES = \
  adbd \
  init \
  init.environ.rc \
  init.rc \
  linker \
  logcat \
  logd \
  reboot \
  sh \
  toolbox \
  toybox \

# SELinux packages
PRODUCT_PACKAGES += \
  sepolicy \
  file_contexts \
  seapp_contexts \
  property_contexts \
  mac_permissions.xml \
  selinux_version \
  service_contexts \

# D-Bus daemon and utilities.
PRODUCT_PACKAGES += \
  dbus-daemon \
  dbus-monitor \
  dbus-send \

# TODO(derat): Move this config file to a saner place.
PRODUCT_COPY_FILES += \
  device/generic/brillo/dbus.conf:system/etc/dbus.conf \

BOARD_SEPOLICY_DIRS += device/generic/brillo/sepolicy

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
