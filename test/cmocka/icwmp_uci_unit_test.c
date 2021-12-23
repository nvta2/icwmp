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
#include <libicwmp/cwmp_uci.h>

#define UCI_WRONG_PATH "cwmp.wrong_section.wrong_option"
struct uci_list *list = NULL;

static int cwmp_uci_unit_tests_init(void **state)
{
	cwmp_uci_init();
	return 0;
}

static int cwmp_uci_unit_tests_clean(void **state)
{
	icwmp_cleanmem();
	if (list != NULL)
		cwmp_free_uci_list(list);
	cwmp_uci_exit();
	return 0;
}

static void cwmp_uci_get_tests(void **state)
{

	char *value = NULL;
	int error;

	error = uci_get_value(UCI_ACS_USERID_PATH, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "iopsys");

	error = uci_get_value(UCI_WRONG_PATH, &value);
	assert_int_equal(error, UCI_OK);
	assert_null(value);

	error = uci_get_state_value(UCI_DHCP_ACS_URL, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "http://192.168.103.160:8080/openacs/acs");

	error = uci_get_state_value(UCI_WRONG_PATH, &value);
	assert_int_equal(error, UCI_OK);
	assert_null(value);

	error = cwmp_uci_get_option_value_string("cwmp", "acs", "dhcp_url", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "http://192.168.103.160:8080/openacs/acs");

	error = cwmp_uci_get_option_value_string("cwmp", "cpe", "userid", UCI_STANDARD_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "iopsys");

	error = cwmp_uci_get_option_value_string("cwmp", "wrong_section", "wrong_option", UCI_STANDARD_CONFIG, &value);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	assert_null(value);

	struct uci_section *s = get_section_by_section_name("cwmp", "acs", "acs", UCI_STANDARD_CONFIG);
	assert_non_null(s);

	error = cwmp_uci_get_value_by_section_string(s, "passwd", &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "iopsys");

	error = cwmp_uci_get_value_by_section_string(s, "wrong_option", &value);
	assert_null(value);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
}

static void cwmp_uci_set_tests(void **state)
{
	int error = UCI_OK;
	char *value = NULL;
	cwmp_uci_set_value("cwmp", "cpe", "exec_download", "1");
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "cpe", "exec_download", UCI_STANDARD_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "1");
	value = NULL;

	error = cwmp_uci_set_varstate_value("cwmp", "acs", "varstatopt", "varstatval");
	assert_int_equal(error, UCI_OK);
	cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "acs", "varstatopt", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "varstatval");
	value = NULL;

	error = cwmp_uci_set_value("cwmp", "wrong_section", "wrong_option", "wrong_value");
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "wront_section", "wrong_option", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	assert_null(value);
	value = NULL;

	cwmp_uci_set_value_by_path("cwmp.cpe.userid", "usertest");
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "cpe", "userid", UCI_STANDARD_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "usertest");
	value = NULL;

	cwmp_uci_set_varstate_value_by_path("cwmp.acs.opt1", "varstatval1");
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "acs", "opt1", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_OK);
	assert_string_equal(value, "varstatval1");
	value = NULL;

	error = cwmp_uci_set_value_by_path("cwmp.wront_section.wrong_option", "wrong_value");
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "wront_section", "wrong_option", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	assert_null(value);

	error = cwmp_uci_set_varstate_value_by_path("cwmp.wront_section.wrong_option", "wrong_value");
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	error = cwmp_uci_get_option_value_string("cwmp", "wront_section", "wrong_option", UCI_VARSTATE_CONFIG, &value);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	assert_null(value);
}

static void cwmp_uci_add_tests(void **state)
{
	struct uci_section *s = NULL;
	int error = UCI_OK, cmp_cfg = 0;

	error = cwmp_uci_add_section("cwmp", "acs", UCI_STANDARD_CONFIG, &s);
	assert_non_null(s);
	assert_int_equal(error, UCI_OK);
	assert_int_equal(strncmp(section_name(s), "cfg", 3), 0);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);

	error = cwmp_uci_add_section("new_package", "new_section", UCI_STANDARD_CONFIG, &s);
	assert_non_null(s);
	assert_int_equal(error, UCI_OK);
	assert_int_equal(strncmp(section_name(s), "cfg", 3), 0);
	cwmp_commit_package("new_package", UCI_STANDARD_CONFIG);

	error = cwmp_uci_add_section_with_specific_name("cwmp", "acs", "new_acs", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_OK);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);

	error = cwmp_uci_add_section_with_specific_name("cwmp", "acs", "new_acs", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_ERR_DUPLICATE);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);

}

static void cwmp_uci_list_tests(void **state)
{
	int error = UCI_OK;
	char *list_string = NULL;

	error = cwmp_uci_add_list_value("cwmp", "cpe", "optionlist", "val1", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_OK);
	error = cwmp_uci_add_list_value("cwmp", "cpe", "optionlist", "val2", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_OK);
	error = cwmp_uci_get_cwmp_standard_option_value_list("cwmp", "cpe", "optionlist", &list);
	assert_int_equal(error, UCI_TYPE_LIST);
	list_string = cwmp_uci_list_to_string(list, ",");
	assert_non_null(list_string);
	assert_string_equal(list_string, "val1,val2");
	list_string = NULL;
	if(list != NULL) {
		cwmp_free_uci_list(list);
		list = NULL;
	}

	error = cwmp_uci_add_list_value("cwmp", "wrong_section", "optionlist", "val1", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_ERR_INVAL);
	error = cwmp_uci_add_list_value("cwmp", "wrong_section", "optionlist", "val2", UCI_STANDARD_CONFIG);
	assert_int_equal(error, UCI_ERR_INVAL);
	error = cwmp_uci_get_cwmp_standard_option_value_list("cwmp", "wrong_section", "optionlist", &list);
	assert_int_equal(error, UCI_ERR_NOTFOUND);
	assert_null(list);
	list_string = cwmp_uci_list_to_string(list, ",");
	assert_null(list_string);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		    cmocka_unit_test(cwmp_uci_get_tests),
			cmocka_unit_test(cwmp_uci_set_tests),
			cmocka_unit_test(cwmp_uci_add_tests),
			cmocka_unit_test(cwmp_uci_list_tests)
	};

	return cmocka_run_group_tests(tests, cwmp_uci_unit_tests_init, cwmp_uci_unit_tests_clean);
}
