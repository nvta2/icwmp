#include "datamodel_interface.h"

char* cwmp_get_parameter_values(char *parameter_name, json_object **parameters)
{
	json_object *params_obj = NULL;
	int e = cwmp_ubus_call("usp.raw", "get", CWMP_UBUS_ARGS{{"path", {.str_val=parameter_name}, UBUS_String}}, 1, &params_obj);
	if (e < 0 || params_obj == NULL) {
		*parameters = NULL;
		return "9002";
	}
	json_object *fault_code = NULL;
	json_object_object_get_ex(params_obj, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		return (char*)json_object_get_string(fault_code);
	}
	json_object_object_get_ex(params_obj, "parameters", parameters);
	return NULL;
}

char* cwmp_set_parameter_value(char* parameter_name, char* value, char* parameter_key)
{
	json_object *set_res;
	int e = cwmp_ubus_call("usp.raw", "set", CWMP_UBUS_ARGS{{"path", {.str_val=parameter_name}, UBUS_String},{"value", {.str_val=value}, UBUS_String}, {"key", {.str_val=parameter_key}, UBUS_String}}, 3, &set_res);
	if (e < 0 || set_res == NULL)
		return "9002";

	json_object *fault_code = NULL;
	json_object_object_get_ex(set_res, "fault", &fault_code);
	if (fault_code != NULL)
		return (char*)json_object_get_string(fault_code);

	json_object *parameters = NULL;
	json_object_object_get_ex(set_res, "parameters", &parameters);
	if (!parameters)
		return "9002";
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *status = NULL;
	json_object_object_get_ex(param_obj, "status", &status);
	char *status_str = (char*)json_object_get_string(status);
	if (status_str && strcmp(status_str,"false") == 0) {
		json_object *fault = NULL;
		json_object_object_get_ex(param_obj, "fault", &fault);
		return (char*)json_object_get_string(fault);
	}
	return NULL;
}

char* cwmp_add_object(char* object_name, char* key, char **instance)
{
	json_object *add_res;
	int e = cwmp_ubus_call("usp.raw", "add_object", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"key", {.str_val=key}, UBUS_String}}, 2, &add_res);
	if (e < 0 || add_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(add_res, "fault", &fault_code);
	if (fault_code != NULL)
		return (char*)json_object_get_string(fault_code);

	json_object *parameters = NULL;
	json_object_object_get_ex(add_res, "parameters", &parameters);
	if (parameters == NULL)
		return "9002";
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *fault = NULL;
	json_object_object_get_ex(param_obj, "fault", &fault);
	if (fault)
		return (char*)json_object_get_string(fault);
	json_object *instance_obj = NULL;
	json_object_object_get_ex(param_obj, "instance", &instance_obj);
	*instance = json_object_get_string(instance_obj);
	return NULL;
}

char* cwmp_delete_object(char* object_name, char* key)
{
	json_object *del_res;
	int e = cwmp_ubus_call("usp.raw", "del_object", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"key", {.str_val=key}, UBUS_String}}, 2, &del_res);
	if (e < 0 || del_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(del_res, "fault", &fault_code);
	if (fault_code != NULL)
		return (char*)json_object_get_string(fault_code);
	json_object *parameters = NULL;
	json_object_object_get_ex(del_res, "parameters", &parameters);
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *status = NULL;
	json_object_object_get_ex(param_obj, "status", &status);
	char *status_str = (char*)json_object_get_string(status);
	if (status_str && strcmp(status_str,"false") == 0) {
		json_object *fault = NULL;
		json_object_object_get_ex(param_obj, "fault", &fault);
		return (char*)json_object_get_string(fault);
	}
	return NULL;
}

char* cwmp_get_parameter_names(char* object_name, bool next_level, json_object **parameters)
{
	json_object *get_name_res;
	int e = cwmp_ubus_call("usp.raw", "object_names", CWMP_UBUS_ARGS{{"path", {.str_val=object_name}, UBUS_String},{"next-level", {.bool_val=next_level}, UBUS_Bool}}, 2, &get_name_res);
	if (e < 0 || get_name_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(get_name_res, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		return (char*)json_object_get_string(fault_code);
	}
	json_object_object_get_ex(get_name_res, "parameters", parameters);
	return NULL;
}

char* cwmp_get_parameter_attributes(char* parameter_name, json_object **parameters)
{
	json_object *get_attributes_res = NULL;
	int e = cwmp_ubus_call("usp.raw", "getm_attributes", CWMP_UBUS_ARGS{{"paths", {.array_value={{.str_value=parameter_name}}}, UBUS_Array_Str}}, 1, &get_attributes_res);
	if ( e < 0 || get_attributes_res == NULL)
		return "9002";
	json_object *fault_code = NULL;
	json_object_object_get_ex(get_attributes_res, "fault", &fault_code);
	if (fault_code != NULL) {
		*parameters = NULL;
		return (char*)json_object_get_string(fault_code);
	}
	json_object_object_get_ex(get_attributes_res, "parameters", parameters);
	json_object *fault = NULL, *param_obj = NULL;
	foreach_jsonobj_in_array(param_obj, *parameters) {
		json_object_object_get_ex(param_obj, "fault", &fault);
		if (fault) {
			return (char*)json_object_get_string(fault);
		}
	}
	return NULL;
}

char* cwmp_set_parameter_attributes(char* parameter_name, char* notification)
{
	json_object *set_attribute_res;
	int e = cwmp_ubus_call("usp.raw", "setm_attributes", CWMP_UBUS_ARGS{{"paths", {.array_value={{.param_value={"path",parameter_name}},{.param_value={"notify-type",notification}},{.param_value={"notify","1"}}}}, UBUS_Array_Obj}}, 1, &set_attribute_res);
	if (e < 0 || set_attribute_res == NULL)
		return "9002";
	json_object *parameters = NULL;
	json_object_object_get_ex(set_attribute_res, "parameters", &parameters);
	json_object *param_obj = json_object_array_get_idx(parameters, 0);
	json_object *fault_code = NULL;
	json_object_object_get_ex(param_obj, "fault", &fault_code);
	if (fault_code != NULL)
		return (char*)json_object_get_string(fault_code);
	return NULL;
}
