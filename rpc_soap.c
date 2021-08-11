/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 *	  Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 *	  Copyright (C) 2012 Jonas Gorski <jonas.gorski@gmail.com>
 */
#include <stdbool.h>

#include "rpc_soap.h"

#include "http.h"
#include "cwmp_time.h"
#include "cwmp_zlib.h"
#include "notifications.h"
#include "messages.h"
#include "backupSession.h"
#include "log.h"
#include "datamodel_interface.h"
#include "cwmp_uci.h"
#include "diagnostic.h"
#include "ubus.h"
#include "cwmp_du_state.h"
#include "download.h"
#include "upload.h"
#include "sched_inform.h"

const struct rpc_cpe_method rpc_cpe_methods[] = { [RPC_CPE_GET_RPC_METHODS] = { "GetRPCMethods", cwmp_handle_rpc_cpe_get_rpc_methods, AMD_1 },
						  [RPC_CPE_SET_PARAMETER_VALUES] = { "SetParameterValues", cwmp_handle_rpc_cpe_set_parameter_values, AMD_1 },
						  [RPC_CPE_GET_PARAMETER_VALUES] = { "GetParameterValues", cwmp_handle_rpc_cpe_get_parameter_values, AMD_1 },
						  [RPC_CPE_GET_PARAMETER_NAMES] = { "GetParameterNames", cwmp_handle_rpc_cpe_get_parameter_names, AMD_1 },
						  [RPC_CPE_SET_PARAMETER_ATTRIBUTES] = { "SetParameterAttributes", cwmp_handle_rpc_cpe_set_parameter_attributes, AMD_1 },
						  [RPC_CPE_GET_PARAMETER_ATTRIBUTES] = { "GetParameterAttributes", cwmp_handle_rpc_cpe_get_parameter_attributes, AMD_1 },
						  [RPC_CPE_ADD_OBJECT] = { "AddObject", cwmp_handle_rpc_cpe_add_object, AMD_1 },
						  [RPC_CPE_DELETE_OBJECT] = { "DeleteObject", cwmp_handle_rpc_cpe_delete_object, AMD_1 },
						  [RPC_CPE_REBOOT] = { "Reboot", cwmp_handle_rpc_cpe_reboot, AMD_1 },
						  [RPC_CPE_DOWNLOAD] = { "Download", cwmp_handle_rpc_cpe_download, AMD_1 },
						  [RPC_CPE_UPLOAD] = { "Upload", cwmp_handle_rpc_cpe_upload, AMD_1 },
						  [RPC_CPE_FACTORY_RESET] = { "FactoryReset", cwmp_handle_rpc_cpe_factory_reset, AMD_1 },
						  [RPC_CPE_CANCEL_TRANSFER] = { "CancelTransfer", cwmp_handle_rpc_cpe_cancel_transfer, AMD_3 },
						  [RPC_CPE_SCHEDULE_INFORM] = { "ScheduleInform", cwmp_handle_rpc_cpe_schedule_inform, AMD_1 },
						  [RPC_CPE_SCHEDULE_DOWNLOAD] = { "ScheduleDownload", cwmp_handle_rpc_cpe_schedule_download, AMD_3 },
						  [RPC_CPE_CHANGE_DU_STATE] = { "ChangeDUState", cwmp_handle_rpc_cpe_change_du_state, AMD_3 },
						  [RPC_CPE_X_FACTORY_RESET_SOFT] = { "X_FactoryResetSoft", cwmp_handle_rpc_cpe_x_factory_reset_soft, AMD_1 },
						  [RPC_CPE_FAULT] = { "Fault", cwmp_handle_rpc_cpe_fault, AMD_1 } };

const struct rpc_acs_method rpc_acs_methods[] = { [RPC_ACS_INFORM] = { "Inform", cwmp_rpc_acs_prepare_message_inform, cwmp_rpc_acs_parse_response_inform, cwmp_rpc_acs_destroy_data_inform },
						  [RPC_ACS_GET_RPC_METHODS] = { "GetRPCMethods", cwmp_rpc_acs_prepare_get_rpc_methods, NULL, NULL },
						  [RPC_ACS_TRANSFER_COMPLETE] = { "TransferComplete", cwmp_rpc_acs_prepare_transfer_complete, NULL, cwmp_rpc_acs_destroy_data_transfer_complete },
						  [RPC_ACS_DU_STATE_CHANGE_COMPLETE] = { "DUStateChangeComplete", cwmp_rpc_acs_prepare_du_state_change_complete, NULL, cwmp_rpc_acs_destroy_data_du_state_change_complete }

};

char *custom_forced_inform_parameters[MAX_NBRE_CUSTOM_INFORM] = { 0 };
int nbre_custom_inform = 0;
char *forced_inform_parameters[] = {
	"Device.RootDataModelVersion", "Device.DeviceInfo.HardwareVersion", "Device.DeviceInfo.SoftwareVersion", "Device.DeviceInfo.ProvisioningCode", "Device.ManagementServer.ParameterKey", "Device.ManagementServer.ConnectionRequestURL", "Device.ManagementServer.AliasBasedAddressing"
};

int xml_handle_message(struct session *session)
{
	struct rpc *rpc_cpe;
	char *c;
	int i;
	mxml_node_t *b;
	struct cwmp *cwmp = &cwmp_main;
	struct config *conf;
	conf = &(cwmp->conf);

	/* get method */

	if (icwmp_asprintf(&c, "%s:%s", ns.soap_env, "Body") == -1) {
		CWMP_LOG(INFO, "Internal error");
		session->fault_code = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	b = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!b) {
		CWMP_LOG(INFO, "Invalid received message");
		session->fault_code = FAULT_CPE_REQUEST_DENIED;
		goto fault;
	}
	session->body_in = b;

	while (1) {
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND_FIRST);
		if (!b)
			goto error;
		if (b->type == MXML_ELEMENT)
			break;
	}

	c = b->value.element.name;
	/* convert QName to localPart, check that ns is the expected one */
	if (strchr(c, ':')) {
		char *tmp = strchr(c, ':');
		size_t ns_len = tmp - c;

		if (strlen(ns.cwmp) != ns_len) {
			CWMP_LOG(INFO, "Invalid received message");
			session->fault_code = FAULT_CPE_REQUEST_DENIED;
			goto fault;
		}

		if (strncmp(ns.cwmp, c, ns_len)) {
			CWMP_LOG(INFO, "Invalid received message");
			session->fault_code = FAULT_CPE_REQUEST_DENIED;
			goto fault;
		}

		c = tmp + 1;
	} else {
		CWMP_LOG(INFO, "Invalid received message");
		session->fault_code = FAULT_CPE_REQUEST_DENIED;
		goto fault;
	}
	CWMP_LOG(INFO, "SOAP RPC message: %s", c);
	rpc_cpe = NULL;
	for (i = 1; i < __RPC_CPE_MAX; i++) {
		if (i != RPC_CPE_FAULT && strcmp(c, rpc_cpe_methods[i].name) == 0 && rpc_cpe_methods[i].amd <= conf->amd_version) {
			CWMP_LOG(INFO, "%s RPC is supported", c);
			rpc_cpe = cwmp_add_session_rpc_cpe(session, i);
			if (rpc_cpe == NULL)
				goto error;
			break;
		}
	}
	if (!rpc_cpe) {
		CWMP_LOG(INFO, "%s RPC is not supported", c);
		session->fault_code = FAULT_CPE_METHOD_NOT_SUPPORTED;
		goto fault;
	}
	return 0;
fault:
	rpc_cpe = cwmp_add_session_rpc_cpe(session, RPC_CPE_FAULT);
	if (rpc_cpe == NULL)
		goto error;
	return 0;
error:
	return -1;
}

/*
 * [RPC ACS]: Inform
 */

static int xml_prepare_events_inform(struct session *session, mxml_node_t *tree)
{
	mxml_node_t *node, *b1, *b2;
	char c[128];
	int n = 0;
	struct list_head *ilist;
	struct event_container *event_container;

	b1 = mxmlFindElement(tree, tree, "Event", NULL, NULL, MXML_DESCEND);
	if (!b1)
		return -1;

	list_for_each (ilist, &(session->head_event_container)) {
		event_container = list_entry(ilist, struct event_container, list);
		node = mxmlNewElement(b1, "EventStruct");
		if (!node)
			goto error;
		b2 = mxmlNewElement(node, "EventCode");
		if (!b2)
			goto error;
		b2 = mxmlNewOpaque(b2, EVENT_CONST[event_container->code].CODE);
		if (!b2)
			goto error;
		b2 = mxmlNewElement(node, "CommandKey");
		if (!b2)
			goto error;
		if (event_container->command_key) {
			b2 = mxmlNewOpaque(b2, event_container->command_key);
			if (!b2)
				goto error;
		}
		mxmlAdd(b1, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
		n++;
	}
	if (n) {
		if (snprintf(c, sizeof(c), "cwmp:EventStruct[%u]", n) == -1)
			return -1;
		mxmlElementSetAttr(b1, "xsi:type", "soap_enc:Array");
		mxmlElementSetAttr(b1, "soap_enc:arrayType", c);
	}
	return 0;

error:
	return -1;
}

static int xml_prepare_parameters_inform(struct cwmp_dm_parameter *dm_parameter, mxml_node_t *parameter_list, int *size)
{
	mxml_node_t *node, *b;
	b = mxmlFindElementOpaque(parameter_list, parameter_list, dm_parameter->name, MXML_DESCEND);
	if (b && dm_parameter->value != NULL) {
		node = b->parent->parent;
		b = mxmlFindElement(node, node, "Value", NULL, NULL, MXML_DESCEND_FIRST);
		if (!b)
			return 0;
		if (b->child && strcmp(dm_parameter->value, b->child->value.opaque) == 0)
			return 0;
		mxmlDelete(b);
		(*size)--;

		goto create_value;
	} else if (dm_parameter->value == NULL)
		return 0;
	node = mxmlNewElement(parameter_list, "ParameterValueStruct");
	if (!node)
		return -1;

	b = mxmlNewElement(node, "Name");
	if (!b)
		return -1;

	b = mxmlNewOpaque(b, dm_parameter->name);
	if (!b)
		return -1;

create_value:
	b = mxmlNewElement(node, "Value");
	if (!b)
		return -1;

#ifdef ACS_MULTI
	mxmlElementSetAttr(b, "xsi:type", (dm_parameter->type && dm_parameter->type[0] != '\0') ? dm_parameter->type : "xsd:string");
#endif
	b = mxmlNewOpaque(b, dm_parameter->value);
	if (!b)
		return -1;

	(*size)++;
	return 0;
}

int cwmp_rpc_acs_prepare_message_inform(struct cwmp *cwmp, struct session *session, struct rpc *this)
{
	struct cwmp_dm_parameter *dm_parameter;
	struct event_container *event_container;
	mxml_node_t *tree, *b, *node, *parameter_list;
	char c[256];
	int size = 0;
	struct list_head *ilist, *jlist;

	if (session == NULL || this == NULL)
		return -1;

#ifdef DUMMY_MODE
	FILE *fp;
	fp = fopen("./ext/soap_msg_templates/cwmp_inform_message.xml", "r");
	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
#else
	tree = mxmlLoadString(NULL, CWMP_INFORM_MESSAGE, MXML_OPAQUE_CALLBACK);
#endif
	if (!tree)
		goto error;
	b = mxmlFindElement(tree, tree, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	mxmlElementSetAttr(b, "xmlns:cwmp", cwmp_urls[(cwmp->conf.supported_amd_version) - 1]);
	if (cwmp->conf.supported_amd_version >= 4) {
		b = mxmlFindElement(tree, tree, "soap_env:Header", NULL, NULL, MXML_DESCEND);
		if (!b)
			goto error;
		node = mxmlNewElement(b, "cwmp:SessionTimeout");
		if (!node)
			goto error;
		mxmlElementSetAttr(node, "soap_env:mustUnderstand", "0");
		node = mxmlNewInteger(node, cwmp->conf.session_timeout);
		if (!node)
			goto error;
	}
	if (cwmp->conf.supported_amd_version >= 5) {
		node = mxmlNewElement(b, "cwmp:SupportedCWMPVersions");
		if (!node)
			goto error;
		mxmlElementSetAttr(node, "soap_env:mustUnderstand", "0");
		node = mxmlNewOpaque(node, xml_get_cwmp_version(cwmp->conf.supported_amd_version));
		if (!node)
			goto error;
	}
	b = mxmlFindElement(tree, tree, "RetryCount", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewInteger(b, cwmp->retry_count_session);
	if (!b)
		goto error;

	if (xml_prepare_events_inform(session, tree))
		goto error;

	b = mxmlFindElement(tree, tree, "CurrentTime", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewOpaque(b, mix_get_time());
	if (!b)
		goto error;

	parameter_list = mxmlFindElement(tree, tree, "ParameterList", NULL, NULL, MXML_DESCEND);
	if (!parameter_list)
		goto error;

	list_for_each (ilist, &(session->head_event_container)) {
		event_container = list_entry(ilist, struct event_container, list);
		list_for_each (jlist, &(event_container->head_dm_parameter)) {
			dm_parameter = list_entry(jlist, struct cwmp_dm_parameter, list);
			if (xml_prepare_parameters_inform(dm_parameter, parameter_list, &size))
				goto error;
		}
	}

	b = mxmlFindElement(tree, tree, "OUI", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlNewOpaque(b, cwmp->deviceid.oui ? cwmp->deviceid.oui : "");
	if (!b)
		goto error;

	b = mxmlFindElement(tree, tree, "Manufacturer", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlNewOpaque(b, cwmp->deviceid.manufacturer ? cwmp->deviceid.manufacturer : "");
	if (!b)
		goto error;

	b = mxmlFindElement(tree, tree, "ProductClass", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlNewOpaque(b, cwmp->deviceid.productclass ? cwmp->deviceid.productclass : "");
	if (!b)
		goto error;

	b = mxmlFindElement(tree, tree, "SerialNumber", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlNewOpaque(b, cwmp->deviceid.serialnumber ? cwmp->deviceid.serialnumber : "");
	if (!b)
		goto error;

	size_t inform_parameters_nbre = sizeof(forced_inform_parameters) / sizeof(forced_inform_parameters[0]);
	size_t i;
	struct cwmp_dm_parameter cwmp_dm_param = { 0 };
	LIST_HEAD(list_inform);
	for (i = 0; i < inform_parameters_nbre; i++) {
		char *fault = cwmp_get_single_parameter_value(forced_inform_parameters[i], &cwmp_dm_param);
		if (fault != NULL)
			continue;
		if (xml_prepare_parameters_inform(&cwmp_dm_param, parameter_list, &size))
			goto error;
	}

	for (i = 0; i < nbre_custom_inform; i++) {
		char *fault = cwmp_get_single_parameter_value(custom_forced_inform_parameters[i], &cwmp_dm_param);
		if (fault != NULL)
			continue;
		if (xml_prepare_parameters_inform(&cwmp_dm_param, parameter_list, &size))
			goto error;
	}

	if (snprintf(c, sizeof(c), "cwmp:ParameterValueStruct[%d]", size) == -1)
		goto error;

	mxmlElementSetAttr(parameter_list, "xsi:type", "soap_enc:Array");
	mxmlElementSetAttr(parameter_list, "soap_enc:arrayType", c);

	session->tree_out = tree;

	return 0;

error:
	CWMP_LOG(ERROR, "Unable Prepare Message Inform", CWMP_BKP_FILE);
	return -1;
}

int cwmp_rpc_acs_parse_response_inform(struct cwmp *cwmp, struct session *session, struct rpc *this __attribute__((unused)))
{
	mxml_node_t *tree, *b;
	int i = -1;
	char *c;
	const char *cwmp_urn;

	tree = session->tree_in;
	if (!tree)
		goto error;
	b = mxmlFindElement(tree, tree, "MaxEnvelopes", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlWalkNext(b, tree, MXML_DESCEND_FIRST);
	if (!b || b->type != MXML_OPAQUE || !b->value.opaque)
		goto error;
	if (cwmp->conf.supported_amd_version == 1) {
		cwmp->conf.amd_version = 1;
		return 0;
	}
	b = mxmlFindElement(tree, tree, "UseCWMPVersion", NULL, NULL, MXML_DESCEND);
	if (b && cwmp->conf.supported_amd_version >= 5) { //IF supported version !=5 acs response dosen't contain UseCWMPVersion
		b = mxmlWalkNext(b, tree, MXML_DESCEND_FIRST);
		if (!b || b->type != MXML_OPAQUE || !b->value.opaque)
			goto error;
		c = (char *)(b->value.opaque);
		if (c && *(c + 1) == '.') {
			c += 2;
			cwmp->conf.amd_version = atoi(c) + 1;
			return 0;
		}
		goto error;
	}
	for (i = 0; cwmp_urls[i] != NULL; i++) {
		cwmp_urn = cwmp_urls[i];
		c = (char *)mxmlElementGetAttrName(tree, cwmp_urn);
		if (c && *(c + 5) == ':') {
			break;
		}
	}
	if (i == 0) {
		cwmp->conf.amd_version = i + 1;
	} else if (i >= 1 && i <= 3) {
		switch (cwmp->conf.supported_amd_version) {
		case 1:
			cwmp->conf.amd_version = 1; //Already done
			break;
		case 2:
		case 3:
		case 4:
			//MIN ACS CPE
			if (cwmp->conf.supported_amd_version <= i + 1)
				cwmp->conf.amd_version = cwmp->conf.supported_amd_version;
			else
				cwmp->conf.amd_version = i + 1;
			break;
		//(cwmp->supported_conf.amd_version < i+1) ?"cwmp->conf.amd_version":"i+1";
		case 5:
			cwmp->conf.amd_version = i + 1;
			break;
		}
	} else if (i >= 4) {
		cwmp->conf.amd_version = cwmp->conf.supported_amd_version;
	}
	return 0;

error:
	return -1;
}

int cwmp_rpc_acs_destroy_data_inform(struct session *session __attribute__((unused)), struct rpc *rpc __attribute__((unused)))
{
	//event_remove_all_event_container(session,RPC_SEND);
	return 0;
}

/*
 * [RPC ACS]: GetRPCMethods
 */

int cwmp_rpc_acs_prepare_get_rpc_methods(struct cwmp *cwmp, struct session *session, struct rpc *rpc __attribute__((unused)))
{
	mxml_node_t *tree, *n;

	tree = mxmlLoadString(NULL, CWMP_RESPONSE_MESSAGE, MXML_OPAQUE_CALLBACK);

	n = mxmlFindElement(tree, tree, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	if (!n)
		return -1;
	mxmlElementSetAttr(n, "xmlns:cwmp", cwmp_urls[(cwmp->conf.amd_version) - 1]);
	n = mxmlFindElement(tree, tree, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		return -1;

	n = mxmlNewElement(n, "cwmp:GetRPCMethods");
	if (!n)
		return -1;

	session->tree_out = tree;

	return 0;
}

int cwmp_rpc_acs_parse_response_get_rpc_methods(struct session *session)
{
	mxml_node_t *tree, *b;

	tree = session->tree_in;
	if (!tree)
		goto error;
	b = mxmlFindElement(tree, tree, "MethodList", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;
	b = mxmlWalkNext(b, tree, MXML_DESCEND_FIRST);
	if (!b || b->type != MXML_OPAQUE || !b->value.opaque)
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC ACS]: TransferComplete
 */

int cwmp_rpc_acs_prepare_transfer_complete(struct cwmp *cwmp, struct session *session, struct rpc *rpc)
{
	mxml_node_t *tree, *n;
	struct transfer_complete *p;

	p = (struct transfer_complete *)rpc->extra_data;
	tree = mxmlLoadString(NULL, CWMP_RESPONSE_MESSAGE, MXML_OPAQUE_CALLBACK);
	n = mxmlFindElement(tree, tree, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto error;
	mxmlElementSetAttr(n, "xmlns:cwmp", cwmp_urls[(cwmp->conf.amd_version) - 1]);

	n = mxmlFindElement(tree, tree, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto error;

	n = mxmlNewElement(n, "cwmp:TransferComplete");
	if (!n)
		goto error;

	n = mxmlNewElement(n, "CommandKey");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, p->command_key ? p->command_key : "");
	if (!n)
		goto error;

	n = n->parent->parent;
	n = mxmlNewElement(n, "StartTime");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, p->start_time);
	if (!n)
		goto error;

	n = n->parent->parent;
	n = mxmlNewElement(n, "CompleteTime");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, mix_get_time());
	if (!n)
		goto error;

	n = n->parent->parent;
	n = mxmlNewElement(n, "FaultStruct");
	if (!n)
		goto error;

	n = mxmlNewElement(n, "FaultCode");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, p->fault_code ? FAULT_CPE_ARRAY[p->fault_code].CODE : "0");
	if (!n)
		goto error;

	n = n->parent->parent;
	n = mxmlNewElement(n, "FaultString");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, p->fault_code ? FAULT_CPE_ARRAY[p->fault_code].DESCRIPTION : "");
	if (!n)
		goto error;

	session->tree_out = tree;

	return 0;

error:
	return -1;
}

/*
 * [RPC ACS]: DUStateChangeComplete
 */

int cwmp_rpc_acs_prepare_du_state_change_complete(struct cwmp *cwmp, struct session *session, struct rpc *rpc)
{
	mxml_node_t *tree, *n, *b, *t;
	struct du_state_change_complete *p;
	struct opresult *q;
	char *c;

	p = (struct du_state_change_complete *)rpc->extra_data;
	tree = mxmlLoadString(NULL, CWMP_RESPONSE_MESSAGE, MXML_OPAQUE_CALLBACK);
	n = mxmlFindElement(tree, tree, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto error;

	mxmlElementSetAttr(n, "xmlns:cwmp", cwmp_urls[(cwmp->conf.amd_version) - 1]);
	n = mxmlFindElement(tree, tree, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto error;

	n = mxmlNewElement(n, "cwmp:DUStateChangeComplete");
	if (!n)
		goto error;

	n = mxmlNewElement(n, "CommandKey");
	if (!n)
		goto error;

	n = mxmlNewOpaque(n, p->command_key);
	if (!n)
		goto error;
	n = n->parent->parent;
	n = mxmlNewElement(n, "Results");
	if (!n)
		goto error;

	list_for_each_entry (q, &(p->list_opresult), list) {
		t = mxmlNewElement(n, "OpResultStruct");
		if (!t)
			goto error;

		b = mxmlNewElement(t, "UUID");
		if (!b)
			goto error;

		c = q->uuid ? strdup(q->uuid) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "DeploymentUnitRef");
		if (!b)
			goto error;

		c = q->du_ref ? strdup(q->du_ref) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "Version");
		if (!b)
			goto error;

		c = q->version ? strdup(q->version) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "CurrentState");
		if (!b)
			goto error;

		c = q->current_state ? strdup(q->current_state) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "StartTime");
		if (!b)
			goto error;

		c = q->start_time ? strdup(q->start_time) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "CompleteTime");
		if (!b)
			goto error;

		c = q->complete_time ? strdup(q->complete_time) : strdup("");
		b = mxmlNewOpaque(b, c);
		FREE(c);
		if (!b)
			goto error;

		b = mxmlNewElement(t, "FaultStruct");
		if (!b)
			goto error;

		b = mxmlNewElement(b, "FaultCode");
		if (!b)
			goto error;

		b = mxmlNewOpaque(b, q->fault ? FAULT_CPE_ARRAY[q->fault].CODE : "0");
		if (!b)
			goto error;

		b = b->parent->parent;
		b = mxmlNewElement(b, "FaultString");
		if (!b)
			goto error;

		b = mxmlNewOpaque(b, q->fault ? FAULT_CPE_ARRAY[q->fault].DESCRIPTION : "");
		if (!b)
			goto error;
	}
	session->tree_out = tree;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: GetParameterValues
 */

int cwmp_handle_rpc_cpe_get_parameter_values(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *parameter_list, *b;
	char *parameter_name = NULL;
	int counter = 0, fault_code = FAULT_CPE_INTERNAL_ERROR;
#ifdef ACS_MULTI
	char c[256];
#endif

	b = session->body_in;
	n = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto fault;

	n = mxmlNewElement(n, "cwmp:GetParameterValuesResponse");
	if (!n)
		goto fault;

	parameter_list = mxmlNewElement(n, "ParameterList");
	if (!parameter_list)
		goto fault;
#ifdef ACS_MULTI
	mxmlElementSetAttr(parameter_list, "xsi:type", "soap_enc:Array");
#endif
	LIST_HEAD(parameters_list);
	while (b) {
		if (b && b->type == MXML_OPAQUE && b->parent->type == MXML_ELEMENT && b->value.opaque && !strcmp(b->parent->value.element.name, "string")) {
			parameter_name = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && /* added in order to support GetParameterValues with empty string*/
		    !strcmp(b->value.element.name, "string") && !b->child) {
			parameter_name = "";
		}
		if (parameter_name) {
			char *err = cwmp_get_parameter_values(parameter_name, &parameters_list);
			if (err) {
				fault_code = cwmp_get_fault_code_by_string(err);
				goto fault;
			}
			struct cwmp_dm_parameter *param_value = NULL;
			list_for_each_entry (param_value, &parameters_list, list) {
				n = mxmlNewElement(parameter_list, "ParameterValueStruct");
				if (!n)
					goto fault;

				n = mxmlNewElement(n, "Name");
				if (!n)
					goto fault;

				n = mxmlNewOpaque(n, param_value->name);
				if (!n)
					goto fault;

				n = n->parent->parent;
				n = mxmlNewElement(n, "Value");
				if (!n)
					goto fault;

#ifdef ACS_MULTI
				mxmlElementSetAttr(n, "xsi:type", param_value->type);
#endif

				n = mxmlNewOpaque(n, param_value->value);
				if (!n)
					goto fault;

				counter++;
			}
			cwmp_free_all_dm_parameter_list(&parameters_list);
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
		parameter_name = NULL;
	}

#ifdef ACS_MULTI
	b = mxmlFindElement(session->tree_out, session->tree_out, "ParameterList", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	if (snprintf(c, sizeof(c), "cwmp:ParameterValueStruct[%d]", counter) == -1)
		goto fault;

	mxmlElementSetAttr(b, "soap_enc:arrayType", c);
#endif

	return 0;

fault:
	cwmp_free_all_dm_parameter_list(&parameters_list);
	if (cwmp_create_fault_message(session, rpc, fault_code))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: GetParameterNames
 */
int cwmp_handle_rpc_cpe_get_parameter_names(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *parameter_list, *b = session->body_in;
	char *parameter_name = NULL;
	char *NextLevel = NULL;
	int counter = 0, fault_code = FAULT_CPE_INTERNAL_ERROR;
	LIST_HEAD(parameters_list);
#ifdef ACS_MULTI
	char c[256];
#endif
	n = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto fault;

	n = mxmlNewElement(n, "cwmp:GetParameterNamesResponse");
	if (!n)
		goto fault;

	parameter_list = mxmlNewElement(n, "ParameterList");
	if (!parameter_list)
		goto fault;

#ifdef ACS_MULTI
	mxmlElementSetAttr(parameter_list, "xsi:type", "soap_enc:Array");
#endif

	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "ParameterPath")) {
			parameter_name = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "ParameterPath") && !b->child) {
			parameter_name = "";
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "NextLevel")) {
			NextLevel = b->value.opaque;
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}

	if (!icwmp_validate_boolean_value(NextLevel)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}

	if (parameter_name && NextLevel) {
		char *err = cwmp_get_parameter_names(parameter_name, strcmp(NextLevel, "true") == 0 || strcmp(NextLevel, "1") == 0 ? true : false, &parameters_list);
		if (err) {
			fault_code = cwmp_get_fault_code_by_string(err);
			goto fault;
		}
	}
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &parameters_list, list) {
		n = mxmlNewElement(parameter_list, "ParameterInfoStruct");
		if (!n)
			goto fault;

		n = mxmlNewElement(n, "Name");
		if (!n)
			goto fault;

		n = mxmlNewOpaque(n, param_value->name);
		if (!n)
			goto fault;

		n = n->parent->parent;
		n = mxmlNewElement(n, "Writable");
		if (!n)
			goto fault;

		n = mxmlNewOpaque(n, param_value->writable ? "1" : "0");
		if (!n)
			goto fault;

		counter++;
	}
	cwmp_free_all_dm_parameter_list(&parameters_list);
#ifdef ACS_MULTI
	b = mxmlFindElement(session->tree_out, session->tree_out, "ParameterList", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	if (snprintf(c, sizeof(c), "cwmp:ParameterInfoStruct[%d]", counter) == -1)
		goto fault;

	mxmlElementSetAttr(b, "soap_enc:arrayType", c);
#endif
	return 0;

fault:
	cwmp_free_all_dm_parameter_list(&parameters_list);
	if (cwmp_create_fault_message(session, rpc, fault_code))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: GetParameterAttributes
 */

int cwmp_handle_rpc_cpe_get_parameter_attributes(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *parameter_list, *b;
	char *parameter_name = NULL;
	int counter = 0, fault_code = FAULT_CPE_INTERNAL_ERROR;
	LIST_HEAD(parameters_list);
#ifdef ACS_MULTI
	char c[256];
#endif
	b = session->body_in;
	n = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto fault;

	n = mxmlNewElement(n, "cwmp:GetParameterAttributesResponse");
	if (!n)
		goto fault;

	parameter_list = mxmlNewElement(n, "ParameterList");
	if (!parameter_list)
		goto fault;

#ifdef ACS_MULTI
	mxmlElementSetAttr(parameter_list, "xsi:type", "soap_enc:Array");
#endif

	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "string")) {
			parameter_name = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "string") && !b->child) {
			parameter_name = "";
		}
		if (parameter_name) {
			char *err = cwmp_get_parameter_attributes(parameter_name, &parameters_list);
			if (err) {
				fault_code = cwmp_get_fault_code_by_string(err);
				goto fault;
			}
			struct cwmp_dm_parameter *param_value = NULL;
			list_for_each_entry (param_value, &parameters_list, list) {
				n = mxmlNewElement(parameter_list, "ParameterAttributeStruct");
				if (!n)
					goto fault;

				n = mxmlNewElement(n, "Name");
				if (!n)
					goto fault;

				n = mxmlNewOpaque(n, param_value->name);
				if (!n)
					goto fault;

				n = n->parent->parent;
				n = mxmlNewElement(n, "Notification");
				if (!n)
					goto fault;

				char notification[2];
				sprintf(notification, "%d", param_value->notification);
				notification[1] = '\0';
				n = mxmlNewOpaque(n, notification);
				if (!n)
					goto fault;

				n = n->parent->parent;
				n = mxmlNewElement(n, "AccessList");
				if (!n)
					goto fault;

				n = mxmlNewOpaque(n, "");
				if (!n)
					goto fault;

				counter++;
			}
			cwmp_free_all_dm_parameter_list(&parameters_list);
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
		parameter_name = NULL;
	}
#ifdef ACS_MULTI
	b = mxmlFindElement(session->tree_out, session->tree_out, "ParameterList", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	if (snprintf(c, sizeof(c), "cwmp:ParameterAttributeStruct[%d]", counter) == -1)
		goto fault;

	mxmlElementSetAttr(b, "soap_enc:arrayType", c);
#endif
	return 0;

fault:
	cwmp_free_all_dm_parameter_list(&parameters_list);
	if (cwmp_create_fault_message(session, rpc, fault_code))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: SetParameterValues
 */

static int is_duplicated_parameter(mxml_node_t *param_node, struct session *session)
{
	mxml_node_t *b = param_node;
	while ((b = mxmlWalkNext(b, session->body_in, MXML_DESCEND))) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Name")) {
			if (strcmp(b->value.opaque, param_node->value.opaque) == 0)
				return -1;
		}
	}
	return 0;
}

int cwmp_handle_rpc_cpe_set_parameter_values(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b, *n;
	char *parameter_name = NULL;
	char *parameter_value = NULL;
	char *parameter_key = NULL;
	char *v, *c = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR, ret = 0;

	b = mxmlFindElement(session->body_in, session->body_in, "ParameterList", NULL, NULL, MXML_DESCEND);
	if (!b) {
		fault_code = FAULT_CPE_REQUEST_DENIED;
		goto fault;
	}

	LIST_HEAD(list_fault_param);
	rpc->list_set_value_fault = &list_fault_param;
	LIST_HEAD(list_set_param_value);
	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Name")) {
			parameter_name = icwmp_strdup(b->value.opaque);
			if (is_duplicated_parameter(b, session)) {
				fault_code = FAULT_CPE_INVALID_ARGUMENTS;
				goto fault;
			}
		}

		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "Name") && !b->child) {
			parameter_name = icwmp_strdup("");
		}

		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Value")) {
			int whitespace = 0;
			parameter_value = icwmp_strdup((char *)mxmlGetOpaque(b));
			n = b->parent;
			while ((b = mxmlWalkNext(b, n, MXML_DESCEND))) {
				v = (char *)mxmlGetOpaque(b);
				if (!whitespace)
					break;
				icwmp_asprintf(&c, "%s %s", parameter_value, v);
				parameter_value = icwmp_strdup(c);
			}
			b = n->last_child;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "Value") && !b->child) {
			parameter_value = icwmp_strdup("");
		}
		if (parameter_name && parameter_value) {
			add_dm_parameter_to_list(&list_set_param_value, parameter_name, parameter_value, NULL, 0, false);
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}

	b = mxmlFindElement(session->body_in, session->body_in, "ParameterKey", NULL, NULL, MXML_DESCEND);
	if (!b) {
		fault_code = FAULT_CPE_REQUEST_DENIED;
		goto fault;
	}
	b = mxmlWalkNext(b, session->tree_in, MXML_DESCEND_FIRST);
	if (b && b->type == MXML_OPAQUE && b->value.opaque)
		parameter_key = b->value.opaque;

	if (!icwmp_validate_string_length(parameter_key, 32)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}

	int flag = 0;
	if (transaction_id == 0) {
		if (!cwmp_transaction_start("cwmp"))
			goto fault;
	}
	fault_code = cwmp_set_multiple_parameters_values(&list_set_param_value, parameter_key ? parameter_key : "", &flag, rpc->list_set_value_fault);
	if (fault_code != FAULT_CPE_NO_FAULT)
		goto fault;

	struct cwmp_dm_parameter *param_value;
	list_for_each_entry (param_value, &list_set_param_value, list)
		set_diagnostic_parameter_structure_value(param_value->name, param_value->value);

	cwmp_free_all_dm_parameter_list(&list_set_param_value);

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:SetParameterValuesResponse");
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "Status");
	if (!b)
		goto fault;

	b = mxmlNewOpaque(b, "1");
	if (!b)
		goto fault;

	if (!cwmp_transaction_commit())
		goto fault;

	cwmp_set_end_session(flag | END_SESSION_RESTART_SERVICES | END_SESSION_SET_NOTIFICATION_UPDATE | END_SESSION_RELOAD);
	return 0;

fault:
	cwmp_free_all_dm_parameter_list(&list_set_param_value);
	if (cwmp_create_fault_message(session, rpc, fault_code))
		ret = CWMP_XML_ERR;

	cwmp_free_all_list_param_fault(rpc->list_set_value_fault);
	if (transaction_id) {
		cwmp_transaction_abort();
		transaction_id = 0;
	}
	return ret;
}

/*
 * [RPC CPE]: SetParameterAttributes
 */

int cwmp_handle_rpc_cpe_set_parameter_attributes(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *b = session->body_in;
	char *parameter_name = NULL, *parameter_notification = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR, ret = 0;
	char *notification_change = NULL;
	char c[256];

	/* handle cwmp:SetParameterAttributes */
	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "SetParameterAttributes") == -1)
		goto fault;

	n = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!n)
		goto fault;
	b = n;

	while (b != NULL) {
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "SetParameterAttributesStruct")) {
			parameter_name = NULL;
			parameter_notification = NULL;
			notification_change = NULL;
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Name")) {
			parameter_name = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "Name") && !b->child) {
			parameter_name = "";
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "NotificationChange")) {
			notification_change = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "NotificationChange") && !b->child) {
			notification_change = "";
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Notification")) {
			parameter_notification = b->value.opaque;
		}
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "Notification") && !b->child) {
			parameter_notification = "";
		}
		if (parameter_name && parameter_notification && notification_change) {
			if (!icwmp_validate_boolean_value(notification_change)) {
				fault_code = FAULT_CPE_INVALID_ARGUMENTS;
				goto fault;
			}
			if (!icwmp_validate_int_in_range(parameter_notification, 0, 6)) {
				fault_code = FAULT_CPE_INVALID_ARGUMENTS;
				goto fault;
			}
			char *err = cwmp_set_parameter_attributes(parameter_name, atoi(parameter_notification));
			if (err) {
				fault_code = cwmp_get_fault_code_by_string(err);
				goto fault;
			}
			parameter_name = NULL;
			parameter_notification = NULL;
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:SetParameterAttributesResponse");
	if (!b)
		goto fault;

	cwmp_set_end_session(END_SESSION_SET_NOTIFICATION_UPDATE | END_SESSION_RESTART_SERVICES | END_SESSION_INIT_NOTIFY);
	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, fault_code))
		ret = CWMP_XML_ERR;

	return ret;
}

/*
 * [RPC CPE]: AddObject
 */

int cwmp_handle_rpc_cpe_add_object(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;
	char *object_name = NULL;
	char *parameter_key = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR, ret = 0;
	char *instance = NULL;

	b = session->body_in;
	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "ParameterKey")) {
			parameter_key = b->value.opaque;
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "ObjectName")) {
			object_name = b->value.opaque;
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}

	if (!icwmp_validate_string_length(parameter_key, 32)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (transaction_id == 0) {
		if (!cwmp_transaction_start("cwmp"))
			goto fault;
	}

	if (object_name) {
		char *err = cwmp_add_object(object_name, parameter_key ? parameter_key : "", &instance);
		if (err) {
			fault_code = cwmp_get_fault_code_by_string(err);
			goto fault;
		}
	} else {
		fault_code = FAULT_CPE_INVALID_PARAMETER_NAME;
		goto fault;
	}

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:AddObjectResponse");
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "InstanceNumber");
	if (!b)
		goto fault;

	b = mxmlNewOpaque(b, instance);
	if (!b)
		goto fault;

	b = b->parent->parent;
	b = mxmlNewElement(b, "Status");
	if (!b)
		goto fault;

	b = mxmlNewOpaque(b, "1");
	if (!b)
		goto fault;

	if (!cwmp_transaction_commit())
		goto fault;

	char *object_path = NULL;
	icwmp_asprintf(&object_path, "%s%s.", object_name, instance);
	cwmp_set_parameter_attributes(object_path, 0);
	FREE(instance);
	cwmp_set_end_session(END_SESSION_RESTART_SERVICES);
	return 0;

fault:
	FREE(instance);
	if (cwmp_create_fault_message(session, rpc, fault_code))
		ret = CWMP_XML_ERR;
	if (transaction_id) {
		cwmp_transaction_abort();
		transaction_id = 0;
	}
	return ret;
}

/*
 * [RPC CPE]: DeleteObject
 */

int cwmp_handle_rpc_cpe_delete_object(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;
	char *object_name = NULL;
	char *parameter_key = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR, ret = 0;

	b = session->body_in;
	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "ObjectName")) {
			object_name = b->value.opaque;
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "ParameterKey")) {
			parameter_key = b->value.opaque;
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(parameter_key, 32)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (transaction_id == 0) {
		if (!cwmp_transaction_start("cwmp"))
			goto fault;
	}
	if (object_name) {
		char *err = cwmp_delete_object(object_name, parameter_key ? parameter_key : "");
		if (err) {
			fault_code = cwmp_get_fault_code_by_string(err);
			goto fault;
		}
	} else {
		fault_code = FAULT_CPE_INVALID_PARAMETER_NAME;
		goto fault;
	}

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:DeleteObjectResponse");
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "Status");
	if (!b)
		goto fault;

	b = mxmlNewOpaque(b, "1");
	if (!b)
		goto fault;

	if (!cwmp_transaction_commit())
		goto fault;

	cwmp_set_end_session(END_SESSION_RESTART_SERVICES);
	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, fault_code))
		ret = CWMP_XML_ERR;
	if (transaction_id) {
		cwmp_transaction_abort();
		transaction_id = 0;
	}
	return ret;
}

/*
 * [RPC CPE]: GetRPCMethods
 */

int cwmp_handle_rpc_cpe_get_rpc_methods(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *method_list;
	int i, counter = 0;
#ifdef ACS_MULTI
	mxml_node_t *b = session->body_in;
	char c[128];
#endif

	n = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto fault;

	n = mxmlNewElement(n, "cwmp:GetRPCMethodsResponse");
	if (!n)
		goto fault;

	method_list = mxmlNewElement(n, "MethodList");
	if (!method_list)
		goto fault;

	for (i = 1; i < __RPC_CPE_MAX; i++) {
		if (i != RPC_CPE_FAULT) {
			n = mxmlNewElement(method_list, "string");
			if (!n)
				goto fault;

			n = mxmlNewOpaque(n, rpc_cpe_methods[i].name);
			if (!n)
				goto fault;

			counter++;
		}
	}
#ifdef ACS_MULTI
	b = mxmlFindElement(session->tree_out, session->tree_out, "MethodList", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	mxmlElementSetAttr(b, "xsi:type", "soap_enc:Array");
	if (snprintf(c, sizeof(c), "xsd:string[%d]", counter) == -1)
		goto fault;

	mxmlElementSetAttr(b, "soap_enc:arrayType", c);
#endif

	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, FAULT_CPE_INTERNAL_ERROR))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: FactoryReset
 */

int cwmp_handle_rpc_cpe_factory_reset(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:FactoryResetResponse");
	if (!b)
		goto fault;

	cwmp_set_end_session(END_SESSION_FACTORY_RESET);

	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, FAULT_CPE_INTERNAL_ERROR))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: X_FactoryResetSoft
 */

int cwmp_handle_rpc_cpe_x_factory_reset_soft(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:X_FactoryResetSoftResponse");
	if (!b)
		goto fault;

	cwmp_set_end_session(END_SESSION_X_FACTORY_RESET_SOFT);

	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, FAULT_CPE_INTERNAL_ERROR))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: CancelTransfer
 */

int cwmp_handle_rpc_cpe_cancel_transfer(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;
	char *command_key = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR;

	b = session->body_in;
	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			command_key = b->value.opaque;
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(command_key, 32)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (command_key) {
		cancel_transfer(command_key);
	}
	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:CancelTransferResponse");
	if (!b)
		goto fault;
	return 0;
fault:
	if (cwmp_create_fault_message(session, rpc, fault_code))
		goto error;
	return 0;

error:
	return -1;
}

int cancel_transfer(char *key)
{
	struct upload *pupload;
	struct download *pdownload;
	struct list_head *ilist, *q;

	if (list_download.next != &(list_download)) {
		list_for_each_safe (ilist, q, &(list_download)) {
			pdownload = list_entry(ilist, struct download, list);
			if (strcmp(pdownload->command_key, key) == 0) {
				pthread_mutex_lock(&mutex_download);
				bkp_session_delete_download(pdownload);
				bkp_session_save();
				list_del(&(pdownload->list));
				if (pdownload->scheduled_time != 0)
					count_download_queue--;
				cwmp_free_download_request(pdownload);
				pthread_mutex_unlock(&mutex_download);
			}
		}
	}
	if (list_upload.next != &(list_upload)) {
		list_for_each_safe (ilist, q, &(list_upload)) {
			pupload = list_entry(ilist, struct upload, list);
			if (strcmp(pupload->command_key, key) == 0) {
				pthread_mutex_lock(&mutex_upload);
				bkp_session_delete_upload(pupload);
				bkp_session_save(); //is it needed
				list_del(&(pupload->list));
				if (pupload->scheduled_time != 0)
					count_download_queue--;
				cwmp_free_upload_request(pupload);
				pthread_mutex_unlock(&mutex_upload);
			}
		}
	}
	// Cancel schedule download
	return CWMP_OK;
}

/*
 * [RPC CPE]: Reboot
 */

int cwmp_handle_rpc_cpe_reboot(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b;
	struct event_container *event_container;
	char *command_key = NULL;
	int fault_code = FAULT_CPE_INTERNAL_ERROR;
	b = session->body_in;

	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			command_key = b->value.opaque;
			commandKey = icwmp_strdup(b->value.opaque);
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(commandKey, 32)) {
		fault_code = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	pthread_mutex_lock(&(cwmp_main.mutex_session_queue));
	event_container = cwmp_add_event_container(&cwmp_main, EVENT_IDX_M_Reboot, command_key);
	if (event_container == NULL) {
		pthread_mutex_unlock(&(cwmp_main.mutex_session_queue));
		goto fault;
	}
	cwmp_save_event_container(event_container);
	pthread_mutex_unlock(&(cwmp_main.mutex_session_queue));

	b = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto fault;

	b = mxmlNewElement(b, "cwmp:RebootResponse");
	if (!b)
		goto fault;

	cwmp_set_end_session(END_SESSION_REBOOT);

	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, fault_code))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: ScheduleInform
 */

int cwmp_handle_rpc_cpe_schedule_inform(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *b = session->body_in;
	char *command_key = NULL;
	struct schedule_inform *schedule_inform;
	time_t scheduled_time;
	struct list_head *ilist;
	int fault = FAULT_CPE_NO_FAULT;
	unsigned int delay_seconds = 0;

	pthread_mutex_lock(&mutex_schedule_inform);

	while (b) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			command_key = b->value.opaque;
		}

		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "DelaySeconds")) {
			delay_seconds = atoi(b->value.opaque);
		}
		b = mxmlWalkNext(b, session->body_in, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(command_key, 32)) {
		fault = FAULT_CPE_INVALID_ARGUMENTS;
		pthread_mutex_unlock(&mutex_schedule_inform);
		goto fault;
	}
	if (delay_seconds <= 0) {
		fault = FAULT_CPE_INVALID_ARGUMENTS;
		pthread_mutex_unlock(&mutex_schedule_inform);
		goto fault;
	}

	if (count_schedule_inform_queue >= MAX_SCHEDULE_INFORM_QUEUE) {
		fault = FAULT_CPE_RESOURCES_EXCEEDED;
		pthread_mutex_unlock(&mutex_schedule_inform);
		goto fault;
	}
	count_schedule_inform_queue++;

	scheduled_time = time(NULL) + delay_seconds;
	list_for_each (ilist, &(list_schedule_inform)) {
		schedule_inform = list_entry(ilist, struct schedule_inform, list);
		if (schedule_inform->scheduled_time >= scheduled_time) {
			break;
		}
	}

	n = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!n)
		goto fault;

	n = mxmlNewElement(n, "cwmp:ScheduleInformResponse");
	if (!n)
		goto fault;

	CWMP_LOG(INFO, "Schedule inform event will start in %us", delay_seconds);
	schedule_inform = calloc(1, sizeof(struct schedule_inform));
	if (schedule_inform == NULL) {
		pthread_mutex_unlock(&mutex_schedule_inform);
		goto fault;
	}
	schedule_inform->commandKey = strdup(command_key);
	schedule_inform->scheduled_time = scheduled_time;
	list_add(&(schedule_inform->list), ilist->prev);
	bkp_session_insert_schedule_inform(schedule_inform->scheduled_time, schedule_inform->commandKey);
	bkp_session_save();
	pthread_mutex_unlock(&mutex_schedule_inform);
	pthread_cond_signal(&threshold_schedule_inform);

success:
	return 0;

fault:
	if (cwmp_create_fault_message(session, rpc, fault ? fault : FAULT_CPE_INTERNAL_ERROR))
		goto error;
	goto success;

error:
	return -1;
}

/*
 * [RPC CPE]: ChangeDuState
 */
int cwmp_handle_rpc_cpe_change_du_state(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *t, *b = session->body_in;
	struct change_du_state *change_du_state = NULL;
	struct operations *elem = NULL;
	int error = FAULT_CPE_NO_FAULT;
	char c[256];

	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "ChangeDUState") == -1) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	n = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!n)
		return -1;
	b = n;

	change_du_state = calloc(1, sizeof(struct change_du_state));
	if (change_du_state == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	INIT_LIST_HEAD(&(change_du_state->list_operation));
	change_du_state->timeout = time(NULL);

	while (b != NULL) {
		t = b;
		if (b && b->type == MXML_ELEMENT && strcmp(b->value.element.name, "Operations") == 0) {
			char *operation = (char *)mxmlElementGetAttrValue(b, "xsi:type");
			if (!strcmp(operation, "cwmp:InstallOpStruct")) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_INSTALL;
				list_add_tail(&(elem->list), &(change_du_state->list_operation));
				t = mxmlWalkNext(t, b, MXML_DESCEND);
				while (t) {
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "URL")) {
						elem->url = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "UUID")) {
						elem->uuid = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "Username")) {
						elem->username = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->parent->type == MXML_ELEMENT && t->value.opaque && !strcmp(t->parent->value.element.name, "Password")) {
						elem->password = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "ExecutionEnvRef")) {
						elem->executionenvref = strdup(t->value.opaque);
					}
					t = mxmlWalkNext(t, b, MXML_DESCEND);
				}
			} else if (!strcmp(operation, "cwmp:UpdateOpStruct")) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_UPDATE;
				list_add_tail(&(elem->list), &(change_du_state->list_operation));
				t = mxmlWalkNext(t, b, MXML_DESCEND);
				while (t) {
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "Username")) {
						elem->username = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "Version")) {
						elem->version = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "URL")) {
						elem->url = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "Password")) {
						elem->password = strdup(t->value.opaque);
					}

					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "UUID")) {
						elem->uuid = strdup(t->value.opaque);
					}

					t = mxmlWalkNext(t, b, MXML_DESCEND);
				}
			} else if (!strcmp(operation, "cwmp:UninstallOpStruct")) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_UNINSTALL;
				list_add_tail(&(elem->list), &(change_du_state->list_operation));
				t = mxmlWalkNext(t, b, MXML_DESCEND);
				while (t) {
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "Version")) {
						elem->version = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "ExecutionEnvRef")) {
						elem->executionenvref = strdup(t->value.opaque);
					}
					if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "UUID")) {
						elem->uuid = strdup(t->value.opaque);
					}
					t = mxmlWalkNext(t, b, MXML_DESCEND);
				}
			}
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			change_du_state->command_key = strdup(b->value.opaque);
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(change_du_state->command_key, 32)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	t = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!t)
		goto fault;

	t = mxmlNewElement(t, "cwmp:ChangeDUStateResponse");
	if (!t)
		goto fault;

	if (error == FAULT_CPE_NO_FAULT) {
		pthread_mutex_lock(&mutex_change_du_state);
		list_add_tail(&(change_du_state->list), &(list_change_du_state));
		bkp_session_insert_change_du_state(change_du_state);
		bkp_session_save();
		pthread_mutex_unlock(&mutex_change_du_state);
		pthread_cond_signal(&threshold_change_du_state);
	}
	return 0;

fault:
	cwmp_free_change_du_state_request(change_du_state);
	if (cwmp_create_fault_message(session, rpc, error))
		goto error;
	return 0;

error:
	return -1;
}

int create_download_upload_response(mxml_node_t *tree_out, enum load_type ltype)
{
	mxml_node_t *t, *b;
	t = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!t)
		return -1;

	t = mxmlNewElement(t, ltype == TYPE_DOWNLOAD ? "cwmp:DownloadResponse" : "cwmp:UploadResponse");
	if (!t)
		return -1;

	b = mxmlNewElement(t, "Status");
	if (!b)
		return -1;

	b = mxmlNewOpaque(b, "1");
	if (!b)
		return -1;

	b = b->parent->parent;
	b = mxmlNewElement(t, "StartTime");
	if (!b)
		return -1;

	b = mxmlNewOpaque(b, "0001-01-01T00:00:00+00:00");
	if (!b)
		return -1;

	b = b->parent->parent;
	b = mxmlNewElement(t, "CompleteTime");
	if (!b)
		return -1;

	b = mxmlNewOpaque(b, "0001-01-01T00:00:00+00:00");
	if (!b)
		return -1;

	return FAULT_CPE_NO_FAULT;
}

/*
 * [RPC CPE]: Download
 */
int cwmp_handle_rpc_cpe_download(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *b = session->body_in;
	char c[256], *tmp, *file_type = NULL;
	int error = FAULT_CPE_NO_FAULT;
	struct download *download = NULL, *idownload;
	struct list_head *ilist;
	time_t scheduled_time = 0;
	time_t download_delay = 0;
	char *str_file_size = NULL, *str_download_delay = NULL;

	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "Download") == -1) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	n = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!n)
		return -1;
	b = n;

	download = calloc(1, sizeof(struct download));
	if (download == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	while (b != NULL) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			download->command_key = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "FileType")) {
			if (download->file_type == NULL) {
				download->file_type = strdup(b->value.opaque);
				file_type = strdup(b->value.opaque);
			} else {
				tmp = file_type;
				if (cwmp_asprintf(&file_type, "%s %s", tmp, b->value.opaque) == -1) {
					error = FAULT_CPE_INTERNAL_ERROR;
					goto fault;
				}
				FREE(tmp);
			}
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "URL")) {
			download->url = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Username")) {
			download->username = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Password")) {
			download->password = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "FileSize")) {
			str_file_size = strdup(b->value.opaque ? b->value.opaque: "0");
			download->file_size = atoi(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "DelaySeconds")) {
			str_download_delay = strdup(b->value.opaque ? b->value.opaque: "0");
			download_delay = atol(b->value.opaque);
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(download->command_key, 32)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(download->url, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(download->username, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(download->password, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_unsignedint(str_file_size)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		FREE(str_file_size);
		goto fault;
	}
	FREE(str_file_size);
	if (!icwmp_validate_unsignedint(str_download_delay)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		FREE(str_download_delay);
		goto fault;
	}
	FREE(str_download_delay);

	if (strcmp(file_type, "1 Firmware Upgrade Image") && strcmp(file_type, "2 Web Content") && strcmp(file_type, "3 Vendor Configuration File") && strcmp(file_type, "4 Tone File") && strcmp(file_type, "5 Ringer File") && strcmp(file_type, "6 Stored Firmware Image")) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
	} else if (count_download_queue >= MAX_DOWNLOAD_QUEUE) {
		error = FAULT_CPE_RESOURCES_EXCEEDED;
	} else if ((download->url == NULL || ((download->url != NULL) && (strcmp(download->url, "") == 0)))) {
		error = FAULT_CPE_REQUEST_DENIED;
	} else if (strstr(download->url, "@") != NULL) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
	} else if (strncmp(download->url, DOWNLOAD_PROTOCOL_HTTP, strlen(DOWNLOAD_PROTOCOL_HTTP)) != 0 && strncmp(download->url, DOWNLOAD_PROTOCOL_HTTPS, strlen(DOWNLOAD_PROTOCOL_HTTPS)) != 0 && strncmp(download->url, DOWNLOAD_PROTOCOL_FTP, strlen(DOWNLOAD_PROTOCOL_FTP)) != 0) {
		error = FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL;
	}

	FREE(file_type);
	if (error != FAULT_CPE_NO_FAULT)
		goto fault;

	if (create_download_upload_response(session->tree_out, TYPE_DOWNLOAD) != FAULT_CPE_NO_FAULT) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	if (error == FAULT_CPE_NO_FAULT) {
		pthread_mutex_lock(&mutex_download);
		if (download_delay != 0)
			scheduled_time = time(NULL) + download_delay;

		list_for_each (ilist, &(list_download)) {
			idownload = list_entry(ilist, struct download, list);
			if (idownload->scheduled_time >= scheduled_time) {
				break;
			}
		}
		list_add(&(download->list), ilist->prev);
		if (download_delay != 0) {
			count_download_queue++;
			download->scheduled_time = scheduled_time;
		}
		bkp_session_insert_download(download);
		bkp_session_save();
		if (download_delay != 0) {
			CWMP_LOG(INFO, "Download will start in %us", download_delay);
		} else {
			CWMP_LOG(INFO, "Download will start at the end of session");
		}

		pthread_mutex_unlock(&mutex_download);
		pthread_cond_signal(&threshold_download);
	}

	return 0;

fault:
	cwmp_free_download_request(download);
	if (cwmp_create_fault_message(session, rpc, error))
		return -1;
	return 0;
}

/*
 * [RPC CPE]: ScheduleDownload
 */
int cwmp_handle_rpc_cpe_schedule_download(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *t, *b = session->body_in;
	char c[256], *tmp, *file_type = NULL;
	char *windowmode0 = NULL, *windowmode1 = NULL, *str_file_size = NULL;
	int i = 0, j = 0;
	int error = FAULT_CPE_NO_FAULT;
	struct download *schedule_download = NULL;
	time_t schedule_download_delay[4] = { 0, 0, 0, 0 };

	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "ScheduleDownload") == -1) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	n = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!n)
		return -1;
	b = n;

	schedule_download = calloc(1, sizeof(struct download));
	if (schedule_download == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	while (b != NULL) {
		t = b;
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			schedule_download->command_key = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "FileType")) {
			if (schedule_download->file_type != NULL) {
				tmp = file_type;
				if (cwmp_asprintf(&file_type, "%s %s", tmp, b->value.opaque) == -1) {
					error = FAULT_CPE_INTERNAL_ERROR;
					goto fault;
				}
			} else {
				schedule_download->file_type = strdup(b->value.opaque);
				file_type = strdup(b->value.opaque);
			}
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "URL")) {
			schedule_download->url = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Username")) {
			schedule_download->username = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Password")) {
			schedule_download->password = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "FileSize")) {
			str_file_size = strdup(b->value.opaque);
			schedule_download->file_size = atoi(b->value.opaque);
		}

		if (b && b->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "TimeWindowList")) {
			if (!t)
				return -1; //TO CHECK*/
			t = mxmlWalkNext(t, b, MXML_DESCEND);
			while (t) {
				if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "WindowStart")) {
					schedule_download_delay[j] = atol(t->value.opaque);
					j++;
				}
				if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "WindowEnd")) {
					schedule_download_delay[j] = atol(t->value.opaque);
					j++;
				}
				if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "WindowMode")) {
					if (schedule_download->timewindowstruct[i].windowmode == NULL) {
						schedule_download->timewindowstruct[i].windowmode = strdup(t->value.opaque);
						if (i == 0)
							windowmode0 = strdup(t->value.opaque);
						else
							windowmode1 = strdup(t->value.opaque);
					} else if (i == 0) {
						tmp = windowmode0;
						if (cwmp_asprintf(&windowmode0, "%s %s", tmp, t->value.opaque) == -1) {
							error = FAULT_CPE_INTERNAL_ERROR;
							goto fault;
						}
					} else if (i == 1) {
						tmp = windowmode1;
						if (cwmp_asprintf(&windowmode1, "%s %s", tmp, t->value.opaque) == -1) {
							error = FAULT_CPE_INTERNAL_ERROR;
							goto fault;
						}
					}
				}

				if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "UserMessage")) {
					schedule_download->timewindowstruct[i].usermessage = strdup(t->value.opaque);
				}
				if (t && t->type == MXML_OPAQUE && t->value.opaque && t->parent->type == MXML_ELEMENT && !strcmp(t->parent->value.element.name, "MaxRetries")) {
					schedule_download->timewindowstruct[i].maxretries = atoi(t->value.opaque);
				}
				t = mxmlWalkNext(t, b, MXML_DESCEND);
			}
			i++;
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(schedule_download->command_key, 32)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(schedule_download->url, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(schedule_download->username, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(schedule_download->password, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_unsignedint(str_file_size)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		FREE(str_file_size);
		goto fault;
	}
	FREE(str_file_size);
	if (strcmp(file_type, "1 Firmware Upgrade Image") && strcmp(file_type, "2 Web Content") && strcmp(file_type, "3 Vendor Configuration File") && strcmp(file_type, "4 Tone File") && strcmp(file_type, "5 Ringer File") && strcmp(file_type, "6 Stored Firmware Image")) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
	} else if ((strcmp(windowmode0, "1 At Any Time") && strcmp(windowmode0, "2 Immediately") && strcmp(windowmode0, "3 When Idle")) || (strcmp(windowmode1, "1 At Any Time") && strcmp(windowmode1, "2 Immediately") && strcmp(windowmode1, "3 When Idle"))) {
		error = FAULT_CPE_REQUEST_DENIED;
	} else if (count_download_queue >= MAX_DOWNLOAD_QUEUE) {
		error = FAULT_CPE_RESOURCES_EXCEEDED;
	} else if ((schedule_download->url == NULL || ((schedule_download->url != NULL) && (strcmp(schedule_download->url, "") == 0)))) {
		error = FAULT_CPE_REQUEST_DENIED;
	} else if (strstr(schedule_download->url, "@") != NULL) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
	} else if (strncmp(schedule_download->url, DOWNLOAD_PROTOCOL_HTTP, strlen(DOWNLOAD_PROTOCOL_HTTP)) != 0 && strncmp(schedule_download->url, DOWNLOAD_PROTOCOL_FTP, strlen(DOWNLOAD_PROTOCOL_FTP)) != 0) {
		error = FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL;
	} else {
		for (j = 0; j < 3; j++) {
			if (schedule_download_delay[j] > schedule_download_delay[j + 1]) {
				error = FAULT_CPE_INVALID_ARGUMENTS;
				break;
			}
		}
	}

	FREE(file_type);
	FREE(windowmode0);
	FREE(windowmode1);
	if (error != FAULT_CPE_NO_FAULT)
		goto fault;

	t = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!t)
		goto fault;

	t = mxmlNewElement(t, "cwmp:ScheduleDownloadResponse"); // ScheduleDownloadResponse has no argument
	if (!t)
		goto fault;

	if (error == FAULT_CPE_NO_FAULT) {
		pthread_mutex_lock(&mutex_schedule_download);
		list_add_tail(&(schedule_download->list), &(list_schedule_download));
		if (schedule_download_delay[0] != 0) {
			count_download_queue++;
		}
		while (i > 0) {
			i--;
			schedule_download->timewindowstruct[i].windowstart = time(NULL) + schedule_download_delay[i * 2];
			schedule_download->timewindowstruct[i].windowend = time(NULL) + schedule_download_delay[i * 2 + 1];
		}
		bkp_session_insert_schedule_download(schedule_download);
		bkp_session_save();
		if (schedule_download_delay[0] != 0) {
			CWMP_LOG(INFO, "Schedule download will start in %us", schedule_download_delay[0]);
		} else {
			CWMP_LOG(INFO, "Schedule Download will start at the end of session");
		}
		pthread_mutex_unlock(&mutex_schedule_download);
		pthread_cond_signal(&threshold_schedule_download);
	}

	return 0;

fault:
	cwmp_free_schedule_download_request(schedule_download);
	if (cwmp_create_fault_message(session, rpc, error))
		goto error;
	return 0;

error:
	return -1;
}

/*
 * [RPC CPE]: Upload
 */
int cwmp_handle_rpc_cpe_upload(struct session *session, struct rpc *rpc)
{
	mxml_node_t *n, *b = session->body_in;
	char *tmp, *file_type = NULL;
	int error = FAULT_CPE_NO_FAULT;
	struct upload *upload = NULL, *iupload;
	struct list_head *ilist;
	time_t scheduled_time = 0;
	time_t upload_delay = 0;
	char *str_upload_delay = NULL;
	char c[256];

	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "Upload") == -1) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	n = mxmlFindElement(session->tree_in, session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (!n)
		return -1;
	b = n;

	upload = calloc(1, sizeof(struct upload));
	if (upload == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}
	upload->f_instance = strdup("");
	while (b != NULL) {
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "CommandKey")) {
			upload->command_key = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "FileType")) {
			if (upload->file_type == NULL) {
				upload->file_type = strdup(b->value.opaque);
				file_type = strdup(b->value.opaque);
			} else {
				tmp = file_type;
				if (cwmp_asprintf(&file_type, "%s %s", tmp, b->value.opaque) == -1) {
					error = FAULT_CPE_INTERNAL_ERROR;
					goto fault;
				}
				if (isdigit(b->value.opaque[0])) {
					upload->f_instance = strdup(b->value.opaque);
				}
			}
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "URL")) {
			upload->url = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Username")) {
			upload->username = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "Password")) {
			upload->password = strdup(b->value.opaque);
		}
		if (b && b->type == MXML_OPAQUE && b->value.opaque && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "DelaySeconds")) {
			str_upload_delay = strdup(b->value.opaque);
			upload_delay = atol(b->value.opaque);
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}
	if (!icwmp_validate_string_length(upload->command_key, 32)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(upload->url, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(upload->username, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_string_length(upload->password, 256)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		goto fault;
	}
	if (!icwmp_validate_unsignedint(str_upload_delay)) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
		FREE(str_upload_delay);
		goto fault;
	}
	FREE(str_upload_delay);

	if (strncmp(file_type, "1 Vendor Configuration File", sizeof "1 Vendor Configuration File" - 1) != 0 && strncmp(file_type, "3 Vendor Configuration File", sizeof "3 Vendor Configuration File" - 1) != 0 && strncmp(file_type, "2 Vendor Log File", sizeof "2 Vendor Log File" - 1) != 0 &&
	    strncmp(file_type, "4 Vendor Log File", sizeof "4 Vendor Log File" - 1) != 0) {
		error = FAULT_CPE_REQUEST_DENIED;
	} else if (count_download_queue >= MAX_DOWNLOAD_QUEUE) {
		error = FAULT_CPE_RESOURCES_EXCEEDED;
	} else if ((upload->url == NULL || ((upload->url != NULL) && (strcmp(upload->url, "") == 0)))) {
		error = FAULT_CPE_REQUEST_DENIED;
	} else if (strstr(upload->url, "@") != NULL) {
		error = FAULT_CPE_INVALID_ARGUMENTS;
	} else if (strncmp(upload->url, DOWNLOAD_PROTOCOL_HTTP, strlen(DOWNLOAD_PROTOCOL_HTTP)) != 0 && strncmp(upload->url, DOWNLOAD_PROTOCOL_FTP, strlen(DOWNLOAD_PROTOCOL_FTP)) != 0) {
		error = FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL;
	}

	FREE(file_type);
	if (error != FAULT_CPE_NO_FAULT) {
		goto fault;
	}

	if (create_download_upload_response(session->tree_out, TYPE_UPLOAD) != FAULT_CPE_NO_FAULT) {
		error = FAULT_CPE_INTERNAL_ERROR;
		goto fault;
	}

	if (error == FAULT_CPE_NO_FAULT) {
		pthread_mutex_lock(&mutex_upload);
		if (upload_delay != 0)
			scheduled_time = time(NULL) + upload_delay;

		list_for_each (ilist, &(list_upload)) {
			iupload = list_entry(ilist, struct upload, list);
			if (iupload->scheduled_time >= scheduled_time) {
				break;
			}
		}
		list_add(&(upload->list), ilist->prev);
		if (upload_delay != 0) {
			count_download_queue++;
			upload->scheduled_time = scheduled_time;
		}
		bkp_session_insert_upload(upload);
		bkp_session_save();
		if (upload_delay != 0) {
			CWMP_LOG(INFO, "Upload will start in %us", upload_delay);
		} else {
			CWMP_LOG(INFO, "Upload will start at the end of session");
		}
		pthread_mutex_unlock(&mutex_upload);
		pthread_cond_signal(&threshold_upload);
	}
	return 0;

fault:
	cwmp_free_upload_request(upload);
	if (cwmp_create_fault_message(session, rpc, error))
		return -1;
	return 0;
}

/*
 * [FAULT]: Fault
 */

int cwmp_handle_rpc_cpe_fault(struct session *session, struct rpc *rpc)
{
	mxml_node_t *b, *t, *u, *body;
	struct cwmp_param_fault *param_fault;
	int idx;

	body = mxmlFindElement(session->tree_out, session->tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);

	b = mxmlNewElement(body, "soap_env:Fault");
	if (!b)
		return -1;

	t = mxmlNewElement(b, "faultcode");
	if (!t)
		return -1;

	u = mxmlNewOpaque(t, (FAULT_CPE_ARRAY[session->fault_code].TYPE == FAULT_CPE_TYPE_CLIENT) ? "Client" : "Server");
	if (!u)
		return -1;

	t = mxmlNewElement(b, "faultstring");
	if (!t)
		return -1;

	u = mxmlNewOpaque(t, "CWMP fault");
	if (!u)
		return -1;

	b = mxmlNewElement(b, "detail");
	if (!b)
		return -1;

	b = mxmlNewElement(b, "cwmp:Fault");
	if (!b)
		return -1;

	t = mxmlNewElement(b, "FaultCode");
	if (!t)
		return -1;

	u = mxmlNewOpaque(t, FAULT_CPE_ARRAY[session->fault_code].CODE);
	if (!u)
		return -1;

	t = mxmlNewElement(b, "FaultString");
	if (!b)
		return -1;

	u = mxmlNewOpaque(t, FAULT_CPE_ARRAY[session->fault_code].DESCRIPTION);
	if (!u)
		return -1;

	if (rpc->type == RPC_CPE_SET_PARAMETER_VALUES) {
		while (rpc->list_set_value_fault->next != rpc->list_set_value_fault) {
			param_fault = list_entry(rpc->list_set_value_fault->next, struct cwmp_param_fault, list);

			if (param_fault->fault) {
				idx = cwmp_get_fault_code(param_fault->fault);

				t = mxmlNewElement(b, "SetParameterValuesFault");
				if (!t)
					return -1;

				u = mxmlNewElement(t, "ParameterName");
				if (!u)
					return -1;

				u = mxmlNewOpaque(u, param_fault->name);
				if (!u)
					return -1;

				u = mxmlNewElement(t, "FaultCode");
				if (!u)
					return -1;

				u = mxmlNewOpaque(u, FAULT_CPE_ARRAY[idx].CODE);
				if (!u)
					return -1;

				u = mxmlNewElement(t, "FaultString");
				if (!u)
					return -1;

				u = mxmlNewOpaque(u, FAULT_CPE_ARRAY[idx].DESCRIPTION);
				if (!u)
					return -1;
			}
			cwmp_del_list_fault_param(param_fault);
		}
	}

	return 0;
}

int cwmp_create_fault_message(struct session *session, struct rpc *rpc_cpe, int fault_code)
{
	CWMP_LOG(INFO, "Fault detected");
	session->fault_code = fault_code;

	MXML_DELETE(session->tree_out);

	if (xml_prepare_msg_out(session))
		return -1;

	CWMP_LOG(INFO, "Preparing the Fault message");
	if (rpc_cpe_methods[RPC_CPE_FAULT].handler(session, rpc_cpe))
		return -1;
	rpc_cpe->type = RPC_CPE_FAULT;

	return 0;
}
