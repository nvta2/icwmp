#ifndef SRC_DATAMODELIFACE_H_
#define SRC_DATAMODELIFACE_H_
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include "ubus.h"
#include "jshn.h"

char* cwmp_get_parameter_values(char *parameter_name, json_object **parameters);
char* cwmp_set_parameter_value(char* parameter_name, char* value, char* parameter_key, int* flag);
char* cwmp_add_object(char* object_name, char* key, char **instance);
char* cwmp_delete_object(char* object_name, char* key);
char* cwmp_get_parameter_names(char* object_name, bool next_level, json_object **parameters);
char* cwmp_get_parameter_attributes(char* parameter_name, json_object **parameters);
char* cwmp_set_parameter_attributes(char* parameter_name, char* notification);
void cwmp_update_enabled_notify_file(unsigned int amd_version, int instance_mode);
#endif /* SRC_DATAMODELIFACE_H_ */
