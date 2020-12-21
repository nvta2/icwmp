#ifndef SRC_DATAMODELIFACE_H_
#define SRC_DATAMODELIFACE_H_
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include "ubus.h"
#include "jshn.h"

#define DM_ROOT_OBJ "Device."
extern bool transaction_started;
extern int transaction_id;
extern json_object *old_global_json_obj;
extern json_object *actual_global_json_obj;
extern json_object *old_list_notify;
extern json_object *actual_list_notify;

enum notify_type {
	OLD_LIST_NOTIFY,
	ACTUAL_LIST_NOTIFY
};

int cwmp_transaction_start(char *app);
int cwmp_transaction_commit();
int cwmp_transaction_abort();
int cwmp_transaction_status();
char* cwmp_get_parameter_values(char *parameter_name, json_object **parameters);
char* cwmp_set_parameter_value(char* parameter_name, char* value, char* parameter_key, int* flag);
char* cwmp_set_multiple_parameters_values(struct list_head parameters_values_list, char* parameter_key, int* flag, json_object **faults_array);
char* cwmp_add_object(char* object_name, char* key, char **instance);
char* cwmp_delete_object(char* object_name, char* key);
char* cwmp_get_parameter_names(char* object_name, bool next_level, json_object **parameters);
char* cwmp_get_parameter_attributes(char* parameter_name, json_object **parameters);
char* cwmp_set_parameter_attributes(char* parameter_name, char* notification);
int cwmp_update_enabled_list_notify(int instance_moden, int notify_type);

#endif /* SRC_DATAMODELIFACE_H_ */
