/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>

#define LOG_TAG "wifi_init"

#include <cutils/log.h>
#include <hardware_brillo/wifi_driver_hal.h>
#include <hardware/hardware.h>

void usage(char* cmd) {
  printf("Usage: %s [<ap|client>]\n", cmd);
}

int wifi_init(wifi_driver_device_t* driver) {
  wifi_driver_error error;

  error = (*driver->wifi_driver_initialize)();
  if (error != WIFI_SUCCESS) {
    ALOGE("WiFi driver init failed: %d", error);
    return error;
  }

  return WIFI_SUCCESS;
}

int wifi_setup(wifi_driver_device_t* driver,
               wifi_driver_mode mode) {
  char device_name[DEFAULT_WIFI_DEVICE_NAME_SIZE];
  wifi_driver_error error;

  error = (*driver->wifi_driver_set_mode)(mode, device_name,
                                            sizeof(device_name));
  if (error != WIFI_SUCCESS) {
    ALOGE("WiFi driver setup failed: %d", error);
    return error;
  }

  printf("%s\n", device_name);
  return WIFI_SUCCESS;
}

int wifi_get_hal(wifi_driver_device_t** pDriver) {
  const hw_module_t* module;
  int ret;

  ret = hw_get_module(WIFI_DRIVER_DEVICE_ID_MAIN, &module);
  if (ret != 0) {
    ALOGE("Failed to find HAL module");
    return WIFI_ERROR_NOT_AVAILABLE;
  }

  if (wifi_driver_open(module, pDriver) != 0) {
    ALOGE("Failed to open WiFi HAL module");
    return WIFI_ERROR_UNKNOWN;
  }

  return WIFI_SUCCESS;
}

int main(int argc, char** argv) {
  char* mode;
  wifi_driver_error error;
  wifi_driver_device_t* driver;

  if (argc < 1 || argc > 2) {
    usage(argv[0]);
    return 1;
  }

  if (wifi_get_hal(&driver) != WIFI_SUCCESS)
    return 1;

  error = wifi_init(driver);
  if (error != WIFI_SUCCESS) {
    // wifi_init should already have logged an error.
    wifi_driver_close(driver);
    return 1;
  }

  if (argc == 2) {
    mode = argv[1];
    if (strcmp(mode, "ap") == 0) {
      error = wifi_setup(driver, WIFI_MODE_AP);
    } else if (strcmp(mode, "client") == 0) {
      error = wifi_setup(driver, WIFI_MODE_STATION);
    } else {
      usage(argv[0]);
      wifi_driver_close(driver);
      return 1;
    }
  }

  wifi_driver_close(driver);

  if (error != WIFI_SUCCESS) {
    ALOGE("wifi_init returned error %d", error);
    return 1;
  }

  return 0;
}
