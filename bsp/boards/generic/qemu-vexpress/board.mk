# The qemu board uses arm's vexpress-a9 platform.
$(call inherit-platform, armltd, vexpressa9)

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 786432000
BOARD_USERDATAIMAGE_PARTITION_SIZE := 576716800
BOARD_CACHEIMAGE_PARTITION_SIZE := 69206016
