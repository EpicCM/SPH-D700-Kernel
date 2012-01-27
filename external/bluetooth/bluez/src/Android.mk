LOCAL_PATH:= $(call my-dir)

# Relative path from current dir to vendor brcm
BRCM_BT_SRC_ROOT_PATH := ../../../../extension/brcm

# Relative path from <mydroid> to brcm base
BRCM_BT_INC_ROOT_PATH := $(LOCAL_PATH)/../../../../extension/brcm

#
# libbluetoothd
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	android_bluez.c \
	adapter.c \
	agent.c \
	btio.c \
	dbus-common.c \
	dbus-hci.c \
	device.c \
	error.c \
	glib-helper.c \
	log.c \
	main.c \
	manager.c \
	oui.c \
	plugin.c \
	rfkill.c \
	sdpd-request.c \
	sdpd-service.c \
	sdpd-server.c \
	sdpd-database.c \
	sdp-xml.c \
	security.c \
	storage.c \
	textfile.c

LOCAL_CFLAGS:= \
	-DVERSION=\"4.69\" \
	-DSTORAGEDIR=\"/data/misc/bluetoothd\" \
	-DCONFIGDIR=\"/etc/bluetooth\" \
	-DSERVICEDIR=\"/system/bin\" \
	-DPLUGINDIR=\"/system/lib/bluez-plugin\" \
	-DANDROID_SET_AID_AND_CAP \
	-DANDROID_EXPAND_NAME

ifeq ($(BOARD_HAVE_BLUETOOTH_BCM),true)
LOCAL_CFLAGS += \
	-DBOARD_HAVE_BLUETOOTH_BCM
endif

ifeq ($(BT_ALT_STACK),true)
LOCAL_CFLAGS += -DBT_ALT_STACK
endif

LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH)/../lib \
	$(LOCAL_PATH)/../gdbus \
	$(LOCAL_PATH)/../plugins \
	$(call include-path-for, glib) \
	$(call include-path-for, glib)/glib \
	$(call include-path-for, dbus)

ifeq ($(BRCM_BT_USE_BTL_IF),true)
LOCAL_SRC_FILES += \
	$(BRCM_BT_SRC_ROOT_PATH)/adaptation/dtun/dtunc_bz4/dtun_clnt.c \
	$(BRCM_BT_SRC_ROOT_PATH)/adaptation/dtun/dtunc_bz4/dtun_device.c \
	$(BRCM_BT_SRC_ROOT_PATH)/adaptation/dtun/dtunc_bz4/dtun_obex.c \
	$(BRCM_BT_SRC_ROOT_PATH)/adaptation/dtun/dtunc_bz4/dtun_hcid.c

LOCAL_C_INCLUDES += \
	$(BRCM_BT_INC_ROOT_PATH)/adaptation/dtun/include \
	$(BRCM_BT_INC_ROOT_PATH)/adaptation/dtun/dtunc_bz4

ifeq ($(TARGET_PRODUCT),SC-02B)
LOCAL_CFLAGS += -DS1_JPN_NTT
endif

LOCAL_CFLAGS += -DBRCM_BT_USE_BTL_IF -DBT_USE_BTL_IF
endif

LOCAL_SHARED_LIBRARIES := \
	libdl \
	libbluetooth \
	libdbus \
	libcutils

LOCAL_STATIC_LIBRARIES := \
	libglib_static \
	libbuiltinplugin \
	libgdbus_static

LOCAL_MODULE:=libbluetoothd

include $(BUILD_SHARED_LIBRARY)

#
# bluetoothd
#

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libbluetoothd

LOCAL_MODULE:=bluetoothd

include $(BUILD_EXECUTABLE)
