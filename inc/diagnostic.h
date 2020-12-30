/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 */

#ifndef __DIAGNOSTIC__H
#define __DIAGNOSTIC__H

bool set_diagnostic_parameter_structure_value(char *parameter_name, char* value);

int cwmp_download_diagnostics();
int cwmp_upload_diagnostics();
int cwmp_ip_ping_diagnostics();
int cwmp_nslookup_diagnostics();
int cwmp_traceroute_diagnostics();
int cwmp_udp_echo_diagnostics();
int cwmp_serverselection_diagnostics();

#endif
