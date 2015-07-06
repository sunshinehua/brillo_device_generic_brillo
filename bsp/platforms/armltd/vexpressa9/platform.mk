# Arm
$(call inherit-arch, arm)

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := true

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 512
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := true

PRODUCT_COPY_FILES += \
  device/generic/brillo/bsp/platforms/armltd/vexpressa9/fstab.brillo:root/fstab.brillo \
  device/generic/brillo/bsp/platforms/armltd/vexpressa9/init.platform.rc:root/init.platform.rc
