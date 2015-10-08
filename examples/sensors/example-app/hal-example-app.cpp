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

// This file constains an example app that uses sensors HAL.

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

int main(int argc, char* argv[]) {
  sensors_module_t* sensor_module = nullptr;

  int ret = hw_get_module(
      SENSORS_HARDWARE_MODULE_ID,
      const_cast<hw_module_t const**>(
          reinterpret_cast<hw_module_t**>(&sensor_module)));
  if (ret || !sensor_module) {
    fprintf(stderr, "Failed to load %s module: %s\n",
            SENSORS_HARDWARE_MODULE_ID, strerror(-ret));
    return 1;
  }

  sensor_t const* sensor_list = nullptr;

  int sensor_count = sensor_module->get_sensors_list(sensor_module,
                                                     &sensor_list);
  printf("Found %d sensors\n", sensor_count);

  int accelerometer_index = -1;

  for (int i = 0; i < sensor_count; i++) {
    printf("Found %s\n", sensor_list[i].name);
    if (sensor_list[i].type == SENSOR_TYPE_ACCELEROMETER) {
      accelerometer_index = i;
    }
  }
  if (accelerometer_index == -1) {
    fprintf(stderr, "No accelerometer found\n");
    return 1;
  }

  // sensors_poll_device_1_t is used in HAL versions >= 1.0.
  sensors_poll_device_1_t* sensor_device = nullptr;

  // sensors_open_1 is used in HAL versions >= 1.0.
  ret = sensors_open_1(&sensor_module->common, &sensor_device);
  if (ret || !sensor_device) {
    fprintf(stderr, "Failed to open the accelerometer device\n");
    return 1;
  }

  ret = sensor_device->activate(
      reinterpret_cast<sensors_poll_device_t*>(sensor_device),
      sensor_list[accelerometer_index].handle, 1 /* enabled */);
  if (ret) {
    fprintf(stderr, "Failed to enable the accelerometer\n");
    sensors_close_1(sensor_device);
    return 1;
  }

  const int kNumSamples = 10;
  const int kNumEvents = 1;
  const int kWaitTimeSecs = 1;

  for (int i = 0; i < kNumSamples; i++) {
    sensors_event_t data;
    int event_count = sensor_device->poll(
        reinterpret_cast<sensors_poll_device_t*>(sensor_device),
        &data, kNumEvents);
    if (!event_count) {
      fprintf(stderr, "Failed to read data from the accelerometer\n");
      break;
    } else {
      printf("Acceleration: x = %f, y = %f, z = %f\n",
             data.acceleration.x, data.acceleration.y, data.acceleration.z);
    }

    sleep(kWaitTimeSecs);
  }

  ret = sensor_device->activate(
      reinterpret_cast<sensors_poll_device_t*>(sensor_device),
      sensor_list[accelerometer_index].handle, 0 /* disabled */);
  if (ret) {
    fprintf(stderr, "Failed to disable the accelerometer\n");
    return 1;
  }

  // sensors_close_1 is used in HAL versions >= 1.0.
  ret = sensors_close_1(sensor_device);
  if (ret) {
    fprintf(stderr, "Failed to close the accelerometer device\n");
    return 1;
  }

  return 0;
}
