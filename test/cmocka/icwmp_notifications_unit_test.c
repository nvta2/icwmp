/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>

#include "common.h"
#include "notifications.h"
#include "cwmp_uci.h"
/*
 * Common functions
 */
char *notifications_test[7] = {"disabled" , "passive", "active", "passive_lw", "passive_passive_lw", "active_lw", "passive_active_lw"};

static LIST_HEAD(parameters_list);

static int cwmp_notifications_unit_tests_init(void **state)
{
	cwmp_main = (struct cwmp*)calloc(1, sizeof(struct cwmp));
	create_cwmp_session_structure();
	memcpy(&(cwmp_main->env), &cwmp_main, sizeof(struct env));
	cwmp_session_init();
	return 0;
}

static int cwmp_notifications_unit_tests_clean(void **state)
{
	clean_list_param_notify();
	clean_list_value_change();
	cwmp_free_all_dm_parameter_list(&parameters_list);
	cwmp_session_exit();
	FREE(cwmp_main->session);
	FREE(cwmp_main);
	return 0;
}

int check_notify_file(char *param, int *ret_notification)
{
	struct blob_buf bbuf;
	char *parameter = NULL;
	int notification = 0;
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
		memset(&bbuf, 0, sizeof(struct blob_buf));
		blob_buf_init(&bbuf, 0);

		if (blobmsg_add_json_from_string(&bbuf, buf) == false) {
			blob_buf_free(&bbuf);
			continue;
		}

		const struct blobmsg_policy p[4] = { { "parameter", BLOBMSG_TYPE_STRING }, { "notification", BLOBMSG_TYPE_INT32 }, { "type", BLOBMSG_TYPE_STRING }, { "value", BLOBMSG_TYPE_STRING } };

		struct blob_attr *tb[4] = { NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 4, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
		parameter = blobmsg_get_string(tb[0]);
		notification = blobmsg_get_u32(tb[1]);
		if (strcmp(param, parameter) == 0) {
			*ret_notification = notification;
		}
		parameter = NULL;
		notification = 0;
		blob_buf_free(&bbuf);
	}
	fclose(fp);
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
	int i, notification = 0;
	struct uci_list *list_notif;
	struct uci_element *e;
	for (i = 0; i < 7; i++) {
		int option_type;
		option_type = cwmp_uci_get_cwmp_varstate_option_value_list("cwmp", "@notifications[0]", notifications_test[i], &list_notif);
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
	clean_list_value_change();
}

int icwmp_notifications_test(void)
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

	return cmocka_run_group_tests(tests, cwmp_notifications_unit_tests_init, cwmp_notifications_unit_tests_clean);
}
