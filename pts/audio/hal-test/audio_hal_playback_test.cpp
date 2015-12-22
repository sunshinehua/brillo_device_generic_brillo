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

// Test app to play audio at the HAL layer.

#include <android-base/logging.h>
#include <audio_utils/sndfile.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <media/AudioTrack.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaSource.h>

#include "SineSource.h"

// This test only supports audio playback with 16-bit PCM.
const audio_format_t kAudioPlaybackFormat = AUDIO_FORMAT_PCM_16_BIT;

// Generate a sinusoidal wave.
//
// Parameters:
//   sample_rate: Rate to sample the sine wave at.
//   num_channels: Number of channels required. For stereo it should be 2 and
//                 for mono should be 1.
//   total_bytes_required: Size of the buffer.
//
// Returns: A pointer to the buffer containing the data.
uint8_t* GenerateData(int sample_rate, int num_channels,
                      size_t total_bytes_required) {
  uint8_t* data = new uint8_t[total_bytes_required];
  android::sp<android::SineSource> source = new android::SineSource(
      sample_rate, num_channels);
  source->start(nullptr); // Initialize without any params.

  // Read sine data.
  size_t num_bytes_copied = 0;
  android::MediaBuffer* buffer = nullptr;
  while (num_bytes_copied < total_bytes_required) {
    android::MediaSource::ReadOptions options;
    source->read(&buffer, &options);
    CHECK(buffer != nullptr);

    if (buffer->size() >= total_bytes_required - num_bytes_copied) {
      // If the amount of bytes left to write is greater than or equal to the
      // size of the buffer, write the amount of bytes left and then exit.
      memcpy(static_cast<void*>(data + num_bytes_copied), buffer->data(),
             total_bytes_required - num_bytes_copied);
      break;
    } else {
      // If the amount of bytes left to write is less than the size of the
      // buffer, write all bytes in the buffer and update num_bytes_copied.
      memcpy(static_cast<void*>(data+num_bytes_copied), buffer->data(),
             buffer->size());
      num_bytes_copied += buffer->size();
    }
  }
  CHECK(buffer != nullptr);
  buffer->release();
  return data;
}

// Play a sine wave.
//
// Parameters:
//   out_stream: A pointer to the output audio stream.
//   config: A pointer to struct that contains audio configuration data.
//
//   Returns: An int which has a non-negative number on success.
int PlaySineWave(audio_stream_out_t* out_stream, audio_config_t* config) {
  // Get buffer size and generate data.
  size_t buffer_size = out_stream->common.get_buffer_size(&out_stream->common);
  int num_channels = audio_channel_count_from_out_mask(config->channel_mask);
  uint8_t* data = GenerateData(config->sample_rate, num_channels, buffer_size);

  const size_t kNumBuffersToWrite = 1000;
  int rc = 0;
  // Write kNumBuffersToWrite buffers to the audio hal.
  for (size_t i = 0; i < kNumBuffersToWrite; i++) {
    size_t bytes_wanted =
        out_stream->common.get_buffer_size(&out_stream->common);
    rc = out_stream->write(
        out_stream, data,
        bytes_wanted <= buffer_size ? bytes_wanted : buffer_size);
    if (rc < 0) {
      LOG(ERROR) << "Writing data to hal failed. (" << strerror(rc) << ")";
      break;
    }
  }
  return rc;
}

// Play audio from a WAV file.
//
// Parameters:
//   out_stream: A pointer to the output audio stream.
//   in_file: A pointer to a SNDFILE object.
//
// Returns: An int which has a non-negative number on success.
int PlayFile(audio_stream_out_t* out_stream, SNDFILE* in_file) {
  size_t buffer_size = out_stream->common.get_buffer_size(&out_stream->common);
  size_t kFrameSize = audio_bytes_per_sample(kAudioPlaybackFormat);
  short* data = new short[buffer_size / kFrameSize];
  int rc = 0;
  sf_count_t frames_read = 1;
  while (frames_read != 0) {
    size_t bytes_wanted =
        out_stream->common.get_buffer_size(&out_stream->common);
    frames_read = sf_readf_short(in_file, data, bytes_wanted / kFrameSize);
    rc = out_stream->write(out_stream, data, frames_read * kFrameSize);
    if (rc < 0) {
      LOG(ERROR) << "Writing data to hal failed. (" << strerror(rc) << ")";
      break;
    }
  }
  return rc;
}

// Prints usage information if input arguments are missing.
void Usage() {
  fprintf(stderr, "Usage: ./audio_hal_playback_test device sample_rate/file\n"
          "If the test passes, you should hear either a beep for a few seconds played "
          "at the specified sample rate or the specified file.\n"
          "device: hex value representing the audio device (see "
          "system/media/audio/include/system/audio.h)\n"
          "Either the sample rate or a file must be passed as an argument.\n"
          "sample_rate: Sample rate to play a sine wave.\n"
          "file: 16-bit PCM wav file to play.\n");
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    Usage();
    return -1;
  }
  // Process command line arguments.
  const int kAudioDeviceBase = 16;
  uint32_t desired_output_device = strtol(
      argv[1], nullptr /* look at full string*/, kAudioDeviceBase);
  const int kSampleRateBase = 10;
  uint32_t desired_sample_rate = strtol(
      argv[2], nullptr /* look at full string*/, kSampleRateBase);
  char* filename = nullptr;
  if (desired_sample_rate == 0) {
    filename = argv[2];
  }

  LOG(INFO) << "Starting audio hal tests.";
  int rc = 0;
  const hw_module_t* module = nullptr;

  // Load audio HAL.
  rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "primary", &module);
  if (rc) {
    LOG(WARNING) << "Could not get primary hw module, trying usb. (" << strerror(rc) << ")";
    rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "usb", &module);
    if (rc) {
      LOG(ERROR) << "Could not get hw module. (" << strerror(rc) << ")";
      return -1;
    }
  }

  // Open audio device.
  CHECK(module != nullptr);
  audio_hw_device_t* audio_device = nullptr;
  rc = audio_hw_device_open(module, &audio_device);
  if (rc) {
    LOG(ERROR) << "Could not open hw device. (" << strerror(rc) << ")";
    return -1;
  }

  // Set to a high number so it doesn't interfere with existing stream handles
  // from AudioFlinger.
  audio_io_handle_t handle = 0x999;
  audio_devices_t output_device =
      static_cast<audio_devices_t>(desired_output_device);
  audio_output_flags_t flags = AUDIO_OUTPUT_FLAG_NONE;
  audio_config_t config;
  SF_INFO file_info;
  SNDFILE* in_file = nullptr;
  if (filename) {
    in_file = sf_open(filename, SFM_READ, &file_info);
    CHECK(in_file != nullptr);
    config.channel_mask = file_info.channels;
    if (!(file_info.format & SF_FORMAT_PCM_16)) {
      LOG(ERROR) << "File must be of format 16-bit PCM.";
      return -1;
    }
    config.format = kAudioPlaybackFormat;
    config.sample_rate = file_info.samplerate;
  } else {
    config.channel_mask = AUDIO_CHANNEL_OUT_STEREO;
    config.format = kAudioPlaybackFormat;
    config.sample_rate = desired_sample_rate;
  }
  LOG(INFO) << "Now playing to " << output_device << " at sample rate "
            << config.sample_rate;
  const char* stream_name = "output_stream";

  // Open audio output stream.
  audio_stream_out_t* out_stream = nullptr;
  CHECK(audio_device != nullptr);
  rc = audio_device->open_output_stream(audio_device, handle, output_device,
                                        flags, &config, &out_stream,
                                        stream_name);
  if (rc) {
    LOG(ERROR) << "Could not open output stream. (" << strerror(rc) << ")";
    return -1;
  }

  // Set volume.
  const float kLeftVolume = 0.75;
  const float kRightVolume = 0.75;
  CHECK(out_stream != nullptr);
  rc = out_stream->set_volume(out_stream, kLeftVolume, kRightVolume);
  if (rc) {
    LOG(ERROR) << "Could not set volume correctly. (" << strerror(rc) << ")";
  }


  if (filename) {
    PlayFile(out_stream, in_file);
  } else {
    PlaySineWave(out_stream, &config);
  }

  // Close output stream and device.
  audio_device->close_output_stream(audio_device, out_stream);
  audio_hw_device_close(audio_device);

  LOG(INFO) << "Done with hal tests";
  return 0;
}
