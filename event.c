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
#include "soap.h"
#include "upload.h"
#include "sched_inform.h"

pthread_mutex_t add_event_mutext = PTHREAD_MUTEX_INITIALIZER;

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

struct event_container *__cwmp_add_event_container(int event_code, char *command_key)
{
	static int id;
	struct event_container *event_container;
	list_for_each_entry(event_container, &cwmp_main->session->events, list) {
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
	list_add_tail(&(event_container->list), &(cwmp_main->session->events));
	event_container->code = event_code;
	event_container->command_key = command_key ? strdup(command_key) : strdup("");
	if ((id < 0) || (id >= MAX_INT_ID)) {
		id = 0;
	}
	id++;
	event_container->id = id;
	return event_container;
}

struct event_container *cwmp_add_event_container(int event_code, char *command_key)
{
	pthread_mutex_lock(&add_event_mutext);
	struct event_container *event = __cwmp_add_event_container(event_code, command_key);
	pthread_mutex_unlock(&add_event_mutext);
	return event;
}

void cwmp_root_cause_event_ipdiagnostic(void)
{
	struct event_container *event_container;

	event_container = cwmp_add_event_container(EVENT_IDX_8DIAGNOSTICS_COMPLETE, "");
	if (event_container == NULL) {
		return;
	}
	cwmp_save_event_container(event_container);
	return;
}

int cwmp_root_cause_event_boot()
{
	if (cwmp_main->env.boot == CWMP_START_BOOT) {
		struct event_container *event_container;
		cwmp_main->env.boot = 0;
		event_container = cwmp_add_event_container(EVENT_IDX_1BOOT, "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
	}
	return CWMP_OK;
}

int event_remove_all_event_container(int rem_from)
{
	while (cwmp_main->session->events.next != &cwmp_main->session->events) {
		struct event_container *event_container;
		event_container = list_entry(cwmp_main->session->events.next, struct event_container, list);
		bkp_session_delete_event(event_container->id, rem_from ? "send" : "queue");
		free(event_container->command_key);
		cwmp_free_all_dm_parameter_list(&(event_container->head_dm_parameter));
		list_del(&(event_container->list));
		free(event_container);
	}
	bkp_session_save();
	return CWMP_OK;
}

int event_remove_noretry_event_container()
{
	struct list_head *ilist, *q;

	list_for_each_safe (ilist, q, &cwmp_main->session->events) {
		struct event_container *event_container;
		event_container = list_entry(ilist, struct event_container, list);

		if (EVENT_CONST[event_container->code].CODE[0] == '6')
			cwmp_main->cwmp_cr_event = 1;

		if (EVENT_CONST[event_container->code].RETRY == 0) {
			free(event_container->command_key);
			cwmp_free_all_dm_parameter_list(&(event_container->head_dm_parameter));
			list_del(&(event_container->list));
			free(event_container);
		}
	}
	return CWMP_OK;
}

int cwmp_root_cause_event_bootstrap()
{

	struct event_container *event_container;
	char *acsurl = NULL;
	int cmp = 0;

	cwmp_load_saved_session(&acsurl, ACS);

	if (acsurl == NULL)
		save_acs_bkp_config();

	if (acsurl == NULL || ((cmp = strcmp(cwmp_main->conf.acsurl, acsurl)) != 0)) {
		event_container = cwmp_add_event_container(EVENT_IDX_0BOOTSTRAP, "");
		FREE(acsurl);
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
		cwmp_scheduleInform_remove_all();
		cwmp_scheduledDownload_remove_all();
		cwmp_scheduled_Download_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
		cwmp_scheduledUpload_remove_all();
	} else {
		FREE(acsurl);
	}

	if (cmp) {
		event_container = cwmp_add_event_container(EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}

		char buf[64] = "Device.ManagementServer.URL";
		add_dm_parameter_to_list(&(event_container->head_dm_parameter), buf, NULL, NULL, 0, false);
		cwmp_save_event_container(event_container);
		save_acs_bkp_config(cwmp_main);
		cwmp_scheduleInform_remove_all();
		cwmp_scheduledDownload_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
		cwmp_scheduled_Download_remove_all();
		cwmp_scheduledUpload_remove_all();
	}

	return CWMP_OK;
}

int cwmp_root_cause_transfer_complete(struct transfer_complete *p)
{
	struct event_container *event_container;
	struct rpc *rpc_acs;

	event_container = cwmp_add_event_container(EVENT_IDX_7TRANSFER_COMPLETE, "");
	if (event_container == NULL) {
		return CWMP_MEM_ERR;
	}
	switch (p->type) {
	case TYPE_DOWNLOAD:
		event_container = cwmp_add_event_container(EVENT_IDX_M_Download, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		break;
	case TYPE_UPLOAD:
		event_container = cwmp_add_event_container(EVENT_IDX_M_Upload, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		break;
	case TYPE_SCHEDULE_DOWNLOAD:
		event_container = cwmp_add_event_container(EVENT_IDX_M_Schedule_Download, p->command_key ? p->command_key : "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		break;
	}
	if ((rpc_acs = cwmp_add_session_rpc_acs(RPC_ACS_TRANSFER_COMPLETE)) == NULL) {
		return CWMP_MEM_ERR;
	}
	rpc_acs->extra_data = (void *)p;
	return CWMP_OK;
}

int cwmp_root_cause_changedustate_complete(struct du_state_change_complete *p)
{
	struct event_container *event_container;
	struct rpc *rpc_acs;

	event_container = cwmp_add_event_container(EVENT_IDX_11DU_STATE_CHANGE_COMPLETE, "");
	if (event_container == NULL) {
		return CWMP_MEM_ERR;
	}

	event_container = cwmp_add_event_container(EVENT_IDX_M_ChangeDUState, p->command_key ? p->command_key : "");
	if (event_container == NULL) {
		return CWMP_MEM_ERR;
	}
	if ((rpc_acs = cwmp_add_session_rpc_acs(RPC_ACS_DU_STATE_CHANGE_COMPLETE)) == NULL) {
		return CWMP_MEM_ERR;
	}
	rpc_acs->extra_data = (void *)p;
	return CWMP_OK;
}

int cwmp_root_cause_get_rpc_method()
{
	if (cwmp_main->env.periodic == CWMP_START_PERIODIC) {
		struct event_container *event_container;

		cwmp_main->env.periodic = 0;
		event_container = cwmp_add_event_container(EVENT_IDX_2PERIODIC, "");
		if (event_container == NULL) {
			return CWMP_MEM_ERR;
		}
		cwmp_save_event_container(event_container);
		if (cwmp_add_session_rpc_acs(RPC_ACS_GET_RPC_METHODS) == NULL) {
			return CWMP_MEM_ERR;
		}
	}

	return CWMP_OK;
}

bool event_exist_in_list(int event)
{
	struct event_container *event_container;
	list_for_each_entry (event_container, &cwmp_main->session->events, list) {
		if (event_container->code == event)
			return true;
	}
	return false;
}

int cwmp_root_cause_event_periodic()
{
	static int period = 0;
	static bool periodic_enable = false;
	static time_t periodic_time = 0;
	char local_time[27] = { 0 };
	struct tm *t_tm;

	if (period == cwmp_main->conf.period && periodic_enable == cwmp_main->conf.periodic_enable && periodic_time == cwmp_main->conf.time)
		return CWMP_OK;

	period = cwmp_main->conf.period;
	periodic_enable = cwmp_main->conf.periodic_enable;
	periodic_time = cwmp_main->conf.time;
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
	return CWMP_OK;
}

void connection_request_ip_value_change(int version)
{
	char *bip = NULL;
	char *ip_version = (version == IPv6) ? "ipv6" : "ip";
	char *ip_value = (version == IPv6) ? cwmp_main->conf.ipv6 : cwmp_main->conf.ip;

	if (version == IPv6)
		cwmp_load_saved_session(&bip, CR_IPv6);
	else
		cwmp_load_saved_session(&bip, CR_IP);

	if (bip == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
		return;
	}
	if (strcmp(bip, ip_value) != 0) {
		struct event_container *event_container;
		event_container = cwmp_add_event_container(EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			FREE(bip);
			return;
		}
		cwmp_save_event_container(event_container);
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
	}
	FREE(bip);
}

void connection_request_port_value_change(int port)
{
	char *bport = NULL;
	char bufport[16];

	snprintf(bufport, sizeof(bufport), "%d", port);

	cwmp_load_saved_session(&bport, CR_PORT);

	if (bport == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", "port", bufport);
		bkp_session_save();
		return;
	}
	if (strcmp(bport, bufport) != 0) {
		struct event_container *event_container;
		event_container = cwmp_add_event_container(EVENT_IDX_4VALUE_CHANGE, "");
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

int cwmp_root_cause_events()
{
	int error;

	if ((error = cwmp_root_cause_event_bootstrap()))
		return error;

	if ((error = cwmp_root_cause_event_boot()))
		return error;

	if ((error = cwmp_root_cause_get_rpc_method()))
		return error;

	if ((error = cwmp_root_cause_event_periodic()))
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
