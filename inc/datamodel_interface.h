#ifndef SRC_DATAMODELIFACE_H_
#define SRC_DATAMODELIFACE_H_
#include "common.h"

#define DM_ROOT_OBJ "Device."
extern int transaction_id;

struct blob_attr *get_parameters_array(struct blob_attr *msg);
int get_fault(struct blob_attr *msg);
bool cwmp_transaction_start(char *app);
bool cwmp_transaction_commit();
bool cwmp_transaction_abort();
bool cwmp_transaction_status();
char *cwmp_get_parameter_values(char *parameter_name, struct list_head *parameters_list);
int cwmp_get_leaf_value(char *leaf, char **value);
char *cwmp_get_multiple_parameters_values(struct list_head *arg_params_list, struct list_head *parameters_list);
char *cwmp_get_single_parameter_value(char *parameter_name, struct cwmp_dm_parameter *dm_parameter);
int cwmp_set_multiple_parameters_values(struct list_head *parameters_values_list, char *parameter_key, int *flag, struct list_head *faults_list);
char *cwmp_add_object(char *object_name, char *key, char **instance);
char *cwmp_delete_object(char *object_name, char *key);
char *cwmp_get_parameter_names(char *object_name, bool next_level, struct list_head *parameters_list);

#endif /* SRC_DATAMODELIFACE_H_ */
