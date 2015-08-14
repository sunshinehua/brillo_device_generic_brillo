/*
 * Copyright 2015 The Android Open Source Project
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

#ifndef BRILLO_WIFI_CONNECT_CONNECTIVITY_COMMON_H_
#define BRILLO_WIFI_CONNECT_CONNECTIVITY_COMMON_H_

class ConnectivityCommon {
 public:
  enum ResponseCode {
    kResponseCodeSuccess,
    kResponseCodeFailure
  };
  static constexpr char kWifiStatusConnected[] = "connected";
  static constexpr char kWifiStatusDisconnected[] = "disconnected";
  static constexpr char kWifiStatusUnknown[] = "unknown";

  static constexpr char kWifiCommandAutoConnect[] = "auto_connect";
  static constexpr char kWifiCommandConnect[] = "connect";
  static constexpr char kWifiCommandDisable[] = "disable";
  static constexpr char kWifiCommandGetStatus[] = "get_status";
  static constexpr char kWifiCommandStartAp[] = "start_ap";
};

#endif  // BRILLO_WIFI_CONNECT_CONNECTIVITY_COMMON_H_

