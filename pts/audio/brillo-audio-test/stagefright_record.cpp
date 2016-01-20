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
#include <audio_utils/sndfile.h>
#include <binder/ProcessState.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/AudioSource.h>

namespace android {

status_t LibstagefrightRecordAudio(const char* filename) {
  android::ProcessState::self()->startThreadPool();

  uint32_t kSampleRateHz = 48000;
  uint32_t kNumChannels = 1;
  AudioSource* audio_source = new AudioSource(AUDIO_SOURCE_DEFAULT, String16(),
                                              kSampleRateHz, kNumChannels);
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
  int duration_secs = 10;
  sleep(duration_secs);

  printf("Recorded 10 secs of audio. Saving audio to file\n");

  SF_INFO info;
  info.frames = 0;
  info.samplerate = kSampleRateHz;
  info.channels = kNumChannels;
  info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  SNDFILE* out_file = sf_open(filename, SFM_WRITE, &info);
  if (out_file == nullptr) {
    LOG(ERROR) << "Could not open file for writing.";
    return INVALID_OPERATION;
  }
  // Under the hood, AudioSource uses AUDIO_FORMAT_PCM_16_BIT.
  audio_format_t kAudioFormat = AUDIO_FORMAT_PCM_16_BIT;
  size_t frame_size = audio_bytes_per_sample(kAudioFormat) * kNumChannels;
  int num_bytes_to_write = (kNumChannels *
                            audio_bytes_per_sample(kAudioFormat) *
                            kSampleRateHz *
                            duration_secs);
  int num_bytes_written = 0;
  // We don't have to worry about reading more data than has been recorded since
  // audio_source is still recording.
  while (num_bytes_written < num_bytes_to_write) {
    MediaBuffer* buffer = NULL;
    audio_source->read(&buffer, NULL /* no read options */);
    if (buffer == NULL) {
      break;
    }
    sf_count_t frame_count = buffer->size() / frame_size;
    sf_writef_short(out_file,
                    reinterpret_cast<short int*>(buffer->data()),
                    frame_count);
    num_bytes_written += buffer->size();
  }
  sf_close(out_file);

  return status;
}

}  // namespace android
