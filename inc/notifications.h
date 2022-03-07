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

#include <pthread.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "common.h"

enum NOTIFICATION_STATUS
{
	NOTIF_NONE = 0,
	NOTIF_PASSIVE = 1 << 1,
	NOTIF_ACTIVE = 1 << 2,
	NOTIF_LW_PASSIVE = 1 << 3,
	NOTIF_LW_ACTIVE = 1 << 4
};

extern struct cwmp_dm_parameter forced_notifications_parameters[];
extern struct list_head list_lw_value_change;
extern struct list_head list_value_change;
extern pthread_mutex_t mutex_value_change;
extern struct list_head list_param_obj_notify;

/*#define foreach_parameter_notification(function, parameter, notification) \
	#ifndef NOTIF_VARIABLES_##function \
	#define NOTIF_VARIABLES_##function \
	struct uci_list *list_notif; \
	struct uci_element *e = NULL; \
	#endif \
	for (notification = 1; notification < 7; i++) { \
		cwmp_uci_get_option_value_list("cwmp", "@notifications[0]", notifications[notification], &list_notif); \
			if (list_notif) { \
				uci_foreach_element(list_notif, e) { \*/


#define DM_ENABLED_NOTIFY "/var/run/icwmpd/dm_enabled_notify"
#define NOTIFY_FILE "/etc/icwmpd/icwmpd_notify"
void ubus_check_value_change_callback(struct ubus_request *req, int type, struct blob_attr *msg);
void cwmp_update_enabled_notify_file(void);
int check_value_change(void);
void create_list_param_obj_notify();
void create_list_param_leaf_notify();
void sotfware_version_value_change(struct cwmp *cwmp, struct transfer_complete *p);
void *thread_periodic_check_notify(void *v);
void send_active_value_change(void);
void add_list_value_change(char *param_name, char *param_data, char *param_type);
void clean_list_value_change();
char *cwmp_set_parameter_attributes(char *parameter_name, int notification);
char *cwmp_get_parameter_attributes(char *parameter_name, struct list_head *parameters_list);
void load_custom_notify_json(struct cwmp *cwmp);
void add_lw_list_value_change(char *param_name, char *param_data, char *param_type);
char *calculate_lwnotification_cnonce();
void cwmp_lwnotification();
void clean_list_param_notify();
void init_list_param_notify();
void reinit_list_param_notify();
#endif /* SRC_INC_NOTIFICATIONS_H_ */
