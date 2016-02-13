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

#include <base/logging.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/MediaExtractor.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/SimpleDecodingSource.h>
#include <media/stagefright/foundation/AMessage.h>

#include "include/MP3Extractor.h"

#include "SineSource.h"

namespace android {

status_t PlayStagefrightMp3(const char* filename, int duration_secs) {
  CHECK(filename);

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
  sp<MediaSource> decoded_source =
      SimpleDecodingSource::Create(media_source);

  // Play mp3.
  AudioPlayer* player = new AudioPlayer(nullptr);  // Initialize without source.
  player->setSource(decoded_source);
  status = player->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start playing audio.";
    return status;
  }
  sleep(duration_secs);
  return status;
}

status_t PlayStagefrightSine(int sample_rate, int num_channels,
                             int duration_secs) {
  AudioPlayer* player = new AudioPlayer(nullptr);  // Initialize without source.
  SineSource* sine_source = new SineSource(sample_rate, num_channels);
  player->setSource(sine_source);
  status_t status = player->start();
  if (status != OK) {
    LOG(ERROR) << "Could not start playing audio.";
    return status;
  }
  sleep(duration_secs);
  player->pause();
  return status;
}

}  //  namespace android
