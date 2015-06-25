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
  service_contexts