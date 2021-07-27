/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>
#include <stdbool.h>

#include <libicwmp/common.h>
#include <libicwmp/config.h>
#include <libicwmp/rpc_soap.h>

struct cwmp cwmp_main_test = { 0 };

static int verify_inform_parameter_in_list(char *parameter)
{
	int i;
	for (i = 0; i < nbre_custom_inform; i++) {
		if (strcmp(parameter, custom_forced_inform_parameters[i]) == 0)
			return 1;
	}
	return 0;
}

void clean_config(struct cwmp *cwmp_test)
{
	FREE(cwmp_test->deviceid.manufacturer);
	FREE(cwmp_test->deviceid.serialnumber);
	FREE(cwmp_test->deviceid.productclass);
	FREE(cwmp_test->deviceid.oui);
	FREE(cwmp_test->deviceid.softwareversion);
	FREE(cwmp_test->conf.lw_notification_hostname);
	FREE(cwmp_test->conf.ip);
	FREE(cwmp_test->conf.ipv6);
	FREE(cwmp_test->conf.acsurl);
	FREE(cwmp_test->conf.acs_userid);
	FREE(cwmp_test->conf.acs_passwd);
	FREE(cwmp_test->conf.interface);
	FREE(cwmp_test->conf.cpe_userid);
	FREE(cwmp_test->conf.cpe_passwd);
	FREE(cwmp_test->conf.ubus_socket);
	FREE(cwmp_test->conf.connection_request_path);
	FREE(cwmp_test->conf.default_wan_iface);
	FREE(cwmp_test->conf.interface);
}

static void cwmp_custom_inform_unit_test(void **state)
{
	struct cwmp *cwmp_test = &cwmp_main_test;
	get_global_config(&(cwmp_test->conf));

	cwmp_test->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_valid.json");
	load_forced_inform_json_file(cwmp_test);
	assert_int_equal(nbre_custom_inform, 2);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 1);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 1);
	FREE(cwmp_test->conf.forced_inform_json_file);
	clean_custom_inform_parameters();

	cwmp_test->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_invalid_json.json");
	load_forced_inform_json_file(cwmp_test);
	assert_int_equal(nbre_custom_inform, 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 0);
	FREE(cwmp_test->conf.forced_inform_json_file);
	clean_custom_inform_parameters();

	cwmp_test->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_invalid_parameter.json");
	load_forced_inform_json_file(cwmp_test);
	assert_int_equal(nbre_custom_inform, 1);
	assert_int_equal(verify_inform_parameter_in_list("Devie.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device."), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 1);
	FREE(cwmp_test->conf.forced_inform_json_file);
	clean_custom_inform_parameters();

	clean_config(cwmp_test);
	icwmp_cleanmem();
}

int main(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(cwmp_custom_inform_unit_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
