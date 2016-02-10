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

#include <audio_utils/primitives.h>
#include <base/logging.h>
#include <media/AudioTrack.h>
#include <media/stagefright/MediaSource.h>

#include "utils.h"

namespace android {

void LibmediaPlayback::InitSine(audio_format_t audio_format, int sample_rate,
                                int num_channels) {
  audio_format_ = audio_format;
  sample_rate_ = sample_rate;
  num_channels_ = num_channels;
  in_file_ = nullptr;
  source_ = new SineSource(sample_rate_, num_channels_);
  source_->start(nullptr);  // Initialize with no parameters.

  // Read data from source and store it in a buffer.
  MediaSource::ReadOptions options;
  source_->read(&sine_data_buffer_, &options);
}

void LibmediaPlayback::InitFile(const char* filename) {
  SF_INFO info;
  in_file_ = sf_open(filename, SFM_READ, &info);
  sample_rate_ = info.samplerate;
  num_channels_ = info.channels;
  audio_format_ = ConvertToAudioFormat(info.format);
}

size_t LibmediaPlayback::FillBuffer(void* data, size_t size) {
  CHECK(data);
  if (in_file_) {
    int frames_read = 0;
    int frame_size = audio_bytes_per_sample(audio_format_) * num_channels_;
    int sample_size = audio_bytes_per_sample(audio_format_);
    if (sample_size == 1) {
      void* temp = malloc(size * sizeof(short));
      frames_read = sf_readf_short(in_file_, reinterpret_cast<short*>(temp),
                                   size / frame_size);
      int num_samples = frames_read * frame_size / sample_size;
      memcpy_to_u8_from_i16(reinterpret_cast<uint8_t*>(data),
                            reinterpret_cast<int16_t*>(temp), num_samples);
      free(temp);
    } else if (sample_size == 2) {
      frames_read = sf_readf_short(in_file_, reinterpret_cast<short*>(data),
                                   size / frame_size);
    } else if (sample_size == 4) {
      frames_read = sf_readf_int(in_file_, reinterpret_cast<int*>(data),
                                 size / frame_size);
    } else {
      LOG(ERROR) << "Could not handle file with sample size = " << sample_size;
      return 0;
    }
    size = frame_size * frames_read;
  } else {
    size = (size < sine_data_buffer_->size()) ? size :
        sine_data_buffer_->size();
    memcpy(data, sine_data_buffer_->data(), size);
  }
  return size;
}

void LibmediaPlayback::AudioCallback(int event, void* user, void* info) {
  switch (event) {
    case AudioTrack::EVENT_MORE_DATA: {
      AudioTrack::Buffer* buffer = static_cast<AudioTrack::Buffer*>(info);
      buffer->size = reinterpret_cast<LibmediaPlayback*>(user)->FillBuffer(
          buffer->raw, buffer->size);
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

status_t LibmediaPlayback::Play(int duration_secs) {
  audio_channel_mask_t audio_mask =
      audio_channel_out_mask_from_count(num_channels_);
  const audio_stream_type_t kStreamType = AUDIO_STREAM_MUSIC;
  size_t frame_count = 0;  // Use default value for frame count.
  audio_output_flags_t audio_output_flags = AUDIO_OUTPUT_FLAG_NONE;

  AudioTrack* track = new AudioTrack(
      kStreamType, sample_rate_, audio_format_, audio_mask, frame_count,
      audio_output_flags, LibmediaPlayback::AudioCallback,
      reinterpret_cast<void*>(this));
  status_t status = track->initCheck();
  if (status != OK) {
    LOG(ERROR) << "Audio track initialization failed.";
    return status;
  }

  float volume = 0.9;
  track->setVolume(volume);
  status = track->start();
  if (status != OK) {
    LOG(ERROR) << "Audio track failed to start.";
    return status;
  }

  sleep(duration_secs);
  track->stop();

  if (in_file_)
    sf_close(in_file_);
  else
    sine_data_buffer_->release();

  return status;
}

}  // namespace android
