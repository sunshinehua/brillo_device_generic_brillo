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

#ifndef BRILLO_WIFI_CONNECT_CONNECTIVITY_LISTENER_H_
#define BRILLO_WIFI_CONNECT_CONNECTIVITY_LISTENER_H_

#include <sys/time.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include <sysutils/FrameworkCommand.h>
#include <sysutils/FrameworkListener.h>

class ConnectivityListener : public FrameworkListener {
 public:
  ConnectivityListener();
  ~ConnectivityListener() override = default;

  // Runs the "wifi_connect" helper with |args|.  Optionally the caller
  // can pass pointers to have the child process id and stdout of the
  // child process.
  bool RunWifiConnect(const std::vector<const char*>& args,
                      pid_t *child_pid, int *output_fd) const;

  // Performs the listener main loop.
  bool PerformMainLoop();

  const std::string& get_wifi_status() const { return wifi_status_; }

 private:
  class AutoConnectCommand : public FrameworkCommand {
   public:
    AutoConnectCommand(const ConnectivityListener* connectivity_listener);
    ~AutoConnectCommand() override = default;
    int runCommand(SocketClient* client, int argc, char**argv) override;
   private:
    const ConnectivityListener* connectivity_listener_;
  };

  class ConnectCommand : public FrameworkCommand {
   public:
    ConnectCommand(const ConnectivityListener* connectivity_listener);
    ~ConnectCommand() override = default;
    int runCommand(SocketClient* client, int argc, char**argv) override;
   private:
    const ConnectivityListener* connectivity_listener_;
  };

  class DisableCommand : public FrameworkCommand {
   public:
    DisableCommand(const ConnectivityListener* connectivity_listener);
    ~DisableCommand() override = default;
    int runCommand(SocketClient* client, int argc, char**argv) override;
   private:
    const ConnectivityListener* connectivity_listener_;
  };

  class GetStatusCommand : public FrameworkCommand {
   public:
    GetStatusCommand(const ConnectivityListener* frameworkListener);
    ~GetStatusCommand() override = default;
    int runCommand(SocketClient* c, int argc, char**argv) override;
   private:
    const ConnectivityListener* connectivity_listener_;
  };

  class StartApCommand : public FrameworkCommand {
   public:
    StartApCommand(const ConnectivityListener* connectivity_listener);
    ~StartApCommand() override = default;
    int runCommand(SocketClient* client, int argc, char**argv) override;
   private:
    const ConnectivityListener* connectivity_listener_;
  };

  bool RequestWifiStatus();
  void ReapChildProcesses();
  bool ReadWifiStatus(int wait_timeout);

  std::string wifi_status_;
  pid_t status_process_;
  int status_fd_;
  time_t last_status_poll_;
};

#endif  // BRILLO_WIFI_CONNECT_CONNECTIVITY_LISTENER_H_

