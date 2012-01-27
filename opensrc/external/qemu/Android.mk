# the following test is made to detect that we were called
# through the 'm' or 'mm' build commands. if not, we use the
# standard QEMU Makefile
#
ifeq ($(filter SPH-D700,$(TARGET_PRODUCT)),)

ifeq ($(DEFAULT_GOAL),droid)
    LOCAL_PATH:= $(call my-dir)
    include $(LOCAL_PATH)/Makefile.android
else
    include Makefile.qemu
endif

endif
