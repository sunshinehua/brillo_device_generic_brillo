//
// Copyright (C) 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "utils.h"

#include <audio_utils/sndfile.h>
#include <base/logging.h>

namespace android {

int ConvertToSndFileFormat(audio_format_t format) {
  int sndfile_format = SF_FORMAT_WAV;
  switch (format) {
    case AUDIO_FORMAT_PCM_8_BIT:
      sndfile_format |= SF_FORMAT_PCM_U8;
      break;
    case AUDIO_FORMAT_PCM_16_BIT:
      sndfile_format |= SF_FORMAT_PCM_16;
      break;
    case AUDIO_FORMAT_PCM_32_BIT:
      sndfile_format |= SF_FORMAT_PCM_32;
      break;
    default:
      LOG(ERROR) << "Unable to convert format.";
      sndfile_format = -1;
  }
  return sndfile_format;
}

audio_format_t ConvertToAudioFormat(int format) {
  audio_format_t audio_format;
  format &= SF_FORMAT_SUBMASK;
  switch (format) {
    case SF_FORMAT_PCM_U8:
      audio_format = AUDIO_FORMAT_PCM_8_BIT;
      break;
    case SF_FORMAT_PCM_16:
      audio_format = AUDIO_FORMAT_PCM_16_BIT;
      break;
    case SF_FORMAT_PCM_32:
      audio_format = AUDIO_FORMAT_PCM_32_BIT;
      break;
    default:
      LOG(ERROR) << "Unable to convert format.";
      audio_format = AUDIO_FORMAT_INVALID;
  }
  return audio_format;
}

}  // namespace android
