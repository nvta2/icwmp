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
 *	Copyright (C) 2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#include <sys/socket.h>

#include "ubus.h"
#include "session.h"
#include "log.h"
#include "cwmp_time.h"
#include "event.h"
#include "backupSession.h"
#include "sched_inform.h"
#include "cwmp_du_state.h"
#include "download.h"
#include "upload.h"
#include "http.h"
#include "soap.h"
#include "cwmp_event.h"

static struct ubus_context *ctx = NULL;

static const char *arr_session_status[] = {
		[SESSION_WAITING] = "waiting",
		[SESSION_RUNNING] = "running",
		[SESSION_FAILURE] = "failure",
		[SESSION_SUCCESS] = "success",
};

enum command
{
	COMMAND_NAME,
	__COMMAND_MAX
};

static const struct blobmsg_policy command_policy[] = {
		[COMMAND_NAME] = {.name = "command", .type = BLOBMSG_TYPE_STRING },
};

static int cwmp_handle_command(struct ubus_context *ctx, struct ubus_object *obj __attribute__((unused)), struct ubus_request_data *req, const char *method __attribute__((unused)), struct blob_attr *msg)
{
	struct blob_attr *tb[__COMMAND_MAX];
	static struct blob_buf blob_command;

	blobmsg_parse(command_policy, ARRAYSIZEOF(command_policy), tb, blob_data(msg), blob_len(msg));

	if (!tb[COMMAND_NAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	memset(&blob_command, 0, sizeof(struct blob_buf));
	blob_buf_init(&blob_command, 0);

	char *cmd = blobmsg_data(tb[COMMAND_NAME]);
	char info[128] = {0};

	if (!strcmp("reload_end_session", cmd)) {
		CWMP_LOG(INFO, "triggered ubus reload_end_session");
		cwmp_set_end_session(END_SESSION_RELOAD);
		blobmsg_add_u32(&blob_command, "status", 0);
		if (snprintf(info, sizeof(info), "icwmpd config will reload at the end of the session") == -1)
			return -1;
	} else if (!strcmp("reload", cmd)) {
		CWMP_LOG(INFO, "triggered ubus reload");
		if (cwmp_main->session->session_status.last_status == SESSION_RUNNING) {
			cwmp_set_end_session(END_SESSION_RELOAD);
			blobmsg_add_u32(&blob_command, "status", 0);
			if (snprintf(info, sizeof(info), "Session running, reload at the end of the session") == -1)
				return -1;
		} else {
			cwmp_apply_acs_changes();
			blobmsg_add_u32(&blob_command, "status", 0);
			if (snprintf(info, sizeof(info), "icwmpd config reloaded") == -1)
				return -1;
		}
	} else if (!strcmp("reboot_end_session", cmd)) {
		CWMP_LOG(INFO, "triggered ubus reboot_end_session");
		cwmp_set_end_session(END_SESSION_REBOOT);
		blobmsg_add_u32(&blob_command, "status", 0);
		if (snprintf(info, sizeof(info), "icwmpd will reboot at the end of the session") == -1)
			return -1;
	} else if (!strcmp("action_end_session", cmd)) {
		CWMP_LOG(INFO, "triggered ubus action_end_session");
		cwmp_set_end_session(END_SESSION_EXTERNAL_ACTION);
		blobmsg_add_u32(&blob_command, "status", 0);
		if (snprintf(info, sizeof(info), "icwmpd will execute the scheduled action commands at the end of the session") == -1)
			return -1;
	} else if (!strcmp("exit", cmd)) {
		ubus_exit = true;
		
		if (snprintf(info, sizeof(info), "icwmpd daemon stopped") == -1)
					return -1;

		cwmp_stop = true;

		if (cwmp_main->session->session_status.last_status == SESSION_RUNNING)
			http_set_timeout();

		uloop_timeout_cancel(&retry_session_timer);
		uloop_timeout_cancel(&priodic_session_timer);
		uloop_timeout_cancel(&session_timer);
		uloop_end();
		shutdown(cwmp_main->cr_socket_desc, SHUT_RDWR);

	} else {
		blobmsg_add_u32(&blob_command, "status", -1);
		if (snprintf(info, sizeof(info), "%s command is not supported", cmd) == -1)
			return -1;
	}

	blobmsg_add_string(&blob_command, "info", info);

	ubus_send_reply(ctx, req, blob_command.head);
	blob_buf_free(&blob_command);

	return 0;
}

static inline time_t get_session_status_next_time()
{
	time_t ntime = 0;
	if (list_schedule_inform.next != &(list_schedule_inform)) {
		struct schedule_inform *schedule_inform;
		schedule_inform = list_entry(list_schedule_inform.next, struct schedule_inform, list);
		ntime = schedule_inform->scheduled_time;
	}
	if (!ntime || (cwmp_main->session->session_status.next_retry && ntime > cwmp_main->session->session_status.next_retry)) {
		ntime = cwmp_main->session->session_status.next_retry;
	}
	if (!ntime || (cwmp_main->session->session_status.next_periodic && ntime > cwmp_main->session->session_status.next_periodic)) {
		ntime = cwmp_main->session->session_status.next_periodic;
	}
	return ntime;
}

static int cwmp_handle_status(struct ubus_context *ctx, struct ubus_object *obj __attribute__((unused)), struct ubus_request_data *req, const char *method __attribute__((unused)), struct blob_attr *msg __attribute__((unused)))
{
	void *c;
	time_t ntime = 0;
	static struct blob_buf blob_status;

	blob_buf_init(&blob_status, 0);

	c = blobmsg_open_table(&blob_status, "cwmp");
	blobmsg_add_string(&blob_status, "status", "up");
	blobmsg_add_string(&blob_status, "start_time", mix_get_time_of(cwmp_main->start_time));
	blobmsg_add_string(&blob_status, "acs_url", cwmp_main->conf.acsurl);
	blobmsg_close_table(&blob_status, c);

	c = blobmsg_open_table(&blob_status, "last_session");
	blobmsg_add_string(&blob_status, "status", cwmp_main->session->session_status.last_start_time ? arr_session_status[cwmp_main->session->session_status.last_status] : "N/A");
	blobmsg_add_string(&blob_status, "start_time", cwmp_main->session->session_status.last_start_time ? mix_get_time_of(cwmp_main->session->session_status.last_start_time) : "N/A");
	blobmsg_add_string(&blob_status, "end_time", cwmp_main->session->session_status.last_end_time ? mix_get_time_of(cwmp_main->session->session_status.last_end_time) : "N/A");
	blobmsg_close_table(&blob_status, c);

	c = blobmsg_open_table(&blob_status, "next_session");
	blobmsg_add_string(&blob_status, "status", arr_session_status[SESSION_WAITING]);
	ntime = get_session_status_next_time();
	blobmsg_add_string(&blob_status, "start_time", ntime ? mix_get_time_of(ntime) : "N/A");
	blobmsg_add_string(&blob_status, "end_time", "N/A");
	blobmsg_close_table(&blob_status, c);

	c = blobmsg_open_table(&blob_status, "statistics");
	blobmsg_add_u32(&blob_status, "success_sessions", cwmp_main->session->session_status.success_session);
	blobmsg_add_u32(&blob_status, "failure_sessions", cwmp_main->session->session_status.failure_session);
	blobmsg_add_u32(&blob_status, "total_sessions", cwmp_main->session->session_status.success_session + cwmp_main->session->session_status.failure_session);
	blobmsg_close_table(&blob_status, c);

	ubus_send_reply(ctx, req, blob_status.head);
	blob_buf_free(&blob_status);

	return 0;
}

enum enum_inform
{
	INFORM_GET_RPC_METHODS,
	INFORM_EVENT,
	__INFORM_MAX
};

static const struct blobmsg_policy inform_policy[] = {
		[INFORM_GET_RPC_METHODS] = {.name = "GetRPCMethods", .type = BLOBMSG_TYPE_BOOL },
		[INFORM_EVENT] = {.name = "event", .type = BLOBMSG_TYPE_STRING },
};

static int cwmp_handle_inform(struct ubus_context *ctx, struct ubus_object *obj __attribute__((unused)), struct ubus_request_data *req, const char *method __attribute__((unused)), struct blob_attr *msg)
{
	struct blob_attr *tb[__INFORM_MAX];
	bool grm = false;
	char *event = "";
	static struct blob_buf blob_inform;

	blob_buf_init(&blob_inform, 0);

	blobmsg_parse(inform_policy, ARRAYSIZEOF(inform_policy), tb, blob_data(msg), blob_len(msg));

	if (tb[INFORM_GET_RPC_METHODS]) {
		grm = blobmsg_data(tb[INFORM_GET_RPC_METHODS]);
	}
	if (tb[INFORM_EVENT]) {
		event = blobmsg_data(tb[INFORM_EVENT]);
	}

	if (grm) {
		struct event_container *event_container;

		event_container = cwmp_add_event_container(EVENT_IDX_2PERIODIC, "");
		if (event_container == NULL)
			return 0;

		cwmp_save_event_container(event_container);
		//session = list_entry(cwmp_main.head_event_container, struct session, head_event_container);
		if (cwmp_add_session_rpc_acs(RPC_ACS_GET_RPC_METHODS) == NULL)
			return 0;

		blobmsg_add_u32(&blob_inform, "status", 1);
		blobmsg_add_string(&blob_inform, "info", "Session with GetRPCMethods will start");
	} else {
		int event_code = cwmp_get_int_event_code(event);
		cwmp_add_event_container(event_code, "");
		if (cwmp_main->session->session_status.last_status == SESSION_RUNNING) {
			blobmsg_add_u32(&blob_inform, "status", -1);
			blobmsg_add_string(&blob_inform, "info", "Session already running, event will be sent at the end of the session");
		} else {
			blobmsg_add_u32(&blob_inform, "status", 1);
			blobmsg_add_string(&blob_inform, "info", "Session started");
		}
	}
	trigger_cwmp_session_timer();
	ubus_send_reply(ctx, req, blob_inform.head);
	blob_buf_free(&blob_inform);
	return 0;
}

static const struct ubus_method freecwmp_methods[] = {
	UBUS_METHOD("command", cwmp_handle_command, command_policy),
	UBUS_METHOD_NOARG("status", cwmp_handle_status),
	UBUS_METHOD("inform", cwmp_handle_inform, inform_policy),
};

static struct ubus_object_type main_object_type = UBUS_OBJECT_TYPE("freecwmpd", freecwmp_methods);

static struct ubus_object main_object = {
	.name = "tr069",
	.type = &main_object_type,
	.methods = freecwmp_methods,
	.n_methods = ARRAYSIZEOF(freecwmp_methods),
};

int cwmp_ubus_init()
{
	ctx = ubus_connect(cwmp_main->conf.ubus_socket);
	if (!ctx)
		return -1;

	ubus_add_uloop(ctx);

	if (ubus_add_object(ctx, &main_object))
		return -1;

	return 0;
}

void cwmp_ubus_exit(void)
{
	if (ctx) {
		ubus_remove_object(ctx, &main_object);
		ubus_free(ctx);
	}
}

int cwmp_ubus_call(const char *obj, const char *method, const struct cwmp_ubus_arg u_args[], int u_args_size, void (*ubus_callback)(struct ubus_request *req, int type, struct blob_attr *msg), void *callback_arg)
{
	uint32_t id;
	int i = 0;
	int rc = 0;
	struct blob_buf b = { 0 };

	struct ubus_context *ubus_ctx = NULL;

	ubus_ctx = ubus_connect(NULL);
	if (ubus_ctx == NULL)
		return -1;

	memset(&b, 0, sizeof(struct blob_buf));
	blob_buf_init(&b, 0);
	for (i = 0; i < u_args_size; i++) {
		if (u_args[i].type == UBUS_String)
			blobmsg_add_string(&b, u_args[i].key, u_args[i].val.str_val);
		else if (u_args[i].type == UBUS_Integer) {
			blobmsg_add_u32(&b, u_args[i].key, u_args[i].val.int_val);
		} else if (u_args[i].type == UBUS_Array_Obj || u_args[i].type == UBUS_Array_Str) {
			void *a;
			int j;
			a = blobmsg_open_array(&b, u_args[i].key);
			if (u_args[i].type == UBUS_Array_Obj) {
				void *t;
				t = blobmsg_open_table(&b, "");
				for (j = 0; j < ARRAY_MAX; j++) {
					if (u_args[i].val.array_value[j].param_value.key == NULL || strlen(u_args[i].val.array_value[j].param_value.key) == 0)
						break;
					blobmsg_add_string(&b, u_args[i].val.array_value[j].param_value.key, u_args[i].val.array_value[j].param_value.value);
				}
				blobmsg_close_table(&b, t);
			}
			if (u_args[i].type == UBUS_Array_Str) {
				for (j = 0; j < ARRAY_MAX; j++) {
					if (u_args[i].val.array_value[j].str_value == NULL || strlen(u_args[i].val.array_value[j].str_value) == 0)
						break;
					blobmsg_add_string(&b, NULL, u_args[i].val.array_value[j].str_value);
				}
			}
			blobmsg_close_array(&b, a);
		} else if (u_args[i].type == UBUS_List_Param_Set) {
			struct cwmp_dm_parameter *param_value  = NULL;
			void *a;
			a = blobmsg_open_array(&b, u_args[i].key);
			list_for_each_entry (param_value, u_args[i].val.param_value_list, list) {
				void *t;
				if (!param_value->name)
					break;
				t = blobmsg_open_table(&b, "");
				blobmsg_add_string(&b, "path", param_value->name);
				blobmsg_add_string(&b, "value", param_value->value);
				blobmsg_close_table(&b, t);
			}
			blobmsg_close_array(&b, a);
		} else if (u_args[i].type == UBUS_List_Param_Get) {
			struct cwmp_dm_parameter *param_value = NULL;
			void *a;
			a = blobmsg_open_array(&b, u_args[i].key);
			list_for_each_entry (param_value, u_args[i].val.param_value_list, list) {
				if (!param_value->name)
					break;
				blobmsg_add_string(&b, NULL, param_value->name);
			}
			blobmsg_close_array(&b, a);
		} else if (u_args[i].type == UBUS_Obj_Obj) {
			struct cwmp_dm_parameter *param_value  = NULL;
			json_object *input_json_obj = json_object_new_object();
			list_for_each_entry (param_value, u_args[i].val.param_value_list, list) {
				if (!param_value->name)
					break;
				json_object_object_add(input_json_obj, param_value->name, json_object_new_string(param_value->value));
			}
			blobmsg_add_json_element(&b, u_args[i].key, input_json_obj);
		} else if (u_args[i].type == UBUS_Bool)
			blobmsg_add_u8(&b, u_args[i].key, u_args[i].val.bool_val);
	}
	if (!ubus_lookup_id(ubus_ctx, obj, &id))
		rc = ubus_invoke(ubus_ctx, id, method, b.head, ubus_callback, callback_arg, 20000);
	else
		rc = -1;
	blob_buf_free(&b);
	if (ubus_ctx) {
		ubus_free(ubus_ctx);
		ubus_ctx = NULL;
	}
	return rc;
}
