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

#include "stagefright_playback.h"

#include <android-base/logging.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/MediaExtractor.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/foundation/AMessage.h>

#include "include/MP3Extractor.h"

#include "SineSource.h"

namespace android {

status_t PlayStagefrightMp3(char* filename, bool wait) {
  CHECK(filename != NULL);
  OMXClient client;
  CHECK_EQ(client.connect(), (status_t)OK);

  FileSource* file_source = new FileSource(filename);
  status_t status = file_source->initCheck();
  if (status != OK) {
    LOG(ERROR) << "Could not open the mp3 file source.";
    return status;
  }

  // Extract track.
  sp<AMessage> message = new AMessage();

  sp<MediaExtractor> media_extractor = new MP3Extractor(file_source, message);
  LOG(INFO) << "Num tracks: " << media_extractor->countTracks();
  sp<MediaSource> media_source = media_extractor->getTrack(0);

  // Decode mp3.
  sp<MetaData> meta_data = media_source->getFormat();
  sp<MediaSource> decoded_source = OMXCodec::Create(
      client.interface(), meta_data, false, media_source);

  // Play mp3.
  AudioPlayer* player = new AudioPlayer(NULL); // Initialize without a source.
  player->setSource(decoded_source);
  status = player->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start playing audio.";
    return status;
  }
  if (wait) {
    sleep(10);
  }
  return status;
}

status_t PlayStagefrightSine(bool wait) {
  AudioPlayer* player = new AudioPlayer(NULL);  // Initialize without a source.
  int kSampleRateHz = 8000;
  int kNumChannels = 2;
  SineSource* sine_source = new SineSource(kSampleRateHz, kNumChannels);
  player->setSource(sine_source);
  status_t status  = player->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start playing audio.";
    return status;
  }
  if (wait) {
    sleep(5);
  }
  player->pause();
  return status;
}

}  //  namespace android
