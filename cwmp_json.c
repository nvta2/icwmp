/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	Copyright (C) 2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#include <unistd.h>
#include <libubus.h>

#include <json-c/json.h>

#include "external.h"
#include "cwmp_json.h"
#include "log.h"

static json_object *jshn_obj = NULL;

static int cwmp_json_message_parse(char **policy, int size, char **tb, char *msg)
{
	int i;
	json_object *obj;

	jshn_obj = json_tokener_parse(msg);
	if (jshn_obj == NULL || json_object_get_type(jshn_obj) != json_type_object) {
		jshn_obj = NULL;
		return -1;
	}
	for (i = 0; i < size; i++) {
		json_object_object_get_ex(jshn_obj, policy[i], &obj);
		if (obj == NULL || json_object_get_type(obj) != json_type_string)
			continue;
		tb[i] = (char *)json_object_get_string(obj);
	}
	return 0;
}

static inline void cwmp_json_message_delete()
{
	if (jshn_obj != NULL)
		json_object_put(jshn_obj);
}
enum download_fault
{
	DOWNLOAD_FAULT,
	__DOWNLOAD_MAX
};

char *download_fault_policy[] = {[DOWNLOAD_FAULT] = "fault_code" };

int cwmp_handle_download_fault(char *msg)
{
	char *tb[__DOWNLOAD_MAX] = { 0 };

	cwmp_json_message_parse(download_fault_policy, ARRAYSIZEOF(download_fault_policy), tb, msg);

	if (!tb[DOWNLOAD_FAULT])
		goto error;

	DD(INFO, "triggered handle download fault %s", tb[DOWNLOAD_FAULT]);

	external_download_fault_resp(tb[DOWNLOAD_FAULT]);

	cwmp_json_message_delete();
	return 0;

error:
	cwmp_json_message_delete();
	return -1;
}

enum upload_fault
{
	UPLOAD_FAULT,
	__UPLOAD_MAX
};

char *upload_fault_policy[] = {[UPLOAD_FAULT] = "fault_code" };

int cwmp_handle_upload_fault(char *msg)
{
	char *tb[__UPLOAD_MAX] = { 0 };

	cwmp_json_message_parse(upload_fault_policy, ARRAYSIZEOF(upload_fault_policy), tb, msg);

	if (!tb[UPLOAD_FAULT])
		goto error;

	DD(INFO, "triggered handle upload fault %s", tb[UPLOAD_FAULT]);

	external_upload_fault_resp(tb[UPLOAD_FAULT]);

	cwmp_json_message_delete();
	return 0;

error:
	cwmp_json_message_delete();
	return -1;
}

enum dustatechange_fault
{
	DUState_Change_FAULT,
	DUState_Change_VERSION,
	DUState_Change_NAME,
	DUState_Change_UUID,
	DUState_Change_ENV,
	__DUSTATE_MAX
};

char *dustatechange_fault_policy[] = {[DUState_Change_FAULT] = "fault_code", [DUState_Change_VERSION] = "package_version", [DUState_Change_NAME] = "package_name", [DUState_Change_UUID] = "package_uuid", [DUState_Change_ENV] = "package_env" };

int cwmp_handle_dustate_change_fault(char *msg)
{
	char *tb[__DUSTATE_MAX] = { 0 };

	cwmp_json_message_parse(dustatechange_fault_policy, ARRAYSIZEOF(dustatechange_fault_policy), tb, msg);

	if (!tb[DUState_Change_FAULT])
		goto error;

	DD(INFO, "triggered handle dustate_change fault:%s version:%s name:%s", tb[DUState_Change_FAULT], tb[DUState_Change_VERSION], tb[DUState_Change_NAME], tb[DUState_Change_UUID], tb[DUState_Change_ENV]);

	external_du_change_state_fault_resp(tb[DUState_Change_FAULT], tb[DUState_Change_VERSION], tb[DUState_Change_NAME], tb[DUState_Change_UUID], tb[DUState_Change_ENV]);

	cwmp_json_message_delete();
	return 0;

error:
	cwmp_json_message_delete();
	return -1;
}

enum uninstall_fault
{
	UNINSTALL_FAULT,
	__UNINSTALL_MAX
};

char *uninstall_fault_policy[] = {[UNINSTALL_FAULT] = "fault_code" };

int cwmp_handle_uninstall_fault(char *msg)
{
	char *tb[__UNINSTALL_MAX] = { 0 };

	cwmp_json_message_parse(uninstall_fault_policy, ARRAYSIZEOF(uninstall_fault_policy), tb, msg);

	if (!tb[UNINSTALL_FAULT])
		goto error;

	DD(INFO, "triggered handle upload fault %s", tb[UNINSTALL_FAULT]);

	external_uninstall_fault_resp(tb[UNINSTALL_FAULT]);

	cwmp_json_message_delete();
	return 0;

error:
	cwmp_json_message_delete();
	return -1;
}

void cwmp_add_json_obj(json_object *json_obj_out, char *object, char *string)
{
	if (object != NULL && string != NULL) {
		json_object *json_obj_tmp = json_object_new_string(string);
		json_object_object_add(json_obj_out, object, json_obj_tmp);
	}
}

void cwmp_json_fprintf(FILE *fp, int argc, struct cwmp_json_arg cwmp_arg[])
{
	int i;
	char *arg;
	json_object *json_obj_out = json_object_new_object();
	if (json_obj_out == NULL)
		return;

	if (argc) {
		for (i = 0; i < argc; i++) {
			cwmp_add_json_obj(json_obj_out, cwmp_arg[i].key, cwmp_arg[i].val);
		}
		arg = (char *)json_object_to_json_string(json_obj_out);
		fprintf(fp, "%s\n", arg);
	}

	json_object_put(json_obj_out);
}
