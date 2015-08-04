/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "testservice"

#include <unistd.h>

#include <log/log.h>

int main(int argc __unused, char **argv __unused) {
    ALOGI("starting");
    while (1) {
        ALOGI("loop iteration");
        sleep(5);
    }
    ALOGI("exiting");
    return 0;
}
