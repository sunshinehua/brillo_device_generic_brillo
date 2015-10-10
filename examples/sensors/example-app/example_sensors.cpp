/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "example_sensors.h"

#include <stdlib.h>
#include <time.h>

#include <hardware/sensors.h>

SensorBase::SensorBase() {}
SensorBase::~SensorBase() {}

int SensorBase::activate(int handle, int enabled) {
  return 0;
}

int SensorBase::setDelay(int handle, int64_t ns) {
  return 0;
}

int SensorBase::batch(int handle, int flags,
                      int64_t period_ns, int64_t timeout) {
  return 0;
}

int SensorBase::flush(int handle) {
  return 0;
}


Accelerometer::Accelerometer() {}
Accelerometer::~Accelerometer() {}

int Accelerometer::pollEvents(sensors_event_t* data, int count) {
  // Returns fake random values.
  data->acceleration.x = static_cast<float>(random() % kMaxRange);
  data->acceleration.y = static_cast<float>(random() % kMaxRange);
  data->acceleration.z = static_cast<float>(random() % kMaxRange);
  return 1;
}


CustomSensor::CustomSensor() {}
CustomSensor::~CustomSensor() {}

int CustomSensor::pollEvents(sensors_event_t* data, int count) {
  if (data) {
    // For this custom sensor, we will return the hour of the local time.
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    if (now) {
      // Any field can be used to return data for a custom sensor.
      // See definition of struct sensors_event_t in hardware/sensors.h for the
      // available fields.
      data->data[0] = now->tm_hour;
      return 1;
    }
  }
  // Note: poll() return value is the number of events being returned. We use
  // -1 to signal an error.
  return -1;
}
