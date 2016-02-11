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

# Include the peripheral HALs
$(foreach f,$(HAL_MAKEFILES), \
    $(if $(wildcard $(f)),$(eval include $(f))))

# The vendor partition package for brillo device.
ifdef BRILLO_VENDOR_PARTITIONS

# The staging directory to store vendor partitions.
intermediates := $(call intermediates-dir-for, PACKAGING, vendor-partitions)

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

vendor_partition_zip := $(intermediates)/$(name)-vendor_partitions-$(FILE_NAME_TAG).zip

# Set up rules to copy all the files to staging directory, and gather the paths of the dest files.
# BRILLO_VENDOR_PARTITIONS contains a list of strings in the format of parent_directory:file_path.
# Each vendor partition's full path is parent_directory/file_path. The directory structure in
# file_path will be preserved.
vendor_partition_copied_files := \
	$(foreach f, $(BRILLO_VENDOR_PARTITIONS) $(PRODUCT_OUT):provision-device, \
		$(eval pair := $(subst :,$(space),$(f))) \
		$(eval src := $(word 1, $(pair))/$(word 2, $(pair))) \
		$(eval dest := $(intermediates)/$(word 2, $(pair))) \
		$(eval $(call copy-one-file, $(src), $(dest))) \
		$(dest))

$(vendor_partition_zip) : $(vendor_partition_copied_files)
	@echo "Package vendor partitions: $@"
	$(hide) cd $(dir $@) && zip -qr $(notdir $@) *

$(call dist-for-goals, dist_files, $(vendor_partition_zip))

endif
