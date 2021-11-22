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

#include <libicwmp/rpc_soap.h>
#include <libicwmp/event.h>
#include <libicwmp/session.h>
#include <libicwmp/config.h>
#include <libicwmp/backupSession.h>
#include <libicwmp/log.h>
#include <libicwmp/download.h>

#include "icwmp_soap_msg_unit_test.h"

struct cwmp cwmp_main_test = { 0 };
struct cwmp *cwmp_test;
struct session *session_test = NULL;
int instance = 0;

#define INVALID_PARAM_KEY "ParameterKeyParameterKeyParameter"
#define INVALID_USER "useruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruseruser1"
#define INVALID_PASS "passwordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpasswordpassword1"
/*
 * End test clean
 */
void clean_config()
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
}

void clean_name_space()
{
	FREE(ns.soap_env);
	FREE(ns.soap_enc);
	FREE(ns.xsd);
	FREE(ns.xsi);
	FREE(ns.cwmp);
}

void unit_test_remove_all_events_by_session(struct session *session)
{
	while (session->head_event_container.next != &(session->head_event_container)) {
		struct event_container *event_container;
		event_container = list_entry(session->head_event_container.next, struct event_container, list);
		free(event_container->command_key);
		cwmp_free_all_dm_parameter_list(&(event_container->head_dm_parameter));
		list_del(&(event_container->list));
		free(event_container);
	}
}

void unit_test_end_test_destruction()
{
	struct session *session = NULL;
	while (cwmp_test->head_session_queue.next != &(cwmp_test->head_session_queue)) {
		session = list_entry(cwmp_test->head_session_queue.next, struct session, list);
		unit_test_remove_all_events_by_session(session);
		cwmp_session_destructor(session);
	}
}

/*
 * CMOCKA functions
 */
static int soap_unit_tests_init(void **state)
{
	cwmp_test = &cwmp_main_test;
	INIT_LIST_HEAD(&(cwmp_test->head_session_queue));
	memcpy(&(cwmp_test->env), &cwmp_test, sizeof(struct env));
	cwmp_uci_init();
	return 0;
}

static int soap_unit_tests_clean(void **state)
{
	icwmp_cleanmem();
	if (session_test != NULL) {
		MXML_DELETE(session_test->tree_in);
		MXML_DELETE(session_test->tree_out);
		session_test->head_rpc_acs.next = NULL;
		session_test->head_rpc_cpe.next = NULL;
		cwmp_session_destructor(session_test);
	}
	icwmp_free_list_services();
	clean_name_space();
	clean_config();
	cwmp_uci_exit();
	return 0;
}

/*
 * UNIT Tests
 */
static void get_config_test(void **state)
{
	int error = get_global_config(&(cwmp_test->conf));
	assert_int_equal(error, CWMP_OK);
	log_set_severity_idx("INFO");
}

static void get_deviceid_test(void **state)
{
	int error = cwmp_get_deviceid(cwmp_test);
	assert_int_equal(error, CWMP_OK);
}

static void add_event_test(void **state)
{
	struct event_container *event_container;
	event_container = cwmp_add_event_container(cwmp_test, EVENT_IDX_1BOOT, "");
	assert_non_null(event_container);
	assert_string_equal(EVENT_CONST[event_container->code].CODE, "1 BOOT");
}

int create_session(struct session **session)
{
	*session = calloc(1, sizeof(struct session));
	if (*session == NULL)
		return 0;

	list_add_tail(&((*session)->list), &(cwmp_test->head_session_queue));

	INIT_LIST_HEAD(&((*session)->head_event_container));
	*session = list_entry((&(cwmp_test->head_session_queue))->next, struct session, list);
	return 1;
}

static void soap_inform_message_test(void **state)
{
	mxml_node_t *env = NULL, *n = NULL, *device_id = NULL, *cwmp_inform = NULL;
	struct session *session = NULL;
	struct rpc *rpc_acs;

	create_session(&session);
	session_test = session;
	rpc_acs = list_entry(&(session->head_rpc_acs), struct rpc, list);

	cwmp_rpc_acs_prepare_message_inform(cwmp_test, session, rpc_acs);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_inform = mxmlFindElement(env, env, "cwmp:Inform", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_inform);
	device_id = mxmlFindElement(cwmp_inform, cwmp_inform, "DeviceId", NULL, NULL, MXML_DESCEND);
	assert_non_null(device_id);
	n = mxmlFindElement(device_id, device_id, "Manufacturer", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(device_id, device_id, "OUI", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(device_id, device_id, "ProductClass", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(device_id, device_id, "SerialNumber", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(cwmp_inform, cwmp_inform, "Event", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "EventStruct", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "EventCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1 BOOT");
	n = mxmlFindElement(cwmp_inform, cwmp_inform, "ParameterList", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	unit_test_end_test_destruction();
	clean_config();
	session_test = NULL;
}

static void prepare_session_for_rpc_method_call(struct session *session)
{
	mxml_node_t *b;
	char *c;

	icwmp_asprintf(&c, "%s:%s", ns.soap_env, "Body");
	b = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);
	session->body_in = b;
	xml_prepare_msg_out(session);
}

static void prepare_gpv_soap_request(struct session *session, char *parameters[], int len)
{
	mxml_node_t *params = NULL, *n = NULL;
	int i;

	session->tree_in = mxmlLoadString(NULL, CWMP_GETPARAMETERVALUES_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	params = mxmlFindElement(session->tree_in, session->tree_in, "ParameterNames", NULL, NULL, MXML_DESCEND);
	for (i = 0; i < len; i++) {
		n = mxmlNewElement(params, "string");
		n = mxmlNewOpaque(n, parameters[i]);
	}
}

static void soap_get_param_value_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL, *name = NULL, *value = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid parameter path
	 */
	char *valid_parameters[1] = { "Device.ManagementServer.PeriodicInformEnable" };
	prepare_gpv_soap_request(session, valid_parameters, 1);

	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_get_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:GetParameterValuesResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "ParameterList", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "ParameterValueStruct", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	name = mxmlFindElement(n, n, "Name", NULL, NULL, MXML_DESCEND);
	assert_non_null(name);
	assert_string_equal(name->child->value.opaque, "Device.ManagementServer.PeriodicInformEnable");
	value = mxmlFindElement(n, n, "Value", NULL, NULL, MXML_DESCEND);
	assert_non_null(value);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	* Wrong parameter path
	*/
	mxml_node_t *fault_code = NULL, *fault_string = NULL, *detail = NULL, *detail_code = NULL, *detail_string = NULL;

	char *invalid_parameter[1] = { "Device.ManagementServereriodicInformEnable" };
	prepare_gpv_soap_request(session, invalid_parameter, 1);

	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_get_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	fault_code = mxmlFindElement(n, n, "faultcode", NULL, NULL, MXML_DESCEND);
	assert_non_null(fault_code);
	fault_string = mxmlFindElement(n, n, "faultstring", NULL, NULL, MXML_DESCEND);
	assert_non_null(fault_string);
	detail = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail);
	detail = mxmlFindElement(detail, detail, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail);
	detail_code = mxmlFindElement(detail, detail, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail_code);
	assert_string_equal(detail_code->child->value.opaque, "9005");
	detail_string = mxmlFindElement(detail, detail, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail_string);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	session->head_rpc_acs.next = NULL;

	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

static void prepare_spv_soap_request(struct session *session, char *parameter, char *value, char *parameter_key)
{
	mxml_node_t *params = NULL, *n = NULL;

	session->tree_in = mxmlLoadString(NULL, CWMP_SETPARAMETERVALUES_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	params = mxmlFindElement(session->tree_in, session->tree_in, "ParameterValueStruct", NULL, NULL, MXML_DESCEND);
	n = mxmlNewElement(params, "Name");
	n = mxmlNewOpaque(n, parameter);
	n = mxmlNewElement(params, "Value");
	mxmlElementSetAttr(n, "xsi:type", "xsd:string");
	n = mxmlNewOpaque(n, value);
	n = mxmlFindElement(session->tree_in, session->tree_in, "ParameterKey", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, parameter_key);
}

static void soap_set_param_value_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid path & writable parameter
	*/
	rpc_cpe->type = RPC_CPE_SET_PARAMETER_VALUES;
	prepare_spv_soap_request(session, "Device.ManagementServer.PeriodicInformEnable", "true", "set_value_test");
	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_set_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:SetParameterValuesResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "Status", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1");

	rpc_cpe->list_set_value_fault = NULL;
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	session->tree_out = NULL;

	/*
	 * Wrong parameter path
	 */
	rpc_cpe->type = RPC_CPE_SET_PARAMETER_VALUES;
	mxml_node_t *cwmp_fault = NULL, *set_val_fault = NULL;

	prepare_spv_soap_request(session, "Device.ManagementServeriodicInformEnable", "mngmt_enable", "set_value_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	set_val_fault = mxmlFindElement(cwmp_fault, cwmp_fault, "SetParameterValuesFault", NULL, NULL, MXML_DESCEND);
	assert_non_null(set_val_fault);
	n = mxmlFindElement(set_val_fault, set_val_fault, "ParameterName", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "Device.ManagementServeriodicInformEnable");
	n = mxmlFindElement(set_val_fault, set_val_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(set_val_fault, set_val_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	rpc_cpe->list_set_value_fault = NULL;
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	session->tree_in = NULL;
	session->tree_out = NULL;

	/*
	 * Not writable & Valid parameter path
	 */
	rpc_cpe->type = RPC_CPE_SET_PARAMETER_VALUES;
	prepare_spv_soap_request(session, "Device.ManagementServer.AliasBasedAddressing", "true", "set_value_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	set_val_fault = mxmlFindElement(cwmp_fault, cwmp_fault, "SetParameterValuesFault", NULL, NULL, MXML_DESCEND);
	assert_non_null(set_val_fault);
	n = mxmlFindElement(set_val_fault, set_val_fault, "ParameterName", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "Device.ManagementServer.AliasBasedAddressing");
	n = mxmlFindElement(set_val_fault, set_val_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9008");
	n = mxmlFindElement(set_val_fault, set_val_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	rpc_cpe->list_set_value_fault = NULL;
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;

	/*
	 * Invalide parameteKey
	 */
	rpc_cpe->type = RPC_CPE_SET_PARAMETER_VALUES;

	prepare_spv_soap_request(session, "Device.ManagementServer.PeriodicInformEnable", "false", INVALID_PARAM_KEY);
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);
	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	set_val_fault = mxmlFindElement(cwmp_fault, cwmp_fault, "SetParameterValuesFault", NULL, NULL, MXML_DESCEND);
	assert_null(set_val_fault);
}

static void prepare_addobj_soap_request(struct session *session, char *object, char *parameter_key)
{
	mxml_node_t *add_node = NULL, *n = NULL;

	session->tree_in = mxmlLoadString(NULL, CWMP_ADDOBJECT_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	add_node = mxmlFindElement(session->tree_in, session->tree_in, "cwmp:AddObject", NULL, NULL, MXML_DESCEND);
	n = mxmlFindElement(add_node, add_node, "ObjectName", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, object);
	n = mxmlFindElement(add_node, add_node, "ParameterKey", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, parameter_key);
}

static void soap_add_object_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL, *add_resp = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;
	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid path & writable object
	 */
	prepare_addobj_soap_request(session, "Device.WiFi.SSID.", "add_object_test");
	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_add_object(session, rpc_cpe);
	assert_int_equal(ret, 0);
	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	add_resp = mxmlFindElement(n, n, "cwmp:AddObjectResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(add_resp);
	n = mxmlFindElement(add_resp, add_resp, "InstanceNumber", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	instance = (n->child && n->child->value.opaque) ? atoi(n->child->value.opaque) : 1;
	n = mxmlFindElement(add_resp, add_resp, "Status", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1");
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Wrong object path
	 */
	mxml_node_t *cwmp_fault = NULL;

	prepare_addobj_soap_request(session, "Device.WiFi.SSI.", "add_object_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_add_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Not writable & Valid object path
	 */
	cwmp_fault = NULL;
	prepare_addobj_soap_request(session, "Device.DeviceInfo.Processor.", "add_object_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_add_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Invalid parameterkey
	 */
	cwmp_fault = NULL;
	prepare_addobj_soap_request(session, "Device.WiFi.SSID.", INVALID_PARAM_KEY);
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_add_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

static void prepare_delobj_soap_request(struct session *session, char *object, char *parameter_key)
{
	mxml_node_t *del_node = NULL, *n = NULL;

	session->tree_in = mxmlLoadString(NULL, CWMP_DELOBJECT_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	del_node = mxmlFindElement(session->tree_in, session->tree_in, "cwmp:DeleteObject", NULL, NULL, MXML_DESCEND);
	n = mxmlFindElement(del_node, del_node, "ObjectName", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, object);
	n = mxmlFindElement(del_node, del_node, "ParameterKey", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, parameter_key);
}

static void soap_delete_object_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL, *add_resp = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid path & writable object
	 */
	char del_obj[32];
	snprintf(del_obj, sizeof(del_obj), "Device.WiFi.SSID.%d.", instance);
	prepare_delobj_soap_request(session, del_obj, "del_object_test");
	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_delete_object(session, rpc_cpe);
	assert_int_equal(ret, 0);
	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	add_resp = mxmlFindElement(n, n, "cwmp:DeleteObjectResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(add_resp);
	n = mxmlFindElement(add_resp, add_resp, "Status", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1");
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Wrong object path
	 */
	mxml_node_t *cwmp_fault = NULL;

	prepare_delobj_soap_request(session, "Device.WiFi.SID.1", "del_object_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_delete_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Not writable & Valid object path
	 */
	cwmp_fault = NULL;
	prepare_delobj_soap_request(session, "Device.DeviceInfo.Processor.2.", "del_object_test");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_delete_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Invalid parameterkey
	 */
	cwmp_fault = NULL;
	prepare_delobj_soap_request(session, "Device.WiFi.SSID.1.", INVALID_PARAM_KEY);
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_delete_object(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

static void prepare_gpa_soap_request(struct session *session, char *parameter)
{
	mxml_node_t *n = NULL;

	session->tree_in = mxmlLoadString(NULL, CWMP_GETATTRIBUTES_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	n = mxmlFindElement(session->tree_in, session->tree_in, "cwmp:GetParameterAttributes", NULL, NULL, MXML_DESCEND);
	n = mxmlFindElement(n, n, "ParameterNames", NULL, NULL, MXML_DESCEND);
	n = mxmlNewElement(n, "string");
	n = mxmlNewOpaque(n, parameter);
}

static void soap_get_parameter_attributes_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL, *param_attr = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid path
	 */
	prepare_gpa_soap_request(session, "Device.DeviceInfo.UpTime");
	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_get_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:GetParameterAttributesResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "ParameterList", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	param_attr = mxmlFindElement(n, n, "ParameterAttributeStruct", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(param_attr, param_attr, "Name", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "Device.DeviceInfo.UpTime");
	n = mxmlFindElement(param_attr, param_attr, "Notification", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(param_attr, param_attr, "AccessList", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Not Valid path
	 */
	mxml_node_t *fault_code = NULL, *fault_string = NULL, *detail = NULL, *detail_code = NULL, *detail_string = NULL;

	char *invalid_parameter[1] = { "Device.DeviceInfo.pTime" };
	prepare_gpv_soap_request(session, invalid_parameter, 1);

	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_get_parameter_values(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	fault_code = mxmlFindElement(n, n, "faultcode", NULL, NULL, MXML_DESCEND);
	assert_non_null(fault_code);
	fault_string = mxmlFindElement(n, n, "faultstring", NULL, NULL, MXML_DESCEND);
	assert_non_null(fault_string);
	detail = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail);
	detail = mxmlFindElement(detail, detail, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail);
	detail_code = mxmlFindElement(detail, detail, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail_code);
	assert_string_equal(detail_code->child->value.opaque, "9005");
	detail_string = mxmlFindElement(detail, detail, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(detail_string);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

static void prepare_spa_soap_request(struct session *session, char *parameter, char *notification, char *notification_change)
{
	mxml_node_t *n = NULL, *set_attr = NULL;

	session->tree_in = mxmlLoadString(NULL, CWMP_SETATTRIBUTES_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	set_attr = mxmlFindElement(session->tree_in, session->tree_in, "SetParameterAttributesStruct", NULL, NULL, MXML_DESCEND);
	n = mxmlFindElement(set_attr, set_attr, "Name", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, parameter);
	n = mxmlFindElement(set_attr, set_attr, "Notification", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, notification);
	n = mxmlFindElement(set_attr, set_attr, "NotificationChange", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, notification_change);
}

static void soap_set_parameter_attributes_message_test(void **state)
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	 * Valid path
	 */
	prepare_spa_soap_request(session, "Device.DeviceInfo.UpTime", "1", "true");
	prepare_session_for_rpc_method_call(session);

	int ret = cwmp_handle_rpc_cpe_set_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:SetParameterAttributesResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_null(n->child);
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Not Valid path
	 */
	mxml_node_t *cwmp_fault = NULL;
	prepare_spa_soap_request(session, "Device.DeviceInfo.pTime", "1", "true");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9005");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Not Valid Notification value
	 */
	prepare_spa_soap_request(session, "Device.DeviceInfo.UpTime", "8", "true");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Invalid Notification String
	 */

	prepare_spa_soap_request(session, "Device.DeviceInfo.UpTime", "balabala", "true");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	/*
	 * Invalid NotificationChange
	 */

	prepare_spa_soap_request(session, "Device.DeviceInfo.UpTime", "2", "5");
	prepare_session_for_rpc_method_call(session);

	ret = cwmp_handle_rpc_cpe_set_parameter_attributes(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

static void prepare_download_soap_request(struct session *session, char *url, char *file_type, char *username, char *password, char *file_size, char *delay_second)
{
	mxml_node_t *n = NULL, *download = NULL;
	session->tree_in = mxmlLoadString(NULL, CWMP_DOWNLOAD_REQ, MXML_OPAQUE_CALLBACK);
	xml_recreate_namespace(session->tree_in);
	download = mxmlFindElement(session->tree_in, session->tree_in, "cwmp:Download", NULL, NULL, MXML_DESCEND);
	n = mxmlFindElement(download, download, "URL", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, url);
	n = mxmlFindElement(download, download, "FileType", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, file_type);
	n = mxmlFindElement(download, download, "Username", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, username);
	n = mxmlFindElement(download, download, "Password", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, password);
	n = mxmlFindElement(download, download, "FileSize", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, file_size);
	n = mxmlFindElement(download, download, "DelaySeconds", NULL, NULL, MXML_DESCEND);
	n = mxmlNewOpaque(n, delay_second);
}

void free_download()
{
	if (list_download.next != &(list_download)) {
		struct list_head *ilist, *q;
		list_for_each_safe (ilist, q, &(list_download)) {
			struct download *download;
			download = list_entry(ilist, struct download, list);
			bkp_session_delete_download(download);
			cwmp_free_download_request(download);
		}
	}
}

static void soap_download_message_test(void **state)          //TODO will be properly done with Download unit tests
{
	struct session *session = NULL;
	mxml_node_t *env = NULL, *n = NULL, *download_node = NULL;
	struct rpc *rpc_cpe;

	create_session(&session);
	session_test = session;

	rpc_cpe = list_entry(&(session->head_rpc_cpe), struct rpc, list);

	/*
	* Valid Arguments
	*/
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "6 Stored Firmware Image", "iopsys", "iopsys", "0", "0");
	prepare_session_for_rpc_method_call(session);
	int ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);
	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	download_node = mxmlFindElement(n, n, "cwmp:DownloadResponse", NULL, NULL, MXML_DESCEND);
	assert_non_null(download_node);
	n = mxmlFindElement(download_node, download_node, "Status", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1");
	n = mxmlFindElement(download_node, download_node, "StartTime", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(download_node, download_node, "CompleteTime", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	/*
	 * wrong FileType
	 */

	mxml_node_t *cwmp_fault = NULL;
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "7 New FileType", "iopsys", "iopsys", "0", "0");
	prepare_session_for_rpc_method_call(session);
	ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);


	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	/*
	 * Wrong FileSize
	 */
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "6 Stored Firmware Image", "iopsys", "iopsys", "-15", "0");
	prepare_session_for_rpc_method_call(session);
	ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);


	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;

	/*
	 * Wrong DelaySeconds
	 */
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "6 Stored Firmware Image", "iopsys", "iopsys", "0", "jjjk");
	prepare_session_for_rpc_method_call(session);
	ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);


	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	/*
	 * Invalid username
	 */
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "6 Stored Firmware Image", INVALID_USER, "iopsys", "0", "0");
	prepare_session_for_rpc_method_call(session);
	ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);


	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	/*
	 * Invalid password
	 */
	prepare_download_soap_request(session, "http://192.168.1.160/tr069/DG400PRIME-A-IOPSYS-6.2.0BETA1-210530_2348_nand_squashfs_update.pkgtb", "6 Stored Firmware Image", "iopsys", INVALID_PASS, "0", "0");
	prepare_session_for_rpc_method_call(session);
	ret = cwmp_handle_rpc_cpe_download(session, rpc_cpe);
	assert_int_equal(ret, 0);

	env = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	assert_non_null(env);
	n = mxmlFindElement(env, env, "soap_env:Header", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(env, env, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);


	n = mxmlFindElement(n, n, "soap_env:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(n, n, "detail", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	cwmp_fault = mxmlFindElement(n, n, "cwmp:Fault", NULL, NULL, MXML_DESCEND);
	assert_non_null(cwmp_fault);
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultCode", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "9003");
	n = mxmlFindElement(cwmp_fault, cwmp_fault, "FaultString", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);

	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	free_download();
	bkp_tree_clean();

	session->head_rpc_acs.next = NULL;
	unit_test_end_test_destruction();
	clean_name_space();
	session_test = NULL;
}

int main(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(get_config_test), cmocka_unit_test(get_deviceid_test),
		    cmocka_unit_test(add_event_test),
		    cmocka_unit_test(soap_inform_message_test),
		    cmocka_unit_test(soap_get_param_value_message_test),
		    //cmocka_unit_test(soap_set_param_value_message_test),
		    cmocka_unit_test(soap_add_object_message_test),
		    cmocka_unit_test(soap_delete_object_message_test),
		    cmocka_unit_test(soap_get_parameter_attributes_message_test),
		    cmocka_unit_test(soap_set_parameter_attributes_message_test),
		    //cmocka_unit_test(soap_download_message_test) //TODO will be properly done with Download unit tests
	};

	return cmocka_run_group_tests(tests, soap_unit_tests_init, soap_unit_tests_clean);
}
