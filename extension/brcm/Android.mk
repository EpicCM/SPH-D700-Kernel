LOCAL_PATH := $(call my-dir)

#
# copy brcm binaries into android
#
ifeq ($(BOARD_HAVE_BLUETOOTH),true)
    include $(all-subdir-makefiles)
endif
