/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_
#include <sys/socket.h>
#include <pthread.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "common.h"
#include "event.h"
#include "datamodel_interface.h"
#include "rpc_soap.h"

enum NOTIFICATION_STATUS
{
	NOTIF_NONE = 0,
	NOTIF_PASSIVE = 1 << 1,
	NOTIF_ACTIVE = 1 << 2,
	NOTIF_LW_PASSIVE = 1 << 3,
	NOTIF_LW_ACTIVE = 1 << 4
};

struct list_head list_lw_value_change;
extern struct list_head list_value_change;
extern pthread_mutex_t mutex_value_change;

#define DM_ENABLED_NOTIFY "/etc/icwmpd/.dm_enabled_notify"
void ubus_check_value_change_callback(struct ubus_request *req, int type, struct blob_attr *msg);
void cwmp_update_enabled_notify_file_callback(struct ubus_request *req, int type, struct blob_attr *msg);
void sotfware_version_value_change(struct cwmp *cwmp, struct transfer_complete *p);
void *thread_periodic_check_notify(void *v);
void send_active_value_change(void);
void add_list_value_change(char *param_name, char *param_data, char *param_type);
void clean_list_value_change();
void add_lw_list_value_change(char *param_name, char *param_data, char *param_type);
char *calculate_lwnotification_cnonce();
void cwmp_lwnotification();
#endif /* SRC_INC_NOTIFICATIONS_H_ */
