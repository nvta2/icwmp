/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdio.h>
#include <libubox/blobmsg_json.h>

#include "common.h"
#include "ubus.h"
#include "cwmp_du_state.h"
#include "log.h"

struct change_du_state_res {
	char **pack_name;
	char **pack_version;
	char **pack_uuid;
	char **pack_env;
	char **fault;
	enum dustate_type type;
};

void ubus_du_install_update_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	struct change_du_state_res *result = (struct change_du_state_res *)req->priv;
	const struct blobmsg_policy p[6] = { { "status", BLOBMSG_TYPE_STRING }, { "error", BLOBMSG_TYPE_STRING }, { "name", BLOBMSG_TYPE_STRING }, { "uuid", BLOBMSG_TYPE_STRING }, { "version", BLOBMSG_TYPE_STRING }, { "environment", BLOBMSG_TYPE_STRING } };
	struct blob_attr *tb[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
	blobmsg_parse(p, 6, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (tb[0] && strcmp(blobmsg_get_string(tb[0]), "1") == 0) {
		*(result->fault) = NULL;
		*(result->pack_name) = strdup(tb[2] ? blobmsg_get_string(tb[2]) : "");
		*(result->pack_uuid) = strdup(tb[3] ? blobmsg_get_string(tb[3]) : "");
		*(result->pack_version) = strdup(tb[4] ? blobmsg_get_string(tb[4]) : "");
		*(result->pack_env) = strdup(tb[5] ? blobmsg_get_string(tb[5]) : "");
	} else {
		if (result->type == DU_INSTALL)
			*(result->fault) = strdup(tb[1] && strcmp(blobmsg_get_string(tb[1]), "Download") ? "9010" : "9018");
		else
			*(result->fault) = strdup("9010");
	}
}

int cwmp_du_install(char *url, char *uuid, char *user, char *pass, char *env, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code)
{
	int e;
	struct change_du_state_res cds_install = {.pack_name = package_name, .pack_version = package_version, .pack_uuid = package_uuid, .pack_env = package_env, .fault = fault_code, .type = DU_INSTALL };
	e = cwmp_ubus_call("swmodules", "du_install",
			   CWMP_UBUS_ARGS{ { "url", {.str_val = url }, UBUS_String }, { "uuid", {.str_val = uuid }, UBUS_String }, { "username", {.str_val = user }, UBUS_String }, { "password", {.str_val = pass }, UBUS_String }, { "environment", {.str_val = env }, UBUS_String } }, 5,
			   ubus_du_install_update_callback, &cds_install);
	if (e < 0) {
		CWMP_LOG(INFO, "Change du state install failed: Ubus err code: %d", e);
		return FAULT_CPE_INTERNAL_ERROR;
	}
	return FAULT_CPE_NO_FAULT;
}

int cwmp_du_update(char *url, char *uuid, char *user, char *pass, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code)
{
	int e;
	struct change_du_state_res cds_update = {.pack_name = package_name, .pack_version = package_version, .pack_uuid = package_uuid, .pack_env = package_env, .fault = fault_code, .type = DU_UPDATE };
	e = cwmp_ubus_call("swmodules", "du_install", CWMP_UBUS_ARGS{ { "url", {.str_val = url }, UBUS_String }, { "uuid", {.str_val = uuid }, UBUS_String }, { "username", {.str_val = user }, UBUS_String }, { "password", {.str_val = pass }, UBUS_String } }, 4, ubus_du_install_update_callback,
			   &cds_update);
	if (e < 0) {
		CWMP_LOG(INFO, "Change du state update failed: Ubus err code: %d", e);
		return FAULT_CPE_INTERNAL_ERROR;
	}
	return FAULT_CPE_NO_FAULT;
}

void ubus_du_uninstall_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	char **fault = (char **)req->priv;
	const struct blobmsg_policy p[2] = { { "status", BLOBMSG_TYPE_STRING } };
	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (tb[0] && strcmp(blobmsg_get_string(tb[0]), "1") == 0)
		*fault = NULL;
	else
		*fault = strdup("9010");
}

int cwmp_du_uninstall(char *package_name, char *package_env, char **fault_code)
{
	int e;
	e = cwmp_ubus_call("swmodules", "du_uninstall", CWMP_UBUS_ARGS{ { "name", {.str_val = package_name }, UBUS_String }, { "environment", {.str_val = package_env }, UBUS_String } }, 2, ubus_du_uninstall_callback, fault_code);
	if (e < 0) {
		CWMP_LOG(INFO, "Change du state uninstall failed: Ubus err code: %d", e);
		return FAULT_CPE_INTERNAL_ERROR;
	}
	return FAULT_CPE_NO_FAULT;
}
