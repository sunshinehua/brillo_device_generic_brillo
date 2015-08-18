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

LOCAL_PATH := $(call my-dir)

LOCAL_INIT_SERVICE := testservice

include $(CLEAR_VARS)
LOCAL_MODULE := $(LOCAL_INIT_SERVICE)
LOCAL_REQUIRED_MODULES := init.$(LOCAL_INIT_SERVICE).rc
LOCAL_SRC_FILES := testservice.c
LOCAL_SHARED_LIBRARIES := libc liblog
LOCAL_CFLAGS := -Werror
include $(BUILD_EXECUTABLE)

ifdef INITRC_TEMPLATE
include $(CLEAR_VARS)
LOCAL_MODULE := init.$(LOCAL_INIT_SERVICE).rc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_INITRCD)

include $(BUILD_SYSTEM)/base_rules.mk

my_args := arg1 arg2
my_groups := inet
.PHONY: $(LOCAL_BUILT_MODULE)
$(LOCAL_BUILT_MODULE): $(INITRC_TEMPLATE)
	$(call generate-initrc-file,$(LOCAL_INIT_SERVICE),$(my_args),$(my_groups))
endif
