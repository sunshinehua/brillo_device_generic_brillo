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

#include "libmedia_playback.h"

#include <base/logging.h>
#include <media/AudioTrack.h>
#include <media/stagefright/MediaSource.h>

namespace android {

MediaBuffer* LibmediaPlayback::sine_data_buffer_ = nullptr;

void LibmediaPlayback::Init(int sample_rate, int num_channels) {
  sample_rate_ = sample_rate;
  num_channels_ = num_channels;
  source_ = new SineSource(sample_rate_, num_channels_);
  source_->start(nullptr);  // Initialize with no parameters.

  // Read data from source and store it in a buffer.
  MediaSource::ReadOptions options;
  source_->read(&sine_data_buffer_, &options);
}

size_t LibmediaPlayback::FillBuffer(void* data, size_t size) {
  CHECK(data);
  size = (size < sine_data_buffer_->size()) ? size : sine_data_buffer_->size();
  memcpy(data, sine_data_buffer_->data(), size);
  return size;
}

void LibmediaPlayback::AudioCallback(int event, void* user, void* info) {
  switch (event) {
    case AudioTrack::EVENT_MORE_DATA: {
      AudioTrack::Buffer* buffer = static_cast<AudioTrack::Buffer*>(info);
      buffer->size = FillBuffer(buffer->raw, buffer->size);
      break;
    }
    case AudioTrack::EVENT_STREAM_END: {
      break;
    }
    default: {
      fprintf(stderr, "Unexpected audio callback event\n");
      break;
    }
  }
}

status_t LibmediaPlayback::Play(audio_format_t audio_format,
                                int duration_secs) {
  audio_channel_mask_t audio_mask =
      audio_channel_out_mask_from_count(num_channels_);
  const audio_stream_type_t kStreamType = AUDIO_STREAM_MUSIC;
  size_t frame_count = 0;  // Use default value for frame count.
  audio_output_flags_t audio_output_flags = AUDIO_OUTPUT_FLAG_NONE;

  AudioTrack* track = new AudioTrack(
      kStreamType, sample_rate_, audio_format, audio_mask, frame_count,
      audio_output_flags, LibmediaPlayback::AudioCallback);
  status_t status = track->initCheck();
  if (status != OK) {
    LOG(ERROR) << "Audio track initialization failed.";
    return status;
  }

  float volume = 0.7;
  track->setVolume(volume);
  status = track->start();
  if (status != OK) {
    LOG(ERROR) << "Audio track failed to start.";
    return status;
  }

  sleep(duration_secs);
  track->stop();
  // Free memory.
  sine_data_buffer_->release();
  return status;
}

}  // namespace android
