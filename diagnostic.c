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
#include <sys/wait.h>
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
#include <libbbfdm/dmdiagnostics.h>
#endif

static int icwmpd_cmd(char *cmd, int n, ...)
{
	va_list arg;
	int i, pid;
	static int dmcmd_pfds[2];
	char *argv[n+2];

	argv[0] = cmd;

	va_start(arg,n);
	for (i=0; i<n; i++)
	{
		argv[i+1] = va_arg(arg, char*);
	}
	va_end(arg);

	argv[n+1] = NULL;

	if (pipe(dmcmd_pfds) < 0)
		return -1;

	if ((pid = fork()) == -1)
		return -1;

	if (pid == 0) {
		/* child */
		close(dmcmd_pfds[0]);
		dup2(dmcmd_pfds[1], 1);
		close(dmcmd_pfds[1]);

		execvp(argv[0], (char **) argv);
		exit(ESRCH);
	} else if (pid < 0)
		return -1;

	/* parent */
	close(dmcmd_pfds[1]);

	int status;
	while (waitpid(pid, &status, 0) != pid)
	{
		kill(pid, 0);
		if (errno == ESRCH) {
			return dmcmd_pfds[0];
		}
	}

	return dmcmd_pfds[0];
}

static int icwmpd_cmd_no_wait(char *cmd, int n, ...)
{
	va_list arg;
	int i, pid;
	static int dmcmd_pfds[2];
	char *argv[n+2];
	static char sargv[4][128];
	argv[0] = cmd;
	va_start(arg,n);
	for (i=0; i<n; i++)
	{
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

#ifndef TR098
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
		icwmpd_cmd("/bin/sh", 5, DOWNLOAD_DIAGNOSTIC_PATH, "run", "cwmp", url, interface);
		uci_get_state_value("cwmp.@downloaddiagnostic[0].DiagnosticState", &status);
		if (status && strcmp(status, "Complete") == 0)
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
		icwmpd_cmd("/bin/sh", 6, UPLOAD_DIAGNOSTIC_PATH, "run", "cwmp", url, interface, size);
		uci_get_state_value("cwmp.@uploaddiagnostic[0].DiagnosticState", &status);
		if (status && strcmp(status, "Complete") == 0)
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
