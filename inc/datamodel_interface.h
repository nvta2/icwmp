#ifndef SRC_DATAMODELIFACE_H_
#define SRC_DATAMODELIFACE_H_
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include "ubus.h"
#include "jshn.h"

#define DM_ROOT_OBJ "Device."
extern bool transaction_started;

int cwmp_transaction_start();
int cwmp_transaction_commit();
int cwmp_transaction_abort();
char* cwmp_get_parameter_values(char *parameter_name, json_object **parameters);
char* cwmp_set_parameter_value(char* parameter_name, char* value, char* parameter_key, int* flag);
char* cwmp_set_multiple_parameters_values(struct list_head parameters_values_list, char* parameter_key, int* flag, json_object **faults_array);
char* cwmp_add_object(char* object_name, char* key, char **instance);
char* cwmp_delete_object(char* object_name, char* key);
char* cwmp_get_parameter_names(char* object_name, bool next_level, json_object **parameters);
char* cwmp_get_parameter_attributes(char* parameter_name, json_object **parameters);
char* cwmp_set_parameter_attributes(char* parameter_name, char* notification);
int cwmp_update_enabled_notify_file(int instance_mode);

#endif /* SRC_DATAMODELIFACE_H_ */
