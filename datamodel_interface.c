/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include "datamodel_interface.h"
#include "log.h"

bool transaction_started = false;
int transaction_id = 0;

json_object *old_global_json_obj = NULL;
json_object *actual_global_json_obj = NULL;
json_object *old_list_notify = NULL;
json_object *actual_list_notify = NULL;

/*
 * Transaction Functions
 */
int cwmp_transaction_start(char *app)
{
	CWMP_LOG (INFO,"Starting transaction ...");
	json_object *transaction_ret = NULL, *status_obj = NULL, *transaction_id_obj = NULL;
	int e = cwmp_ubus_call("usp.raw", "transaction_start", CWMP_UBUS_ARGS{{"app", {.str_val=app}, UBUS_String}}, 1, &transaction_ret);
	if (e!=0) {
		CWMP_LOG (INFO,"Transaction start failed: Ubus err code: %d", e);
		return 0;
	}
	json_object_object_get_ex(transaction_ret, "status", &status_obj);
	if (strcmp((char*)json_object_get_string(status_obj), "true") != 0) {
		json_object *error = NULL;
		json_object_object_get_ex(transaction_ret, "error", &error);
		FREE_JSON(status_obj)
		FREE_JSON(error)
		FREE_JSON(transaction_ret)
		CWMP_LOG (INFO,"Transaction start failed: %s\n",(char*)json_object_get_string(error));
		return 0;
	}
	FREE_JSON(status_obj)
	json_object_object_get_ex(transaction_ret, "transaction_id", &transaction_id_obj);
	if (transaction_id_obj == NULL) {
		FREE_JSON(transaction_ret)
		return 0;
	}
	transaction_id = atoi((char*)json_object_get_string(transaction_id_obj));
	FREE_JSON(transaction_id_obj)
	FREE_JSON(transaction_ret)
	return 1;
}

int cwmp_transaction_commit()
{
	CWMP_LOG (INFO,"Transaction Commit ...");
	json_object *transaction_ret = NULL, *status_obj = NULL;
	int e = cwmp_ubus_call("usp.raw", "transaction_commit", CWMP_UBUS_ARGS{{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 1, &transaction_ret);
	if (e!=0) {
		CWMP_LOG (INFO,"Transaction commit failed: Ubus err code: %d", e)
		return 0;
	}
	json_object_object_get_ex(transaction_ret, "status", &status_obj);
	if (strcmp((char*)json_object_get_string(status_obj), "true") != 0) {
		json_object *error = NULL;
		json_object_object_get_ex(transaction_ret, "error", &error);
		CWMP_LOG (INFO,"Transaction commit failed: %s\n",(char*)json_object_get_string(error));
		transaction_id = 0;
		FREE_JSON(status_obj)
		FREE_JSON(error)
		FREE_JSON(transaction_ret)
		return 0;
	}
	transaction_id = 0;
	FREE_JSON(status_obj)
	FREE_JSON(transaction_ret)
	return 1;
}

int cwmp_transaction_abort()
{
	CWMP_LOG (INFO,"Transaction Abort ...");
	json_object *transaction_ret = NULL, *status_obj = NULL;
	int e = cwmp_ubus_call("usp.raw", "transaction_abort", CWMP_UBUS_ARGS{{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 1, &transaction_ret);
	if (e!=0) {
		CWMP_LOG (INFO,"Transaction abort failed: Ubus err code: %d", e);
		return 0;
	}
	json_object_object_get_ex(transaction_ret, "status", &status_obj);
	if (strcmp((char*)json_object_get_string(status_obj), "true") != 0) {
		json_object *error = NULL;
		json_object_object_get_ex(transaction_ret, "error", &error);
		CWMP_LOG (INFO,"Transaction abort failed: %s\n",(char*)json_object_get_string(error));
		FREE_JSON(status_obj)
		FREE_JSON(error)
		FREE_JSON(transaction_ret)
		return 0;
	}
	FREE_JSON(status_obj)
	FREE_JSON(transaction_ret)
	return 1;
}

int cwmp_transaction_status()
{
	json_object *status_obj = NULL, *transaction_ret = NULL;
	int e = cwmp_ubus_call("usp.raw", "transaction_status", CWMP_UBUS_ARGS{{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 1, &transaction_ret);
	if (e!=0) {
		CWMP_LOG (INFO,"Transaction status failed: Ubus err code: %d", e);
		return 0;
	}
	json_object_object_get_ex(transaction_ret, "status", &status_obj);
	if (!status_obj || strcmp((char*)json_object_get_string(status_obj), "on-going") != 0) {
		CWMP_LOG (INFO,"Transaction with id: %d is not available anymore\n",transaction_id);
		FREE_JSON(status_obj)
		FREE_JSON(transaction_ret)
		return 0;
	}
	FREE_JSON(status_obj)
	FREE_JSON(transaction_ret)
	return 1;
}
/*
 * RPC Methods Functions
 */
char* cwmp_get_parameter_values(char *parameter_name, json_object **parameters)
{
	json_object *params_obj = NULL;
	char *fault = NULL;
	int e = cwmp_ubus_call("usp.raw", "get", CWMP_UBUS_ARGS{{"path", {.str_val=!parameter_name||parameter_name[0]=='\0'?DM_ROOT_OBJ:parameter_name}, UBUS_String}}, 1, &params_obj);
	if (e < 0 || params_obj == NULL) {
		*parameters = NULL;
		return "9002";
	}
	json_object *fault_code = NULL;
	json_object_object_get_ex(params_obj, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		fault = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(params_obj)
		return fault;
	}
	json_object_object_get_ex(params_obj, "parameters", parameters);
	return NULL;
}

char* cwmp_set_parameter_value(char* parameter_name, char* value, char* parameter_key, int* flag)
{
	json_object *set_res;
	char *fault = NULL;
	int e = cwmp_ubus_call("usp.raw", "set", CWMP_UBUS_ARGS{{"path", {.str_val=parameter_name}, UBUS_String},{"value", {.str_val=value}, UBUS_String}, {"key", {.str_val=parameter_key}, UBUS_String},{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 3, &set_res);

	if (e < 0 || set_res == NULL)
		return "9002";

	json_object *fault_code = NULL;
	json_object_object_get_ex(set_res, "fault", &fault_code);
	if (fault_code != NULL) {
		fault = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(set_res)
		return fault;
	}
	json_object *status = NULL;
	json_object_object_get_ex(set_res, "status", &status);
	char *status_str = NULL;
	if (status) {
		status_str = (char*)json_object_get_string(status);
		if (status_str && strcmp(status_str,"true") == 0) {
			json_object *flag_obj = NULL;
			json_object_object_get_ex(set_res, "flag", &flag_obj);
			*flag = flag_obj?atoi((char*)json_object_get_string(flag_obj)):0;
			FREE_JSON(flag_obj)
			FREE_JSON(status)
			FREE_JSON(set_res)
			return NULL;
		}
	}
	json_object *parameters = NULL;
	json_object_object_get_ex(set_res, "parameters", &parameters);

	if (!parameters)
		return "9002";
	json_object *param_obj = json_object_array_get_idx(parameters, 0);

	json_object_object_get_ex(param_obj, "status", &status);
	status_str = (char*)json_object_get_string(status);
	if (status_str && strcmp(status_str,"false") == 0) {
		json_object *fault = NULL;
		json_object_object_get_ex(param_obj, "fault", &fault);
		return (char*)json_object_get_string(fault);
	}
	return NULL;
}

char* cwmp_set_multiple_parameters_values(struct list_head parameters_values_list, char* parameter_key, int* flag, json_object **faults_array)
{
	json_object *set_res;
	int e = cwmp_ubus_call("usp.raw", "setm_values", CWMP_UBUS_ARGS{{"pv_tuple", {.param_value_list=&parameters_values_list}, UBUS_List_Param}, {"key", {.str_val=parameter_key}, UBUS_String},{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 3, &set_res);
	if (e < 0 || set_res == NULL)
		return "9002";

	json_object *status = NULL;
	json_object_object_get_ex(set_res, "status", &status);
	char *status_str = NULL;
	if (status) {
		status_str = (char*)json_object_get_string(status);
		if (status_str && strcmp(status_str,"true") == 0) {
			json_object *flag_obj = NULL;
			json_object_object_get_ex(set_res, "flag", &flag_obj);
			*flag = flag_obj?atoi((char*)json_object_get_string(flag_obj)):0;
			free(status_str);
			status_str = NULL;
			return NULL;
		}
		if(status_str) {
			free(status_str);
			status_str = NULL;
		}
	}
	json_object_object_get_ex(set_res, "parameters", faults_array);
	return "Fault";
}

char* cwmp_add_object(char* object_name, char* key, char **instance)
{
	json_object *add_res;
	char *err = NULL;

	int e = cwmp_ubus_call("usp.raw", "add_object", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"key", {.str_val=key}, UBUS_String},{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 3, &add_res);
	if (e < 0 || add_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(add_res, "fault", &fault_code);
	if (fault_code != NULL) {
		err = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(add_res)
		return err;
	}

	json_object *parameters = NULL;
	json_object_object_get_ex(add_res, "parameters", &parameters);
	if (parameters == NULL) {
		FREE_JSON(add_res)
		return "9002";
	}
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *fault = NULL;
	json_object_object_get_ex(param_obj, "fault", &fault);
	if (fault) {
		err = strdup((char*)json_object_get_string(fault));
		FREE_JSON(parameters)
		FREE_JSON(fault)
		FREE_JSON(add_res)
		return err;
	}
	json_object *instance_obj = NULL;
	json_object_object_get_ex(param_obj, "instance", &instance_obj);
	*instance = strdup((char *)json_object_get_string(instance_obj));
	FREE_JSON(instance_obj)
	FREE_JSON(add_res)
	return NULL;
}

char* cwmp_delete_object(char* object_name, char* key)
{
	json_object *del_res;
	char *err = NULL;

	int e = cwmp_ubus_call("usp.raw", "del_object", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"key", {.str_val=key}, UBUS_String},{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 3, &del_res);
	if (e < 0 || del_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(del_res, "fault", &fault_code);
	if (fault_code != NULL) {
		err = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(del_res)
		return err;
	}
	json_object *parameters = NULL;
	json_object_object_get_ex(del_res, "parameters", &parameters);
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *status = NULL;
	json_object_object_get_ex(param_obj, "status", &status);
	char *status_str = (char*)json_object_get_string(status);
	if (status_str && strcmp(status_str,"false") == 0) {
		json_object *fault = NULL;
		json_object_object_get_ex(param_obj, "fault", &fault);
		err = strdup((char*)json_object_get_string(fault));
		FREE_JSON(fault)
		FREE_JSON(status)
		FREE_JSON(del_res)
		return err;
	}
	FREE_JSON(status)
	FREE_JSON(del_res)
	return NULL;
}

char* cwmp_get_parameter_names(char* object_name, bool next_level, json_object **parameters)
{
	json_object *get_name_res;
	char *err = NULL;

	int e = cwmp_ubus_call("usp.raw", "object_names", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"next-level", {.bool_val=next_level}, UBUS_Bool}}, 2, &get_name_res);
	if (e < 0 || get_name_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(get_name_res, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		err = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(get_name_res)
		return err;
	}
	json_object_object_get_ex(get_name_res, "parameters", parameters);
	return NULL;
}

char* cwmp_get_parameter_attributes(char* parameter_name, json_object **parameters)
{
	json_object *get_attributes_res = NULL;
	char *err = NULL;

	int e = cwmp_ubus_call("usp.raw", "getm_attributes", CWMP_UBUS_ARGS{{"paths", {.array_value={{.str_value=!parameter_name||parameter_name[0]=='\0'?DM_ROOT_OBJ:parameter_name}}}, UBUS_Array_Str}}, 1, &get_attributes_res);
	if ( e < 0 || get_attributes_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(get_attributes_res, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		err = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
		FREE_JSON(get_attributes_res)
		return err;
	}
	json_object_object_get_ex(get_attributes_res, "parameters", parameters);
	json_object *fault = NULL, *param_obj = NULL;
	foreach_jsonobj_in_array(param_obj, *parameters) {
		json_object_object_get_ex(param_obj, "fault", &fault);
		if (fault) {
			err = strdup((char*)json_object_get_string(fault));
			FREE_JSON(fault)
			FREE_JSON(parameters)
			break;
		}
	}
	FREE_JSON(param_obj)
	return err;
}

char* cwmp_set_parameter_attributes(char* parameter_name, char* notification)
{
	json_object *set_attribute_res;
	char *err = NULL;
	int e = cwmp_ubus_call("usp.raw", "setm_attributes", CWMP_UBUS_ARGS{{"paths", {.array_value={{.param_value={"path",parameter_name}},{.param_value={"notify-type",notification}},{.param_value={"notify","1"}}}}, UBUS_Array_Obj},{"transaction_id", {.int_val=transaction_id}, UBUS_Integer}}, 2, &set_attribute_res);
	if (e < 0 || set_attribute_res == NULL)
		return "9002";
	json_object *parameters = NULL;
	json_object_object_get_ex(set_attribute_res, "parameters", &parameters);
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *fault_code = NULL;
	json_object_object_get_ex(param_obj, "fault", &fault_code);
	if (fault_code != NULL) {
		err = strdup((char*)json_object_get_string(fault_code));
		FREE_JSON(fault_code)
	}
	FREE_JSON(param_obj)
	FREE_JSON(parameters)
	FREE_JSON(set_attribute_res)
	return err;
}

/*
 * Init Notify Function
 */
int cwmp_update_enabled_list_notify(int instance_mode, int notify_type)
{
	int e;
	CWMP_LOG (INFO,"Get List Notify for %s paramters values", notify_type == OLD_LIST_NOTIFY ? "old": "actual");
	if (notify_type == OLD_LIST_NOTIFY) {
		CWMP_LOG (INFO,"%s line %d", __FILE__, __LINE__);
		FREE_JSON(old_list_notify)
		FREE_JSON(old_global_json_obj)
		CWMP_LOG (INFO,"%s:%s line %d", __FILE__, __FUNCTION__, __LINE__);
		e = cwmp_ubus_call("usp.raw", "list_notify", CWMP_UBUS_ARGS{{"instance_mode", {.int_val=instance_mode}, UBUS_Integer}}, 1, &old_global_json_obj);
		if (e)
			return e;
		json_object_object_get_ex(old_global_json_obj, "parameters", &old_list_notify);
		CWMP_LOG (INFO,"%s line %d", __FILE__, __LINE__);
	}
	else {
		CWMP_LOG (INFO,"%s line %d", __FILE__, __LINE__);
		FREE_JSON(actual_list_notify)
		FREE_JSON(actual_global_json_obj)
		CWMP_LOG (INFO,"%s line %d", __FILE__, __FUNCTION__, __LINE__);
		e = cwmp_ubus_call("usp.raw", "list_notify", CWMP_UBUS_ARGS{{"instance_mode", {.int_val=instance_mode}, UBUS_Integer}}, 1, &actual_global_json_obj);
		if (e)
			return e;
		json_object_object_get_ex(actual_global_json_obj, "parameters", &actual_list_notify);
		CWMP_LOG (INFO,"%s line %d", __FILE__, __FUNCTION__, __LINE__);
	}
	return 0;
}
