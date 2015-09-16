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

#ifndef EXAMPLE_APP_LIBMEDIA_PLAYBACK_H_
#define EXAMPLE_APP_LIBMEDIA_PLAYBACK_H_

#include <media/stagefright/MediaBuffer.h>

#include "SineSource.h"

namespace android {

// Plays audio (a sine wave) using libmedia. Sample usage:
//   LibmediaPlayback l_play;
//   l_play.Init();
//   l_play.Play();
class LibmediaPlayback {
 public:
  // Initialize data and place the data in a MediaBuffer.
  void Init();

  // Callback function for playing audio. See
  // frameworks/include/av/media/AudioTrack.h for more information.
  static void AudioCallback(int event, void* user, void* info);

  // Play audio as a music stream. This function waits until the playback is
  // completed.
  status_t Play();

 private:
  // Helper function used by AudioCallback. Takes size bytes from
  // sine_data_buffer and puts it in data.
  // Parameters:
  //   data:         pointer to buffer that needs to be filled with audio data.
  //   size:         number of bytes to write to the buffer.
  static size_t FillBuffer(void* data, size_t size);

  // Buffer to store the sine data created in Init().
  static MediaBuffer* sine_data_buffer;

  // Source to create the sine wave.
  sp<SineSource> source;
};

}  // namespace android

#endif  // EXAMPLE_APP_LIBMEDIA_PLAYBACK_H_
