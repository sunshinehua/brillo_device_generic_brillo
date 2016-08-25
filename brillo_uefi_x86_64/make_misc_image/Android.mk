#
# Copyright 2016 The Android Open Source Project
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

common_cflags := \
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
common_cppflags := \
    -Wnon-virtual-dtor \
    -fno-strict-aliasing
common_ldflags := \
    -Wl,--gc-sections

include $(CLEAR_VARS)
LOCAL_MODULE := libmiscimg_unittest
LOCAL_MODULE_HOST_OS := linux
LOCAL_CPP_EXTENSION := .cc
LOCAL_CLANG := true
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_LDFLAGS := $(common_ldflags)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/make_misc_image.h \
    $(LOCAL_PATH)/../boot_loader/bub_ab_flow.h \
    $(LOCAL_PATH)/../boot_loader/bub_image_util.h \
    $(LOCAL_PATH)/../boot_loader/bub_sysdeps.h \
    $(LOCAL_PATH)/../boot_loader/bub_util.h \
    external/gtest/include
LOCAL_STATIC_LIBRARIES := \
    libbub_host \
    libgmock_host \
    libgtest_host
LOCAL_SHARED_LIBRARIES := \
    libchrome
LOCAL_SRC_FILES := \
    make_misc_image.cc \
    make_misc_image_unittest.cc \
    ../boot_loader/bub_image_util.cc
LOCAL_LDLIBS_linux := -lrt
include $(BUILD_HOST_NATIVE_TEST)

include $(CLEAR_VARS)
LOCAL_MODULE := make_misc_image
LOCAL_MODULE_HOST_OS := linux
LOCAL_IS_HOST_MODULE := true
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_CPP_EXTENSION := .cc
LOCAL_CLANG := true
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_LDFLAGS := $(common_ldflags)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/make_misc_image.h \
    $(LOCAL_PATH)/../boot_loader/bub_ab_flow.h \
    $(LOCAL_PATH)/../boot_loader/bub_image_util.h \
    $(LOCAL_PATH)/../boot_loader/bub_sysdeps.h \
    $(LOCAL_PATH)/../boot_loader/bub_util.h
LOCAL_STATIC_LIBRARIES := \
    libbub_host \
    libgmock_host \
    libgtest_host
LOCAL_SHARED_LIBRARIES := \
    libchrome
LOCAL_SRC_FILES := \
    main.cc \
    make_misc_image.cc \
    ../boot_loader/bub_image_util.cc
LOCAL_LDLIBS_linux := -lrt
include $(BUILD_HOST_EXECUTABLE)
