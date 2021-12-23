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
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>

#include <libicwmp/common.h>
#include <libicwmp/backupSession.h>
#include <libicwmp/xml.h>
#include <libicwmp/config.h>
#include <libicwmp/cwmp_time.h>
#include <libicwmp/event.h>
#include <libicwmp/cwmp_uci.h>
#include <libicwmp/cwmp_cli.h>

static char *add_instance = NULL;

static int cwmp_cli_unit_tests_init(void **state)
{
	cwmp_main = (struct cwmp*)calloc(1, sizeof(struct cwmp));
	create_cwmp_session_structure();
	memcpy(&(cwmp_main->env), &cwmp_main, sizeof(struct env));
	cwmp_session_init();
	return 0;
}

static int cwmp_cli_unit_tests_clean(void **state)
{
	icwmp_free_list_services();
	cwmp_session_exit();
	FREE(cwmp_main->session);
	FREE(cwmp_main);
	FREE(add_instance);
	return 0;
}

static void cwmp_execute_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * One argument: Valid
	 */
	char *argsget_valid[] = {"Device.DeviceInfo.", NULL};
	fault = execute_cwmp_cli_command("get", argsget_valid);
	assert_null(fault);

	/*
	 * No arguments: Valid
	 */
	char *argsget_valid_null[] = {NULL, NULL};
	fault = execute_cwmp_cli_command("get", argsget_valid_null);
	assert_null(fault);

	/*
	 * One argument: Not Valid (9005)
	 */
	char *argsget_not_valid[] = {"Device.DeviceInf.", NULL};
	fault = execute_cwmp_cli_command("get", argsget_not_valid);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	FREE(fault);

	/*
	 * One argument: Not Valid (9003)
	 */
	char *argsset_not_valid[] = {"Device.WiFi.SSID.1.SSID", NULL};
	fault = execute_cwmp_cli_command("set", argsset_not_valid);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");
	FREE(fault);

	/*
	 * No argumenst: Not Valid
	 */
	char *argset_no_arg[] = {NULL, NULL};
	fault = execute_cwmp_cli_command("set", argset_no_arg);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");
	FREE(fault);

	/*
	 * Two arguments: Valid
	 */
	char *argsset_valid[] = {"Device.WiFi.SSID.1.SSID", "wifi_1_ssid"};
	fault = execute_cwmp_cli_command("set", argsset_valid);
	assert_null(fault);

	/*
	 * Two arguments: Not Valid (9008)
	 */
	char *argset_two_arg_not_valid[] = {"Device.DeviceInfo.UpTime", "889"};
	fault = execute_cwmp_cli_command("set", argset_two_arg_not_valid);
	assert_non_null(fault);
	assert_string_equal(fault, "9008");
	FREE(fault);
}

static void cwmp_get_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * Get: input is NULL
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_get_out_1 = { 0 };
	fault = cmd_get_exec_func(input_null, &cmd_get_out_1);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_get_out_1.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_get_out_1.param_list);

	/*
	 * Get: input is valid parameter path
	 */
	struct cmd_input input_valid = {"Device.WiFi.", NULL};
	union cmd_result cmd_get_out_2 = { 0 };
	fault = cmd_get_exec_func(input_valid, &cmd_get_out_2);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_get_out_2.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_get_out_2.param_list);

	/*
	 * Get: input is invalid parameter path
	 */
	struct cmd_input input_invalid = {"Device.WrongPath.", NULL};
	union cmd_result cmd_get_out_3 = { 0 };
	fault = cmd_get_exec_func(input_invalid, &cmd_get_out_3);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	//assert_int_equal((int)list_empty(cmd_get_out_3.param_list), 1);
}

static void cwmp_set_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * Set: both two inputs are null
	 */
	struct cmd_input inputs_null = {NULL, NULL};
	union cmd_result cmd_set_out_1 = { 0 };
	fault = cmd_set_exec_func(inputs_null, &cmd_set_out_1);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");

	/*
	 * Set: only second input is null
	 */
	struct cmd_input input2_null = {"Device.WiFi.SSID.1.SSID", NULL};
	union cmd_result cmd_set_out_2 = { 0 };
	fault = cmd_set_exec_func(input2_null, &cmd_set_out_2);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");

	/*
	 * Set: Not null and valid inputs
	 */
	struct cmd_input inputs_valid = {"Device.WiFi.SSID.1.SSID", "wifi_1_new_ssid"};
	union cmd_result cmd_set_out_3 = { 0 };
	fault = cmd_set_exec_func(inputs_valid, &cmd_set_out_3);
	assert_null(fault);

	/*
	 * Set: Not null inputs and first input is invalid
	 */
	struct cmd_input input1_invalid = {"Device.WiFi.WrongPath", "wifi_1_other_ssid"};
	union cmd_result cmd_set_out_4 = { 0 };
	fault = cmd_set_exec_func(input1_invalid, &cmd_set_out_4);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");

	/*
	 * Set: Not null inputs and second input is invalid
	 */
	struct cmd_input input2_invalid = {"Device.WiFi.SSID.1.SSID", "wrongssidwrongssidwrongssidwrongs"};
	union cmd_result cmd_set_out_5 = { 0 };
	fault = cmd_set_exec_func(input2_invalid, &cmd_set_out_5);
	assert_non_null(fault);
	assert_string_equal(fault, "9007");

	/*
	 * Set: Not null inputs and first input is read only parameter
	 */
	struct cmd_input input1_ro = {"Device.WiFi.SSID.1.MACAddress", "55:a8:88:b9:0d:5a"};
	union cmd_result cmd_set_out_6 = { 0 };
	fault = cmd_set_exec_func(input1_ro, &cmd_set_out_6);
	assert_non_null(fault);
	assert_string_equal(fault, "9008");
}

static void cwmp_add_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * Add: input is null
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_add_out_1 = { 0 };
	fault = cmd_add_exec_func(input_null, &cmd_add_out_1);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");
	assert_null(cmd_add_out_1.instance);

	/*
	 * Add: input is valid object path
	 */
	struct cmd_input input_valid = {"Device.WiFi.SSID.", NULL};
	union cmd_result cmd_add_out_2 = { 0 };
	fault = cmd_add_exec_func(input_valid, &cmd_add_out_2);
	assert_null(fault);
	assert_non_null(cmd_add_out_2.instance);
	assert_int_equal(atoi(cmd_add_out_2.instance) > 0, 1);
	add_instance = cmd_add_out_2.instance;

	/*
	 * Add: input is invalid object path
	 */
	struct cmd_input input_invalid_param_path = {"Device.WiFi.WrongObjectPath.", NULL};
	union cmd_result cmd_add_out_3 = { 0 };
	fault = cmd_add_exec_func(input_invalid_param_path, &cmd_add_out_3);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	assert_null(cmd_add_out_3.instance);

	/*
	 * Add: input is non writable object path
	 */
	struct cmd_input input_read_only_obj = {"Device.DeviceInfo.Processor.", NULL};
	union cmd_result cmd_add_out_4 = { 0 };
	fault = cmd_add_exec_func(input_read_only_obj, &cmd_add_out_4);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	assert_null(cmd_add_out_4.instance);
}

static void cwmp_del_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * Delete: input is null
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_del_out_1 = { 0 };
	fault = cmd_del_exec_func(input_null, &cmd_del_out_1);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");
	assert_null(cmd_del_out_1.instance);

	/*
	 * Delete: input is valid object path
	 */
	char *del_object = NULL;
	icwmp_asprintf(&del_object, "Device.WiFi.SSID.%s.", add_instance);
	struct cmd_input input_valid = {del_object, NULL};
	union cmd_result cmd_del_out_2 = { 0 };
	fault = cmd_del_exec_func(input_valid, &cmd_del_out_2);
	assert_null(fault);

	/*
	 * Delete: input is invalid object path
	 */
	struct cmd_input input_invalid_param_path = {"Device.WiFi.WrongObjectPath.1.", NULL};
	union cmd_result cmd_del_out_3 = { 0 };
	fault = cmd_del_exec_func(input_invalid_param_path, &cmd_del_out_3);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");

	/*
	 * Delete: input is non writable object path
	 */
	struct cmd_input input_read_only_obj = {"Device.DeviceInfo.Processor.1.", NULL};
	union cmd_result cmd_del_out_4 = { 0 };
	fault = cmd_del_exec_func(input_read_only_obj, &cmd_del_out_4);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
}

static void cwmp_set_notif_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * SetAttributes: both two inputs are null
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_setnotif_out_1 = { 0 };
	fault = cmd_set_notif_exec_func(input_null, &cmd_setnotif_out_1);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");

	/*
	 * SetAttributes: only second input is null
	 */
	struct cmd_input input2_null = {"Device.WiFi.SSID.", NULL};
	union cmd_result cmd_set_out_2 = { 0 };
	fault = cmd_set_notif_exec_func(input2_null, &cmd_set_out_2);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");

	/*
	 * SetAttributes: Not null and valid inputs
	 */
	struct cmd_input inputs_valid = {"Device.WiFi.SSID.", "1"};
	union cmd_result cmd_set_out_3 = { 0 };
	fault = cmd_set_notif_exec_func(inputs_valid, &cmd_set_out_3);
	assert_null(fault);

	/*
	 * SetAttributes: Not null inputs and first input is invalid
	 */
	struct cmd_input input1_invalid = {"Device.WiFi.WrongPath", "2"};
	union cmd_result cmd_set_out_4 = { 0 };
	fault = cmd_set_notif_exec_func(input1_invalid, &cmd_set_out_4);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");

	/*
	 * SetAttributes: Not null inputs and second input is invalid
	 */
	struct cmd_input input2_invalid = {"Device.WiFi.SSID.1.SSID", "8"};
	union cmd_result cmd_set_out_5 = { 0 };
	fault = cmd_set_notif_exec_func(input2_invalid, &cmd_set_out_5);
	assert_non_null(fault);
	assert_string_equal(fault, "9003");

}

static void cwmp_get_notif_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * GetAttributes: input is NULL
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_getnotif_out_1 = { 0 };
	fault = cmd_get_notif_exec_func(input_null, &cmd_getnotif_out_1);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnotif_out_1.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnotif_out_1.param_list);

	/*
	 * GetAttributes: input is valid parameter path
	 */
	struct cmd_input input_valid = {"Device.WiFi.", NULL};
	union cmd_result cmd_getnotif_out_2 = { 0 };
	fault = cmd_get_notif_exec_func(input_valid, &cmd_getnotif_out_2);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnotif_out_2.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnotif_out_2.param_list);

	/*
	 * GetAttributes: input is invalid parameter path
	 */
	struct cmd_input input_invalid = {"Device.WrongPath.", NULL};
	union cmd_result cmd_getnotif_out_3 = { 0 };
	fault = cmd_get_notif_exec_func(input_invalid, &cmd_getnotif_out_3);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	//assert_int_equal((int)list_empty(cmd_get_out_3.param_list), 1);
}

static void cwmp_get_names_cli_unit_test(void **state)
{
	char *fault = NULL;

	/*
	 * GetNames: inputs are NULL
	 */
	struct cmd_input input_null = {NULL, NULL};
	union cmd_result cmd_getnames_out_1 = { 0 };
	fault = cmd_get_names_exec_func(input_null, &cmd_getnames_out_1);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_1.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_1.param_list);

	/*
	 * GetNames: input is valid parameter path
	 */
	struct cmd_input input_valid = {"Device.WiFi.", NULL};
	union cmd_result cmd_getnames_out_2 = { 0 };
	fault = cmd_get_names_exec_func(input_valid, &cmd_getnames_out_2);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_2.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_2.param_list);

	/*
	 * GetNames: input is invalid parameter path
	 */
	struct cmd_input input_invalid = {"Device.WrongPath.", NULL};
	union cmd_result cmd_getnames_out_3 = { 0 };
	fault = cmd_get_names_exec_func(input_invalid, &cmd_getnames_out_3);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	//assert_int_equal((int)list_empty(cmd_get_out_3.param_list), 1);

	/*
	 * GetNames: First input is NULL and second input (next level) is 0
	 */
	struct cmd_input input_null_i2_0 = {NULL, "0"};
	union cmd_result cmd_getnames_out_4 = { 0 };
	fault = cmd_get_names_exec_func(input_null_i2_0, &cmd_getnames_out_4);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_4.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_4.param_list);

	/*
	 * GetNames: input is valid parameter path and second input (next leve) is 0
	 */
	struct cmd_input input_valid_i2_0 = {"Device.WiFi.", "0"};
	union cmd_result cmd_getnames_out_5 = { 0 };
	fault = cmd_get_names_exec_func(input_valid_i2_0, &cmd_getnames_out_5);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_5.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_5.param_list);

	/*
	 * GetNames: First input is NULL and second input (next level) is 1
	 */
	struct cmd_input input_null_i2_1 = {NULL, "1"};
	union cmd_result cmd_getnames_out_6 = { 0 };
	fault = cmd_get_names_exec_func(input_null_i2_1, &cmd_getnames_out_6);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_6.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_6.param_list);

	/*
	 * GetNames: input is valid parameter path and second input (next leve) is 1
	 */
	struct cmd_input input_valid_i2_1 = {"Device.WiFi.", "1"};
	union cmd_result cmd_getnames_out_7 = { 0 };
	fault = cmd_get_names_exec_func(input_valid_i2_1, &cmd_getnames_out_7);
	assert_null(fault);
	assert_int_equal((int)list_empty(cmd_getnames_out_7.param_list), 0);
	cwmp_free_all_dm_parameter_list(cmd_getnames_out_7.param_list);
}

int main(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(cwmp_execute_cli_unit_test),
			cmocka_unit_test(cwmp_get_cli_unit_test),
			cmocka_unit_test(cwmp_set_cli_unit_test),
			cmocka_unit_test(cwmp_add_cli_unit_test),
			cmocka_unit_test(cwmp_del_cli_unit_test),
			cmocka_unit_test(cwmp_set_notif_cli_unit_test),
			cmocka_unit_test(cwmp_get_notif_cli_unit_test),
			cmocka_unit_test(cwmp_get_names_cli_unit_test)
	};

	return cmocka_run_group_tests(tests, cwmp_cli_unit_tests_init, cwmp_cli_unit_tests_clean);
}
