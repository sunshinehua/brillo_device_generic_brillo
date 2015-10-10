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

#ifndef SENSORS_SENSORS_HAL_H
#define SENSORS_SENSORS_HAL_H

/*
 * SENSOR_TYPE_CUSTOM
 * The custom sensor type must have a integer constant starting from 27.
 * The string name of the custom type must also be defined.
 */
#define SENSOR_TYPE_CUSTOM         (27)
#define SENSOR_STRING_TYPE_CUSTOM  "brillo.custom"

#include <stdint.h>

#include <hardware/sensors.h>

#include "example_sensors.h"

class SensorContext {
 public:
  sensors_poll_device_1_t device;

  SensorContext(const hw_module_t* module);
  ~SensorContext();

  enum AvailableSensors {
    kAccelerometer,
    kCustom,
    kNumSensors
  };

 private:
  int activate(int handle, int enabled);
  int setDelay(int handle, int64_t ns);
  int pollEvents(sensors_event_t* data, int count);
  int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
  int flush(int handle);

  // The wrapper pass through to the specific instantiation of
  // the SensorContext.
  static int CloseWrapper(hw_device_t* dev);
  static int ActivateWrapper(sensors_poll_device_t* dev, int handle,
                             int enabled);
  static int SetDelayWrapper(sensors_poll_device_t* dev, int handle,
                             int64_t ns);
  static int PollEventsWrapper(sensors_poll_device_t* dev,
                               sensors_event_t* data, int count);
  static int BatchWrapper(sensors_poll_device_1_t* dev, int handle, int flags,
                          int64_t period_ns, int64_t timeout);
  static int FlushWrapper(sensors_poll_device_1_t* dev, int handle);

  SensorBase* sensors[AvailableSensors::kNumSensors];
};

#endif  // SENSORS_SENSORS_HAL_H
