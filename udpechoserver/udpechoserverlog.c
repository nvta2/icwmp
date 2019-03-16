/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#include <time.h>
#include <syslog.h>
#include <stdarg.h>
#include "udpechoserverlog.h"
#include "udpechoserver.h"
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

void udpechoserver_log(int priority, const char *format, ...)
{
	va_list vl;
	char *log;

	if (priority <= cur_udpechoserver_conf.loglevel) {
#ifdef DEBUG
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		va_start(vl, format);
		printf("%d-%02d-%02d %02d:%02d:%02d [udpechoserver] %s - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, log_str[priority]);
		vprintf(format, vl);
		va_end(vl);
		printf("\n");
#endif
		openlog("udpechoserver", 0, LOG_DAEMON);
		va_start(vl, format);
		vsyslog(log_syslogmap[priority], format, vl);
		va_end(vl);
		closelog();
	}
}
