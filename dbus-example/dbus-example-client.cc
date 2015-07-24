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

#include <unistd.h>

#include <dbus/bus.h>
#include <dbus/message.h>
#include <dbus/object_proxy.h>
#include <utils/Log.h>

#include "constants.h"

// Used implicitly.
#undef LOG_TAG
#define LOG_TAG "dbus-example-client"

int main(int argc, char *argv[]) {
  dbus::Bus::Options options;
  options.bus_type = dbus::Bus::SYSTEM;
  scoped_refptr<dbus::Bus> bus(new dbus::Bus(options));
  CHECK(bus->Connect());
  dbus::ObjectProxy* proxy = bus->GetObjectProxy(
      dbus_example::kServiceName, dbus::ObjectPath(dbus_example::kServicePath));
  CHECK(proxy);

  int32_t token = 1;
  while (true) {
    dbus::MethodCall method_call(dbus_example::kInterface,
                                 dbus_example::kMethodName);
    dbus::MessageWriter writer(&method_call);
    writer.AppendInt32(token);

    ALOGI("Calling %s with %d", dbus_example::kMethodName, token);
    scoped_ptr<dbus::Response> response(
        proxy->CallMethodAndBlock(
            &method_call, dbus::ObjectProxy::TIMEOUT_USE_DEFAULT));
    if (!response) {
      ALOGE("Didn't receive response from server");
    } else {
      int32_t response_token = 0;
      dbus::MessageReader reader(response.get());
      if (!reader.PopInt32(&response_token))
        ALOGE("Missing token in response from server");
      else
        ALOGI("Received %d", response_token);
    }

    token++;
    sleep(1);
  }

  return 0;
}
