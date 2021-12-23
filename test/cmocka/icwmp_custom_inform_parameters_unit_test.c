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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>
#include <stdbool.h>

#include <libicwmp/common.h>
#include <libicwmp/config.h>
#include <libicwmp/soap.h>

static int custom_inform_unit_tests_init(void **state)
{
	cwmp_main = (struct cwmp*)calloc(1, sizeof(struct cwmp));
	get_global_config();
	return 0;
}

void clean_config()
{
	FREE(cwmp_main->deviceid.manufacturer);
	FREE(cwmp_main->deviceid.serialnumber);
	FREE(cwmp_main->deviceid.productclass);
	FREE(cwmp_main->deviceid.oui);
	FREE(cwmp_main->deviceid.softwareversion);
	FREE(cwmp_main->conf.lw_notification_hostname);
	FREE(cwmp_main->conf.ip);
	FREE(cwmp_main->conf.ipv6);
	FREE(cwmp_main->conf.acsurl);
	FREE(cwmp_main->conf.acs_userid);
	FREE(cwmp_main->conf.acs_passwd);
	FREE(cwmp_main->conf.interface);
	FREE(cwmp_main->conf.cpe_userid);
	FREE(cwmp_main->conf.cpe_passwd);
	FREE(cwmp_main->conf.ubus_socket);
	FREE(cwmp_main->conf.connection_request_path);
	FREE(cwmp_main->conf.default_wan_iface);
	FREE(cwmp_main->conf.interface);
}

static int custom_inform_unit_tests_clean(void **state)
{
	clean_config();
	icwmp_cleanmem();
	FREE(cwmp_main);
	return 0;
}

static int verify_inform_parameter_in_list(char *parameter)
{
	int i;
	for (i = 0; i < nbre_custom_inform; i++) {
		if (strcmp(parameter, custom_forced_inform_parameters[i]) == 0)
			return 1;
	}
	return 0;
}

static void cwmp_custom_inform_unit_test(void **state)
{
	cwmp_main->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_valid.json");
	load_forced_inform_json_file();
	assert_int_equal(nbre_custom_inform, 2);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 1);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 1);
	FREE(cwmp_main->conf.forced_inform_json_file);
	clean_custom_inform_parameters();

	cwmp_main->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_invalid_json.json");
	load_forced_inform_json_file();
	assert_int_equal(nbre_custom_inform, 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 0);
	FREE(cwmp_main->conf.forced_inform_json_file);
	clean_custom_inform_parameters();

	cwmp_main->conf.forced_inform_json_file = strdup("/etc/icwmpd/forced_inform_invalid_parameter.json");
	load_forced_inform_json_file();
	assert_int_equal(nbre_custom_inform, 1);
	assert_int_equal(verify_inform_parameter_in_list("Devie.DeviceInfo.X_IOPSYS_EU_BaseMACAddress"), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device."), 0);
	assert_int_equal(verify_inform_parameter_in_list("Device.DeviceInfo.UpTime"), 1);
	FREE(cwmp_main->conf.forced_inform_json_file);
	clean_custom_inform_parameters();
}

int main(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(cwmp_custom_inform_unit_test),
	};

	return cmocka_run_group_tests(tests, custom_inform_unit_tests_init, custom_inform_unit_tests_clean);
}
