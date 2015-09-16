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

#include "libmedia_record.h"

#include <base/logging.h>
#include <media/AudioRecord.h>
#include <media/AudioTrack.h>

namespace android {

std::vector<int8_t> LibmediaRecord::audio_data;
size_t LibmediaRecord::bytes_transferred_so_far = 0;

// Callback for recording audio.
void LibmediaRecord::RecordCallback(int event, void* user, void* info) {
  switch (event) {
    case AudioRecord::EVENT_MORE_DATA: {
      AudioRecord::Buffer* buffer = static_cast<AudioRecord::Buffer*>(info);
      for (size_t i = 0; i < buffer->size; i++) {
        audio_data.push_back(buffer->i8[i]);
      }
      break;
    }
    case AudioRecord::EVENT_OVERRUN: {
      fprintf(stderr, "overrun reported\n");
      break;
    }
    default: {
      fprintf(stderr, "Unexpected record callback event\n");
      break;
    }
  }
}

// Callback for playing recorded audio.
void LibmediaRecord::PlayCallback(int event, void* user, void* info) {
  switch (event) {
    case AudioTrack::EVENT_MORE_DATA: {
      AudioTrack::Buffer* buffer = (AudioTrack::Buffer*) info;
      for (size_t i = 0; i < buffer->size; i++) {
        buffer->i8[i] = audio_data[bytes_transferred_so_far];
        bytes_transferred_so_far++;
      }
      break;
    }
    default: {
      fprintf(stderr, "Unexpected play callback event\n");
      break;
    }
  }
}

status_t LibmediaRecord::Record() {
    uint32_t kSampleRateHz = 8000;
    size_t min_frame_count;
    status_t status = AudioRecord::getMinFrameCount(&min_frame_count,
                                                    kSampleRateHz,
                                                    AUDIO_FORMAT_PCM_16_BIT,
                                                    AUDIO_CHANNEL_IN_MONO);
    if (status != OK) {
      ALOGE("Could not get the min frame count.");
      return status;
    }

    // Make sure that the AudioRecord callback never returns more than the
    // maximum buffer size.
    int max_buffer_size = 2048;
    uint32_t channel_count = 1;
    uint32_t frame_count = max_buffer_size / sizeof(int16_t) / channel_count;

    // Make sure that the AudioRecord total buffer size is large enough.
    size_t buf_count = 2;
    while ((buf_count * frame_count) < min_frame_count) {
      buf_count++;
    }

    sp<AudioRecord> record = new AudioRecord(
        AUDIO_SOURCE_MIC, kSampleRateHz, AUDIO_FORMAT_PCM_16_BIT,
        AUDIO_CHANNEL_IN_MONO, String16(), (size_t) (buf_count * frame_count),
        LibmediaRecord::RecordCallback, NULL, frame_count);
    status = record->initCheck();
    if (status != OK) {
      ALOGE("Could not initialize audio record.");
      return status;
    }

    printf("Starting recording. Please make noise into microphone.\n");
    status = record->start();
    if (status != OK) {
      ALOGE("Could not start recording.");
      return status;
    }
    // Record for 10 seconds before playback starts.
    uint32_t duration_secs = 10;
    sleep(duration_secs);
    record->stop();

    printf("Starting playback.\n");
    AudioTrack* playback = new AudioTrack(
        AUDIO_STREAM_MUSIC, kSampleRateHz, AUDIO_FORMAT_PCM_16_BIT,
        AUDIO_CHANNEL_OUT_MONO, 0, AUDIO_OUTPUT_FLAG_NONE,
        LibmediaRecord::PlayCallback);
    status = playback->initCheck();
    if (status != OK) {
      ALOGE("Could not initialize audio playback.");
      return status;
    }
    status = playback->start();
    if (status != OK) {
      ALOGE("Could not start playback.");
      return status;
    }
    sleep(duration_secs);
    playback->stop();
    return status;
}

}  // namespace android
