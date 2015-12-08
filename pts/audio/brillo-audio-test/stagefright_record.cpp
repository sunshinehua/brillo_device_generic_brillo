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

#include "stagefright_record.h"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/AudioSource.h>

namespace android {

status_t LibstagefrightRecordAudio() {
  android::ProcessState::self()->startThreadPool();

  uint32_t kSampleRateHz = 8000;
  AudioSource* audio_source = new AudioSource(AUDIO_SOURCE_MIC, String16(),
                                              kSampleRateHz, 1);
  status_t status = audio_source->initCheck();
  if (status != OK) {
    LOG(ERROR) << "Could not initialize audio source correctly.";
    return status;
  }
  status = audio_source->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start recording audio.";
    return status;
  }
  int duration_secs = 5;
  sleep(duration_secs);

  printf("Done recording. Re-playing audio\n");
  AudioPlayer* player = new AudioPlayer(NULL); // Initialize without a source.
  player->setSource(audio_source);
  status = player->start(true);
  if (status != OK) {
    LOG(ERROR) << "Could not audio playback.";
    return status;
  }
  sleep(duration_secs);
  player->pause();
  return status;
}

}  // namespace android
