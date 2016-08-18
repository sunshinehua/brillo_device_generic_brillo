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
#

LOCAL_PATH := $(my-dir)

bub_common_cflags := \
    -D_FILE_OFFSET_BITS=64 \
    -D_POSIX_C_SOURCE=199309L \
    -Wa,--noexecstack \
    -Werror \
    -Wall \
    -Wextra \
    -Wformat=2 \
    -Wno-psabi \
    -Wno-unused-parameter \
    -ffunction-sections \
    -fstack-protector-strong \
    -fvisibility=hidden
bub_common_cppflags := \
    -Wnon-virtual-dtor \
    -fno-strict-aliasing
bub_common_ldflags := \
    -Wl,--gc-sections


# Build for the host (for unit tests).
include $(CLEAR_VARS)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_MODULE := libbub_host
LOCAL_MODULE_HOST_OS := linux
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_CLANG := true
LOCAL_CFLAGS := $(bub_common_cflags) -fno-stack-protector -DBUB_ENABLE_DEBUG -DBUB_COMPILATION
LOCAL_LDFLAGS := $(bub_common_ldflags)
LOCAL_C_INCLUDES :=
LOCAL_SRC_FILES := \
    bub_ab_flow.c \
    bub_sysdeps_posix.c \
    bub_util.c \
    bub_crc32.c
include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libbub_host_unittest
LOCAL_MODULE_HOST_OS := linux
LOCAL_CPP_EXTENSION := .cc
LOCAL_CLANG := true
LOCAL_CFLAGS := $(bub_common_cflags) -DBUB_ENABLE_DEBUG -DBUB_COMPILATION
LOCAL_CPPFLAGS := $(bub_common_cppflags)
LOCAL_LDFLAGS := $(bub_common_ldflags)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/bub_ab_flow.h \
    $(LOCAL_PATH)/bub_image_util.h \
    $(LOCAL_PATH)/bub_sysdeps.h \
    $(LOCAL_PATH)/bub_util.h \
    external/gtest/include
LOCAL_STATIC_LIBRARIES := \
    libbub_host \
    libgmock_host \
    libgtest_host
LOCAL_SHARED_LIBRARIES := \
    libchrome
LOCAL_SRC_FILES := \
    bub_ab_flow_unittest.cc \
    bub_image_util.cc
LOCAL_LDLIBS_linux := -lrt
include $(BUILD_HOST_NATIVE_TEST)
