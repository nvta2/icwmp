/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#include "common.h"
#include "diagnostic.h"
#include "config.h"
#include "xml.h"
#include "datamodel_interface.h"
#include "ubus.h"
#include "cwmp_uci.h"

struct diagnostic_input
{
	char *input_name;
	char *package;
	char *section;
	char *option;
	int (*get_input_value)(char *package, char* section, char* option, char **value);
};


int get_diagnostic_value_by_uci(char *package, char* section, char* option, char **value);
int get_dm_ip_iface_object_by_uci_iface(char *package, char* section, char* option, char **dmiface);

#define DOWNLOAD_NUMBER_INPUTS 7
#define UPLOAD_NUMBER_INPUTS 8

#define DOWNLOAD_DIAG_OP_NAME "DownloadDiagnostics"
#define UPLOAD_DIAG_OP_NAME "UploadDiagnostics"

struct diagnostic_input download_diagnostics_array[DOWNLOAD_NUMBER_INPUTS]={
		{"Interface","dmmap_diagnostics","download","interface",get_dm_ip_iface_object_by_uci_iface},
		{"DownloadURL","dmmap_diagnostics","download","url",get_diagnostic_value_by_uci},
		{"DSCP","dmmap_diagnostics","download","DSCP",get_diagnostic_value_by_uci},
		{"EthernetPriority","dmmap_diagnostics","download","ethernetpriority",get_diagnostic_value_by_uci},
		{"ProtocolVersion","dmmap_diagnostics","download","ProtocolVersion",get_diagnostic_value_by_uci},
		{"NumberOfConnections","dmmap_diagnostics","download","NumberOfConnections",get_diagnostic_value_by_uci},
		{"EnablePerConnectionResults","dmmap_diagnostics","download","EnablePerConnection",get_diagnostic_value_by_uci}
};

struct diagnostic_input upload_diagnostics_array[UPLOAD_NUMBER_INPUTS]={
		{"Interface","dmmap_diagnostics","upload","interface",get_dm_ip_iface_object_by_uci_iface},
		{"UploadURL","dmmap_diagnostics","upload","url",get_diagnostic_value_by_uci},
		{"TestFileLength","dmmap_diagnostics","upload","TestFileLength",get_diagnostic_value_by_uci},
		{"DSCP","dmmap_diagnostics","upload","DSCP",get_diagnostic_value_by_uci},
		{"EthernetPriority","dmmap_diagnostics","upload","ethernetpriority",get_diagnostic_value_by_uci},
		{"ProtocolVersion","dmmap_diagnostics","upload","ProtocolVersion",get_diagnostic_value_by_uci},
		{"NumberOfConnections","dmmap_diagnostics","upload","NumberOfConnections",get_diagnostic_value_by_uci},
		{"EnablePerConnectionResults","dmmap_diagnostics","upload","EnablePerConnection",get_diagnostic_value_by_uci}
};

static int icwmpd_cmd_no_wait(char *cmd, int n, ...)
{
	va_list arg;
	int i, pid;
	char *argv[n+2];
	static char sargv[4][128];

	argv[0] = cmd;

	va_start(arg, n);
	for (i = 0; i < n; i++) {
		strcpy(sargv[i], va_arg(arg, char*));
		argv[i+1] = sargv[i];
	}
	va_end(arg);

	argv[n+1] = NULL;

	if ((pid = fork()) == -1)
		return -1;

	if (pid == 0) {
		execvp(argv[0], (char **) argv);
		exit(ESRCH);
	} else if (pid < 0)
		return -1;

	return 0;
}

int get_diagnostic_value_by_uci(char *package, char* section, char* option, char **value)
{
	if(cwmp_uci_get_option_value_string(package, section, option, UCI_BBFDM_CONFIG, value) != UCI_OK)
		return -1;
	return 0;
}

int get_dm_ip_iface_object_by_uci_iface(char *package, char* section, char* option, char **dmiface)
{

	struct uci_section *s = NULL;
	char *iface = NULL, *ip_inst = NULL;

	if(cwmp_uci_get_option_value_string(package, section, option, UCI_BBFDM_CONFIG, &iface) != UCI_OK)
		return -1;
	cwmp_uci_init(UCI_BBFDM_CONFIG);
	cwmp_uci_path_foreach_option_eq("dmmap_network", "interface", "section_name", iface, s) {
		cwmp_uci_get_value_by_section_string(s, "ip_int_instance", &ip_inst);
		asprintf(dmiface, "Device.IP.Interface.%s.", ip_inst);
		break;
	}
	cwmp_uci_exit();
	return 0;
}

int cwmp_start_diagnostic(int diagnostic_type)
{
	int e, i, number_inputs;
	char *value = NULL, *operate_name = NULL;

	struct diagnostic_input *diagnostics_array;
	if (diagnostic_type == DOWNLOAD_DIAGNOSTIC) {
		diagnostics_array = download_diagnostics_array;
		number_inputs = DOWNLOAD_NUMBER_INPUTS;
		operate_name = strdup(DOWNLOAD_DIAG_OP_NAME);
	} else {
		diagnostics_array = upload_diagnostics_array;
		number_inputs = UPLOAD_NUMBER_INPUTS;
		operate_name = strdup(UPLOAD_DIAG_OP_NAME);
	}
	LIST_HEAD(diagnostics_param_value_list);
	for (i=0; i<number_inputs; i++) {
		if (diagnostics_array[i].get_input_value(diagnostics_array[i].package, diagnostics_array[i].section, diagnostics_array[i].option, &value) != 0)
			continue;
		cwmp_add_list_param_value(diagnostics_array[i].input_name, value, &diagnostics_param_value_list);
		FREE(value);
	}

	e = cwmp_ubus_call("usp.raw", "operate", CWMP_UBUS_ARGS{{"path", {.str_val="Device.IP.Diagnostics."}, UBUS_String},{"action", {.str_val=operate_name}, UBUS_String},{"input", {.param_value_list=&diagnostics_param_value_list}, UBUS_Obj_Obj}}, 3, &old_global_json_obj);
	if(e)
		return -1;
	cwmp_root_cause_event_ipdiagnostic();

	return 0;
}

int cwmp_ip_ping_diagnostic()
{
	icwmpd_cmd_no_wait("/bin/sh", 3, IPPING_PATH, "run", "cwmp");
    return 0;
}

int cwmp_nslookup_diagnostic()
{
	icwmpd_cmd_no_wait("/bin/sh", 3, NSLOOKUP_PATH, "run", "cwmp");
    return 0;
}

int cwmp_traceroute_diagnostic()
{
	icwmpd_cmd_no_wait("/bin/sh", 3, TRACEROUTE_PATH, "run", "cwmp");
    return 0;
}

int cwmp_udp_echo_diagnostic()
{
	icwmpd_cmd_no_wait("/bin/sh", 3, UDPECHO_PATH, "run", "cwmp");
    return 0;
}

int cwmp_serverselection_diagnostic()
{
	icwmpd_cmd_no_wait("/bin/sh", 3, SERVERSELECTION_PATH, "run", "cwmp");
    return 0;
}
