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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := connectivity
LOCAL_CLANG := true
LOCAL_CPPFLAGS := -std=c++11 -Wall -Werror
LOCAL_SHARED_LIBRARIES := libcutils libminijail libsysutils
LOCAL_SRC_FILES := connectivity_listener.cpp connectivity_common.cpp
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := libconnectivity
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CLANG := true
LOCAL_CPPFLAGS := -std=c++11 -Wall -Werror
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_SRC_FILES := connectivity_client.cpp connectivity_common.cpp
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := connectivity_test
LOCAL_CLANG := true
LOCAL_CPPFLAGS := -Wall -Werror
LOCAL_SHARED_LIBRARIES := libconnectivity
LOCAL_SRC_FILES := connectivity_test.cpp
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := init.connectivity.rc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_INITRCD)

include $(BUILD_SYSTEM)/base_rules.mk

$(LOCAL_BUILT_MODULE): $(INITRC_TEMPLATE)
	@echo "Generate: $< -> $@"
	@mkdir -p $(dir $@)
	$(hide) sed -e 's?%SERVICENAME%?connectivity?g' \
		    -e 's?%GROUPS%??g' \
		    -e 's?%ARGS%??g' \
		    -e 's?user.*?user root?g' \
		    -e 's?seclabel.*?seclabel u:r:connectivity:s0?g' $< > $@
	$(hide) echo "    socket connectivity stream 0666 root inet" >> $@
	cat $@

include $(CLEAR_VARS)

LOCAL_MODULE := wifi_connect
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := wifi_connect
LOCAL_REQUIRED_MODULES := \
  connectivity \
  connectivity_test \
  dnsmasq \
  init.connectivity.rc \
  wifi_init \

include $(BUILD_PREBUILT)
