#
# Copyright 2015 The Android Open Source Project
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

BOARD_SEPOLICY_DIRS += \
	build/target/board/generic/sepolicy \

PRODUCT_PACKAGES += \
	audio.primary.goldfish \
	camera.goldfish \
	camera.goldfish.jpeg \
	gps.goldfish \
	lights.goldfish \
	power.goldfish \
	qemud \
	qemu-props \
	sensors.goldfish \
	vibrator.goldfish \

PRODUCT_COPY_FILES += \
	device/generic/brillo/brilloemulator/init.brillo.rc:root/init.brillo.rc \
	device/generic/brillo/bsp/boards/generic/emulator-arm/init.goldfish.rc:root/init.goldfish.rc \
	device/generic/brillo/bsp/boards/generic/emulator-arm/init.goldfish.rc:root/init.qemu.rc \
	device/generic/brillo/bsp/boards/generic/emulator-arm/init.goldfish.sh:system/etc/init.goldfish.sh \
	device/generic/brillo/bsp/boards/generic/emulator-arm/fstab.goldfish:root/fstab.goldfish \
	device/generic/brillo/bsp/boards/generic/emulator-arm/fstab.qemu:root/fstab.qemu \
	device/generic/brillo/bsp/boards/generic/emulator-arm/ueventd.goldfish.rc:root/ueventd.goldfish.rc \
	system/core/rootdir/ueventd.rc:root/ueventd.rc \
