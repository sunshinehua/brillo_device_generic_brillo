#!/system/bin/sh
#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Opens the firewall port needed to connect to adbd over TCP.
#
# During normal use the adbd port is set once and never changed, so this
# script doesn't try to close any previously opened ports.

BINPATH=/system/bin

function open_adbd_port {
  local adbd_port

  # service.adb.tcp.port has priority, persist.adb.tcp.port is secondary.
  adbd_port=`getprop service.adb.tcp.port`
  if [ -z "${adbd_port}" ]; then
    adbd_port=`getprop persist.adb.tcp.port`
  fi

  if [ "${adbd_port}" -gt 0 ]; then
    for iptables in ip{,6}tables; do
      "${BINPATH}/${iptables}" -I INPUT -p tcp --dport "${adbd_port}" --syn \
                               -m state --state NEW -j ACCEPT -w
    done
  fi
}

open_adbd_port
