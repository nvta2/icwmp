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

int cwmp_start_diagnostic(int diagnostic_type)
{
	start_upload_download_diagnostic(diagnostic_type, "cwmp");
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
