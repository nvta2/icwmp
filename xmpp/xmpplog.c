/*
 * Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 * Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include "xmpplog.h"
#include "xmpp.h"

static const int log_syslogmap[] = {
	[SCRIT] = LOG_CRIT,
	[SWARNING] = LOG_WARNING,
	[SNOTICE] = LOG_NOTICE,
	[SINFO] = LOG_INFO,
	[SDEBUG] = LOG_DEBUG
};

static const char* log_str[] = {
	[SCRIT] = "CRITICAL",
	[SWARNING] = "WARNING",
	[SNOTICE] = "NOTICE",
	[SINFO] = "INFO",
	[SDEBUG] = "DEBUG"
};

void xmpp_log(int priority, const char *format, ...)
{
	va_list vl;
	char *log;

	if (priority <= cur_xmpp_conf.xmpp_loglevel) {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		va_start(vl, format);
		printf("%d-%02d-%02d %02d:%02d:%02d [xmpp] %s - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, log_str[priority]);
		vprintf(format, vl);
		va_end(vl);
		printf("\n");
		openlog("xmpp", 0, LOG_DAEMON);
		va_start(vl, format);
		vsyslog(log_syslogmap[priority], format, vl);
		va_end(vl);
		closelog();
	}
}
