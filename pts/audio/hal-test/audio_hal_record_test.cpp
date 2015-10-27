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

// Test app to record audio at the HAL layer.

#include <stdio.h>
#include <time.h>
#include <vector>

#include <base/logging.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: ./audio_hal_record_test device sample_rate filename\n"
            "If the test passes, the noises made during recording should be "
            "saved to the specified file.\n"
            "device: hex value representing the audio device (see "
            "system/media/audio/include/system/audio.h). Note that the "
            "AUDIO_DEVICE_BIT_IN mask is optional.\n"
            "sample_rate: Sample rate to record audio at.\n"
            "filename: File to save the raw data to so it can be analyzed on a "
            "computer.\n");
    return -1;
  }
  // Process command line arguments.
  const int kAudioDeviceBase = 16;
  uint32_t desired_input_device = strtol(
      argv[1], nullptr /* look at full string*/, kAudioDeviceBase);
  CHECK_GT(desired_input_device, 0);
  desired_input_device |= AUDIO_DEVICE_BIT_IN;

  uint32_t desired_sample_rate = atoi(argv[2]);
  CHECK_GT(desired_sample_rate, 0);
  const char* output_filename = argv[3];
  CHECK(output_filename != nullptr);

  LOG(INFO) << "Starting audio hal recording test.";
  int rc = 0;
  const hw_module_t* module = nullptr;

  // Load audio HAL.
  rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "primary", &module);
  if (rc) {
    LOG(ERROR) << "Could not get hw module. (" << strerror(-rc) << ")";
    return -1;
  }
  CHECK(module != nullptr);

  // Load audio device.
  audio_hw_device_t* audio_device = nullptr;
  rc = audio_hw_device_open(module, &audio_device);
  if (rc) {
    LOG(ERROR) << "Could not open hw device. (" << strerror(-rc) << ")";
    return -1;
  }
  CHECK(audio_device != nullptr);

  // Set to a high number so it doesn't interfere with existing stream handles
  // from AudioFlinger.
  audio_io_handle_t handle = 0x999;
  audio_devices_t input_device =
      static_cast<audio_devices_t>(desired_input_device);
  audio_config_t config;
  config.channel_mask = AUDIO_CHANNEL_IN_MONO;
  config.format = AUDIO_FORMAT_PCM_16_BIT;
  config.sample_rate = desired_sample_rate;
  LOG(INFO) << "Now recording from " << input_device << " at sample rate "
            << config.sample_rate;
  audio_input_flags_t flags = AUDIO_INPUT_FLAG_NONE;
  const char* kStreamName = "input_stream";
  const audio_source_t kInputSource = AUDIO_SOURCE_DEFAULT;

  // Open audio input stream.
  audio_stream_in_t* in_stream = nullptr;
  rc = audio_device->open_input_stream(audio_device, handle, input_device,
                                       &config, &in_stream, flags, kStreamName,
                                       kInputSource);
  if (rc) {
    LOG(ERROR) << "Could not open input stream. (" << strerror(-rc) << ")";
    return -1;
  }

  // Create a storage buffer to store all recorded data.
  const size_t kRecordedDataSize = 100 * 1000;  // 100k of storage.
  std::vector<uint8_t> recorded_data(kRecordedDataSize);

  // Get buffer size to get upper bound on data to read from the HAL.
  size_t buffer_size = in_stream->common.get_buffer_size(&in_stream->common);

  // Set the input source to AUDIO_SOURCE_MIC.
  in_stream->common.set_parameters(&in_stream->common, "input_source=1");

  // Capture kRecordedDataSize bytes from the microphone.
  printf("Please speak into the microphone for several seconds.\n");
  time_t start_time = time(0);
  double kMaxLoopTime = 30;
  size_t buffer_offset = 0;
  while (buffer_offset < kRecordedDataSize) {
    size_t bytes_to_copy =
        (buffer_offset + buffer_size < kRecordedDataSize) ? buffer_size :
        kRecordedDataSize - buffer_offset;
    ssize_t bytes_read = in_stream->read(
        in_stream, recorded_data.data() + buffer_offset, bytes_to_copy);
    if (bytes_read < 0) {
      // Timer to exit loop after 30 seconds.
      if (difftime(time(0), start_time) > kMaxLoopTime) {
        LOG(ERROR) << "Test timed out.";
        break;
      }
      // Did not read any bytes this iteration.
      continue;
    } else {
      buffer_offset += bytes_read;
    }
  }

  // Close input stream and device.
  audio_device->close_input_stream(audio_device, in_stream);
  audio_hw_device_close(audio_device);

  // Save results to a file for offline analysis.
  FILE* out_file = fopen(output_filename, "w");
  if (out_file == NULL) {
    LOG(ERROR) << "Could not open output file.";
    return -1;
  }
  fwrite(reinterpret_cast<void*>(recorded_data.data()), sizeof(uint8_t),
         buffer_offset, out_file);
  fclose(out_file);

  // TODO(ralphnathan): Allow for file playback using the audio_hal_play_test
  // instead of having to do adb pull and then play on the host. (b/25183495)

  // Print instructions to access the file.
  int num_channels = audio_channel_count_from_out_mask(config.channel_mask);
  printf("The audio recording has been saved to %s. Please use adb pull to get "
         "the file and play it using audacity. The audio data has the "
         "following characteristics:\nsample rate: %i\nformat: 16 bit pcm\n"
         "num channels: %i\n",
         output_filename, desired_sample_rate, num_channels);
  LOG(INFO) << "Done with hal record test";
  return 0;
}
