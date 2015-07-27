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

#ifndef DEVICE_GENERIC_BRILLO_DBUS_EXAMPLE_CONSTANTS_H_
#define DEVICE_GENERIC_BRILLO_DBUS_EXAMPLE_CONSTANTS_H_

namespace dbus_example {

// D-Bus-related constants.
static const char kServicePath[] = "/com/android/brillo/DBusExample";
static const char kServiceName[] = "com.android.brillo.DBusExample";
static const char kInterface[] = "com.android.brillo.DBusExample";
static const char kMethodName[] = "Ping";

}  // namespace dbus_example

#endif  // DEVICE_GENERIC_BRILLO_DBUS_EXAMPLE_CONSTANTS_H_
