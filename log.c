/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *
 */

#include <sys/stat.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include "common.h"
#include "log.h"

static char *SEVERITY_NAMES[8] = { "[EMERG]  ", "[ALERT]  ", "[CRITIC] ", "[ERROR]  ", "[WARNING]", "[NOTICE] ", "[INFO]   ", "[DEBUG]  " };
static int log_severity = DEFAULT_LOG_SEVERITY;
static long int log_max_size = DEFAULT_LOG_FILE_SIZE;
static char log_file_name[256];
static bool enable_log_file = true;
static bool enable_log_stdout = false;
static bool enable_log_syslog = true;
static pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;

int log_set_severity_idx(char *value)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (strstr(SEVERITY_NAMES[i], value) != NULL) {
			log_severity = i;
			return 0;
		}
	}
	return 1;
}

int log_set_log_file_name(char *value)
{
	if (value != NULL) {
		CWMP_STRNCPY(log_file_name, value, sizeof(log_file_name));
	} else {
		CWMP_STRNCPY(log_file_name, DEFAULT_LOG_FILE_NAME, sizeof(log_file_name));
	}
	return 1;
}

int log_set_file_max_size(char *value)
{
	if (value != NULL) {
		log_max_size = atol(value);
	} else {
		log_max_size = 102400;
	}
	return 1;
}

int log_set_on_console(char *value)
{
	if (strcmp(value, "enable") == 0) {
		enable_log_stdout = true;
	}
	if (strcmp(value, "disable") == 0) {
		enable_log_stdout = false;
	}
	return 1;
}

int log_set_on_file(char *value)
{
	if (strcmp(value, "enable") == 0) {
		enable_log_file = true;
	}
	if (strcmp(value, "disable") == 0) {
		enable_log_file = false;
	}
	return 1;
}

int log_set_on_syslog(char *value)
{
	if ((strcasecmp(value, "TRUE") == 0) || (strcmp(value, "1") == 0) || (strcasecmp(value, "enable") == 0)) {
		enable_log_syslog = true;
	} else {
		enable_log_syslog = false;
	}
	return 1;
}

void puts_log(int severity, const char *fmt, ...)
{
	va_list args;
	int i;
	struct tm *Tm;
	struct timeval tv;
	FILE *pLog = NULL;
	struct stat st;
	long int size = 0;
	char log_file_name_bak[258];
	char buf[1024];
	char buf_file[1024];

	pthread_mutex_lock(&mutex_log);

	if (severity > log_severity) {
		goto end;
	}

	gettimeofday(&tv, 0);
	Tm = localtime(&tv.tv_sec);
	i = snprintf(buf, sizeof(buf), "%02d-%02d-%4d, %02d:%02d:%02d %s ", Tm->tm_mday, Tm->tm_mon + 1, Tm->tm_year + 1900, Tm->tm_hour, Tm->tm_min, Tm->tm_sec, SEVERITY_NAMES[severity]);
	if (strlen(log_file_name) == 0) {
		CWMP_STRNCPY(log_file_name, DEFAULT_LOG_FILE_NAME, sizeof(log_file_name));
	}
	if (enable_log_file) {
		if (stat(log_file_name, &st) == 0) {
			size = st.st_size;
		}
		if (size >= log_max_size) {
			snprintf(log_file_name_bak, sizeof(log_file_name_bak), "%s.1", log_file_name);
			rename(log_file_name, log_file_name_bak);
			pLog = fopen(log_file_name, "w");
		} else {
			pLog = fopen(log_file_name, "a+");
		}
	}
	va_start(args, fmt);
	i += vsprintf(buf + i, (const char *)fmt, args);
	if (enable_log_file) {
		CWMP_STRNCPY(buf_file, buf, sizeof(buf_file));
		buf_file[strlen(buf)] = '\n';
		buf_file[strlen(buf) + 1] = '\0';
		fputs(buf_file, pLog);
	}
	va_end(args);
	if (enable_log_file) {
		fclose(pLog);
	}
	if (enable_log_stdout) {
		puts(buf);
	}

	if (enable_log_syslog) {
		va_start(args, fmt);
		vsyslog(severity, fmt, args);
		va_end(args);
	}
end:
	pthread_mutex_unlock(&mutex_log);
}

void puts_log_xmlmsg(int severity, char *msg, int msgtype)
{
	struct tm *Tm;
	struct timeval tv;
	FILE *pLog = NULL;
	struct stat st;
	long int size = 0;
	char log_file_name_bak[258];
	char buf[1024];
	char *description, *separator;

	pthread_mutex_lock(&mutex_log);

	if (severity > log_severity) {
		goto end;
	}

	gettimeofday(&tv, 0);
	Tm = localtime(&tv.tv_sec);
	snprintf(buf, sizeof(buf), "%02d-%02d-%4d, %02d:%02d:%02d %s ", Tm->tm_mday, Tm->tm_mon + 1, Tm->tm_year + 1900, Tm->tm_hour, Tm->tm_min, Tm->tm_sec, SEVERITY_NAMES[severity]);
	if (strlen(log_file_name) == 0) {
		CWMP_STRNCPY(log_file_name, DEFAULT_LOG_FILE_NAME, sizeof(log_file_name));
	}

	if (msgtype == XML_MSG_IN) {
		description = "MESSAGE IN\n";
		separator = "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";

	} else {
		description = "MESSAGE OUT\n";
		separator = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	}
	if (enable_log_file) {
		if (stat(log_file_name, &st) == 0) {
			size = st.st_size;
		}
		if (size >= log_max_size) {
			snprintf(log_file_name_bak, sizeof(log_file_name_bak), "%s.1", log_file_name);
			rename(log_file_name, log_file_name_bak);
			pLog = fopen(log_file_name, "w");
		} else {
			pLog = fopen(log_file_name, "a+");
		}
		fputs(buf, pLog);
		fputs(description, pLog);
		fputs(separator, pLog);
		fputs(msg, pLog);
		fputs("\n", pLog);
		fputs(separator, pLog);
		fclose(pLog);
	}
	if (enable_log_stdout) {
		puts(buf);
		puts(description);
		puts(separator);
		puts(msg);
		puts("\n");
		puts(separator);
	}

	if (enable_log_syslog) {
		syslog(severity, "%s: %s", ((msgtype == XML_MSG_IN) ? "IN" : "OUT"), msg);
		if (sizeof(buf) < strlen(msg))
			syslog(severity, "Truncated message at %zu characters", strlen(msg));
	}
end:
	pthread_mutex_unlock(&mutex_log);
}
