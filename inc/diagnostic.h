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

int cwmp_ip_ping_diagnostic();
#ifndef TR098
int cwmp_start_diagnostic(int diagnostic_type);
#endif
int cwmp_nslookup_diagnostic();
int cwmp_traceroute_diagnostic();
int cwmp_udp_echo_diagnostic();
int cwmp_serverselection_diagnostic();

#endif
