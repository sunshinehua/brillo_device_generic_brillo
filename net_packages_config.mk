#
# Copyright (C) 2016 The Android Open Source Project
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

# Build configuration for networking packages.

# We must select a wpa_supplicant version, or the AOSP version won't be built.
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211

# Enable WPA CLI support.
CONFIG_CTRL_IFACE=y
# Enable WPA supplicant D-Bus support.
CONFIG_CTRL_IFACE_DBUS=y
CONFIG_CTRL_IFACE_DBUS_NEW=y

# Enable disable_vht option in WPA supplicant.
CONFIG_VHT_OVERRIDES=y

# WPA supplicant basic authentication methods.
CONFIG_DYNAMIC_EAP_METHODS=y
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_MD5=y
CONFIG_EAP_MSCHAPV2=y
CONFIG_EAP_TLS=y
CONFIG_EAP_PEAP=y
CONFIG_EAP_TTLS=y
CONFIG_EAP_GTC=y
CONFIG_EAP_OTP=y
CONFIG_EAP_LEAP=y
CONFIG_PKCS12=y
CONFIG_PEERKEY=y
CONFIG_BGSCAN_SIMPLE=y
CONFIG_BGSCAN_LEARN=y
CONFIG_IEEE80211W=y

# Settings for dhcpcd-6.8.2.
DHCPCD_USE_IPV6=yes
DHCPCD_USE_DBUS=yes
