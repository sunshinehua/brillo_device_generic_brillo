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

#ifndef BRILLO_WIFI_CONNECT_CONNECTIVITY_CLIENT_H_
#define BRILLO_WIFI_CONNECT_CONNECTIVITY_CLIENT_H_

#include <sys/time.h>
#include <sys/types.h>

#include <string>
#include <vector>

class ConnectivityClient  {
 public:
  ConnectivityClient() = default;
  virtual ~ConnectivityClient() = default;

  // Connect to an AP with ssid |ssid|.  If |passphrase| is empty, it
  // attempts to access an open access point, otherwise it assumes WPA.
  bool ConnectToAccessPoint(const std::string& ssid,
                            const std::string& passphrase) const;

  // Disables access point services on the device.
  bool DisableAccessPoint() const;

  // Turn on an open access point broadcasting ssid |ssid|.
  bool EnableAccessPoint(const std::string& ssid) const;

  // Returns whether the system is connected.
  bool IsConnected() const;

 private:
  // Send a |command| to the server.  If |fd| is non-null, the socket is
  // returned, otherwise it is closed.  Returns true only on success.
  bool SendRequest(const std::vector<std::string>& command, int* fd) const;

  // Waits for a reply on |fd| and retuns it in |reply|.  Returns true only
  // if a reply is successfully received.
  bool AwaitReply(int fd, std::string* reply) const;
};

#endif  // BRILLO_WIFI_CONNECT_CONNECTIVITY_CLIENT_H_

