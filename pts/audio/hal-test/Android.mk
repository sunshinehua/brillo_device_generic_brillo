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

LOCAL_PATH := $(call my-dir)

# audio_hal_playback_test
# ==============================================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := audio_hal_playback_test.cpp
LOCAL_MODULE := audio_hal_playback_test
ifdef BRILLO
LOCAL_MODULE_TAGS := eng
endif
LOCAL_CFLAGS += -Wall -Werror
LOCAL_SHARED_LIBRARIES := \
  libaudioutils \
  libbase \
  libhardware \
  liblog \
  libsinesource \
  libstagefright \
  libstagefright_foundation \
  libutils
LOCAL_STATIC_LIBRARIES := libsndfile
LOCAL_C_INCLUDES := \
  $(TOP)/device/generic/brillo/pts/audio/common \
  $(TOP)/system/media/audio_utils/include
include $(BUILD_EXECUTABLE)

# audio_hal_record_test
# ==============================================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := audio_hal_record_test.cpp
LOCAL_MODULE := audio_hal_record_test
ifdef BRILLO
LOCAL_MODULE_TAGS := eng
endif
LOCAL_CFLAGS += -Wall -Werror -Wno-sign-compare
LOCAL_SHARED_LIBRARIES := \
  libaudioutils \
  libbase \
  libhardware \
  liblog \
  libutils
LOCAL_STATIC_LIBRARIES := libsndfile
LOCAL_C_INCLUDES := \
  $(TOP)/system/media/audio_utils/include
include $(BUILD_EXECUTABLE)
