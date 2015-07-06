# This product uses the emulator board
$(call inherit-board, generic, qemu-vexpress)

PRODUCT_COPY_FILES += \
  device/generic/brillo/brilloemulator/init.brillo.rc:root/init.brillo.rc


