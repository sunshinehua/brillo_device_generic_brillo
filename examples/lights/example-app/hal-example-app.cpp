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

#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <hardware/hardware.h>
#include <hardware/lights.h>

int main() {
  const hw_module_t* module = nullptr;
  struct light_device_t* light_device = nullptr;

  int ret = hw_get_module(LIGHTS_HARDWARE_MODULE_ID, &module);
  if (ret || !module) {
    err(1, "Failed to load %s module", LIGHTS_HARDWARE_MODULE_ID);
  }

  ret = module->methods->open(
      module, LIGHT_ID_NOTIFICATIONS,
      reinterpret_cast<struct hw_device_t**>(&light_device));
  if (ret || !light_device) {
    err(1, "Failed to open light device for %s", LIGHT_ID_NOTIFICATIONS);
  }

  struct light_state_t state = {
      color: 1,
      flashMode: LIGHT_FLASH_NONE,
      flashOnMS: 0,
      flashOffMS: 0,
      brightnessMode: 0,
  };

  light_device->set_light(light_device, &state);

  sleep(3);

  state.color = 0;

  light_device->set_light(light_device, &state);

  light_device->common.close(
      reinterpret_cast<struct hw_device_t*>(light_device));

  return 0;
}
