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

#include <base/at_exit.h>
#include <base/bind.h>
#include <base/memory/ref_counted.h>
#include <base/memory/scoped_ptr.h>
#include <base/message_loop/message_loop.h>
#include <base/run_loop.h>
#include <dbus/bus.h>
#include <dbus/exported_object.h>
#include <dbus/message.h>
#include <utils/Log.h>

#include "constants.h"

// Used implicitly.
#undef LOG_TAG
#define LOG_TAG "dbus-example-daemon"

namespace dbus_example {
namespace {

class Daemon {
 public:
  Daemon() : dbus_object_(nullptr) {}
  ~Daemon() {}

  void Init() {
    ALOGI("Connecting to system bus");
    dbus::Bus::Options options;
    options.bus_type = dbus::Bus::SYSTEM;
    bus_ = new dbus::Bus(options);
    CHECK(bus_->Connect());

    ALOGI("Exporting method %s on %s", kMethodName, kServicePath);
    dbus_object_ = bus_->GetExportedObject(dbus::ObjectPath(kServicePath));
    CHECK(dbus_object_->ExportMethodAndBlock(
        kInterface, kMethodName,
        base::Bind(&Daemon::HandleMethodCall, base::Unretained(this))));

    ALOGI("Requesting ownership of %s", kServiceName);
    CHECK(bus_->RequestOwnershipAndBlock(kServiceName,
                                         dbus::Bus::REQUIRE_PRIMARY));
  }

 private:
  void HandleMethodCall(dbus::MethodCall* method_call,
                        dbus::ExportedObject::ResponseSender response_sender) {
    int32_t token = 0;
    dbus::MessageReader reader(method_call);
    if (!reader.PopInt32(&token)) {
      ALOGE("Request didn't have token");
      response_sender.Run(scoped_ptr<dbus::Response>(
          dbus::ErrorResponse::FromMethodCall(method_call,
              DBUS_ERROR_INVALID_ARGS, "Expected token argument")));
      return;
    }

    scoped_ptr<dbus::Response> response =
        dbus::Response::FromMethodCall(method_call);
    dbus::MessageWriter writer(response.get());
    writer.AppendInt32(token);
    ALOGI("Replying to request with token %d", token);
    response_sender.Run(response.Pass());
  }

  scoped_refptr<dbus::Bus> bus_;
  dbus::ExportedObject* dbus_object_;  // weak; owned by |bus_|

  DISALLOW_COPY_AND_ASSIGN(Daemon);
};

}  // namespace
}  // namespace dbus_example

int main(int argc, char *argv[]) {
  base::AtExitManager at_exit;
  base::MessageLoopForIO loop;
  dbus_example::Daemon daemon;
  daemon.Init();
  base::RunLoop().Run();
  return 0;
}
