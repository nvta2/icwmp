/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include "datamodel_interface.h"
#include "log.h"
#include "ubus.h"
#include "notifications.h"

int transaction_id = 0;

enum get_results_types
{
	LIST,
	FAULT
};

struct object_result {
	char **instance;
	char fault[5];
	bool status;
};

struct list_params_result {
	struct list_head *parameters_list;
	char fault[5];
	enum get_results_types type;
};

struct setm_values_res {
	bool status;
	int *flag;
	struct list_head *faults_list;
};
/*
 * Common functions
 */
struct blob_attr *get_parameters_array(struct blob_attr *msg)
{
	struct blob_attr *parameters = NULL;
	struct blob_attr *cur;
	int rem;

	blobmsg_for_each_attr(cur, msg, rem)
	{
		if (blobmsg_type(cur) == BLOBMSG_TYPE_ARRAY) {
			parameters = cur;
			break;
		}
	}
	return parameters;
}

char *get_status(struct blob_attr *msg)
{
	char *status = NULL;
	const struct blobmsg_policy p[1] = { { "status", BLOBMSG_TYPE_STRING } };
	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (tb[0])
		status = blobmsg_get_string(tb[0]);
	return status;
}

int get_fault(struct blob_attr *msg)
{
	int fault = FAULT_CPE_NO_FAULT;
	const struct blobmsg_policy p[1] = { { "fault", BLOBMSG_TYPE_INT32 } };
	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (tb[0])
		fault = blobmsg_get_u32(tb[0]);
	return fault;
}

int get_parameters_list_from_parameters_blob_array(struct blob_attr *parameters, struct list_head *parameters_list)
{
	const struct blobmsg_policy p[5] = { { "parameter", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING }, { "type", BLOBMSG_TYPE_STRING }, { "notification", BLOBMSG_TYPE_STRING }, { "writable", BLOBMSG_TYPE_STRING } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, parameters, rem)
	{
		int fault_code = get_fault(cur);
		if (fault_code != FAULT_CPE_NO_FAULT) {
			cwmp_free_all_dm_parameter_list(parameters_list);
			return fault_code;
		}
		struct blob_attr *tb[5] = { NULL, NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 5, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;
		int notification = 0;
		bool writable = 0;
		if (tb[1] && strncmp(blobmsg_get_string(tb[1]), "1", 1) == 0)
			notification = 1;
		else if (tb[1] && strncmp(blobmsg_get_string(tb[1]), "2", 1) == 0)
			notification = 2;
		if (tb[4] && (strncmp(blobmsg_get_string(tb[4]), "1", 1) == 0 || strcasecmp(blobmsg_get_string(tb[4]), "true") == 0))
			writable = true;
		add_dm_parameter_to_list(parameters_list, blobmsg_get_string(tb[0]), tb[1] ? blobmsg_get_string(tb[1]) : "", tb[2] ? blobmsg_get_string(tb[2]) : "", notification, writable);
	}
	return FAULT_CPE_NO_FAULT;
}

int get_single_fault_from_blob_attr(struct blob_attr *msg)
{
	int fault_code = FAULT_CPE_NO_FAULT;
	fault_code = get_fault(msg);
	if (fault_code != FAULT_CPE_NO_FAULT)
		return fault_code;
	struct blob_attr *faults_array = get_parameters_array(msg);
	if (faults_array == NULL)
		return FAULT_CPE_NO_FAULT;
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, faults_array, rem)
	{
		fault_code = get_fault(cur);
		if (fault_code != FAULT_CPE_NO_FAULT)
			break;
	}
	return fault_code;
}

/*
 * Transaction Functions
 */
void ubus_transaction_commit_callback(struct ubus_request *req __attribute__((unused)), int type __attribute__((unused)), struct blob_attr *msg)
{
	bool *status = (bool *)req->priv;
	const struct blobmsg_policy p[1] = { { "status", BLOBMSG_TYPE_BOOL } };
	struct blob_attr *updated_services = NULL;
	struct blob_attr *cur;
	int rem;

	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (!tb[0]) {
		*status = false;
		return;
	}
	*status = blobmsg_get_u8(tb[0]);
	if (*status == false)
		return;

	blobmsg_for_each_attr(cur, msg, rem)
	{
		if (blobmsg_type(cur) == BLOBMSG_TYPE_ARRAY) {
			updated_services = cur;
			break;
		}
	}
	if (updated_services) {
		blobmsg_for_each_attr(cur, updated_services, rem)
		{
			char *service_name = blobmsg_get_string(cur);
			if (!service_name || strlen(service_name) == 0 || strcmp(service_name, "cwmp") == 0)
				continue;
			CWMP_LOG(INFO, "Detected service: %s will be restarted in the end session", service_name);
			/*Add the service to the list*/
			if (strcmp(service_name, "cwmp") != 0)
				icwmp_add_service(service_name);
		}
	}
}

void ubus_transaction_callback(struct ubus_request *req __attribute__((unused)), int type __attribute__((unused)), struct blob_attr *msg)
{
	bool *status = (bool *)req->priv;
	const struct blobmsg_policy p[2] = { { "status", BLOBMSG_TYPE_BOOL }, { "transaction_id", BLOBMSG_TYPE_INT32 } };
	struct blob_attr *tb[2] = { NULL, NULL };
	blobmsg_parse(p, 2, tb, blobmsg_data(msg), blobmsg_len(msg));
	*status = blobmsg_get_u8(tb[0]);
	if (*status == true && tb[1])
		transaction_id = blobmsg_get_u32(tb[1]);
}

void ubus_transaction_status_callback(struct ubus_request *req __attribute__((unused)), int type __attribute__((unused)), struct blob_attr *msg)
{
	bool *status = (bool *)req->priv;
	char *status_str = NULL;
	const struct blobmsg_policy p[2] = { { "status", BLOBMSG_TYPE_STRING } };
	struct blob_attr *tb[2] = { NULL, NULL };
	blobmsg_parse(p, 2, tb, blobmsg_data(msg), blobmsg_len(msg));
	status_str = blobmsg_get_string(tb[0]);
	if (strcmp(status_str, "on-going") == 0)
		*status = true;
	else
		*status = false;
}

bool cwmp_transaction_start(char *app)
{
	CWMP_LOG(INFO, "Starting transaction ...");
	bool status = false;
	int e = cwmp_ubus_call(USP_OBJECT_NAME, "transaction_start", CWMP_UBUS_ARGS{ { "app", { .str_val = app }, UBUS_String } }, 1, ubus_transaction_callback, &status);
	if (e != 0) {
		CWMP_LOG(INFO, "Transaction start failed: Ubus err code: %d", e);
		status = false;
	}
	if (!status) {
		CWMP_LOG(INFO, "Transaction Start doesn't success\n");
	}
	return status;
}

bool cwmp_transaction_commit()
{
	CWMP_LOG(INFO, "Transaction Commit ...");
	bool status = false;
	int e = cwmp_ubus_call(USP_OBJECT_NAME, "transaction_commit", CWMP_UBUS_ARGS{ { "transaction_id", { .int_val = transaction_id }, UBUS_Integer }, { "restart_services", { .bool_val = false }, UBUS_Bool } }, 2, ubus_transaction_commit_callback, &status);
	if (e != 0) {
		CWMP_LOG(INFO, "Transaction commit failed: Ubus err code: %d", e);
		status = false;
	}
	if (!status) {
		CWMP_LOG(INFO, "Transaction Commit with id: %d doesn't success\n", transaction_id);
	}

	transaction_id = 0;
	return status;
}

bool cwmp_transaction_abort()
{
	CWMP_LOG(INFO, "Transaction Abort ...");
	bool status = false;
	int e = cwmp_ubus_call(USP_OBJECT_NAME, "transaction_abort", CWMP_UBUS_ARGS{ { "transaction_id", { .int_val = transaction_id }, UBUS_Integer } }, 1, ubus_transaction_callback, &status);
	if (e != 0) {
		CWMP_LOG(INFO, "Transaction abort failed: Ubus err code: %d", e);
		status = false;
	}
	if (!status) {
		CWMP_LOG(INFO, "Transaction Abort of transaction with id: %d failed\n", transaction_id);
	}
	transaction_id = 0;
	return status;
}

bool cwmp_transaction_status()
{
	CWMP_LOG(INFO, "Transaction Status");
	bool status = false;
	int e = cwmp_ubus_call(USP_OBJECT_NAME, "transaction_status", CWMP_UBUS_ARGS{ { "transaction_id", { .int_val = transaction_id }, UBUS_Integer } }, 1, ubus_transaction_status_callback, &status);
	if (e != 0) {
		CWMP_LOG(INFO, "Transaction status failed: Ubus err code: %d", e);
		return false;
	}
	if (!status) {
		CWMP_LOG(INFO, "Transaction with id: %d is not available anymore\n", transaction_id);
	}
	return status;
}
/*
 * Get single _parameter value
 */

void ubus_get_single_parameter_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	struct blob_attr *parameters = get_parameters_array(msg);
	struct cwmp_dm_parameter *result = (struct cwmp_dm_parameter *)req->priv;
	if (parameters == NULL) {
		int fault_code = get_fault(msg);
		result->name = NULL;
		result->type = NULL;
		icwmp_asprintf(&result->value, "%d", fault_code);
		return;
	}
	const struct blobmsg_policy p[3] = { { "parameter", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING }, { "type", BLOBMSG_TYPE_STRING } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, parameters, rem)
	{
		struct blob_attr *tb[3] = { NULL, NULL, NULL };
		blobmsg_parse(p, 3, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;
		result->name = icwmp_strdup(blobmsg_get_string(tb[0]));
		result->value = icwmp_strdup(tb[1] ? blobmsg_get_string(tb[1]) : "");
		result->type = icwmp_strdup(tb[2] ? blobmsg_get_string(tb[2]) : "");
		break;
	}
}

char *cwmp_get_single_parameter_value(char *parameter_name, struct cwmp_dm_parameter *dm_parameter)
{
	int e;
	struct cwmp *cwmp = &cwmp_main;
	e = cwmp_ubus_call(USP_OBJECT_NAME, "get", CWMP_UBUS_ARGS{ { "path", { .str_val = !parameter_name || parameter_name[0] == '\0' ? DM_ROOT_OBJ : parameter_name }, UBUS_String }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 3, ubus_get_single_parameter_callback, dm_parameter);
	if (e < 0) {
		CWMP_LOG(INFO, "get ubus method failed: Ubus err code: %d", e);
		return "9002";
	}

	if (dm_parameter->name == NULL) {
		CWMP_LOG(INFO, "Get parameter_values failed: fault_code: %s", dm_parameter->value); // the value is the fault
		return dm_parameter->value;
	}
	return NULL;
}

/*
 * Get parameter Values/Names/Notify
 */
void ubus_get_parameter_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	struct blob_attr *parameters = get_parameters_array(msg);
	struct list_params_result *result = (struct list_params_result *)req->priv;
	int fault_code = FAULT_CPE_NO_FAULT;
	if (parameters == NULL) {
		fault_code = get_fault(msg);
		snprintf(result->fault, 5, "%d", fault_code);
		result->type = FAULT;
		return;
	}
	result->type = LIST;
	fault_code = get_parameters_list_from_parameters_blob_array(parameters, result->parameters_list);
	if (fault_code != FAULT_CPE_NO_FAULT) {
		snprintf(result->fault, 5, "%d", fault_code);
		result->type = FAULT;
	}
}

char *cwmp_get_parameter_values(char *parameter_name, struct list_head *parameters_list)
{
	int e;
	struct cwmp *cwmp = &cwmp_main;
	struct list_params_result get_result = { .parameters_list = parameters_list };
	e = cwmp_ubus_call(USP_OBJECT_NAME, "get", CWMP_UBUS_ARGS{ { "path", { .str_val = !parameter_name || parameter_name[0] == '\0' ? DM_ROOT_OBJ : parameter_name }, UBUS_String }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 3, ubus_get_parameter_callback, &get_result);
	if (e < 0) {
		CWMP_LOG(INFO, "get ubus method failed: Ubus err code: %d", e);
		return "9002";
	}

	if (get_result.type == FAULT) {
		CWMP_LOG(INFO, "Get parameter values failed: fault_code: %s", get_result.fault);
		return icwmp_strdup(get_result.fault);
	}
	return NULL;
}

char *cwmp_get_multiple_parameters_values(struct list_head *arg_params_list, struct list_head *parameters_list)
{
	int e;
	struct cwmp *cwmp = &cwmp_main;
	struct list_params_result get_result = { .parameters_list = parameters_list };
	e = cwmp_ubus_call(USP_OBJECT_NAME, "getm_values", CWMP_UBUS_ARGS{ { "paths", { .param_value_list = arg_params_list }, UBUS_List_Param_Get }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 2, ubus_get_parameter_callback, &get_result );
	if (e < 0) {
		CWMP_LOG(INFO, "getm_values ubus method failed: Ubus err code: %d", e);
		return "9002";
	}

	if (get_result.type == FAULT) {
		CWMP_LOG(INFO, "Get multiple parameters values failed: fault_code: %s", get_result.fault);
		return strdup(get_result.fault);
	}
	return NULL;
}

char *cwmp_get_parameter_names(char *object_name, bool next_level, struct list_head *parameters_list)
{
	int e;
	struct list_params_result get_result = { .parameters_list = parameters_list };
	struct cwmp *cwmp = &cwmp_main;
	e = cwmp_ubus_call(USP_OBJECT_NAME, "object_names", CWMP_UBUS_ARGS{ { "path", { .str_val = object_name }, UBUS_String }, { "next-level", { .bool_val = next_level }, UBUS_Bool }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 4, ubus_get_parameter_callback, &get_result);
	if (e < 0) {
		CWMP_LOG(INFO, "object_names ubus method failed: Ubus err code: %d", e);
		return "9002";
	}

	if (get_result.type == FAULT) {
		CWMP_LOG(INFO, "GetParameterNames failed: fault_code: %s", get_result.fault);
		return icwmp_strdup(get_result.fault);
	}

	return NULL;
}

/*
 * Set multiple parameter values
 */

void ubus_setm_values_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	struct setm_values_res *set_result = (struct setm_values_res *)req->priv;
	const struct blobmsg_policy p[2] = { { "status", BLOBMSG_TYPE_BOOL }, { "flag", BLOBMSG_TYPE_INT64 } };
	struct blob_attr *tb[2] = { NULL, NULL };
	blobmsg_parse(p, 2, tb, blobmsg_data(msg), blobmsg_len(msg));
	if (tb[0]) {
		set_result->status = blobmsg_get_u8(tb[0]);
		if (set_result->status) {
			int *flag = set_result->flag;
			*flag = tb[1] ? blobmsg_get_u64(tb[1]) : 0;
			return;
		}
	}
	set_result->status = false;
	struct blob_attr *faults_params = get_parameters_array(msg);
	const struct blobmsg_policy pfault[3] = { { "path", BLOBMSG_TYPE_STRING }, { "fault", BLOBMSG_TYPE_INT32 }, { "status", BLOBMSG_TYPE_BOOL } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, faults_params, rem)
	{
		struct blob_attr *tbi[3] = { NULL, NULL, NULL };
		blobmsg_parse(pfault, 3, tbi, blobmsg_data(cur), blobmsg_len(cur));
		if (!tbi[0] || !tbi[1])
			continue;
		cwmp_add_list_fault_param(blobmsg_get_string(tbi[0]), blobmsg_get_u32(tbi[1]), set_result->faults_list);
	}
}

int cwmp_set_multiple_parameters_values(struct list_head *parameters_values_list, char *parameter_key, int *flag, struct list_head *faults_list)
{
	int e;
	struct setm_values_res set_result = { .flag = flag, .faults_list = faults_list };
	struct cwmp *cwmp = &cwmp_main;
	e = cwmp_ubus_call(USP_OBJECT_NAME, "setm_values",
			   CWMP_UBUS_ARGS{ { "pv_tuple", { .param_value_list = parameters_values_list }, UBUS_List_Param_Set }, { "key", { .str_val = parameter_key }, UBUS_String }, { "transaction_id", { .int_val = transaction_id }, UBUS_Integer }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 5,
			   ubus_setm_values_callback, &set_result);

	if (e < 0) {
		CWMP_LOG(INFO, "setm_values ubus method failed: Ubus err code: %d", e);
		return FAULT_CPE_INTERNAL_ERROR;
	}
	if (set_result.status == false) {
		CWMP_LOG(INFO, "Set parameter_values failed");
		return FAULT_CPE_INVALID_ARGUMENTS;
	}

	return FAULT_CPE_NO_FAULT;
}

/*
 * Add Delete object
 */

void ubus_objects_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	int fault_code = get_single_fault_from_blob_attr(msg);
	struct object_result *result = (struct object_result *)req->priv;
	if (fault_code != FAULT_CPE_NO_FAULT) {
		snprintf(result->fault, 5, "%d", fault_code);
		result->status = false;
		return;
	}
	struct blob_attr *parameters = get_parameters_array(msg);
	const struct blobmsg_policy p[2] = { { "status", BLOBMSG_TYPE_BOOL }, { "instance", BLOBMSG_TYPE_STRING } };
	struct blob_attr *cur;
	int rem;
	blobmsg_for_each_attr(cur, parameters, rem)
	{
		struct blob_attr *tb[2] = { NULL, NULL };
		blobmsg_parse(p, 2, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;
		result->status = blobmsg_get_u8(tb[0]);
		if (tb[1]) {
			char **instance = result->instance;
			*instance = strdup(blobmsg_get_string(tb[1]));
		}
		break;
	}
}

char *cwmp_add_object(char *object_name, char *key, char **instance)
{
	int e;
	struct cwmp *cwmp = &cwmp_main;
	struct object_result add_result = { .instance = instance };
	e = cwmp_ubus_call(USP_OBJECT_NAME, "add_object", CWMP_UBUS_ARGS{ { "path", { .str_val = object_name }, UBUS_String }, { "key", { .str_val = key }, UBUS_String }, { "transaction_id", { .int_val = transaction_id }, UBUS_Integer }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 5,
			   ubus_objects_callback, &add_result);

	if (e < 0) {
		CWMP_LOG(INFO, "add_object ubus method failed: Ubus err code: %d", e);
		return "9002";
	}
	if (add_result.status == false) {
		CWMP_LOG(INFO, "AddObject failed");
		return icwmp_strdup(add_result.fault);
	}
	return NULL;
}

char *cwmp_delete_object(char *object_name, char *key)
{
	int e;
	struct object_result add_result = { .instance = NULL };
	struct cwmp *cwmp = &cwmp_main;
	e = cwmp_ubus_call(USP_OBJECT_NAME, "del_object", CWMP_UBUS_ARGS{ { "path", { .str_val = object_name }, UBUS_String }, { "key", { .str_val = key }, UBUS_String }, { "transaction_id", { .int_val = transaction_id }, UBUS_Integer }, { "proto", { .str_val = "cwmp" }, UBUS_String }, { "instance_mode", { .int_val = cwmp->conf.instance_mode }, UBUS_Integer } }, 5,
			   ubus_objects_callback, &add_result);
	if (e < 0) {
		CWMP_LOG(INFO, "del_object ubus method failed: Ubus err code: %d", e);
		return "9002";
	}

	if (add_result.status == false) {
		CWMP_LOG(INFO, "DeleteObject failed");
		return icwmp_strdup(add_result.fault);
	}

	return NULL;
}
