/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <pthread.h>

#include "backupSession.h"
#include "log.h"
#include "event.h"
#include "session.h"
#include "cwmp_du_state.h"
#include "download.h"
#include "rpc_soap.h"
#include "upload.h"
#include "sched_inform.h"

const struct EVENT_CONST_STRUCT EVENT_CONST[] = {[EVENT_IDX_0BOOTSTRAP] = { "0 BOOTSTRAP", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_1BOOT] = { "1 BOOT", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL },
						 [EVENT_IDX_2PERIODIC] = { "2 PERIODIC", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_3SCHEDULED] = { "3 SCHEDULED", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_4VALUE_CHANGE] = { "4 VALUE CHANGE", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL },
						 [EVENT_IDX_5KICKED] = { "5 KICKED", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_6CONNECTION_REQUEST] = { "6 CONNECTION REQUEST", EVENT_TYPE_SINGLE, 0 },
						 [EVENT_IDX_7TRANSFER_COMPLETE] = { "7 TRANSFER COMPLETE", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_8DIAGNOSTICS_COMPLETE] = { "8 DIAGNOSTICS COMPLETE", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL },
						 [EVENT_IDX_9REQUEST_DOWNLOAD] = { "9 REQUEST DOWNLOAD", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_10AUTONOMOUS_TRANSFER_COMPLETE] = { "10 AUTONOMOUS TRANSFER COMPLETE", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_11DU_STATE_CHANGE_COMPLETE] = { "11 DU STATE CHANGE COMPLETE", EVENT_TYPE_SINGLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_Reboot] = { "M Reboot", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_ScheduleInform] = { "M ScheduleInform", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_Download] = { "M Download", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_Schedule_Download] = { "M ScheduleDownload", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_Upload] = { "M Upload", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT },
						 [EVENT_IDX_M_ChangeDUState] = { "M ChangeDUState", EVENT_TYPE_MULTIPLE, EVENT_RETRY_AFTER_TRANSMIT_FAIL | EVENT_RETRY_AFTER_REBOOT } };

void cwmp_save_event_container(struct event_container *event_container) //to be moved to backupsession
{
	if (EVENT_CONST[event_container->code].RETRY & EVENT_RETRY_AFTER_REBOOT) {
		struct list_head *ilist;
		mxml_node_t *b;

		b = bkp_session_insert_event(event_container->code, event_container->command_key, event_container->id, "queue");

		list_for_each (ilist, &(event_container->head_dm_parameter)) {
			struct cwmp_dm_parameter *dm_parameter;
			dm_parameter = list_entry(ilist, struct cwmp_dm_parameter, list);
			bkp_session_insert_parameter(b, dm_parameter->name);
		}
		bkp_session_save();
	}
}

struct event_container *cwmp_add_event_container(struct cwmp *cwmp, int event_code, char *command_key)
{
	static int id;
	struct event_container *event_container;
	struct session *session;
	struct list_head *ilist;

	if (cwmp->head_event_container == NULL) {
		session = cwmp_add_queue_session(cwmp);
		if (session == NULL) {
			return NULL;
		}
		cwmp->head_event_container = &(session->head_event_container);
	}
	session = list_entry(cwmp->head_event_container, struct session, head_event_container);
	list_for_each (ilist, cwmp->head_event_container) {
		event_container = list_entry(ilist, struct event_container, list);
		if (event_container->code == event_code && EVENT_CONST[event_code].TYPE == EVENT_TYPE_SINGLE) {
			return event_container;
		}
		if (event_container->code > event_code) {
			break;
		}
	}
	event_container = calloc(1, sizeof(struct event_container));
	if (event_container == NULL) {
		return NULL;
	}
	INIT_LIST_HEAD(&(event_container->head_dm_parameter));
	list_add(&(event_container->list), ilist->prev);
	event_container->code = event_code;
	event_container->command_key = command_key ? strdup(command_key) : strdup("");
	if ((id < 0) || (id >= MAX_INT_ID)) {
		id = 0;
	}
	id++;
	event_container->id = id;
	return event_container;
}

void cwmp_root_cause_event_ipdiagnostic(void)
{
	struct cwmp *cwmp = &cwmp_main;
	struct event_container *event_container;

	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_8DIAGNOSTICS_COMPLETE, "");
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return;
	}
	cwmp_save_event_container(event_container);
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	pthread_cond_signal(&(cwmp->threshold_session_send));
	return;
}

int cwmp_root_cause_event_boot(struct cwmp *cwmp)
{
	if (cwmp->env.boot == CWMP_START_BOOT) {
		struct event_container *event_container;
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		cwmp->env.boot = 0;
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_1BOOT, "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	}
	return CWMP_OK;
}

int event_remove_all_event_container(struct session *session, int rem_from)
{
	while (session->head_event_container.next != &(session->head_event_container)) {
		struct event_container *event_container;
		event_container = list_entry(session->head_event_container.next, struct event_container, list);
		bkp_session_delete_event(event_container->id, rem_from ? "send" : "queue");
		free(event_container->command_key);
		cwmp_free_all_dm_parameter_list(&(event_container->head_dm_parameter));
		list_del(&(event_container->list));
		free(event_container);
	}
	bkp_session_save();
	return CWMP_OK;
}

int event_remove_noretry_event_container(struct session *session, struct cwmp *cwmp)
{
	struct list_head *ilist, *q;

	list_for_each_safe (ilist, q, &(session->head_event_container)) {
		struct event_container *event_container;
		event_container = list_entry(ilist, struct event_container, list);

		if (EVENT_CONST[event_container->code].CODE[0] == '6')
			cwmp->cwmp_cr_event = 1;

		if (EVENT_CONST[event_container->code].RETRY == 0) {
			free(event_container->command_key);
			cwmp_free_all_dm_parameter_list(&(event_container->head_dm_parameter));
			list_del(&(event_container->list));
			free(event_container);
		}
	}
	return CWMP_OK;
}

int cwmp_root_cause_event_bootstrap(struct cwmp *cwmp)
{

	struct event_container *event_container;
	char *acsurl = NULL;
	int cmp = 0;

	cwmp_load_saved_session(cwmp, &acsurl, ACS);

	if (acsurl == NULL)
		save_acs_bkp_config(cwmp);

	if (acsurl == NULL || ((cmp = strcmp(cwmp->conf.acsurl, acsurl)) != 0)) {
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		if (cwmp->head_event_container != NULL && cwmp->head_session_queue.next != &(cwmp->head_session_queue)) {
			struct session *session;
			session = list_entry(cwmp->head_event_container, struct session, head_event_container);
			event_remove_all_event_container(session, RPC_QUEUE);
		}
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_0BOOTSTRAP, "");
		FREE(acsurl);
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
		cwmp_scheduleInform_remove_all();
		cwmp_scheduledDownload_remove_all();
		cwmp_scheduled_Download_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
		cwmp_scheduledUpload_remove_all();
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	} else {
		FREE(acsurl);
	}

	if (cmp) {
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}

		char buf[64] = "Device.ManagementServer.URL";
		add_dm_parameter_to_list(&(event_container->head_dm_parameter), buf, NULL, NULL, 0, false);
		cwmp_save_event_container(event_container);
		save_acs_bkp_config(cwmp);
		cwmp_scheduleInform_remove_all();
		cwmp_scheduledDownload_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
		cwmp_scheduled_Download_remove_all();
		cwmp_scheduledUpload_remove_all();
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	}

	return CWMP_OK;
}

int cwmp_root_cause_transfer_complete(struct cwmp *cwmp, struct transfer_complete *p)
{
	struct event_container *event_container;
	struct session *session;
	struct rpc *rpc_acs;

	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_7TRANSFER_COMPLETE, "");
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	switch (p->type) {
	case TYPE_DOWNLOAD:
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_M_Download, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		break;
	case TYPE_UPLOAD:
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_M_Upload, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		break;
	case TYPE_SCHEDULE_DOWNLOAD:
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_M_Schedule_Download, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		break;
	}
	session = list_entry(cwmp->head_event_container, struct session, head_event_container);
	if ((rpc_acs = cwmp_add_session_rpc_acs(session, RPC_ACS_TRANSFER_COMPLETE)) == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	rpc_acs->extra_data = (void *)p;
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	return CWMP_OK;
}

int cwmp_root_cause_changedustate_complete(struct cwmp *cwmp, struct du_state_change_complete *p)
{
	struct event_container *event_container;
	struct session *session;
	struct rpc *rpc_acs;

	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_11DU_STATE_CHANGE_COMPLETE, "");
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}

	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_M_ChangeDUState, p->command_key ? p->command_key : "");
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	session = list_entry(cwmp->head_event_container, struct session, head_event_container);
	if ((rpc_acs = cwmp_add_session_rpc_acs(session, RPC_ACS_DU_STATE_CHANGE_COMPLETE)) == NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	rpc_acs->extra_data = (void *)p;
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	return CWMP_OK;
}

int cwmp_root_cause_get_rpc_method(struct cwmp *cwmp)
{
	if (cwmp->env.periodic == CWMP_START_PERIODIC) {
		struct event_container *event_container;
		struct session *session;

		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		cwmp->env.periodic = 0;
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_2PERIODIC, "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
		session = list_entry(cwmp->head_event_container, struct session, head_event_container);
		if (cwmp_add_session_rpc_acs(session, RPC_ACS_GET_RPC_METHODS) == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	}

	return CWMP_OK;
}

void *thread_event_periodic(void *v)
{
	struct cwmp *cwmp = (struct cwmp *)v;
	struct event_container *event_container;
	static int periodic_interval;
	static bool periodic_enable;
	static time_t periodic_time;
	static struct timespec periodic_timeout = { 0, 0 };
	time_t current_time;
	long int delta_time;

	periodic_interval = cwmp->conf.period;
	periodic_enable = cwmp->conf.periodic_enable;
	periodic_time = cwmp->conf.time;

	for (;;) {
		pthread_mutex_lock(&(cwmp->mutex_periodic));
		if (cwmp->conf.periodic_enable) {
			current_time = time(NULL);
			if (periodic_time != 0) {
				delta_time = (current_time - periodic_time) % periodic_interval;
				if (delta_time >= 0)
					periodic_timeout.tv_sec = current_time + periodic_interval - delta_time;
				else
					periodic_timeout.tv_sec = current_time - delta_time;
			} else {
				periodic_timeout.tv_sec = current_time + periodic_interval;
			}
			cwmp->session_status.next_periodic = periodic_timeout.tv_sec;
			pthread_cond_timedwait(&(cwmp->threshold_periodic), &(cwmp->mutex_periodic), &periodic_timeout);
		} else {
			cwmp->session_status.next_periodic = 0;
			pthread_cond_wait(&(cwmp->threshold_periodic), &(cwmp->mutex_periodic));
		}
		pthread_mutex_unlock(&(cwmp->mutex_periodic));

		if (thread_end)
			break;

		if (periodic_interval != cwmp->conf.period || periodic_enable != cwmp->conf.periodic_enable || periodic_time != cwmp->conf.time) {
			periodic_enable = cwmp->conf.periodic_enable;
			periodic_interval = cwmp->conf.period;
			periodic_time = cwmp->conf.time;
			continue;
		}
		CWMP_LOG(INFO, "Periodic thread: add periodic event in the queue");
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_2PERIODIC, "");
		if (event_container == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			continue;
		}
		cwmp_save_event_container(event_container);
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		pthread_cond_signal(&(cwmp->threshold_session_send));
	}
	return NULL;
}

bool event_exist_in_list(struct cwmp *cwmp, int event)
{
	struct event_container *event_container;
	list_for_each_entry (event_container, cwmp->head_event_container, list) {
		if (event_container->code == event)
			return true;
	}
	return false;
}

int cwmp_root_cause_event_periodic(struct cwmp *cwmp)
{
	static int period = 0;
	static bool periodic_enable = false;
	static time_t periodic_time = 0;
	char local_time[27] = { 0 };
	struct tm *t_tm;

	if (period == cwmp->conf.period && periodic_enable == cwmp->conf.periodic_enable && periodic_time == cwmp->conf.time)
		return CWMP_OK;

	pthread_mutex_lock(&(cwmp->mutex_periodic));
	period = cwmp->conf.period;
	periodic_enable = cwmp->conf.periodic_enable;
	periodic_time = cwmp->conf.time;
	CWMP_LOG(INFO, periodic_enable ? "Periodic event is enabled. Interval period = %ds" : "Periodic event is disabled", period);

	t_tm = localtime(&periodic_time);
	if (t_tm == NULL)
		return CWMP_GEN_ERR;

	if (strftime(local_time, sizeof(local_time), "%FT%T%z", t_tm) == 0)
		return CWMP_GEN_ERR;

	local_time[25] = local_time[24];
	local_time[24] = local_time[23];
	local_time[22] = ':';
	local_time[26] = '\0';

	CWMP_LOG(INFO, periodic_time ? "Periodic time is %s" : "Periodic time is Unknown", local_time);
	pthread_mutex_unlock(&(cwmp->mutex_periodic));
	pthread_cond_signal(&(cwmp->threshold_periodic));
	return CWMP_OK;
}

void connection_request_ip_value_change(struct cwmp *cwmp, int version)
{
	char *bip = NULL;
	char *ip_version = (version == IPv6) ? "ipv6" : "ip";
	char *ip_value = (version == IPv6) ? cwmp->conf.ipv6 : cwmp->conf.ip;

	if (version == IPv6)
		cwmp_load_saved_session(cwmp, &bip, CR_IPv6);
	else
		cwmp_load_saved_session(cwmp, &bip, CR_IP);

	if (bip == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
		return;
	}
	if (strcmp(bip, ip_value) != 0) {
		struct event_container *event_container;
		pthread_mutex_lock(&(cwmp->mutex_session_queue));
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			FREE(bip);
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return;
		}
		cwmp_save_event_container(event_container);
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		pthread_cond_signal(&(cwmp->threshold_session_send));
	}
	FREE(bip);
}

void connection_request_port_value_change(struct cwmp *cwmp, int port)
{
	char *bport = NULL;
	char bufport[16];

	snprintf(bufport, sizeof(bufport), "%d", port);

	cwmp_load_saved_session(cwmp, &bport, CR_PORT);

	if (bport == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", "port", bufport);
		bkp_session_save();
		return;
	}
	if (strcmp(bport, bufport) != 0) {
		struct event_container *event_container;
		event_container = cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			FREE(bport);
			return;
		}
		cwmp_save_event_container(event_container);
		bkp_session_simple_insert_in_parent("connection_request", "port", bufport);
		bkp_session_save();
	}
	FREE(bport);
}

int cwmp_root_cause_events(struct cwmp *cwmp)
{
	int error;

	if ((error = cwmp_root_cause_event_bootstrap(cwmp)))
		return error;

	if ((error = cwmp_root_cause_event_boot(cwmp)))
		return error;

	if ((error = cwmp_root_cause_get_rpc_method(cwmp)))
		return error;

	if ((error = cwmp_root_cause_event_periodic(cwmp)))
		return error;

	return CWMP_OK;
}

int cwmp_get_int_event_code(char *code)
{
	if (code && code[0] == '1')
		return EVENT_IDX_1BOOT;

	else if (code && code[0] == '2')
		return EVENT_IDX_2PERIODIC;

	else if (code && code[0] == '3')
		return EVENT_IDX_3SCHEDULED;

	else if (code && code[0] == '4')
		return EVENT_IDX_4VALUE_CHANGE;

	else if (code && code[0] == '6')
		return EVENT_IDX_6CONNECTION_REQUEST;

	else if (code && code[0] == '8')
		return EVENT_IDX_8DIAGNOSTICS_COMPLETE;

	else
		return EVENT_IDX_6CONNECTION_REQUEST;
}
