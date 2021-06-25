#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>

#include <libicwmp/common.h>
#include <libicwmp/config.h>
#include <libicwmp/datamodel_interface.h>
#include <libicwmp/event.h>
#include <libicwmp/xml.h>
#include <libicwmp/session.h>
#include <libicwmp/log.h>

static int dm_iface_unit_tests_clean(void **state)
{
	icwmp_cleanmem();
	return 0;
}

/*
 * datamodel interface tests
 */
static void dm_get_parameter_values_test(void **state)
{
	char *fault = NULL;
	LIST_HEAD(parameters_list);
	/*
	 * Test of valid parameter path
	 */
	fault = cwmp_get_parameter_values("Device.DeviceInfo.UpTime", &parameters_list);
	assert_null(fault);
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &parameters_list, list) {
		assert_non_null(param_value->name);
		assert_string_equal(param_value->name, "Device.DeviceInfo.UpTime");
		break;
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of non valid parameter path
	 */
	fault = cwmp_get_parameter_values("Device.Deviceno.UpTime", &parameters_list);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of valid multi-instance_object_path
	 */
	fault = cwmp_get_parameter_values("Device.WiFi.SSID.", &parameters_list);
	assert_null(fault);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of valid not multi-instance_object_path
	 */
	fault = cwmp_get_parameter_values("Device.DeviceInfo.", &parameters_list);
	assert_null(fault);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of non valid object path
	 */
	fault = cwmp_get_parameter_values("Device.Deviceno.", &parameters_list);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

static void dm_set_multiple_parameter_values_test(void **state)
{
	int flag = 0;
	int fault = 0;
	LargestIntegralType flag_values[15] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
	LargestIntegralType faults_values[15] = { 9005, 9007, 9008 };
	int fault_code = 0;
	char *fault_name = NULL;
	struct cwmp_param_fault *param_fault = NULL;
	LIST_HEAD(list_set_param_value);
	LIST_HEAD(faults_array);

	/*
	 * Test of one valid parameter
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.Alias", "wifi_alias_1", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_wifi_ssid_alias", &flag, &faults_array);
	assert_int_equal(fault, 0);
	assert_in_set(flag, flag_values, 15);
	cwmp_transaction_commit();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	flag = 0;

	add_dm_parameter_to_list(&list_set_param_value, "Device.ManagementServer.Username", "iopsys_user", NULL, 0, false); //for other flag value
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "mngt_server_user", &flag, &faults_array);
	assert_int_equal(fault, 0);
	assert_in_set(flag, flag_values, 15);
	cwmp_transaction_commit();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	fault = 0;
	flag = 0;

	/*
	 * Test of non valid parameter path
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.Alis", "wifi_alias_1", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_wifi_ssid_alias", &flag, &faults_array);
	assert_non_null(fault);
	list_for_each_entry (param_fault, &faults_array, list) {
		fault_code = param_fault->fault;
		fault_name = param_fault->name;
		break;
	}
	assert_int_not_equal(fault, 0);
	assert_int_equal(fault_code, 9005);
	assert_non_null(fault_name);
	cwmp_transaction_abort();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	cwmp_free_all_list_param_fault(&faults_array);
	fault_code = 0;
	fault_name = NULL;
	param_fault = NULL;
	fault = 0;

	/*
	 * Test of non writable, valid parameter path
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.ATM.Link.1.Status", "Up", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_atm_link_status", &flag, &faults_array);
	assert_int_not_equal(fault, 0);
	list_for_each_entry (param_fault, &faults_array, list) {
		fault_code = param_fault->fault;
		fault_name = param_fault->name;
		break;
	}
	assert_int_not_equal(fault, 0);
	assert_int_equal(fault_code, 9008);
	assert_non_null(fault_name);
	cwmp_transaction_abort();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	cwmp_free_all_list_param_fault(&faults_array);
	fault = 0;
	fault_code = 0;
	fault_name = NULL;
	param_fault = NULL;

	/*
	 * Test of writable, valid parameter path wrong value
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.Enable", "tre", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_wifi_ssid_alias", &flag, &faults_array);
	assert_non_null(fault);
	list_for_each_entry (param_fault, &faults_array, list) {
		fault_code = param_fault->fault;
		fault_name = param_fault->name;
		break;
	}
	assert_int_not_equal(fault, 0);
	assert_int_equal(fault_code, 9007);
	assert_non_null(fault_name);
	cwmp_transaction_abort();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	cwmp_free_all_list_param_fault(&faults_array);
	fault_code = 0;
	fault_name = NULL;
	param_fault = NULL;
	fault = 0;

	/*
	 * Test of list of valid parameters
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.Alias", "wifi_alias1_1", NULL, 0, false);
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.SSID", "wifi_ssid_2", NULL, 0, false);
	add_dm_parameter_to_list(&list_set_param_value, "Device.ManagementServer.Username", "iopsys_user_1", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_wifi_ssids_aliases", &flag, &faults_array);
	assert_int_equal(fault, 0);
	assert_in_set(flag, flag_values, 15);
	cwmp_transaction_commit();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);

	/*
	 * Test of list wrong parameters values
	 */
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.SSID", "wifi_ssid_2", NULL, 0, false);
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.1.Enable", "tre", NULL, 0, false);
	add_dm_parameter_to_list(&list_set_param_value, "Device.WiFi.SSID.2.Alis", "wifi_2", NULL, 0, false);
	add_dm_parameter_to_list(&list_set_param_value, "Device.ATM.Link.1.Status", "Up", NULL, 0, false);
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_multiple_parameters_values(&list_set_param_value, "set_wrongs", &flag, &faults_array);
	assert_int_not_equal(fault, 0);
	list_for_each_entry (param_fault, &faults_array, list) {
		assert_in_set(param_fault->fault, faults_values, 3);
	}
	cwmp_transaction_commit();
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
}

static void dm_add_object_test(void **state)
{
	char *instance = NULL;
	char *fault;

	/*
	 * Add valid path and writable object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_add_object("Device.WiFi.SSID.", "add_ssid", &instance);
	assert_non_null(instance);
	assert_null(fault);
	cwmp_transaction_commit();
	FREE(instance);

	/*
	 * Add not valid path object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_add_object("Device.WiFi.SIDl.", "add_ssid", &instance);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	assert_null(instance);
	cwmp_transaction_commit();
	FREE(instance);

	/*
	 * Add valid path not writable object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_add_object("Device.DeviceInfo.Processor.", "add_ssid", &instance);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	assert_null(instance);
	cwmp_transaction_commit();
	FREE(instance);
}

static void dm_delete_object_test(void **state)
{
	char *fault = NULL;

	/*
	 * Delete valid path and writable object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_delete_object("Device.WiFi.SSID.2.", "del_ssid");
	assert_null(fault);
	cwmp_transaction_commit();

	/*
	 * Delete not valid path object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_delete_object("Device.WiFi.SIDl.3.", "del_ssid");
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_transaction_commit();

	/*
	 * Delte valid path not writable object
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_delete_object("Device.DeviceInfo.Processor.2.", "del_processor");
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_transaction_commit();
}

static void dm_get_parameter_names_test(void **state)
{
	char *fault = NULL;
	LIST_HEAD(parameters_list);

	/*
	 * Valid multi-instance object path
	 */
	fault = cwmp_get_parameter_names("Device.WiFi.SSID.", true, &parameters_list);
	assert_null(fault);
	struct cwmp_dm_parameter *param_value = NULL;
	int nbre_objs = 0;
	list_for_each_entry (param_value, &parameters_list, list) {
		nbre_objs++;
	}
	assert_int_not_equal(nbre_objs, 0);
	cwmp_free_all_dm_parameter_list(&parameters_list);
	nbre_objs = 0;

	/*
	 * Valid not multi-instance object path
	 */
	fault = cwmp_get_parameter_names("Device.DeviceInfo.", true, &parameters_list);
	assert_null(fault);
	list_for_each_entry (param_value, &parameters_list, list) {
		nbre_objs++;
	}
	assert_int_not_equal(nbre_objs, 0);
	cwmp_free_all_dm_parameter_list(&parameters_list);
	nbre_objs = 0;

	/*
	 * Not valid object path
	 */
	fault = cwmp_get_parameter_names("Device.Devicenfo.", true, &parameters_list);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
}

void dm_set_parameter_attributes_test(void **state)
{
	char *fault = NULL;

	/*
	 * Valid parameter path
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_parameter_attributes("Device.DeviceInfo.UpTime", "1");
	assert_null(fault);
	cwmp_transaction_commit();

	/*
	 * Not valid parameter path
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_parameter_attributes("Device.DeviceInfo.pTime", "1");
	assert_non_null(fault);
	cwmp_transaction_commit();

	/*
	 * Valid object path
	 */
	cwmp_transaction_start("cwmp");
	fault = cwmp_set_parameter_attributes("Device.WiFi.SSID.", "1");
	assert_null(fault);
	cwmp_transaction_commit();
}

static void dm_get_parameter_attributes_test(void **state)
{
	char *fault = NULL;
	LIST_HEAD(parameters_list);

	/*
	 * Test of valid parameter path
	 */
	fault = cwmp_get_parameter_attributes("Device.DeviceInfo.ProvisioningCode", &parameters_list);
	assert_null(fault);
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &parameters_list, list) {
		assert_non_null(param_value->name);
		assert_string_equal(param_value->name, "Device.DeviceInfo.ProvisioningCode");
		break;
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of non valid parameter path
	 */
	fault = cwmp_get_parameter_attributes("Device.Deviceno.UpTime", &parameters_list);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of valid multi-instance_object_path
	 */
	fault = cwmp_get_parameter_attributes("Device.WiFi.SSID.", &parameters_list);
	assert_null(fault);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of valid not multi-instance_object_path
	 */
	fault = cwmp_get_parameter_attributes("Device.DeviceInfo.", &parameters_list);
	assert_null(fault);
	cwmp_free_all_dm_parameter_list(&parameters_list);

	/*
	 * Test of non valid object path
	 */
	fault = cwmp_get_parameter_attributes("Device.Deviceno.", &parameters_list);
	assert_non_null(fault);
	assert_string_equal(fault, "9005");
	cwmp_free_all_dm_parameter_list(&parameters_list);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(dm_get_parameter_values_test), //
		cmocka_unit_test(dm_set_multiple_parameter_values_test),
		cmocka_unit_test(dm_add_object_test),
		cmocka_unit_test(dm_delete_object_test),
		cmocka_unit_test(dm_get_parameter_names_test),
		cmocka_unit_test(dm_set_parameter_attributes_test),
		cmocka_unit_test(dm_get_parameter_attributes_test),
	};

	return cmocka_run_group_tests(tests, NULL, dm_iface_unit_tests_clean);
}
