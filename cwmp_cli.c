#include <stdio.h>

#include "common.h"
#include "datamodel_interface.h"

LIST_HEAD(parameters_list);

struct fault_resp {
	int fault_index;
	char *fault_code;
	char *fault_message;
};

const struct fault_resp faults_array[] = { { FAULT_CPE_INTERNAL_ERROR, "9002", "Internal error" }, //Internal error
					   { FAULT_CPE_INVALID_PARAMETER_NAME, "9005", "Invalid parameter name" }, //Invalid parameter name
					   { FAULT_CPE_INVALID_PARAMETER_VALUE, "9007", "Invalid parameter value" }, //Invalid Parameter value
					   { FAULT_CPE_NON_WRITABLE_PARAMETER, "9008", "Attempt to set a non-writable parameter" }, //Non writable parameter
					   { FAULT_CPE_NOTIFICATION_REJECTED, "9009", "Notification request rejected" } };

char *get_fault_message_by_fault_code(char *fault_code)
{
	size_t i;
	size_t faults_array_size = sizeof(faults_array) / sizeof(struct fault_resp);
	for (i = 0; i < faults_array_size; i++) {
		if (strcmp(faults_array[i].fault_code, fault_code) == 0)
			return faults_array[i].fault_message;
	}
	return NULL;
}

union cmd_result {
	struct list_head *param_list;
	char *instance;
};

struct cmd_input {
	char *first_input;
	char *second_input;
};

struct cwmp_cli_command_struct {
	char *command_name;
	char *(*cmd_exec_func)(struct cmd_input in, union cmd_result *out);
	void (*display_cmd_result)(struct cmd_input in, union cmd_result res, char *fault);
};

/*
 * Get_Values
 */
char *cmd_get_exec_func(struct cmd_input in, union cmd_result *res)
{
	res->param_list = &parameters_list;
	char *fault = cwmp_get_parameter_values(in.first_input, res->param_list);
	return fault;
}

void display_get_cmd_result(struct cmd_input in __attribute__((unused)), union cmd_result res, char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, res.param_list, list) {
		fprintf(stdout, "%s => %s\n", param_value->name, param_value->value);
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

/*
 * Set_Values
 */
char *cmd_set_exec_func(struct cmd_input in, union cmd_result *res __attribute__((unused)))
{
	int flag;
	if (!transaction_started) {
		if (!cwmp_transaction_start("cwmp"))
			return strdup(get_fault_message_by_fault_code("9002"));

		transaction_started = true;
	}
	LIST_HEAD(list_set_param_value);
	LIST_HEAD(faults_list);
	add_dm_parameter_to_list(&list_set_param_value, in.first_input, in.second_input, NULL, 0, false);
	int fault_idx = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_key", &flag, &faults_list);
	if (fault_idx != FAULT_CPE_NO_FAULT) {
		struct cwmp_param_fault *param_fault = NULL;
		list_for_each_entry (param_fault, &faults_list, list) {
			char fault[5];
			snprintf(fault, 5, "%d", param_fault->fault);
			if (transaction_started) {
				cwmp_transaction_abort();
				transaction_started = false;
			}
			return strdup(fault);
		}
	}
	if (transaction_started) {
		cwmp_transaction_commit();
		transaction_started = false;
	}
	return NULL;
}

void display_set_cmd_result(struct cmd_input in, union cmd_result res __attribute__((unused)), char *fault)
{
	if (fault == NULL) {
		fprintf(stdout, "Set value is successfully done\n");
		fprintf(stdout, "%s => %s\n", in.first_input, in.second_input);
		return;
	}
	fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
	FREE(fault);
}

/*
 * Add_Object
 */
char *cmd_add_exec_func(struct cmd_input in, union cmd_result *res)
{
	if (!transaction_started) {
		if (!cwmp_transaction_start("cwmp"))
			return strdup(get_fault_message_by_fault_code("9002"));

		transaction_started = true;
	}

	char *fault = cwmp_add_object(in.first_input, in.second_input ? in.second_input : "add_obj", &(res->instance));
	if (fault != NULL) {
		if (transaction_started) {
			cwmp_transaction_abort();
			transaction_started = false;
		}
		return strdup(fault);
	}
	if (transaction_started) {
		cwmp_transaction_commit();
		transaction_started = false;
	}
	return NULL;
}

void display_add_cmd_result(struct cmd_input in, union cmd_result res, char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	if (in.first_input[strlen(in.first_input) - 1] == '.')
		fprintf(stdout, "Added %s%s.\n", in.first_input, res.instance);
	else
		fprintf(stdout, "Added %s.%s.\n", in.first_input, res.instance);
}

/*
 * Delete_Object
 */
char *cmd_del_exec_func(struct cmd_input in, union cmd_result *res __attribute__((unused)))
{
	if (!transaction_started) {
		if (!cwmp_transaction_start("cwmp"))
			return strdup(get_fault_message_by_fault_code("9002"));

		transaction_started = true;
	}

	char *fault = cwmp_delete_object(in.first_input, in.second_input ? in.second_input : "del_obj");
	if (fault != NULL) {
		if (transaction_started) {
			cwmp_transaction_abort();
			transaction_started = false;
		}
		return strdup(fault);
	}
	if (transaction_started) {
		cwmp_transaction_commit();
		transaction_started = false;
	}
	return NULL;
}

void display_del_cmd_result(struct cmd_input in, union cmd_result res __attribute__((unused)), char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	fprintf(stdout, "Deleted %s\n", in.first_input);
}

/*
 * Get_Notifications
 */
char *cmd_get_notif_exec_func(struct cmd_input in, union cmd_result *res)
{
	res->param_list = &parameters_list;
	char *fault = cwmp_get_parameter_attributes(in.first_input, res->param_list);
	return fault;
}

void display_get_notif_cmd_result(struct cmd_input in __attribute__((unused)), union cmd_result res, char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, res.param_list, list) {
		fprintf(stdout, "%s => %s\n", param_value->name, param_value->notification == 2 ? "active" : param_value->notification == 1 ? "passive" : "off");
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

/*
 * Set_Notifications
 */
char *cmd_set_notif_exec_func(struct cmd_input in, union cmd_result *res __attribute__((unused)))
{
	if (!transaction_started) {
		if (!cwmp_transaction_start("cwmp"))
			return strdup(get_fault_message_by_fault_code("9002"));

		transaction_started = true;
	}
	char *fault = cwmp_set_parameter_attributes(in.first_input, in.second_input);
	if (fault != NULL) {
		if (transaction_started) {
			cwmp_transaction_abort();
			transaction_started = false;
		}
		return strdup(fault);
	}
	if (transaction_started) {
		cwmp_transaction_commit();
		transaction_started = false;
	}
	return NULL;
}

void display_set_notif_cmd_result(struct cmd_input in, union cmd_result res __attribute__((unused)), char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	fprintf(stdout, "%s => %s\n", in.first_input, in.second_input);
}

/*
 * Get_Names
 */
char *cmd_get_names_exec_func(struct cmd_input in, union cmd_result *res)
{
	res->param_list = &parameters_list;
	bool next_level = in.second_input && (strcmp(in.second_input, "1") == 0 || strcasecmp(in.second_input, "true") == 0) ? true : false;
	char *fault = cwmp_get_parameter_names(in.first_input, next_level, res->param_list);
	return fault;
}

void display_get_names_cmd_result(struct cmd_input in __attribute__((unused)), union cmd_result res, char *fault)
{
	if (fault != NULL) {
		fprintf(stderr, "Fault %s: %s\n", fault, get_fault_message_by_fault_code(fault));
		FREE(fault);
		return;
	}
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, res.param_list, list) {
		fprintf(stdout, "%s => %s\n", param_value->name, param_value->writable ? "writable" : "not-writable");
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

/*
 * Main
 */
void cwmp_cli_help()
{
	printf("Valid commands:\n");
	printf("	help 					=> show this help\n");
	printf("	get [path-expr] 			=> get parameter values\n");
	printf("	get_names [path-expr] [next-level] 	=> get parameter names\n");
	printf("	set [path-expr] [value] 		=> set parameter value\n");
	printf("	add [object] 				=> add object\n");
	printf("	del [object] 				=> delete object\n");
	printf("	get_notif [path-expr]			=> get parameter notifications\n");
	printf("	set_notif [path-expr] [notification]	=> set parameter notifications\n");
}

const struct cwmp_cli_command_struct icwmp_commands[] = {
	{ "get", cmd_get_exec_func, display_get_cmd_result }, //get_values
	{ "get_names", cmd_get_names_exec_func, display_get_names_cmd_result }, //get_names
	{ "set", cmd_set_exec_func, display_set_cmd_result }, //set_values
	{ "add", cmd_add_exec_func, display_add_cmd_result }, //add_object
	{ "del", cmd_del_exec_func, display_del_cmd_result }, //delete_object
	{ "get_notif", cmd_get_notif_exec_func, display_get_notif_cmd_result }, //get_notifications
	{ "set_notif", cmd_set_notif_exec_func, display_set_notif_cmd_result }, //set_notifications
};

void execute_cwmp_cli_command(char *cmd, char *args[])
{
	if (!cmd || strlen(cmd) <= 0) {
		printf("You must add a command as input: \n\n");
		goto cli_help;
	}
	if (strcmp(cmd, "help") == 0)
		goto cli_help;
	struct cmd_input cmd_in = { args[0] ? args[0] : NULL, args[0] && args[1] ? args[1] : NULL };
	union cmd_result cmd_out = { 0 };
	size_t i;
	size_t commands_array_size = sizeof(icwmp_commands) / sizeof(struct cwmp_cli_command_struct);
	for (i = 0; i < commands_array_size; i++) {
		if (strcmp(icwmp_commands[i].command_name, cmd) == 0) {
			char *fault = icwmp_commands[i].cmd_exec_func(cmd_in, &cmd_out);
			icwmp_commands[i].display_cmd_result(cmd_in, cmd_out, fault);
			return;
		}
	}
	printf("Wrong cwmp cli command: %s\n", cmd);
cli_help:
	cwmp_cli_help();
}
