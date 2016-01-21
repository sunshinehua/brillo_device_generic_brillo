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

#ifndef PTS_BRILLO_AUDIO_TEST_STAGEFRIGHT_RECORD_H_
#define PTS_BRILLO_AUDIO_TEST_STAGEFRIGHT_RECORD_H_

#include <utils/Errors.h>

namespace android {

// Record audio using stagefright and save it to a file.
// Parameters:
//   filename: WAV file to save audio to.
//   sample_rate: Sample rate in hz.
//   num_channels: Number of channels to use.
//   duration_secs: Duration to play audio for in seconds.
status_t LibstagefrightRecordAudio(const char* filename, int sample_rate,
                                   int num_channels, int duration_secs);

}  // namespace android

#endif  // PTS_BRILLO_AUDIO_TEST_STAGEFRIGHT_RECORD_H_
