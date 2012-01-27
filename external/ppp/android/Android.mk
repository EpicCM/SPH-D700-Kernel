#
# Copyright (C) 2009 The Android Open Source Project
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

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := vpn-routes.c ip-up-vpn.c
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_MODULE := ip-up-vpn
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/ppp

include $(BUILD_EXECUTABLE)

IP_DOWN_VPN_MODULE := ip-down-vpn
IP_UP_VPN_PATH := $(LOCAL_MODULE_PATH)/$(LOCAL_MODULE)
IP_DOWN_VPN_PATH := $(LOCAL_MODULE_PATH)/$(IP_DOWN_VPN_MODULE)

$(IP_DOWN_VPN_MODULE): IP_UP_VPN_MODULE := $(LOCAL_MODULE)
$(IP_DOWN_VPN_MODULE): $(LOCAL_INSTALLED_MODULE) $(LOCAL_PATH)/Android.mk
	@echo "Create symlink $(IP_DOWN_VPN_PATH) --> $(IP_UP_VPN_MODULE)"
	@rm -rf $(IP_DOWN_VPN_PATH)
	ln -sf $(IP_UP_VPN_MODULE) $(IP_DOWN_VPN_PATH)

ALL_DEFAULT_INSTALLED_MODULES += $(IP_DOWN_VPN_MODULE)

ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
    $(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(IP_DOWN_VPN_MODULE)

