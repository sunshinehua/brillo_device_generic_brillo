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

#include <stdlib.h>
#include <stdio.h>
#include <base/logging.h>

#include "libmedia_playback.h"
#include "libmedia_record.h"
#include "stagefright_playback.h"
#include "stagefright_record.h"

enum TestMode {
  kPlayLibmedia,
  kPlayStagefright,
  kPlayMultiple,
  kRecordLibmedia,
  kRecordStageFright,
  kInvalid
};

void usage() {
  fprintf(stderr, "Usage: ./brillo_audio_test [option].\n"
         "play_libmedia - play raw audio stream using libmedia \n"
         "play_stagefright filename - play mp3 using libstagefright \n"
         "play_multiple filename - play multiple audio stream using both"
         " libmedia and libstagefright \n"
         "record_libmedia - record using libmedia\n"
         "record_stagefright - record audio using stagefright\n"
         );
}

TestMode parseMode(char* arg) {
  if (!strcmp(arg, "play_libmedia"))
    return kPlayLibmedia;
  if (!strcmp(arg, "play_stagefright"))
    return kPlayStagefright;
  if (!strcmp(arg, "play_multiple"))
    return kPlayMultiple;
  if (!strcmp(arg, "record_stagefright"))
    return kRecordStageFright;
  if (!strcmp(arg, "record_libmedia"))
    return kRecordLibmedia;
  return kInvalid;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    usage();
    return -1;
  } else {
    android::LibmediaPlayback l_play;
    android::LibmediaRecord l_record;
    TestMode mode = parseMode(argv[1]);
    android::status_t status = android::UNKNOWN_ERROR;
    switch (mode) {
      case kPlayLibmedia:
        l_play.Init();
        status = l_play.Play();
        break;
      case kPlayStagefright:
        status = android::PlayStagefrightMp3(argv[2], true);
        break;
      case kPlayMultiple:
        status = android::PlayStagefrightMp3(argv[2], false);
        if (status != android::OK) {
          LOG(ERROR) << "Could not play mp3 using stagefright.";
          return -1;
        }
        sleep(10);
        l_play.Init();
        status = l_play.Play();
        break;
      case kRecordLibmedia:
        status = l_record.Record();
        break;
      case kRecordStageFright:
        status = android::LibstagefrightRecordAudio();
        break;
      default:
        usage();
        return -1;
    }
    if (status != android::OK) {
      LOG(ERROR) << "Could not play audio correctly.";
      return -1;
    }
  }
  printf("Done\n");
  return 0;
}
