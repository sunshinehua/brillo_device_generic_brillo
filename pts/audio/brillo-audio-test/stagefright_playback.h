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

#ifndef EXAMPLE_APP_STAGEFRIGHT_PLAYBACK_H_
#define EXAMPLE_APP_STAGEFRIGHT_PLAYBACK_H_

#include <utils/Errors.h>

namespace android {

// Plays audio using stagefright. Specifically, this function opens a .mp3 file,
// decodes it and plays it.
// Parameters:
//   filename:  MP3 file to be played.
//   wait:      Bool to indicate whether this function should sleep or not. If
//              true, the function waits for 10 seconds before returning.
status_t PlayStagefrightMp3(char* filename, bool wait);

// Plays audio using stagefright. This function plays a sine wave for 10 secs.
// Parameters:
//   wait:      Bool to indicate whether this function should sleep or not. If
//              true, the function waits for 5 seconds before returning.
status_t PlayStagefrightSine(bool wait);
}  // namespace android

#endif  // EXAMPLE_APP_STAGEFRIGHT_PLAYBACK_H_
