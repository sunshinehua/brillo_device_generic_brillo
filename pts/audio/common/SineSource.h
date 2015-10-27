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

#ifndef SINE_SOURCE_H_

#define SINE_SOURCE_H_

#include <media/stagefright/MediaSource.h>
#include <utils/Compat.h>

namespace android {

class MediaBufferGroup;

struct SineSource : public MediaSource {
    SineSource(int32_t sampleRate, int32_t numChannels);

    virtual status_t start(MetaData *params);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **out, const ReadOptions *options = NULL);

protected:
    virtual ~SineSource();

private:
    enum { kBufferSize = 8192 };
    static const CONSTEXPR double kFrequency = 500.0;

    bool mStarted;
    int32_t mSampleRate;
    int32_t mNumChannels;
    size_t mPhase;

    MediaBufferGroup *mGroup;
};

}  // namespace android

#endif // SINE_SOURCE_H_
