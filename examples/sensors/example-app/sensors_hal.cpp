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

#include "sensors_hal.h"

#include <errno.h>
#include <string.h>

#include "example_sensors.h"

SensorContext::SensorContext(const hw_module_t* module) {
  memset(&device, 0, sizeof(device));

  device.common.tag = HARDWARE_DEVICE_TAG;
  device.common.version = SENSORS_DEVICE_API_VERSION_1_0;
  device.common.module = const_cast<hw_module_t*>(module);
  device.common.close = CloseWrapper;
  device.activate = ActivateWrapper;
  device.setDelay = SetDelayWrapper;
  device.poll = PollEventsWrapper;
  device.batch = BatchWrapper;
  device.flush = FlushWrapper;

  sensors[AvailableSensors::kAccelerometer] = new Accelerometer();
  sensors[AvailableSensors::kCustom] = new CustomSensor();
}

SensorContext::~SensorContext() {
  for (int i = 0; i < AvailableSensors::kNumSensors; i++) {
    if (sensors[i]) delete sensors[i];
  }
}

int SensorContext::activate(int handle, int enabled) {
  return sensors[handle]->activate(handle, enabled);
}

int SensorContext::setDelay(int handle, int64_t ns) {
  return sensors[handle]->setDelay(handle, ns);
}

int SensorContext::pollEvents(sensors_event_t* data, int count) {
  return sensors[0]->pollEvents(data, count);
}

int SensorContext::batch(int handle, int flags,
                         int64_t period_ns, int64_t timeout) {
  return sensors[handle]->batch(handle, flags, period_ns, timeout);
}

int SensorContext::flush(int handle) {
  return sensors[handle]->flush(handle);
}

// static
int SensorContext::CloseWrapper(hw_device_t* dev) {
  SensorContext* sensor_context = reinterpret_cast<SensorContext*>(dev);
  if (sensor_context) {
    delete sensor_context;
  }
  return 0;
}

// static
int SensorContext::ActivateWrapper(sensors_poll_device_t* dev,
                                   int handle, int enabled) {
  return reinterpret_cast<SensorContext*>(dev)->activate(handle, enabled);
}

// static
int SensorContext::SetDelayWrapper(sensors_poll_device_t* dev,
                                   int handle, int64_t ns) {
  return reinterpret_cast<SensorContext*>(dev)->setDelay(handle, ns);
}

// static
int SensorContext::PollEventsWrapper(sensors_poll_device_t* dev,
                                     sensors_event_t* data, int count) {
  return reinterpret_cast<SensorContext*>(dev)->pollEvents(data, count);
}

// static
int SensorContext::BatchWrapper(sensors_poll_device_1_t* dev, int handle,
                                int flags, int64_t period_ns, int64_t timeout) {
  return reinterpret_cast<SensorContext*>(dev)->batch(handle, flags, period_ns,
                                                      timeout);
}

// static
int SensorContext::FlushWrapper(sensors_poll_device_1_t* dev,
                                int handle) {
  return reinterpret_cast<SensorContext*>(dev)->flush(handle);
}


static int open_sensors(const struct hw_module_t* module,
                        const char* id, struct hw_device_t** device) {
  SensorContext* ctx = new SensorContext(module);
  *device = &ctx->device.common;

  return 0;
}

static struct hw_module_methods_t sensors_module_methods = {
  open: open_sensors,
};

static struct sensor_t kSensorList[] = {
  { name: "Broken Accelerometer",
    vendor: "Unknown",
    version: 1,
    handle: SensorContext::AvailableSensors::kAccelerometer,
    type: SENSOR_TYPE_ACCELEROMETER,
    maxRange: static_cast<float>(Accelerometer::kMaxRange),
    resolution: 100.0f,
    power: 0.0f,
    minDelay: 10000,
    fifoReservedEventCount: 0,
    fifoMaxEventCount: 0,
    SENSOR_STRING_TYPE_ACCELEROMETER,
    requiredPermission: "",
    maxDelay: 0,
    flags: SENSOR_FLAG_CONTINUOUS_MODE,
    reserved: {},
  },
  { name: "Custom Hour of Day Sensor",
    vendor: "Unknown",
    version: 1,
    handle: SensorContext::AvailableSensors::kCustom,
    type: SENSOR_TYPE_CUSTOM,
    maxRange: 24.0f,
    resolution: 1.0f,
    power: 0.0f,
    minDelay: 1,
    fifoReservedEventCount: 0,
    fifoMaxEventCount: 0,
    SENSOR_STRING_TYPE_CUSTOM,
    requiredPermission: "",
    maxDelay: 0,
    flags: SENSOR_FLAG_CONTINUOUS_MODE,
    reserved: {},
  },
};

static int get_sensors_list(struct sensors_module_t* module,
                            struct sensor_t const** list) {
  if (!list) return 0;
  *list = kSensorList;
  return sizeof(kSensorList) / sizeof(kSensorList[0]);
}

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name: "Example Sensor Module",
        author: "Google",
        methods: &sensors_module_methods,
        dso: NULL,
        reserved: {0},
    },
    get_sensors_list: get_sensors_list,
    set_operation_mode: NULL
};
