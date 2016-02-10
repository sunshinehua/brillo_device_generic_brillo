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

#include <audio_utils/primitives.h>
#include <audio_utils/sndfile.h>
#include <base/logging.h>
#include <media/AudioRecord.h>
#include <media/AudioTrack.h>

#include "utils.h"

namespace android {

void LibmediaRecord::RecordCallback(int event, void* user, void* info) {
  switch (event) {
    case AudioRecord::EVENT_MORE_DATA: {
      AudioRecord::Buffer* buffer = static_cast<AudioRecord::Buffer*>(info);
      LibmediaRecord* lr = reinterpret_cast<LibmediaRecord*>(user);
      for (size_t i = 0; i < buffer->size; i++) {
        lr->audio_data_.push_back(buffer->i8[i]);
      }
      break;
    }
    case AudioRecord::EVENT_OVERRUN: {
      LOG(ERROR) << "Overrun reported.";
      break;
    }
    default: {
      LOG(ERROR) << "Unexpected callback event.";
      break;
    }
  }
}

status_t LibmediaRecord::Record(const char* filename, int sample_rate,
                                int num_channels, audio_format_t audio_format,
                                int duration_secs) {
  size_t min_frame_count;
  audio_channel_mask_t channel_in_mask =
      audio_channel_in_mask_from_count(num_channels);
  status_t status = AudioRecord::getMinFrameCount(
      &min_frame_count, sample_rate, audio_format, channel_in_mask);
  if (status != OK) {
    LOG(ERROR) << "Could not get the min frame count.";
    return status;
  }

  // Make sure that the AudioRecord callback never returns more than the
  // maximum buffer size.
  int max_buffer_size = 2048;
  uint32_t frame_count = max_buffer_size / (
      audio_bytes_per_sample(audio_format) * num_channels);

  // Make sure that the AudioRecord total buffer size is large enough.
  size_t buf_count = 2;
  while ((buf_count * frame_count) < min_frame_count) {
    buf_count++;
  }

  audio_source_t audio_source = AUDIO_SOURCE_DEFAULT;
  String16 package_name = String16();
  void* user = reinterpret_cast<void*>(this);
  sp<AudioRecord> record =
      new AudioRecord(audio_source, sample_rate, audio_format, channel_in_mask,
                      package_name, (size_t)(buf_count * frame_count),
                      LibmediaRecord::RecordCallback, user, frame_count);
  status = record->initCheck();
  if (status != OK) {
    LOG(ERROR) << "Could not initialize audio record.";
    return status;
  }

  LOG(INFO) << "Starting recording. Please make noise into microphone.";
  status = record->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start recording.";
    return status;
  }
  // Record for the specified number of seconds.
  sleep(duration_secs);
  record->stop();

  LOG(INFO) << "Done recording. Writing data to file.";
  SF_INFO info;
  info.frames = 0;
  info.samplerate = sample_rate;
  info.channels = num_channels;
  info.format = ConvertToSndFileFormat(audio_format);
  if (info.format == -1) {
    LOG(ERROR) << "Unsupported format. Cannot write to file.";
    return BAD_TYPE;
  }

  SNDFILE* out_file = sf_open(filename, SFM_WRITE, &info);
  if (out_file == nullptr) {
    LOG(ERROR) << "Could not open file for writing.";
    return INVALID_OPERATION;
  }
  size_t frame_size = audio_bytes_per_sample(audio_format) * num_channels;
  sf_count_t file_frame_count = audio_data_.size() / frame_size;
  if (audio_format == AUDIO_FORMAT_PCM_8_BIT) {
    short* temp = new short[audio_data_.size()];
    memcpy_to_i16_from_u8(temp, reinterpret_cast<uint8_t*>(audio_data_.data()),
                          audio_data_.size());
    int frames_written = sf_writef_short(out_file, temp, file_frame_count);
    CHECK(frames_written == file_frame_count);
    delete[] temp;
  } else if (audio_format == AUDIO_FORMAT_PCM_16_BIT) {
    int frames_written = sf_writef_short(
        out_file, reinterpret_cast<short int*>(audio_data_.data()),
        file_frame_count);
    CHECK(frames_written == file_frame_count);
  } else if (audio_format == AUDIO_FORMAT_PCM_32_BIT) {
    int frames_written = sf_writef_int(
        out_file, reinterpret_cast<int*>(audio_data_.data()), file_frame_count);
    CHECK(frames_written == file_frame_count);
  } else {
    LOG(ERROR) << "Audio format not supported. Could not write to file.";
  }
  sf_close(out_file);
  return status;
}

}  // namespace android
