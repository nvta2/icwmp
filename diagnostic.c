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

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "cwmp.h"
#include "backupSession.h"
#include "xml.h"
#include "log.h"
#include "external.h"
#include "ubus.h"
#include "diagnostic.h"
#include "config.h"
#ifdef TR098
#include <libtr098/dmentry.h>
#include <libtr098/dmcommon.h>
#else
#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmcommon.h>
#include <libbbfdm/dmdiagnostics.h>

int cwmp_start_diagnostic(int diagnostic_type)
{
	char *url = NULL;
	char *interface = NULL;
	char *size = NULL;
	int error;
	char *status;

	if (diagnostic_type == DOWNLOAD_DIAGNOSTIC) {
		uci_get_state_value("cwmp.@downloaddiagnostic[0].url", &url);
		uci_get_state_value("cwmp.@downloaddiagnostic[0].device", &interface);
	}
	else
	{
		uci_get_state_value("cwmp.@uploaddiagnostic[0].url", &url);
		uci_get_state_value("cwmp.@uploaddiagnostic[0].TestFileLength", &size);
		uci_get_state_value("cwmp.@uploaddiagnostic[0].device", &interface);
	}
	if( url == NULL || ((url != NULL) && (strcmp(url,"")==0))
		|| ((strncmp(url,DOWNLOAD_PROTOCOL_FTP,strlen(DOWNLOAD_PROTOCOL_FTP))!=0) &&
		(strstr(url,"@") != NULL && strncmp(url,DOWNLOAD_PROTOCOL_HTTP,strlen(DOWNLOAD_PROTOCOL_HTTP)) == 0))
	)
	{
		CWMP_LOG(ERROR,"Invalid URL %s", url);
		free(url);
		return -1;
	}

	if ( interface == NULL || interface[0] == '\0' )
	{
		error = get_default_gateway_device(&interface);
		if (error == -1)
		{
			CWMP_LOG(ERROR,"Interface value: Empty");
			free(interface);
			return -1;
		}
	}
	if (diagnostic_type == DOWNLOAD_DIAGNOSTIC)
	{
		CWMP_LOG(INFO,"Launch Download diagnostic with url %s", url);
		dmcmd("/bin/sh", 5, DOWNLOAD_DIAGNOSTIC_PATH, "run", "cwmp", url, interface);
		uci_get_state_value("cwmp.@downloaddiagnostic[0].DiagnosticState", &status);
		if (status && strcmp(status, "Completed") == 0)
		{
			init_download_stats();
			CWMP_LOG(INFO,"Extract Download stats");
			if(strncmp(url,DOWNLOAD_PROTOCOL_HTTP,strlen(DOWNLOAD_PROTOCOL_HTTP)) == 0)
				extract_stats(DOWNLOAD_DUMP_FILE, DOWNLOAD_DIAGNOSTIC_HTTP, DOWNLOAD_DIAGNOSTIC, "cwmp");
			if(strncmp(url,DOWNLOAD_PROTOCOL_FTP,strlen(DOWNLOAD_PROTOCOL_FTP)) == 0)
				extract_stats(DOWNLOAD_DUMP_FILE, DOWNLOAD_DIAGNOSTIC_FTP, DOWNLOAD_DIAGNOSTIC, "cwmp");
			cwmp_root_cause_event_ipdiagnostic();
		}
		else if (status && strncmp(status, "Error_", strlen("Error_")) == 0)
			cwmp_root_cause_event_ipdiagnostic();
		free(status);
	}
	else
	{
		CWMP_LOG(INFO,"Launch Upload diagnostic with url %s", url);
		dmcmd("/bin/sh", 6, UPLOAD_DIAGNOSTIC_PATH, "run", "cwmp", url, interface, size);
		uci_get_state_value("cwmp.@uploaddiagnostic[0].DiagnosticState", &status);
		if (status && strcmp(status, "Completed") == 0)
		{
			init_upload_stats();
			CWMP_LOG(INFO,"Extract Upload stats");
			if(strncmp(url,DOWNLOAD_PROTOCOL_HTTP,strlen(DOWNLOAD_PROTOCOL_HTTP)) == 0)
				extract_stats(UPLOAD_DUMP_FILE, DOWNLOAD_DIAGNOSTIC_HTTP, UPLOAD_DIAGNOSTIC, "cwmp");
			if(strncmp(url,DOWNLOAD_PROTOCOL_FTP,strlen(DOWNLOAD_PROTOCOL_FTP)) == 0)
				extract_stats(UPLOAD_DUMP_FILE, DOWNLOAD_DIAGNOSTIC_FTP, UPLOAD_DIAGNOSTIC, "cwmp");
			cwmp_root_cause_event_ipdiagnostic();
		}
		else if (status && strncmp(status, "Error_", strlen("Error_")) == 0)
			cwmp_root_cause_event_ipdiagnostic();
		free(status);
		free(size);
	}
	free(url);
	free(interface);
	return 0;
}
#endif

int cwmp_ip_ping_diagnostic()
{
	dmcmd_no_wait("/bin/sh", 3, IPPING_PATH, "run", "cwmp");
    return 0;
}

int cwmp_nslookup_diagnostic()
{
	dmcmd_no_wait("/bin/sh", 3, NSLOOKUP_PATH, "run", "cwmp");
    return 0;
}

int cwmp_traceroute_diagnostic()
{
	dmcmd_no_wait("/bin/sh", 3, TRACEROUTE_PATH, "run", "cwmp");
    return 0;
}

int cwmp_udp_echo_diagnostic()
{
	dmcmd_no_wait("/bin/sh", 3, UDPECHO_PATH, "run", "cwmp");
    return 0;
}

int cwmp_serverselection_diagnostic()
{
	dmcmd_no_wait("/bin/sh", 3, SERVERSELECTION_PATH, "run", "cwmp");
    return 0;
}
