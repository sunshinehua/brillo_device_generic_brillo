#
# Copyright (C) 2016 The Android Open Source Project
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

# Make a zipfile containing the DTB if it has been built but not appended
# to the kernel image already.

ifdef TARGET_KERNEL_DTB
ifndef TARGET_KERNEL_DTB_APPEND

emul_name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE), debug)
  emul_name := $(emul_name)_$(TARGET_BUILD_TYPE)
endif
emul_name := $(emul_name)-dtb-$(FILE_NAME_TAG)

EMUL_ZIP := $(TARGET_OUT_INTERMEDIATES)/$(emul_name).zip

EMUL_DTB := $(PRODUCT_OUT)/kernel.dtb

$(EMUL_ZIP): $(EMUL_DTB)
	$(hide) echo "Package dtb: $@"
	$(hide) rm -rf $@
	$(hide) mkdir -p $(dir $@)
	$(hide) zip -j $@ $(EMUL_DTB)

$(call dist-for-goals, dist_files, $(EMUL_ZIP))

endif
endif
