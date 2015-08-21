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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

int main() {
  struct sensors_module_t* sensor_module;
  sensors_poll_device_1_t* sensor_device;
  sensor_t const* sensor_list;
  sensors_event_t data;
  int index = -1;

  int err = hw_get_module(
      SENSORS_HARDWARE_MODULE_ID, (hw_module_t const**)&sensor_module);
  if (err) {
    fprintf(stderr, "Failed to load %s module: %s\n",
            SENSORS_HARDWARE_MODULE_ID, strerror(-err));
  }

  if (sensor_module) {
    int count = sensor_module->get_sensors_list(sensor_module, &sensor_list);
    printf("Found %d sensors.\n", count);
    for (int i = 0; i < count; ++i) {
      printf("Found %s.\n", sensor_list[i].name);

      if (sensor_list[i].type == SENSOR_TYPE_ACCELEROMETER) {
        index = i;
      }
    }

    if (index == -1) {
      fprintf(stderr, "No accelerometer found.\n");
    }

    err = sensors_open_1(&sensor_module->common, &sensor_device);
    if (err) {
      fprintf(stderr, "Failed to open device for module %s: %s\n",
              SENSORS_HARDWARE_MODULE_ID, strerror(-err));
    }
  }

  if (sensor_device) {
    err = sensor_device->activate(
        reinterpret_cast<struct sensors_poll_device_t*>(sensor_device),
        sensor_list[index].handle, 1);
    if (err) {
      fprintf(stderr, "Failed to enable the accelerometer.\n");
    }

    for (int i = 0; i < 10; ++i) {
      int count = sensor_device->poll(
          reinterpret_cast<struct sensors_poll_device_t*>(sensor_device),
          &data, 1);

      if (!count) {
        fprintf(stderr, "Failed to read data from the accelerometer.\n");
      } else {
        printf("Acceleration: x = %f, y = %f, z = %f\n",
              data.acceleration.x, data.acceleration.y, data.acceleration.z);
      }

      sleep(1);
    }

    err = sensor_device->activate(
        reinterpret_cast<struct sensors_poll_device_t*>(sensor_device),
        sensor_list[index].handle, 0);
    if (err) {
      fprintf(stderr, "Failed to disable the accelerometer.\n");
    }
  }

  return 0;
}
