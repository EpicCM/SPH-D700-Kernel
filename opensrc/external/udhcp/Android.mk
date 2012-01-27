ifneq ($(TARGET_SIMULATOR),true)

#LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE := udhcpd
#LOCAL_MODULE_TAGS := eng
#LOCAL_CFLAGS := -g -DDEBUG -W -Wall -Wstrict-prototypes -DVERSION=\"0.9.8\"
#LOCAL_SRC_FILES := options.c socket.c packet.c pidfile.c dhcpd.c arpping.c files.c leases.c serverpacket.c						  
#LOCAL_C_INCLUDES := $(KERNEL_HEADERS)
#LOCAL_STATIC_LIBRARIES := libcutils libc
#include $(BUILD_EXECUTABLE)
###############################
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/bin
LOCAL_MODULE := udhcpd
LOCAL_MODULE_TAGS := user eng
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
##################################

endif