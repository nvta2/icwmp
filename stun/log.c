/*
 * log.c : conatains function used log traces
 *
 * TR-069 STUN  client software
 *	Copyright (C) 2018 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>

#include "log.h"
#include "config.h"

static const char* log_str[] = {
	[SCRIT] = "CRITICAL",
	[SWARNING] = "WARNING",
	[SNOTICE] = "NOTICE",
	[SINFO] = "INFO",
	[SDEBUG] = "DEBUG"
};

void stun_log(int priority, const char *format, ...)
{
	va_list vl;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	va_start(vl, format);
	printf("%d-%02d-%02d %02d:%02d:%02d [icwmp_stund] %s - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, log_str[priority]);
	vprintf(format, vl);
	va_end(vl);
	printf("\n");
}
