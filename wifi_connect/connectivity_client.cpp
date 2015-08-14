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

#include "connectivity_client.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define LOG_TAG "ConnectivityClient"

#include <cutils/log.h>
#include <utils/Compat.h>

#include "connectivity_common.h"

namespace {
const char kConnectivitySocket[] = "/dev/socket/connectivity";
const int kReplyTimeoutMicroseconds = 10000;

// Matches CMD_BUF_SIZE internal to FrameworkListener.
const int kCommandBufferSize = 1024;
}  // namespace

bool ConnectivityClient::SendRequest(
    const std::vector<std::string>& command, int* fd) const {
  if (!command.size()) {
    ALOGE("Ignoring empty command");
    return false;
  }

  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock == -1) {
    ALOGE("Socket open failed: %s", strerror(errno));
    return false;
  }

  sockaddr_un server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sun_family = AF_UNIX;
  strncpy(server_address.sun_path, kConnectivitySocket,
          sizeof(server_address.sun_path));

  if (connect(sock, reinterpret_cast<sockaddr*>(&server_address),
              sizeof(server_address)) == -1) {
    ALOGE("Socket connect failed: %s", strerror(errno));
    close(sock);
    return false;
  }

  // Avoiding dependent libraries here at the cost of efficiency...
  std::string command_string;
  for (const auto& el : command) {
    if (!command_string.empty())
      command_string.append(" ");
    command_string.append("\"" + el + "\"");
  }

  // Send the command string complete with null terminator.
  if (TEMP_FAILURE_RETRY(send(
      sock, command_string.c_str(), command_string.size() + 1, 0)) == -1) {
    ALOGE("Socket send failed: %s", strerror(errno));
    close(sock);
    return false;
  }

  if (fd) {
    *fd = sock;
  } else {
    close(sock);
  }

  return true;
}

bool ConnectivityClient::AwaitReply(int fd, std::string* reply) const {
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);
  timeval wait_tv{0, kReplyTimeoutMicroseconds};
  int result =
      TEMP_FAILURE_RETRY(select(fd + 1, &read_fds, nullptr, nullptr,
                                &wait_tv));
  if (result == -1) {
    ALOGE("Select failed: %s", strerror(errno));
    close(fd);
    return false;
  }

  if (!FD_ISSET(fd, &read_fds)) {
    ALOGE("Timed out read of reply");
    close(fd);
    return false;
  }

  char buffer[kCommandBufferSize];
  ssize_t read_size = TEMP_FAILURE_RETRY(read(fd, buffer, sizeof(buffer)));
  close(fd);

  if (read_size == -1) {
    ALOGE("Read reply failed: %s", strerror(errno));
    return false;
  }

  // Responses contain a response code, a space, then the response content,
  // and a null termination, e.g., "0 connected\0".
  if (buffer[read_size - 1] != '\0') {
    ALOGE("Reply buffer is not terminated");
    return false;
  }

  char *response = strchr(buffer, ' ');
  if (response == nullptr)
    response = buffer;
  else
    response++;
  
  reply->assign(response, buffer + read_size - 1);
  return true;
}

bool ConnectivityClient::DisableAccessPoint() const {
  return SendRequest({ConnectivityCommon::kWifiCommandDisable}, nullptr) &&
      SendRequest({ConnectivityCommon::kWifiCommandAutoConnect}, nullptr);
}

bool ConnectivityClient::EnableAccessPoint(const std::string& ssid) const {
  return SendRequest({ConnectivityCommon::kWifiCommandStartAp, ssid}, nullptr);
}

bool ConnectivityClient::ConnectToAccessPoint(
    const std::string& ssid, const std::string& passphrase) const {
  return
      SendRequest({ConnectivityCommon::kWifiCommandConnect, ssid, passphrase},
                  nullptr);
}

bool ConnectivityClient::IsConnected() const {
  int fd;
  if (!SendRequest({ConnectivityCommon::kWifiCommandGetStatus}, &fd))
    return false;

  std::string reply;
  if (!AwaitReply(fd, &reply))
    return false;

  return reply == ConnectivityCommon::kWifiStatusConnected;
}

