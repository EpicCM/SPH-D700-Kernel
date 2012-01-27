LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=  ip.c rtm_map.c iprule.c iproute.c \
                    
LOCAL_MODULE := ip

LOCAL_SYSTEM_SHARED_LIBRARIES := \
	libc libm libdl

LOCAL_SHARED_LIBRARIES += libiprouteutil libnetlink

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) external/iproute2/include

LOCAL_CFLAGS := -O2 -g -W -Wall 

include $(BUILD_EXECUTABLE)

