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

#include <android-base/logging.h>
#include <audio_utils/sndfile.h>
#include <media/AudioRecord.h>
#include <media/AudioTrack.h>

namespace android {

std::vector<int8_t> LibmediaRecord::audio_data;

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

status_t LibmediaRecord::Record(const char* filename) {
    uint32_t kSampleRateHz = 48000;
    size_t min_frame_count;
    audio_format_t audio_format = AUDIO_FORMAT_PCM_16_BIT;
    audio_channel_mask_t channel_in_mask = AUDIO_CHANNEL_IN_MONO;
    status_t status = AudioRecord::getMinFrameCount(&min_frame_count,
                                                    kSampleRateHz,
                                                    audio_format,
                                                    channel_in_mask);
    if (status != OK) {
      LOG(ERROR) << "Could not get the min frame count.";
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

    audio_source_t audio_source = AUDIO_SOURCE_MIC;
    String16 package_name = String16();
    void* user = NULL;
    sp<AudioRecord> record = new AudioRecord(
        audio_source, kSampleRateHz, audio_format, channel_in_mask,
        package_name, (size_t) (buf_count * frame_count),
        LibmediaRecord::RecordCallback, user, frame_count);
    status = record->initCheck();
    if (status != OK) {
      LOG(ERROR) << "Could not initialize audio record.";
      return status;
    }

    printf("Starting recording. Please make noise into microphone.\n");
    status = record->start();
    if (status != OK) {
      LOG(ERROR) << "Could not start recording.";
      return status;
    }
    // Record for 10 seconds.
    uint32_t duration_secs = 10;
    sleep(duration_secs);
    record->stop();

    printf("Done recording. Writing data to file.\n");
    SF_INFO info;
    info.frames = 0;
    info.samplerate = kSampleRateHz;
    info.channels = audio_channel_count_from_out_mask(channel_in_mask);
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* out_file = sf_open(filename, SFM_WRITE, &info);
    if (out_file == nullptr) {
      LOG(ERROR) << "Could not open file for writing.";
      return INVALID_OPERATION;
    }
    size_t frame_size = audio_bytes_per_sample(audio_format) * info.channels;
    sf_count_t file_frame_count = audio_data.size() / frame_size;
    sf_writef_short(out_file,
                    reinterpret_cast<short int*>(audio_data.data()),
                    file_frame_count);
    sf_close(out_file);
    return status;
}

}  // namespace android
