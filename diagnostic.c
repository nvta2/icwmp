/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *	  Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include <stdarg.h>
#include <unistd.h>
#include "common.h"
#include "diagnostic.h"
#include "config.h"
#include "xml.h"
#include "datamodel_interface.h"
#include "ubus.h"
#include "cwmp_uci.h"

struct diagnostic_input {
	char *input_name;
	char *parameter_name;
	char *value;
};

#define DOWNLOAD_NUMBER_INPUTS 7
#define UPLOAD_NUMBER_INPUTS 8
#define IPPING_NUMBER_INPUTS 7
#define SESERVERSELECT_NUMBER_INPUTS 6
#define TRACEROUTE_NUMBER_INPUTS 8
#define UDPECHO_NUMBER_INPUTS 9
#define NSLKUP_NUMBER_INPUTS 5

#define IP_DIAGNOSTICS_OBJECT "Device.IP.Diagnostics."
#define DNS_DIAGNOSTICS_OBJECT "Device.DNS.Diagnostics."

#define DOWNLOAD_DIAG_ACT_NAME "DownloadDiagnostics"
#define UPLOAD_DIAG_ACT_NAME "UploadDiagnostics"
#define IPPING_DIAG_ACT_NAME "IPPing"
#define SERVER_SELECTION_DIAG_ACT_NAME "ServerSelectionDiagnostics"
#define TRACE_ROUTE_DIAG_ACT_NAME "TraceRoute"
#define UDPECHO_DIAG_ACT_NAME "UDPEchoDiagnostics"
#define NSLOOKUP_DIAG_ACT_NAME "NSLookupDiagnostics"

struct diagnostic_input download_diagnostics_array[DOWNLOAD_NUMBER_INPUTS] = {
	{ "Interface", "Device.IP.Diagnostics.DownloadDiagnostics.Interface", NULL },
	{ "DownloadURL", "Device.IP.Diagnostics.DownloadDiagnostics.DownloadURL", NULL },
	{ "DSCP", "Device.IP.Diagnostics.DownloadDiagnostics.DSCP", NULL },
	{ "EthernetPriority", "Device.IP.Diagnostics.DownloadDiagnostics.EthernetPriority", NULL },
	{ "ProtocolVersion", "Device.IP.Diagnostics.DownloadDiagnostics.ProtocolVersion", NULL },
	{ "NumberOfConnections", "Device.IP.Diagnostics.DownloadDiagnostics.NumberOfConnections", NULL },
	{ "EnablePerConnectionResults", "Device.IP.Diagnostics.DownloadDiagnostics.EnablePerConnection", NULL },
	//{"TimeBasedTestDuration","Device.IP.Diagnostics.DownloadDiagnostics.TimeBasedTestDuration",NULL},
	//{"TimeBasedTestMeasurementInterval","Device.IP.Diagnostics.DownloadDiagnostics.TimeBasedTestMeasurementInterval",NULL},
	//{"TimeBasedTestMeasurementOffset","Device.IP.Diagnostics.DownloadDiagnostics.TimeBasedTestMeasurementOffset",NULL}
};

struct diagnostic_input upload_diagnostics_array[UPLOAD_NUMBER_INPUTS] = {
	{ "Interface", "Device.IP.Diagnostics.UploadDiagnostics.Interface", NULL },
	{ "UploadURL", "Device.IP.Diagnostics.UploadDiagnostics.UploadURL", NULL },
	{ "TestFileLength", "Device.IP.Diagnostics.UploadDiagnostics.TestFileLength", NULL },
	{ "DSCP", "Device.IP.Diagnostics.UploadDiagnostics.DSCP", NULL },
	{ "EthernetPriority", "Device.IP.Diagnostics.UploadDiagnostics.EthernetPriority", NULL },
	{ "ProtocolVersion", "Device.IP.Diagnostics.UploadDiagnostics.ProtocolVersion", NULL },
	{ "NumberOfConnections", "Device.IP.Diagnostics.UploadDiagnostics.NumberOfConnections", NULL },
	{ "EnablePerConnectionResults", "Device.IP.Diagnostics.UploadDiagnostics.EnablePerConnection", NULL },
	//{"TimeBasedTestDuration","Device.IP.Diagnostics.UploadDiagnostics.TimeBasedTestDuration",NULL},
	//{"TimeBasedTestMeasurementInterval","Device.IP.Diagnostics.UploadDiagnostics.TimeBasedTestMeasurementInterval",NULL},
	//{"TimeBasedTestMeasurementOffset","Device.IP.Diagnostics.UploadDiagnostics.TimeBasedTestMeasurementOffset",NULL}
};

struct diagnostic_input ipping_diagnostics_array[IPPING_NUMBER_INPUTS] = { { "Host", "Device.IP.Diagnostics.IPPing.Host", NULL },
									   { "NumberOfRepetitions", "Device.IP.Diagnostics.IPPing.NumberOfRepetitions", NULL },
									   { "Timeout", "Device.IP.Diagnostics.IPPing.Timeout", NULL },
									   { "Interface", "Device.IP.Diagnostics.IPPing.Interface", NULL },
									   { "ProtocolVersion", "Device.IP.Diagnostics.IPPing.ProtocolVersion", NULL },
									   { "DSCP", "Device.IP.Diagnostics.IPPing.DSCP", NULL },
									   { "DataBlockSize", "Device.IP.Diagnostics.IPPing.DataBlockSize", NULL } };
struct diagnostic_input seserverselection_diagnostics_array[SESERVERSELECT_NUMBER_INPUTS] = { { "Interface", "Device.IP.Diagnostics.ServerSelectionDiagnostics.Interface", NULL },
											      { "Protocol", "Device.IP.Diagnostics.ServerSelectionDiagnostics.Protocol", NULL },
											      { "HostList", "Device.IP.Diagnostics.ServerSelectionDiagnostics.HostList", NULL },
											      { "ProtocolVersion", "Device.IP.Diagnostics.ServerSelectionDiagnostics.ProtocolVersion", NULL },
											      { "NumberOfRepetitions", "Device.IP.Diagnostics.ServerSelectionDiagnostics.NumberOfRepetitions", NULL },
											      { "Timeout", "Device.IP.Diagnostics.ServerSelectionDiagnostics.Timeout", NULL } };

struct diagnostic_input traceroute_diagnostics_array[TRACEROUTE_NUMBER_INPUTS] = { { "Interface", "Device.IP.Diagnostics.TraceRoute.Interface", NULL },
										   { "Host", "Device.IP.Diagnostics.TraceRoute.Host", NULL },
										   { "NumberOfTries", "Device.IP.Diagnostics.TraceRoute.NumberOfTries", NULL },
										   { "ProtocolVersion", "Device.IP.Diagnostics.TraceRoute.ProtocolVersion", NULL },
										   { "Timeout", "Device.IP.Diagnostics.TraceRoute.Timeout", NULL },
										   { "DataBlockSize", "Device.IP.Diagnostics.TraceRoute.DataBlockSize", NULL },
										   { "DSCP", "Device.IP.Diagnostics.TraceRoute.DSCP", NULL },
										   { "MaxHopCount", "Device.IP.Diagnostics.TraceRoute.MaxHopCount", NULL } };

struct diagnostic_input udpecho_diagnostics_array[UDPECHO_NUMBER_INPUTS] = {
	{ "Interface", "Device.IP.Diagnostics.UDPEchoDiagnostics.Interface", NULL },
	{ "Host", "Device.IP.Diagnostics.UDPEchoDiagnostics.Host", NULL },
	{ "Port", "Device.IP.Diagnostics.UDPEchoDiagnostics.Port", NULL },
	{ "NumberOfRepetitions", "Device.IP.Diagnostics.UDPEchoDiagnostics.NumberOfRepetitions", NULL },
	{ "Timeout", "Device.IP.Diagnostics.UDPEchoDiagnostics.Timeout", NULL },
	{ "DataBlockSize", "Device.IP.Diagnostics.UDPEchoDiagnostics.DataBlockSize", NULL },
	{ "DSCP", "Device.IP.Diagnostics.UDPEchoDiagnostics.DSCP", NULL },
	{ "InterTransmissionTime", "Device.IP.Diagnostics.UDPEchoDiagnostics.InterTransmissionTime", NULL },
	{ "ProtocolVersion", "Device.IP.Diagnostics.UDPEchoDiagnostics.ProtocolVersion", NULL },
	//{"EnableIndividualPacketResults","Device.IP.Diagnostics.UDPEchoDiagnostics.EnableIndividualPacketResults",NULL}
};

struct diagnostic_input nslookup_diagnostics_array[NSLKUP_NUMBER_INPUTS] = { { "Interface", "Device.DNS.Diagnostics.NSLookupDiagnostics.Interface", NULL },
									     { "HostName", "Device.DNS.Diagnostics.NSLookupDiagnostics.HostName", NULL },
									     { "DNSServer", "Device.DNS.Diagnostics.NSLookupDiagnostics.DNSServer", NULL },
									     { "NumberOfRepetitions", "Device.DNS.Diagnostics.NSLookupDiagnostics.NumberOfRepetitions", NULL },
									     { "Timeout", "Device.DNS.Diagnostics.NSLookupDiagnostics.Timeout", NULL } };

static bool set_specific_diagnostic_object_parameter_structure_value(struct diagnostic_input (*diagnostics_array)[], int number_inputs, char *parameter, char *value)
{
	int i;
	for (i = 0; i < number_inputs; i++) {
		if (strcmp((*diagnostics_array)[i].parameter_name, parameter) == 0) {
			FREE((*diagnostics_array)[i].value);
			(*diagnostics_array)[i].value = strdup(value);
			return true;
		}
	}
	return false;
}

bool set_diagnostic_parameter_structure_value(char *parameter_name, char *value) //returns false in case the parameter is not among diagnostics parameters
{
	return set_specific_diagnostic_object_parameter_structure_value(&download_diagnostics_array, DOWNLOAD_NUMBER_INPUTS, parameter_name, value) || set_specific_diagnostic_object_parameter_structure_value(&upload_diagnostics_array, UPLOAD_NUMBER_INPUTS, parameter_name, value) ||
	       set_specific_diagnostic_object_parameter_structure_value(&ipping_diagnostics_array, IPPING_NUMBER_INPUTS, parameter_name, value) || set_specific_diagnostic_object_parameter_structure_value(&nslookup_diagnostics_array, NSLKUP_NUMBER_INPUTS, parameter_name, value) ||
	       set_specific_diagnostic_object_parameter_structure_value(&traceroute_diagnostics_array, TRACEROUTE_NUMBER_INPUTS, parameter_name, value) || set_specific_diagnostic_object_parameter_structure_value(&udpecho_diagnostics_array, UDPECHO_NUMBER_INPUTS, parameter_name, value) ||
	       set_specific_diagnostic_object_parameter_structure_value(&seserverselection_diagnostics_array, SESERVERSELECT_NUMBER_INPUTS, parameter_name, value);
}

void empty_ubus_callback(struct ubus_request *req __attribute__((unused)), int type __attribute__((unused)), struct blob_attr *msg __attribute__((unused))) {}
static int cwmp_diagnostics_operate(char *diagnostics_object, char *action_name, struct diagnostic_input diagnostics_array[], int number_inputs)
{
	int e, i;

	LIST_HEAD(diagnostics_param_value_list);
	for (i = 0; i < number_inputs; i++) {
		if (diagnostics_array[i].value == NULL || diagnostics_array[i].value[0] == '\0')
			continue;
		add_dm_parameter_to_list(&diagnostics_param_value_list, diagnostics_array[i].input_name, diagnostics_array[i].value, NULL, 0, false);
	}
	e = cwmp_ubus_call("usp.raw", "operate", CWMP_UBUS_ARGS{ { "path", {.str_val = diagnostics_object }, UBUS_String }, { "action", {.str_val = action_name }, UBUS_String }, { "input", {.param_value_list = &diagnostics_param_value_list }, UBUS_Obj_Obj } }, 3, empty_ubus_callback, NULL);
	if (e)
		return -1;
	return 0;
}

int cwmp_download_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, DOWNLOAD_DIAG_ACT_NAME, download_diagnostics_array, DOWNLOAD_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_upload_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, UPLOAD_DIAG_ACT_NAME, upload_diagnostics_array, UPLOAD_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_ip_ping_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, IPPING_DIAG_ACT_NAME, ipping_diagnostics_array, IPPING_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_nslookup_diagnostics()
{
	if (cwmp_diagnostics_operate(DNS_DIAGNOSTICS_OBJECT, NSLOOKUP_DIAG_ACT_NAME, nslookup_diagnostics_array, NSLKUP_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_traceroute_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, TRACE_ROUTE_DIAG_ACT_NAME, traceroute_diagnostics_array, TRACEROUTE_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_udp_echo_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, UDPECHO_DIAG_ACT_NAME, udpecho_diagnostics_array, UDPECHO_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}

int cwmp_serverselection_diagnostics()
{
	if (cwmp_diagnostics_operate(IP_DIAGNOSTICS_OBJECT, SERVER_SELECTION_DIAG_ACT_NAME, seserverselection_diagnostics_array, SESERVERSELECT_NUMBER_INPUTS) == -1)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();
	return 0;
}
