/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>

#include <libicwmp/common.h>
#include <libicwmp/notifications.h>
#include <libicwmp/cwmp_uci.h>
/*
 * Common functions
 */
char *notifications_test[7] = {"disabled" , "passive", "active", "passive_lw", "passive_passive_lw", "active_lw", "passive_active_lw"};

int check_notify_file(char *param, int *ret_notification)
{
	FILE *fp;
	int nbre_iterations = 0;
	char buf[1280];
	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL)
		return nbre_iterations;
	while (fgets(buf, 1280, fp) != NULL) {
		int len = strlen(buf);
		if (len) {
			nbre_iterations++;
			buf[len - 1] = '\0';
		}
		char parameter[256] = { 0 }, notification[2] = { 0 }, value[1024] = { 0 }, type[32] = { 0 };
		sscanf(buf, "parameter:%256s notifcation:%2s type:%32s value:%1024s\n", parameter, notification, type, value);
		if (strcmp(param, parameter) == 0) {
			*ret_notification = atoi(notification);
		}
	}
	return nbre_iterations;
}

int get_parameter_notification_from_list_head(struct list_head *params_list, char *parameter_name)
{
	struct cwmp_dm_parameter *param_iter = NULL;
	list_for_each_entry (param_iter, params_list, list) {
		if (strcmp(parameter_name, param_iter->name) == 0)
			return param_iter->notification;
	}
	return 0;
}

int get_parameter_notification_from_notifications_uci_list(char *parameter_name)
{
	int i, option_type, notification = 0;
	struct uci_list *list_notif;
	struct uci_element *e;
	cwmp_uci_init(UCI_STANDARD_CONFIG);
	for (i = 0; i < 7; i++) {
		option_type = cwmp_uci_get_option_value_list("cwmp", "@notifications[0]", notifications_test[i], &list_notif);
		if (list_notif) {
			uci_foreach_element(list_notif, e) {
				if (strcmp(e->name, parameter_name) == 0) {
					notification = i;
					break;
				}
			}
		}
		if (option_type == UCI_TYPE_STRING)
			cwmp_free_uci_list(list_notif);
		if(notification > 0)
			break;
	}
	cwmp_uci_exit();
	return notification;
}

int get_parameter_in_list_value_change(char *parameter_name)
{
	struct cwmp_dm_parameter *param_iter = NULL;
	list_for_each_entry (param_iter, &list_value_change, list) {
		if (strcmp(parameter_name, param_iter->name) == 0)
			return 1;
	}
	return 0;
}
////////////////////////////////////////////

static void cwmp_init_list_param_notify_unit_test_default(void **state)
{
	init_list_param_notify();
	assert_int_equal((int)list_empty(&list_param_obj_notify), 1);
}

static void cwmp_update_notify_file_unit_test_default(void **state)
{
	cwmp_update_enabled_notify_file();
	int notification = 0, nbre_iter = 0;
	nbre_iter = check_notify_file("Device.DeviceInfo.ProvisioningCode", &notification);
	assert_int_equal(nbre_iter, 2);
	assert_int_equal(notification, 2);
}

static void cwmp_get_parameter_attribute_unit_test_default(void **state)
{

	LIST_HEAD(parameters_list);
	char *err = NULL;

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.SoftwareVersion", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.SoftwareVersion"), 2);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.ProvisioningCode", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.ProvisioningCode"), 2);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.UpTime", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.UpTime"), 0);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.Upime", &parameters_list);
	assert_non_null(err);
	assert_string_equal(err, "9005");
	assert_int_equal((int)list_empty(&parameters_list), 1);
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

static void cwmp_check_value_change_unit_test_default(void **state)
{
	int is_notify = check_value_change();
	assert_int_equal(is_notify, NOTIF_NONE);
	assert_int_equal((int)list_empty(&list_value_change), 1);
	assert_int_equal((int)list_empty(&list_lw_value_change), 1);
}

static void cwmp_set_parameter_attributes_parameter_sub_parameter_1_unit_test(void **state)
{
	char *err = NULL;
	//char *list_notif = NULL;

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.UpTime", 1);
	assert_null(err);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.UpTime"), 1);

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.", 1);
	assert_null(err);

	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo."), 1);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.UpTime"), 0);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.ProvisioningCode"), 0);

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.UpTime", 1);
	assert_null(err);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.UpTime"), 0);

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.Processor.1.", 2);
	assert_null(err);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.Processor.1."), 2);
	assert_int_equal(get_parameter_notification_from_notifications_uci_list("Device.DeviceInfo.Processor.1.Alias"), 0);

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.Upime", 2);
	assert_non_null(err);
	assert_string_equal(err, "9005");

	err = cwmp_set_parameter_attributes("Device.DeviceInfo.ProvisioningCode", 0);
	assert_non_null(err);
	assert_string_equal(err, "9009");
	reinit_list_param_notify();
}

static void cwmp_get_parameter_attributes_parameter_sub_parameter_1_unit_test(void **state)
{
	LIST_HEAD(parameters_list);
	char *err = NULL;

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.UpTime", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.UpTime"), 1);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.Processor.", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.Processor.1.Alias"), 2);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.Processor.1.Architecture"), 2);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.Processor.2.Alias"), 1);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.Processor.2.Architecture"), 1);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	err = cwmp_get_parameter_attributes("Device.DeviceInfo.ProvisioningCode", &parameters_list);
	assert_null(err);
	assert_int_equal((int)list_empty(&parameters_list), 0);
	assert_int_equal(get_parameter_notification_from_list_head(&parameters_list, "Device.DeviceInfo.ProvisioningCode"), 2);
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

static void cwmp_update_notify_file_1_unit_test(void **state)
{
	cwmp_update_enabled_notify_file();
	int notification = 0, nbre_iter = 0;

	nbre_iter = check_notify_file("Device.DeviceInfo.ProvisioningCode", &notification);
	assert_int_equal(nbre_iter > 2, 1);
	assert_int_equal(notification, 2);
	notification = 0;
	nbre_iter = 0;

	nbre_iter = check_notify_file("Device.DeviceInfo.UpTime", &notification);
	assert_int_equal(nbre_iter > 2, 1);
	assert_int_equal(notification, 1);
	notification = 0;
	nbre_iter = 0;
}

static void cwmp_check_value_change_1_unit_test(void **state)
{
	sleep(2);
	int is_notify = check_value_change();
	assert_int_equal(is_notify, NOTIF_PASSIVE);
	assert_int_equal((int)list_empty(&list_value_change), 0);
	assert_int_equal((int)list_empty(&list_lw_value_change), 1);
	assert_int_equal(get_parameter_in_list_value_change("Device.DeviceInfo.UpTime"), 1);
}

int main(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(cwmp_init_list_param_notify_unit_test_default),
		    cmocka_unit_test(cwmp_update_notify_file_unit_test_default),
		    cmocka_unit_test(cwmp_get_parameter_attribute_unit_test_default),
		    cmocka_unit_test(cwmp_check_value_change_unit_test_default),
		    cmocka_unit_test(cwmp_set_parameter_attributes_parameter_sub_parameter_1_unit_test),
		    cmocka_unit_test(cwmp_get_parameter_attributes_parameter_sub_parameter_1_unit_test),
		    cmocka_unit_test(cwmp_update_notify_file_1_unit_test),
		    cmocka_unit_test(cwmp_check_value_change_1_unit_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
