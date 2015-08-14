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

#include <stdio.h>

#include <string>

#include "connectivity_client.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: %s <cmd>\n", argv[0]);
    return 1;
  }

  ConnectivityClient cc;

  std::string cmd(argv[1]);
  if (cmd == "status") {
    if (cc.IsConnected())
      printf("Connected\n");
    else
      printf("Not connected\n");
  } else if (cmd == "connect") {
    if (argc < 3) {
      fprintf(stderr, "Needs SSID argument\n");
      return 1;
    }
    std::string passphrase;
    if (argc >= 4)
        passphrase = argv[3];
    cc.ConnectToAccessPoint(std::string(argv[2]), passphrase);
  } else if (cmd == "disableap") {
    cc.DisableAccessPoint();
  } else if (cmd == "ap") {
    if (argc < 3) {
      fprintf(stderr, "Needs SSID argument\n");
      return 1;
    }
    cc.EnableAccessPoint(std::string(argv[2]));
  }
  return 0;
}
