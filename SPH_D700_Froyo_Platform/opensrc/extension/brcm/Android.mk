LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
    include $(all-subdir-makefiles)
endif
