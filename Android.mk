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

# -----------------------------------------------------------------
# The vendor partition package for brillo device.

ifdef BRILLO_VENDOR_PARTITIONS

ifneq "" "$(filter eng.%,$(BUILD_NUMBER))"
  # BUILD_NUMBER has a timestamp in it, which means that
  # it will change every time.  Pick a stable value.
  FILE_NAME_TAG := eng.$(USER)
else
  FILE_NAME_TAG := $(BUILD_NUMBER)
endif

name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE),debug)
  name := $(name)_debug
endif
name := $(name)-vendor_partitions-$(FILE_NAME_TAG)

BRILLO_VENDOR_PARTITIONS_TARGET := $(PRODUCT_OUT)/$(name).zip

$(BRILLO_VENDOR_PARTITIONS_TARGET) : \
  $(BRILLO_VENDOR_PARTITIONS) \
  $(PRODUCT_OUT)/provision-device
	@echo "Package vendor partitions: $@"
	$(hide) rm -rf $@
	$(hide) mkdir -p $(dir $@)
	$(hide) zip -qj $@ $^

$(call dist-for-goals, dist_files, $(BRILLO_VENDOR_PARTITIONS_TARGET))

endif
