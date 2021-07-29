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
#include <unistd.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <netdb.h>
#include <libubox/list.h>

#include "notifications.h"
#include "ubus.h"

LIST_HEAD(list_value_change);
LIST_HEAD(list_lw_value_change);
pthread_mutex_t mutex_value_change = PTHREAD_MUTEX_INITIALIZER;

void cwmp_update_enabled_notify_file_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	FILE *fp;
	int *int_ret = (int *)req->priv;
	struct blob_buf bbuf;
	*int_ret = get_fault(msg);
	if (*int_ret) {
		*int_ret = 0;
		return;
	}
	remove(DM_ENABLED_NOTIFY);
	fp = fopen(DM_ENABLED_NOTIFY, "a");
	if (fp == NULL) {
		*int_ret = 0;
		return;
	}
	struct blob_attr *parameters = get_parameters_array(msg);
	const struct blobmsg_policy p[5] = { { "parameter", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING }, { "type", BLOBMSG_TYPE_STRING }, { "notification", BLOBMSG_TYPE_STRING } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, parameters, rem)
	{
		struct blob_attr *tb[4] = { NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 4, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;
		memset(&bbuf, 0, sizeof(struct blob_buf));
		blob_buf_init(&bbuf, 0);
		blobmsg_add_string(&bbuf, "parameter", blobmsg_get_string(tb[0]));
		blobmsg_add_string(&bbuf, "notification", tb[3] ? blobmsg_get_string(tb[3]) : "");
		blobmsg_add_string(&bbuf, "type", tb[2] ? blobmsg_get_string(tb[2]) : "");
		blobmsg_add_string(&bbuf, "value", tb[1] ? blobmsg_get_string(tb[1]) : "");
		char *notification_line = blobmsg_format_json(bbuf.head, true);
		if (notification_line != NULL) {
			fprintf(fp, "%s\n", notification_line);
			FREE(notification_line);
		}
		blob_buf_free(&bbuf);
	}
	fclose(fp);
}

void get_parameter_value_from_parameters_list(struct blob_attr *list_notif, char *parameter_name, char **value, char **type)
{
	const struct blobmsg_policy p[5] = { { "parameter", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING }, { "type", BLOBMSG_TYPE_STRING } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, list_notif, rem)
	{
		struct blob_attr *tb[3] = { NULL, NULL, NULL };
		blobmsg_parse(p, 3, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;
		if (strcmp(parameter_name, blobmsg_get_string(tb[0])) != 0)
			continue;
		*value = strdup(tb[1] ? blobmsg_get_string(tb[1]) : "");
		*type = strdup(tb[2] ? blobmsg_get_string(tb[2]) : "");
		break;
	}
}

void ubus_check_value_change_callback(struct ubus_request *req, int typearg __attribute__((unused)), struct blob_attr *msg)
{
	FILE *fp;
	char buf[1280];
	char *dm_value = NULL, *dm_type = NULL;
	int *int_ret = (int *)req->priv;
	struct blob_buf bbuf;

	char *parameter = NULL, *notification = NULL, *value = NULL, *type = NULL;

	*int_ret = get_fault(msg);
	if (*int_ret) {
		*int_ret = 0;
		return;
	}

	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL) {
		*int_ret = 0;
		return;
	}

	struct blob_attr *list_notify = get_parameters_array(msg);
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
		const struct blobmsg_policy p[4] = { { "parameter", BLOBMSG_TYPE_STRING }, { "notification", BLOBMSG_TYPE_STRING }, { "type", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING } };
		struct blob_attr *tb[4] = { NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 4, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
		parameter = blobmsg_get_string(tb[0]);
		notification = blobmsg_get_string(tb[1]);
		type = blobmsg_get_string(tb[2]);
		value = blobmsg_get_string(tb[3]);
		get_parameter_value_from_parameters_list(list_notify, parameter, &dm_value, &dm_type);
		if (dm_value == NULL && dm_type == NULL) {
			blob_buf_free(&bbuf);
			parameter = NULL;
			notification = NULL;
			type = NULL;
			value = NULL;
			continue;
		}
		if ((strlen(notification) > 0) && (notification[0] >= '1') && (dm_value != NULL) && (strcmp(dm_value, value) != 0)) {
			if (notification[0] == '1' || notification[0] == '2')
				add_list_value_change(parameter, dm_value, dm_type);
			if (notification[0] >= '3')
				add_lw_list_value_change(parameter, dm_value, dm_type);

			if (notification[0] == '1')
				*int_ret |= NOTIF_PASSIVE;
			if (notification[0] == '2')
				*int_ret |= NOTIF_ACTIVE;

			if (notification[0] == '5' || notification[0] == '6')
				*int_ret |= NOTIF_LW_ACTIVE;
		}
		FREE(dm_value);
		FREE(dm_type);
		parameter = NULL;
		notification = NULL;
		type = NULL;
		value = NULL;
		blob_buf_free(&bbuf);
	}
	fclose(fp);
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
	static int periodic_interval;
	static bool periodic_enable;
	static struct timespec periodic_timeout = { 0, 0 };
	time_t current_time;
	int is_notify = 0;

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

static void message_compute_signature(char *msg_out, char *signature)
{
	int i;
	int result_len = 20;
	unsigned char *result;
	struct cwmp *cwmp = &cwmp_main;
	struct config *conf;
	conf = &(cwmp->conf);
	/*	unsigned char *HMAC(const EVP_MD *evp_md, const void *key, int key_len,
	                    const unsigned char *d, size_t n, unsigned char *md,
	                    unsigned int *md_len);*/
	result = HMAC(EVP_sha1(), conf->acs_passwd, strlen(conf->acs_passwd), (unsigned char *)msg_out, strlen(msg_out), NULL, NULL);
	for (i = 0; i < result_len; i++) {
		sprintf(&(signature[i * 2]), "%02X", result[i]);
	}
	signature[i * 2] = '\0';
	FREE(result);
}

char *calculate_lwnotification_cnonce()
{
	int i;
	char *cnonce = malloc(33 * sizeof(char));
	icwmp_srand((unsigned int)time(NULL));
	for (i = 0; i < 4; i++) {
		sprintf(&(cnonce[i * 8]), "%08x", icwmp_rand());
	}
	cnonce[i * 8] = '\0';
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
	struct cwmp_dm_parameter *dm_parameter;
	while (list_lw_value_change.next != &list_lw_value_change) {
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
	message_compute_signature(msg_out, signature);
	snprintf(msg, sizeof(msg), "%s \n %s: %s \n %s: %s \n %s: %zd\n %s: %s\n\n%s", "POST /HTTPS/1.1", "HOST", conf->lw_notification_hostname, "Content-Type", "test/xml; charset=utf-8", "Content-Lenght", strlen(msg_out), "Signature", signature, msg_out);

	send_udp_message(servaddr, msg);
	free_all_list_lw_notify();
	//freeaddrinfo(servaddr); //To check
	FREE(msg_out);
}
