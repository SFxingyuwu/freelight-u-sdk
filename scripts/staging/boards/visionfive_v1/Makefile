
#CROSS_COMPILE_RV64_ENV_STARFIVE=$(CROSS_COMPILE_RV64_ENV)

#BOARD_BUILD_DEPENDENT_PACKAGES=

include $(MK_BOARDS_DIR)/$(BOARD_NAME)/u-boot.mk
include $(MK_BOARDS_DIR)/$(BOARD_NAME)/opensbi.mk
include $(MK_BOARDS_DIR)/$(BOARD_NAME)/linux.mk

include $(MK_BOARDS_DIR)/$(BOARD_NAME)/fit.mk

visionfive_v1_all: nvdla-demo visionfive_v1_uboot_all visionfive_v1_linux_all visionfive_v1_fit
