/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <netdb.h>
#include <fcntl.h>

#include "notifications.h"
#include "cwmp_uci.h"
#include "datamodel_interface.h"
#include "ssl_utils.h"
#include "log.h"
#include "event.h"
#include "xml.h"

LIST_HEAD(list_value_change);
LIST_HEAD(list_lw_value_change);
LIST_HEAD(list_param_obj_notify);
pthread_mutex_t mutex_value_change = PTHREAD_MUTEX_INITIALIZER;

char *notifications[7] = {"disabled" , "passive", "active", "passive_lw", "passive_passive_lw", "active_lw", "passive_active_lw"};

struct cwmp_dm_parameter forced_notifications_parameters[] = {
	{.name = "Device.DeviceInfo.SoftwareVersion", .notification = 2, .forced_notification_param = true},
	{.name = "Device.DeviceInfo.ProvisioningCode", .notification = 2, .forced_notification_param = true}
};

/*
 * Common functions
 */
static bool parameter_is_subobject_of_parameter(char *parent, char *child)
{
	if (strcmp(parent, child) == 0)
		return false;
	if (strncmp(parent, child, strlen(parent)) == 0)
		return true;
	return false;
}

int check_parameter_forced_notification(const char *parameter)
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(forced_notifications_parameters); i++) {
		if (strcmp(forced_notifications_parameters[i].name, parameter) == 0)
			return forced_notifications_parameters[i].notification;
	}

	return 0;
}

char *check_valid_parameter_path(char *parameter_name)
{
	char *error = NULL;
	LIST_HEAD(parameters_list);

	error = cwmp_get_parameter_names(parameter_name, true, &parameters_list);

	if (error != NULL && strcmp(error, "9003") == 0)
		error = cwmp_get_parameter_values(parameter_name, &parameters_list);

	cwmp_free_all_dm_parameter_list(&parameters_list);

	return error;
}

/*
 * SetParameterAttributes
 */
int add_uci_option_notification(char *parameter_name, int notification)
{
	char *notification_type = NULL;
	struct uci_section *s = NULL;
	int ret = 0;

	ret = cwmp_uci_get_section_type("cwmp", "@notifications[0]", UCI_VARSTATE_CONFIG, &notification_type);
	if (ret != UCI_OK)
		return -1;

	if (notification_type == NULL || notification_type[0] == '\0') {
		cwmp_uci_add_section("cwmp", "notifications", UCI_VARSTATE_CONFIG, &s);
	}
	ret = cwmp_uci_add_list_value("cwmp", "@notifications[0]", notifications[notification], parameter_name, UCI_VARSTATE_CONFIG);
	if (ret != UCI_OK)
		return -1;

	ret = cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
	return ret;
}

bool check_parent_with_different_notification(char *parameter_name, int notification)
{
	struct uci_list *list_notif;
	struct uci_element *e = NULL;
	int i;
	for (i = 0; i < 7; i++) {
		int option_type;

		if (i == notification)
			continue;
		option_type = cwmp_uci_get_cwmp_varstate_option_value_list("cwmp", "@notifications[0]", notifications[i], &list_notif);
		if (list_notif) {
			uci_foreach_element(list_notif, e) {
				if (parameter_is_subobject_of_parameter(e->name, parameter_name))
					return true;
			}
		}
		if (option_type == UCI_TYPE_STRING)
			cwmp_free_uci_list(list_notif);
	}
	return false;
}

bool update_notifications_list(char *parameter_name, int notification)
{
	struct uci_list *list_notif;
	struct uci_element *e = NULL, *tmp = NULL;
	int i;
	char *ename = NULL;
	bool update_ret = true;

	for (i = 0; i < 7; i++) {
		int option_type;
		option_type = cwmp_uci_get_cwmp_varstate_option_value_list("cwmp", "@notifications[0]", notifications[i], &list_notif);
		if (list_notif) {
			uci_foreach_element_safe(list_notif, tmp, e) {
				if (e->name == NULL)
					continue;
				ename = strdup(e->name);
				if ((strcmp(parameter_name, e->name) == 0 && (i != notification)) || parameter_is_subobject_of_parameter(parameter_name, e->name))
					cwmp_uci_del_list_value("cwmp", "@notifications[0]", notifications[i], e->name, UCI_VARSTATE_CONFIG);
				if (ename && (strcmp(parameter_name, ename) == 0 || parameter_is_subobject_of_parameter(ename, parameter_name) ) && (i == notification))
					update_ret = false;
				FREE(ename);
			}
			cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
		}
		if (option_type == UCI_TYPE_STRING)
			cwmp_free_uci_list(list_notif);
	}

	if (update_ret && notification == 0 && !check_parent_with_different_notification(parameter_name, 0))
		update_ret = false;
	return update_ret;
}

char *cwmp_set_parameter_attributes(char *parameter_name, int notification)
{
	char *error = NULL;

	error = check_valid_parameter_path(parameter_name);

	if (error != NULL)
		return error;

	if (check_parameter_forced_notification(parameter_name))
		return "9009";

	if (update_notifications_list(parameter_name, notification) == true)
		add_uci_option_notification(parameter_name, notification);

	return NULL;
}

/*
 * GetPrameterAttributes
 */
int get_parameter_family_notifications(char *parameter_name, struct list_head *childs_notifications) {

	struct uci_list *list_notif;
	struct uci_element *e = NULL;
	int i, notif_ret = 0;
	char *parent_param = NULL;

	for (i = 0; i < 7; i++) {
		int option_type;

		option_type = cwmp_uci_get_cwmp_varstate_option_value_list("cwmp", "@notifications[0]", notifications[i], &list_notif);
		if (list_notif) {
			uci_foreach_element(list_notif, e) {
				if (parameter_is_subobject_of_parameter(parameter_name, e->name)) {
					add_dm_parameter_to_list(childs_notifications, e->name, "", "", i, false);
				}
				if (parameter_is_subobject_of_parameter(e->name, parameter_name) && (parent_param == NULL || parameter_is_subobject_of_parameter(parent_param, e->name))) {
						parent_param = e->name;
						notif_ret = i;
				}
				if (strcmp(parameter_name, e->name) == 0)
					notif_ret = i;
			}
		}
		if (option_type == UCI_TYPE_STRING)
			cwmp_free_uci_list(list_notif);
	}
	return notif_ret;
}

int get_parameter_leaf_notification_from_childs_list(char *parameter_name, struct list_head *childs_list)
{
	char *parent = NULL;
	int ret_notif = -1;
	struct cwmp_dm_parameter *param_value;
	if (childs_list == NULL)
		return -1;
	list_for_each_entry (param_value, childs_list, list) {
		if (strcmp(param_value->name, parameter_name) == 0) {
			ret_notif = param_value->notification;
			break;
		}
		if (parameter_is_subobject_of_parameter(param_value->name, parameter_name) && ( parent == NULL || parameter_is_subobject_of_parameter(parent, param_value->name))) {
			parent = param_value->name;
			ret_notif = param_value->notification;
		}
	}
	return ret_notif;
}

char *cwmp_get_parameter_attributes(char *parameter_name, struct list_head *parameters_list)
{
	char *error = NULL;

	error = check_valid_parameter_path(parameter_name);

	if (error != NULL)
		return error;
	LIST_HEAD(childs_notifs);
	int notification = get_parameter_family_notifications(parameter_name, &childs_notifs);
	LIST_HEAD(params_list);
	error = cwmp_get_parameter_values(parameter_name, &params_list);
	if (error != NULL) {
		cwmp_free_all_dm_parameter_list(&childs_notifs);
		return error;
	}
	struct cwmp_dm_parameter *param_value;
	list_for_each_entry (param_value, &params_list, list) {
		int notif_leaf;
		notif_leaf = check_parameter_forced_notification(param_value->name);
		if (notif_leaf > 0) {
			add_dm_parameter_to_list(parameters_list, param_value->name, "", "", notif_leaf, false);
			continue;
		}
		notif_leaf = get_parameter_leaf_notification_from_childs_list(param_value->name, &childs_notifs);
		if (notif_leaf == -1) {
			add_dm_parameter_to_list(parameters_list, param_value->name, "", "", notification, false);
		}
		else {
			add_dm_parameter_to_list(parameters_list, param_value->name, "", "", notif_leaf, false);
		}
	}
	cwmp_free_all_dm_parameter_list(&childs_notifs);
	cwmp_free_all_dm_parameter_list(&params_list);
	return NULL;
}

/*
 * Update notify file
 */
bool parameter_is_other_notif_object_child(char *parent, char *parameter)
{
	struct list_head list_iter, *list_ptr;
	list_iter.next = list_param_obj_notify.next;
	list_iter.prev = list_param_obj_notify.prev;
	while (list_iter.prev != &list_param_obj_notify) {
		struct cwmp_dm_parameter *dm_parameter;
		if (list_iter.prev == NULL)
			continue;

		dm_parameter = list_entry(list_iter.prev, struct cwmp_dm_parameter, list);
		list_ptr = list_iter.prev;
		list_iter.prev = list_ptr->prev;
		list_iter.next = list_ptr->next;
		if (strcmp(parent, dm_parameter->name) == 0)
			continue;
		if (strncmp(parent, dm_parameter->name, strlen(parent)) == 0 && strncmp(parameter, dm_parameter->name, strlen(dm_parameter->name)) == 0)
			return true;
	}
	return false;
}

void create_list_param_obj_notify()
{
	struct uci_list *list_notif;
	struct uci_element *e = NULL;
	int i;
	for (i = 0; i < 7; i++) {
		int option_type;
		option_type = cwmp_uci_get_cwmp_varstate_option_value_list("cwmp", "@notifications[0]", notifications[i], &list_notif);
		if (list_notif) {
			uci_foreach_element(list_notif, e) {
				add_dm_parameter_to_list(&list_param_obj_notify, e->name, "", "", i, false);
			}
		}
		if (option_type == UCI_TYPE_STRING)
			cwmp_free_uci_list(list_notif);
	}
}

char* updated_list_param_leaf_notify_with_sub_parameter_list(struct list_head *list_param_leaf_notify, struct cwmp_dm_parameter parent_parameter, void (*update_notify_file_line_arg)(FILE *notify_file, char *param_name, char *param_type, char *param_value, int notification), FILE* notify_file_arg)
{
	struct cwmp_dm_parameter *param_iter;
	LIST_HEAD(params_list);
	char *err = cwmp_get_parameter_values(parent_parameter.name, &params_list);
	if (err)
		return err;
	list_for_each_entry (param_iter, &params_list, list) {
		if (parent_parameter.forced_notification_param || (!parameter_is_other_notif_object_child(parent_parameter.name, param_iter->name) && !check_parameter_forced_notification(param_iter->name))) {
			if (list_param_leaf_notify != NULL)
				add_dm_parameter_to_list(list_param_leaf_notify, param_iter->name, param_iter->value, "", parent_parameter.notification, false);
			if (notify_file_arg != NULL && update_notify_file_line_arg != NULL)
				update_notify_file_line_arg(notify_file_arg, param_iter->name, param_iter->type, param_iter->value, parent_parameter.notification);
		}
	}
	cwmp_free_all_dm_parameter_list(&params_list);
	return NULL;
}

void create_list_param_leaf_notify(struct list_head *list_param_leaf_notify, void (*update_notify_file_line_arg)(FILE *notify_file, char *param_name, char *param_type, char *param_value, int notification), FILE* notify_file_arg)
{
	struct cwmp_dm_parameter *param_iter;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(forced_notifications_parameters); i++)
		updated_list_param_leaf_notify_with_sub_parameter_list(list_param_leaf_notify, forced_notifications_parameters[i], update_notify_file_line_arg, notify_file_arg);

	list_for_each_entry (param_iter, &list_param_obj_notify, list) {
		if (param_iter->notification == 0)
			continue;
		param_iter->forced_notification_param = false;
		updated_list_param_leaf_notify_with_sub_parameter_list(list_param_leaf_notify, *param_iter, update_notify_file_line_arg, notify_file_arg);
	}
}

void init_list_param_notify()
{
	create_list_param_obj_notify();
}

void clean_list_param_notify()
{
	cwmp_free_all_dm_parameter_list(&list_param_obj_notify);
}

void reinit_list_param_notify()
{
	clean_list_param_notify();
	init_list_param_notify();
}

void update_notify_file_line(FILE *notify_file, char *param_name, char *param_type, char *param_value, int notification)
{
	if (notify_file == NULL)
		return;
	struct blob_buf bbuf;
	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);
	blobmsg_add_string(&bbuf, "parameter", param_name);
	blobmsg_add_u32(&bbuf, "notification", notification);
	blobmsg_add_string(&bbuf, "type", param_type);
	blobmsg_add_string(&bbuf, "value", param_value);
	char *notification_line = blobmsg_format_json(bbuf.head, true);
	if (notification_line != NULL) {
		fprintf(notify_file, "%s\n", notification_line);
		FREE(notification_line);
	}
	blob_buf_free(&bbuf);
}

void cwmp_update_enabled_notify_file(void)
{
	FILE *fp = NULL;

	LIST_HEAD(list_notify_params);
	remove(DM_ENABLED_NOTIFY);
	fp = fopen(DM_ENABLED_NOTIFY, "a");
	if (fp == NULL)
		return;

	create_list_param_leaf_notify(NULL, update_notify_file_line, fp);
	fclose(fp);
}

/*
 * Load custom notify json file
 */
void load_custom_notify_json(struct cwmp *cwmp)
{
	struct blob_buf bbuf;
	struct blob_attr *cur;
	struct blob_attr *custom_notify_list = NULL;
	int rem;

	cwmp->custom_notify_active = false;
	if (cwmp->conf.custom_notify_json == NULL || !file_exists(cwmp->conf.custom_notify_json))
		return;

	if (file_exists(NOTIFY_FILE))
		return;

	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);

	if (blobmsg_add_json_from_file(&bbuf, cwmp->conf.custom_notify_json) == false) {
		CWMP_LOG(WARNING, "The file %s is not a valid JSON file", cwmp->conf.custom_notify_json);
		blob_buf_free(&bbuf);
		creat(NOTIFY_FILE , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		return;
	}

	const struct blobmsg_policy p_notif[1] = { { "custom_notification", BLOBMSG_TYPE_ARRAY } };
	struct blob_attr *tb_notif[1] = { NULL};
	blobmsg_parse(p_notif, 1, tb_notif, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
	if (tb_notif[0] == NULL) {
		CWMP_LOG(WARNING, "The JSON file %s doesn't contain a notify parameters list", cwmp->conf.custom_notify_json);
		blob_buf_free(&bbuf);
		creat(NOTIFY_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		return;
	}
	custom_notify_list = tb_notif[0];

	const struct blobmsg_policy p[2] = { { "parameter", BLOBMSG_TYPE_STRING }, { "notify_type", BLOBMSG_TYPE_STRING } };
	blobmsg_for_each_attr(cur, custom_notify_list, rem)
	{
		struct blob_attr *tb[2] = { NULL, NULL };
		blobmsg_parse(p, 2, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0] || !tb[1])
			continue;
		if (!icwmp_validate_int_in_range(blobmsg_get_string(tb[1]), 0, 6)) {
			CWMP_LOG(WARNING, "Wrong notification value: %s", blobmsg_get_string(tb[1]));
			continue;
		}
		char *fault = cwmp_set_parameter_attributes(blobmsg_get_string(tb[0]), atoi(blobmsg_get_string(tb[1])));
		if (fault == NULL)
			continue;
		if (strcmp(fault, "9005") == 0) {
			CWMP_LOG(WARNING, "The parameter %s is wrong path", blobmsg_get_string(tb[0]));
			continue;
		}
		if (strcmp(fault, "9009") == 0) {
			CWMP_LOG(WARNING, "This parameter %s is forced notification parameter, can't be changed", blobmsg_get_string(tb[0]));
			continue;
		}
	}
	blob_buf_free(&bbuf);
	creat(NOTIFY_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	cwmp->custom_notify_active = true;
}

/*
 * Check value change
 */
void get_parameter_value_from_parameters_list(struct list_head *params_list, char *parameter_name, char **value, char **type)
{
	struct cwmp_dm_parameter *param_value;
	list_for_each_entry (param_value, params_list, list) {
		if (param_value->name == NULL)
			continue;
		if (strcmp(parameter_name, param_value->name) != 0)
			continue;
		*value = strdup(param_value->value ? param_value->value : "");
		*type = strdup(param_value->type ? param_value->type : "");
	}
}

int check_value_change(void)
{
	FILE *fp;
	char buf[1280];
	char *dm_value = NULL, *dm_type = NULL;
	int int_ret = 0;
	struct blob_buf bbuf;

	char *parameter = NULL, *value = NULL;
	int notification = 0;
	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL)
		return int_ret;

	LIST_HEAD(list_notify_params);
	create_list_param_leaf_notify(&list_notify_params, NULL, NULL);
	while (fgets(buf, 1280, fp) != NULL) {
		int len = strlen(buf);
		if (len)
			buf[len - 1] = '\0';

		memset(&bbuf, 0, sizeof(struct blob_buf));
		blob_buf_init(&bbuf, 0);

		if (blobmsg_add_json_from_string(&bbuf, buf) == false) {
			blob_buf_free(&bbuf);
			continue;
		}
		const struct blobmsg_policy p[4] = { { "parameter", BLOBMSG_TYPE_STRING }, { "notification", BLOBMSG_TYPE_INT32 }, { "type", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING } };

		struct blob_attr *tb[4] = { NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 4, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
		parameter = blobmsg_get_string(tb[0]);
		notification = blobmsg_get_u32(tb[1]);
		//type = blobmsg_get_string(tb[2]);
		value = blobmsg_get_string(tb[3]);
		get_parameter_value_from_parameters_list(&list_notify_params, parameter, &dm_value, &dm_type);
		if (dm_value == NULL && dm_type == NULL){
			blob_buf_free(&bbuf);
			parameter = NULL;
			notification = 0;
			//type = NULL;
			value = NULL;
			continue;
		}
		if ((notification >= 1) && (dm_value != NULL) && (strcmp(dm_value, value) != 0)) {
			if (notification == 1 || notification == 2)
				add_list_value_change(parameter, dm_value, dm_type);
			if (notification >= 3)
				add_lw_list_value_change(parameter, dm_value, dm_type);

			if (notification == 1)
				int_ret |= NOTIF_PASSIVE;
			if (notification == 2)
				int_ret |= NOTIF_ACTIVE;

			if (notification == 5 || notification == 6)
				int_ret |= NOTIF_LW_ACTIVE;
		}
		FREE(dm_value);
		FREE(dm_type);
		parameter = NULL;
		notification = 0;
		//type = NULL;
		value = NULL;
		blob_buf_free(&bbuf);
	}
	fclose(fp);
	cwmp_free_all_dm_parameter_list(&list_notify_params);
	return int_ret;
}

void sotfware_version_value_change(struct cwmp *cwmp, struct transfer_complete *p)
{
	char *current_software_version = NULL;

	if (!p->old_software_version || p->old_software_version[0] == 0)
		return;

	current_software_version = cwmp->deviceid.softwareversion;
	if (p->old_software_version && current_software_version && strcmp(p->old_software_version, current_software_version) != 0) {
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	}
}

void *thread_periodic_check_notify(void *v)
{
	struct cwmp *cwmp = (struct cwmp *)v;
	int periodic_interval;
	bool periodic_enable;
	struct timespec periodic_timeout = { 0, 0 };
	time_t current_time;
	int is_notify;

	periodic_interval = cwmp->conf.periodic_notify_interval;
	periodic_enable = cwmp->conf.periodic_notify_enable;

	for (;;) {
		if (periodic_enable) {
			pthread_mutex_lock(&(cwmp->mutex_notify_periodic));
			current_time = time(NULL);
			periodic_timeout.tv_sec = current_time + periodic_interval;

			if (thread_end)
				break;

			pthread_cond_timedwait(&(cwmp->threshold_notify_periodic), &(cwmp->mutex_notify_periodic), &periodic_timeout);

			if (thread_end)
				break;

			pthread_mutex_lock(&(cwmp->mutex_session_send));
			is_notify = check_value_change();
			if (is_notify > 0)
				cwmp_update_enabled_notify_file();
			pthread_mutex_unlock(&(cwmp->mutex_session_send));
			if (is_notify & NOTIF_ACTIVE)
				send_active_value_change();
			if (is_notify & NOTIF_LW_ACTIVE)
				cwmp_lwnotification();
			pthread_mutex_unlock(&(cwmp->mutex_notify_periodic));
		} else
			break;
	}
	return NULL;
}

void add_list_value_change(char *param_name, char *param_data, char *param_type)
{
	pthread_mutex_lock(&(mutex_value_change));
	add_dm_parameter_to_list(&list_value_change, param_name, param_data, param_type, 0, false);
	pthread_mutex_unlock(&(mutex_value_change));
}

void clean_list_value_change()
{
	pthread_mutex_lock(&(mutex_value_change));
	cwmp_free_all_dm_parameter_list(&list_value_change);
	pthread_mutex_unlock(&(mutex_value_change));
}

void send_active_value_change(void)
{
	struct cwmp *cwmp = &cwmp_main;
	struct event_container *event_container;

	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return;
	}

	cwmp_save_event_container(event_container);
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	pthread_cond_signal(&(cwmp->threshold_session_send));
	return;
}

/*
 * Light Weight Notifications
 */
void add_lw_list_value_change(char *param_name, char *param_data, char *param_type) { add_dm_parameter_to_list(&list_lw_value_change, param_name, param_data, param_type, 0, false); }
static void udplw_server_param(struct addrinfo **res)
{
	struct addrinfo hints = { 0 };
	struct cwmp *cwmp = &cwmp_main;
	struct config *conf;
	char port[32];
	conf = &(cwmp->conf);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	snprintf(port, sizeof(port), "%d", conf->lw_notification_port);
	getaddrinfo(conf->lw_notification_hostname, port, &hints, res);
}

char *calculate_lwnotification_cnonce()
{
	char *cnonce = calloc(33, sizeof(char));
	if (cnonce == NULL)
		return NULL;

	char *rand = generate_random_string(16);
	if (rand == NULL) {
		free(cnonce);
		return NULL;
	}

	snprintf(cnonce, 33, "%s", rand);
	free(rand);
	return cnonce;
}

static void send_udp_message(struct addrinfo *servaddr, char *msg)
{
	int fd;

	fd = socket(servaddr->ai_family, SOCK_DGRAM, 0);

	if (fd >= 0) {
		sendto(fd, msg, strlen(msg), 0, servaddr->ai_addr, servaddr->ai_addrlen);
		close(fd);
	}
}

void del_list_lw_notify(struct cwmp_dm_parameter *dm_parameter)
{
	list_del(&dm_parameter->list);
	free(dm_parameter->name);
	free(dm_parameter);
}

static void free_all_list_lw_notify()
{
	while (list_lw_value_change.next != &list_lw_value_change) {
		struct cwmp_dm_parameter *dm_parameter;
		if (list_lw_value_change.next == NULL)
			continue;
		dm_parameter = list_entry(list_lw_value_change.next, struct cwmp_dm_parameter, list);
		del_list_lw_notify(dm_parameter);
	}
}

void cwmp_lwnotification()
{
	char msg[1024], *msg_out;
	char signature[41];
	struct addrinfo *servaddr;
	struct cwmp *cwmp = &cwmp_main;
	struct config *conf;
	conf = &(cwmp->conf);

	udplw_server_param(&servaddr);
	xml_prepare_lwnotification_message(&msg_out);
	message_compute_signature(msg_out, signature, sizeof(signature));
	snprintf(msg, sizeof(msg), "%s \n %s: %s \n %s: %s \n %s: %zu\n %s: %s\n\n%s", "POST /HTTPS/1.1", "HOST", conf->lw_notification_hostname, "Content-Type", "test/xml; charset=utf-8", "Content-Lenght", strlen(msg_out), "Signature", signature, msg_out);

	send_udp_message(servaddr, msg);
	free_all_list_lw_notify();
	//freeaddrinfo(servaddr); //To check
	FREE(msg_out);
}
