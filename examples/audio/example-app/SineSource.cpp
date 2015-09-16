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

// File copied from frameworks/av/cmds/stagefright

#include "SineSource.h"

#include <math.h>

#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>

namespace android {

SineSource::SineSource(int32_t sampleRate, int32_t numChannels)
    : mStarted(false),
      mSampleRate(sampleRate),
      mNumChannels(numChannels),
      mPhase(0),
      mGroup(NULL) {
    CHECK(numChannels == 1 || numChannels == 2);
}

SineSource::~SineSource() {
    if (mStarted) {
        stop();
    }
}

status_t SineSource::start(MetaData * /* params */) {
    CHECK(!mStarted);

    mGroup = new MediaBufferGroup;
    mGroup->add_buffer(new MediaBuffer(kBufferSize));

    mPhase = 0;
    mStarted = true;

    return OK;
}

status_t SineSource::stop() {
    CHECK(mStarted);

    delete mGroup;
    mGroup = NULL;

    mStarted = false;

    return OK;
}

sp<MetaData> SineSource::getFormat() {
    sp<MetaData> meta = new MetaData;
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
    meta->setInt32(kKeyChannelCount, mNumChannels);
    meta->setInt32(kKeySampleRate, mSampleRate);
    meta->setInt32(kKeyMaxInputSize, kBufferSize);

    return meta;
}

status_t SineSource::read(
        MediaBuffer **out, const ReadOptions * /* options */) {
    *out = NULL;

    MediaBuffer *buffer;
    status_t err = mGroup->acquire_buffer(&buffer);

    if (err != OK) {
        return err;
    }

    size_t frameSize = mNumChannels * sizeof(int16_t);
    size_t numFramesPerBuffer = buffer->size() / frameSize;

    int16_t *ptr = (int16_t *)buffer->data();

    const double k = kFrequency / mSampleRate * (2.0 * M_PI);

    double x = mPhase * k;
    for (size_t i = 0; i < numFramesPerBuffer; ++i) {
        int16_t amplitude = (int16_t)(32767.0 * sin(x));
        *ptr++ = amplitude;
        if (mNumChannels == 2) {
            *ptr++ = amplitude;
        }

        x += k;
    }

    buffer->meta_data()->setInt64(
            kKeyTime, ((int64_t)mPhase * 1000000) / mSampleRate);

    mPhase += numFramesPerBuffer;

    buffer->set_range(0, numFramesPerBuffer * frameSize);

    *out = buffer;

    return OK;
}

}  // namespace android
