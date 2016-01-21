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

// Test app to play/record audio using libmedia and libstagefright.

#include <stdio.h>
#include <stdlib.h>

#include <base/logging.h>
#include <brillo/flag_helper.h>
#include <brillo/syslog_logging.h>

#include "libmedia_playback.h"
#include "libmedia_record.h"
#include "stagefright_playback.h"
#include "stagefright_record.h"

enum TestMode {
  kPlayLibmedia,
  kPlayStagefrightSine,
  kPlayStagefrightMp3,
  kPlayMultiple,
  kRecordLibmedia,
  kRecordStageFright,
  kInvalid
};

audio_format_t get_format(int sample_size) {
  switch (sample_size) {
    case 1:
      return AUDIO_FORMAT_PCM_8_BIT;
    case 2:
      return AUDIO_FORMAT_PCM_16_BIT;
    case 4:
      return AUDIO_FORMAT_PCM_32_BIT;
    default:
      LOG(ERROR) << "Invalid sample size. Must be 1, 2, or 4.";
      return AUDIO_FORMAT_INVALID;
  }
}

int main(int argc, char* argv[]) {
  // Use libmedia or stagefright.
  DEFINE_bool(libmedia, false, "Use libmedia for playback/recording.");
  DEFINE_bool(stagefright, false, "Use stagefright for playback/recording.");
  // Play or record.
  DEFINE_bool(playback, false, "Play audio.");
  DEFINE_bool(record, false, "Record audio.");
  // Play sine wave or a file.
  DEFINE_bool(sine, false, "Play a sine wave.");
  DEFINE_string(filename, "", "WAV file to play/write to.");
  // Other options.
  DEFINE_int32(duration_secs, 10, "Duration to play/record audio in seconds.");
  DEFINE_int32(num_channels, 1, "Number of channels");
  DEFINE_int32(sample_rate, 48000, "Sample rate.");
  DEFINE_int32(sample_size, 2, "Sample size (in bytes)");

  brillo::FlagHelper::Init(argc, argv, "Brillo Audio Test");
  brillo::InitLog(brillo::kLogToSyslog | brillo::kLogHeader);

  // Check for invalid combinations of arguments.
  if (!(FLAGS_libmedia ^ FLAGS_stagefright)) {
    LOG(ERROR) << "Must select either libmedia or stagefright.";
    return 1;
  }
  if (!(FLAGS_playback ^ FLAGS_record)) {
    LOG(ERROR) << "Must select either playback or record.";
    return 1;
  }
  if (FLAGS_record && FLAGS_filename == "") {
    LOG(ERROR) << "Must provide a filename for recording.";
    return 1;
  }
  if (FLAGS_playback && !(FLAGS_sine ^ (FLAGS_filename != ""))) {
    LOG(ERROR) << "Must provide either a filename or play a sine wave.";
    return 1;
  }

  // Parse the arguments.
  TestMode mode = kInvalid;
  if (FLAGS_libmedia) {
    if (FLAGS_playback)
      mode = kPlayLibmedia;
    else
      mode = kRecordLibmedia;
  } else {
    if (FLAGS_playback) {
      if (FLAGS_sine)
        mode = kPlayStagefrightSine;
      else
        mode = kPlayStagefrightMp3;
    } else {
      mode = kRecordStageFright;
    }
  }

  audio_format_t audio_format = get_format(FLAGS_sample_size);

  android::status_t status = android::OK;
  switch (mode) {
    case kPlayLibmedia: {
      android::LibmediaPlayback l_play;
      l_play.Init(FLAGS_sample_rate, FLAGS_num_channels);
      status = l_play.Play(audio_format, FLAGS_duration_secs);
      break;
    }
    case kPlayStagefrightMp3: {
      status = android::PlayStagefrightMp3(FLAGS_filename.c_str(),
                                           FLAGS_duration_secs);
      break;
    }
    case kPlayStagefrightSine: {
      status = android::PlayStagefrightSine(
          FLAGS_sample_rate, FLAGS_num_channels, FLAGS_duration_secs);
      break;
    }
    case kRecordLibmedia: {
      android::LibmediaRecord l_record;
      status = l_record.Record(FLAGS_filename.c_str(), FLAGS_sample_rate,
                               FLAGS_num_channels, audio_format,
                               FLAGS_duration_secs);
      break;
    }
    case kRecordStageFright: {
      status = android::LibstagefrightRecordAudio(
          FLAGS_filename.c_str(), FLAGS_sample_rate, FLAGS_num_channels,
          FLAGS_duration_secs);
      break;
    }
    default: {
      LOG(ERROR) << "Invalid test mode.";
      return 1;
    }
  }
  if (status == android::OK)
    LOG(INFO) << "brillo_audio_test ran successfully.";
  else
    LOG(INFO) << "brillo_audio_test failed.";
  return status;
}
