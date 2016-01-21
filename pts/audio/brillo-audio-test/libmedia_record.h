//
// Copyright (C) 2015 The Android Open Source Project
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

#ifndef PTS_BRILLO_AUDIO_TEST_LIBMEDIA_RECORD_H_
#define PTS_BRILLO_AUDIO_TEST_LIBMEDIA_RECORD_H_

#include <vector>

#include <system/audio.h>
#include <utils/Errors.h>

namespace android {

// Records audio using libmedia. After recording the audio for a few seconds,
// the captured audio is saved to a file. Sample usage:
//   LibmediaRecord l_record;
//   l_record.Record(...);
class LibmediaRecord {
 public:
  // Callback function called when audio is being recorded. This function copies
  // the buffer passed in to audio_data. See
  // frameworks/include/av/media/AudioRecord.h for more information.
  static void RecordCallback(int event, void* user, void* info);

  // Callback function called when audio is being replayed. This function copies
  // data from audio_data to the buffer so it can be played. See
  // frameworks/include/av/media/AudioRecord.h for more information.
  static void PlayCallback(int event, void* user, void* info);

  // Records audio for a few seconds and stores it to a file.
  // Parameters:
  //   filename: WAV file to save audio to.
  //   sample_rate: Sample rate in hz.
  //   num_channels: Number of channels to use.
  //   audio_format: Audio format to record in.
  //   duration_secs: Duration in seconds to record for.
  status_t Record(const char* filename, int sample_rate, int num_channels,
                  audio_format_t audio_format, int duration_secs);

 private:
  // A buffer used to store the captured audio.
  static std::vector<int8_t> audio_data;
};

}  // namespace android

#endif  // PTS_BRILLO_AUDIO_TEST_LIBMEDIA_RECORD_H_
