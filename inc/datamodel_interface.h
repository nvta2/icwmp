#ifndef SRC_DATAMODELIFACE_H_
#define SRC_DATAMODELIFACE_H_
#include "common.h"
#include "cwmp_json.h"

#define DM_ROOT_OBJ "Device."
extern bool transaction_started;
extern int transaction_id;

bool cwmp_transaction_start(char *app);
bool cwmp_transaction_commit();
bool cwmp_transaction_abort();
bool cwmp_transaction_status();
char *cwmp_get_parameter_values(char *parameter_name, struct list_head *parameters_list);
int cwmp_set_multiple_parameters_values(struct list_head parameters_values_list, char *parameter_key, int *flag, struct list_head *faults_list);
char *cwmp_add_object(char *object_name, char *key, char **instance);
char *cwmp_delete_object(char *object_name, char *key);
char *cwmp_get_parameter_names(char *object_name, bool next_level, struct list_head *parameters_list);
char *cwmp_get_parameter_attributes(char *parameter_name, struct list_head *parameters_list);
char *cwmp_set_parameter_attributes(char *parameter_name, char *notification);
int cwmp_update_enabled_list_notify(int instance_moden, struct list_head *parameters_list);

#endif /* SRC_DATAMODELIFACE_H_ */
