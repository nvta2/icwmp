/*
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
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
#include <strophe.h>
#include "xmpplog.h"
#include "xmpp.h"
#define DEBUG

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

void cwmp_xmpp_log(int priority, const char *format, ...)
{
	va_list vl;

	if (priority <= cur_xmpp_conf.xmpp_loglevel) {
#ifdef DEBUG
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		va_start(vl, format);
		printf("%d-%02d-%02d %02d:%02d:%02d [xmpp] %s - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, log_str[priority]);
		vprintf(format, vl);
		va_end(vl);
		printf("\n");
#endif
		openlog("xmpp", 0, LOG_DAEMON);
		va_start(vl, format);
		vsyslog(log_syslogmap[priority], format, vl);
		va_end(vl);
		closelog();
	}
}

void cwmp_xmpp_log_handler(void * const userdata,
			 const xmpp_log_level_t level,
			 const char * const area,
			 const char * const msg)
{
	int priority = SDEBUG;
	xmpp_log_level_t filter_level = * (xmpp_log_level_t*)userdata;
	if( level >= filter_level )
	{
		switch( level )
		{
		case XMPP_LEVEL_ERROR:	priority = SCRIT; break;
		case XMPP_LEVEL_WARN:	priority = SWARNING; break;
		case XMPP_LEVEL_INFO:	priority = SINFO; break;
		case XMPP_LEVEL_DEBUG:	priority = SDEBUG; break;
		}
		cwmp_xmpp_log( priority, "%s %s", area, msg );
	}
}

xmpp_log_level_t xmpp_log_get_level(int conf_loglevel)
{
	xmpp_log_level_t xmpp_level = XMPP_LEVEL_DEBUG;
	switch( conf_loglevel )
	{
	case SCRIT:		xmpp_level = XMPP_LEVEL_ERROR; break;
	case SWARNING:	xmpp_level = XMPP_LEVEL_WARN; break;
	case SINFO:		xmpp_level = XMPP_LEVEL_INFO; break;
	case SNOTICE:	xmpp_level = XMPP_LEVEL_INFO; break;
	case SDEBUG:	xmpp_level = XMPP_LEVEL_DEBUG; break;
	}
	return xmpp_level;
}
