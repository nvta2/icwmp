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

LIST_HEAD(list_value_change);
LIST_HEAD(list_lw_value_change);
pthread_mutex_t mutex_value_change = PTHREAD_MUTEX_INITIALIZER;

int cwmp_update_enabled_notify_file()
{
	struct cwmp *cwmp = &cwmp_main;
	FILE *fp;
	LIST_HEAD(list_notify);
	int e = cwmp_update_enabled_list_notify(cwmp->conf.instance_mode, &list_notify);
	if (e)
		return 0;
	remove(DM_ENABLED_NOTIFY);

	fp = fopen(DM_ENABLED_NOTIFY, "a");
	if (fp == NULL) {
		cwmp_free_all_dm_parameter_list(&list_notify);
		return 0;
	}
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &list_notify, list) {
		char *notif_line = NULL;
		cwmp_asprintf(&notif_line, "parameter:%s notifcation:%d type:%s value:%s", param_value->name, param_value->notification, param_value->type, param_value->value);
		fprintf(fp, "%s\n", notif_line);
		FREE(notif_line);
	}
	fclose(fp);
	cwmp_free_all_dm_parameter_list(&list_notify);
	return 1;
}

void get_parameter_value_from_parameters_list(struct list_head *list_notif, char *parameter_name, struct cwmp_dm_parameter **ret_dm_param)
{
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, list_notif, list) {
		if (param_value->name && strcmp(param_value->name, parameter_name) != 0) {
			continue;
		}
		*ret_dm_param = (struct cwmp_dm_parameter *)calloc(1, sizeof(struct cwmp_dm_parameter));
		(*ret_dm_param)->name = strdup(param_value->name);
		(*ret_dm_param)->value = strdup(param_value->value);
		(*ret_dm_param)->type = strdup(param_value->type);
		break;
	}
}

int check_value_change(void)
{
	FILE *fp;
	char buf[1280];

	struct cwmp *cwmp = &cwmp_main;
	struct cwmp_dm_parameter *dm_parameter = NULL;
	int is_notify = 0;
	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL)
		return false;
	LIST_HEAD(list_notify);
	cwmp_update_enabled_list_notify(cwmp->conf.instance_mode, &list_notify);
	while (fgets(buf, 1280, fp) != NULL) {
		int len = strlen(buf);
		if (len)
			buf[len - 1] = '\0';
		char parameter[128] = { 0 }, notification[2] = { 0 }, value[1024] = { 0 }, type[32] = { 0 };
		sscanf(buf, "parameter:%s notifcation:%s type:%s value:%s\n", parameter, notification, type, value);
		get_parameter_value_from_parameters_list(&list_notify, parameter, &dm_parameter);
		if (dm_parameter == NULL)
			continue;
		if ((strlen(notification) > 0) && (notification[0] >= '1') && (dm_parameter->value != NULL) && (strcmp(dm_parameter->value, value) != 0)) {
			if (notification[0] == '1' || notification[0] == '2')
				add_list_value_change(parameter, dm_parameter->value, dm_parameter->type);
			if (notification[0] >= '3')
				add_lw_list_value_change(parameter, dm_parameter->value, dm_parameter->type);

			if (notification[0] == '1')
				is_notify |= NOTIF_PASSIVE;
			if (notification[0] == '2')
				is_notify |= NOTIF_ACTIVE;

			if (notification[0] == '5' || notification[0] == '6')
				is_notify |= NOTIF_LW_ACTIVE;
		}
		FREE(dm_parameter->name);
		FREE(dm_parameter->value);
		FREE(dm_parameter->type);
		FREE(dm_parameter);
	}
	fclose(fp);
	cwmp_free_all_dm_parameter_list(&list_notify);
	return is_notify;
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
			pthread_cond_timedwait(&(cwmp->threshold_notify_periodic), &(cwmp->mutex_notify_periodic), &periodic_timeout);
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
	return CWMP_OK;
}

void add_list_value_change(char *param_name, char *param_data, char *param_type)
{
	pthread_mutex_lock(&(mutex_value_change));
	add_dm_parameter_to_list(&list_value_change, param_name, param_data, param_type, 0, false);
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
	char *port;
	conf = &(cwmp->conf);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	cwmp_asprintf(&port, "%d", conf->lw_notification_port);
	getaddrinfo(conf->lw_notification_hostname, port, &hints, res);
	//FREE(port);
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
	srand((unsigned int)time(NULL));
	for (i = 0; i < 4; i++) {
		sprintf(&(cnonce[i * 8]), "%08x", rand());
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
	char *msg, *msg_out;
	char signature[41];
	struct addrinfo *servaddr;
	struct cwmp *cwmp = &cwmp_main;
	struct config *conf;
	conf = &(cwmp->conf);

	udplw_server_param(&servaddr);
	xml_prepare_lwnotification_message(&msg_out);
	message_compute_signature(msg_out, signature);
	cwmp_asprintf(&msg, "%s \n %s: %s \n %s: %s \n %s: %zd\n %s: %s\n\n%s", "POST /HTTPS/1.1", "HOST", conf->lw_notification_hostname, "Content-Type", "test/xml; charset=utf-8", "Content-Lenght", strlen(msg_out), "Signature", signature, msg_out);

	send_udp_message(servaddr, msg);
	free_all_list_lw_notify();
	//freeaddrinfo(servaddr); //To check
	FREE(msg);
	FREE(msg_out);
}
