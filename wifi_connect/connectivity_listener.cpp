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

#include "connectivity_listener.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#define LOG_TAG "ConnectivityListener"

#include <cutils/log.h>
#include <libminijail.h>
#include <utils/Compat.h>

#include "connectivity_common.h"

namespace {
const char kWifiConnectCommand[] = "/system/bin/wifi_connect";
const char kWifiConnectApMode[] = "ap";
const char kWifiConnectClientMode[] = "client";
const char kWifiConnectDisableMode[] = "disable";
const char kWifiConnectStatusMode[] = "status";
const char kWifiConnectStatusResultSuccess[] = "Connected";
const int kStatusPollPeriodSeconds = 10;
}  // namespace

ConnectivityListener::ConnectivityListener()
    : FrameworkListener("connectivity"),
      wifi_status_(ConnectivityCommon::kWifiStatusUnknown),
      status_process_(0),
      status_fd_(-1),
      last_status_poll_ (0) {
  registerCmd(new AutoConnectCommand(this));
  registerCmd(new ConnectCommand(this));
  registerCmd(new DisableCommand(this));
  registerCmd(new GetStatusCommand(this));
  registerCmd(new StartApCommand(this));
}

ConnectivityListener::AutoConnectCommand::AutoConnectCommand(
    const ConnectivityListener* connectivity_listener)
    : FrameworkCommand(ConnectivityCommon::kWifiCommandAutoConnect),
      connectivity_listener_(connectivity_listener) {}

int ConnectivityListener::AutoConnectCommand::runCommand(
    SocketClient* client, int /* argc */, char** /* argv */) {
  if (connectivity_listener_->RunWifiConnect({}, nullptr, nullptr)) {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeSuccess, "command started", false);
  } else {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeFailure, "command failed", false);
  }
  return 0;
}

ConnectivityListener::ConnectCommand::ConnectCommand(
    const ConnectivityListener* connectivity_listener)
    : FrameworkCommand(ConnectivityCommon::kWifiCommandConnect),
      connectivity_listener_(connectivity_listener) {}

int ConnectivityListener::ConnectCommand::runCommand(
    SocketClient* client, int argc, char** argv) {
  std::vector<const char*> args{kWifiConnectClientMode};
  if (argc == 2) {
    // SSID only.
    args.push_back(argv[1]);
  } else if (argc == 3) {
    // SSID plus passphrase.
    args.push_back(argv[1]);
    args.push_back(argv[2]);
  } else {
    client->sendMsg(ConnectivityCommon::kResponseCodeFailure,
                    "Must either be 1 or two arguments", false);
    return -1;
  }

  if (connectivity_listener_->RunWifiConnect(args, nullptr, nullptr)) {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeSuccess, "command started", false);
  } else {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeFailure, "command failed", false);
  }
  return 0;
}

ConnectivityListener::DisableCommand::DisableCommand(
    const ConnectivityListener* connectivity_listener)
    : FrameworkCommand(ConnectivityCommon::kWifiCommandDisable),
      connectivity_listener_(connectivity_listener) {}

int ConnectivityListener::DisableCommand::runCommand(
    SocketClient* client, int /* argc */, char** /* argv */) {
  std::vector<const char*> args{kWifiConnectDisableMode};
  if (connectivity_listener_->RunWifiConnect(args, nullptr, nullptr)) {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeSuccess, "command started", false);
  } else {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeFailure, "command failed", false);
  }
  return 0;
}

ConnectivityListener::GetStatusCommand::GetStatusCommand(
    const ConnectivityListener* connectivity_listener)
    : FrameworkCommand(ConnectivityCommon::kWifiCommandGetStatus),
      connectivity_listener_(connectivity_listener) {}

int ConnectivityListener::GetStatusCommand::runCommand(
    SocketClient* client, int /* argc */, char** /* argv */) {
  std::string status = connectivity_listener_->get_wifi_status();
  client->sendMsg(
      ConnectivityCommon::kResponseCodeSuccess, status.c_str(), false);
  return 0;
}

ConnectivityListener::StartApCommand::StartApCommand(
    const ConnectivityListener* connectivity_listener)
    : FrameworkCommand(ConnectivityCommon::kWifiCommandStartAp),
      connectivity_listener_(connectivity_listener) {}

int ConnectivityListener::StartApCommand::runCommand(
    SocketClient* client, int argc, char** argv) {
  std::vector<const char*> args{kWifiConnectApMode};
  if (argc == 2) {
    // SSID only.
    args.push_back(argv[1]);
  } else if (argc == 3) {
    // SSID plus passphrase.
    args.push_back(argv[1]);
    args.push_back(argv[2]);
  } else {
    client->sendMsg(ConnectivityCommon::kResponseCodeFailure,
                    "Must either be 1 or two arguments", false);
    return -1;
  }
  if (connectivity_listener_->RunWifiConnect(args, nullptr, nullptr)) {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeSuccess, "command started", false);
  } else {
    client->sendMsg(
        ConnectivityCommon::kResponseCodeFailure, "command failed", false);
  }
  return 0;
}

bool ConnectivityListener::RunWifiConnect(
    const std::vector<const char*>& args, pid_t *child_pid,
    int *output_fd) const {
  std::vector<const char*> full_args{kWifiConnectCommand};
  for (const auto& arg : args) {
    full_args.push_back(arg);
  }
  full_args.push_back(nullptr);

  struct minijail *jail = minijail_new();

  int ret = minijail_run_pid_pipes(
      jail,
      kWifiConnectCommand,
      const_cast<char* const*>(full_args.data()),
      child_pid,
      nullptr,  // stdin fd
      output_fd,
      nullptr);  // stderr fd
  minijail_destroy(jail);

  if (ret != 0) {
    ALOGE("Unable to run wifi_connect: %s", strerror(errno));
    return false;
  }

  return true;
}

bool ConnectivityListener::RequestWifiStatus() {
  std::vector<const char*> args{ kWifiConnectStatusMode };
  if (status_process_) {
    kill(status_process_, SIGTERM);
    status_process_ = 0;
  }
  if (status_fd_ != -1) {
    close(status_fd_);
    status_fd_ = -1;
  }
  bool ret = RunWifiConnect(args, &status_process_, &status_fd_);
  if (ret && status_fd_ != -1)
    fcntl(status_fd_, F_SETFL, fcntl(status_fd_, F_GETFL) | O_NONBLOCK);

  return ret;
}

void ConnectivityListener::ReapChildProcesses() {
  for (;;) {
    pid_t pid = waitpid(-1, nullptr, WNOHANG);
    if (pid == 0 || pid == -1)
      break;
    if (pid == status_process_) {
      status_process_ = 0;
    }
  }
}

bool ConnectivityListener::ReadWifiStatus(int wait_timeout) {
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(status_fd_, &read_fds);
  struct timeval wait_tv{wait_timeout, 0};
  int result =
      TEMP_FAILURE_RETRY(select(status_fd_ + 1, &read_fds, nullptr, nullptr,
                                &wait_tv));
  if (result == -1) {
    ALOGE("select failed: %s", strerror(errno));
    return false;
  }

  bool retrieved_status = false;
  if (FD_ISSET(status_fd_, &read_fds)) {
    // Read (and compare) up to but not including the null-terminator.
    char buffer[sizeof(kWifiConnectStatusResultSuccess) - 1];
    std::string new_status;

    // Due to buffering in the pipe, any string read from the pipe should
    // contain enough data to tell if we are connected.
    if (TEMP_FAILURE_RETRY(read(status_fd_, buffer,
                                sizeof(buffer))) == sizeof(buffer) &&
        memcmp(buffer, kWifiConnectStatusResultSuccess, sizeof(buffer)) == 0) {
      new_status = ConnectivityCommon::kWifiStatusConnected;
    } else {
      new_status = ConnectivityCommon::kWifiStatusDisconnected;
    }

    if (new_status != wifi_status_) {
      ALOGI("Connectivity status changed: %s -> %s", wifi_status_.c_str(),
            new_status.c_str());
      wifi_status_ = new_status;
    }

    retrieved_status = true;
  }

  if (status_process_ == 0 || retrieved_status) {
    close(status_fd_);
    status_fd_ = -1;
  }

  return true;
}

bool ConnectivityListener::PerformMainLoop() {
  // Run at-boot sequence.
  RunWifiConnect({}, nullptr, nullptr);

  // Loop forever while the listener works in a separate thread.
  while (true) {
    ReapChildProcesses();
    time_t now = time(NULL);
    if (!last_status_poll_ ||
        (now - last_status_poll_) >= kStatusPollPeriodSeconds) {
      RequestWifiStatus();
      last_status_poll_ = now;
    }

    int wait_timeout = last_status_poll_ + kStatusPollPeriodSeconds - now;
    if (status_fd_ == -1) {
      sleep(wait_timeout);
    } else if (!ReadWifiStatus(wait_timeout)) {
      return false;
    }
  }
  return true;
}

int main() {
  ConnectivityListener* listener = new ConnectivityListener();
  if (listener->startListener() != 0) {
    ALOGE("Unable to start listener: %s", strerror(errno));
    return 1;
  }

  ALOGI("listener started");
  listener->PerformMainLoop();
  return 0;
}
