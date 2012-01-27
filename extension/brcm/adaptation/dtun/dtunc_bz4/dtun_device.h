/************************************************************************************
 *
 *  Copyright (C) 2009-2010 Broadcom Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, version 2, as published by
 *  the Free Software Foundation (the "GPL").
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php,
 *  or by writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 *
 ************************************************************************************/
#define GENERIC_AUDIO_UUID	"00001203-0000-1000-8000-00805F9B34FB"

#define HSP_HS_UUID		"00001108-0000-1000-8000-00805F9B34FB"
#define HSP_AG_UUID		"00001112-0000-1000-8000-00805F9B34FB"

#define HFP_HS_UUID		"0000111E-0000-1000-8000-00805F9B34FB"
#define HFP_AG_UUID		"0000111F-0000-1000-8000-00805F9B34FB"

#define ADVANCED_AUDIO_UUID	"0000110D-0000-1000-8000-00805F9B34FB"

#define A2DP_SOURCE_UUID	"0000110A-0000-1000-8000-00805F9B34FB"
#define A2DP_SINK_UUID		"0000110B-0000-1000-8000-00805F9B34FB"

#define AVRCP_REMOTE_UUID	"0000110E-0000-1000-8000-00805F9B34FB"
#define AVRCP_TARGET_UUID	"0000110C-0000-1000-8000-00805F9B34FB"

/* Move these to respective .h files once they exist */
#define AUDIO_SOURCE_INTERFACE		"org.bluez.AudioSource"
#define AUDIO_CONTROL_INTERFACE		"org.bluez.Control"

struct source;
struct control;
struct target;
struct sink;
struct headset;
struct gateway;
struct dev_priv;

struct audio_device {
	struct btd_device *btd_dev;

	DBusConnection *conn;
	char *path;
	bdaddr_t src;
	bdaddr_t dst;

	gboolean auto_connect;

	struct headset *headset;
	struct sink *sink;
};

struct audio_device *manager_get_device(DBusConnection *conn, 
                                        const bdaddr_t *src,
					const bdaddr_t *dst,
					gboolean create);

struct audio_device *manager_find_device(const char *path,
					const bdaddr_t *src,
					const bdaddr_t *dst,
					const char *interface,
					gboolean connected);

struct audio_device *audio_device_register(DBusConnection *conn,
					struct btd_device *device,
					const char *path, const bdaddr_t *src,
					const bdaddr_t *dst);

void audio_device_unregister(struct audio_device *device);

gboolean audio_device_is_connected(struct audio_device *dev,
						const char *interface);

typedef void (*authorization_cb) (DBusError *derr, void *user_data);

int audio_device_request_authorization(struct audio_device *dev,
					const char *uuid, authorization_cb cb,
					void *user_data);
