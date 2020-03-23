/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#include "times.h"

char *bulkdata_get_time(void)
{
	static char local_time[64];
	time_t t_time;
	struct tm *t_tm;

	t_time = time(NULL);
	t_tm = localtime(&t_time);
	if (t_tm == NULL)
		return NULL;

	if(strftime(local_time, sizeof(local_time),"Date: %a, %d %b %Y %X%z GMT", t_tm) == 0)
		return NULL;

	return local_time;
}

void get_time_stamp(char *format, char **timestamp)
{
	struct tm *ts;
	char buf[32];
	time_t now = time(NULL);

	if(strcmp(format, "unix") == 0) {
		asprintf(timestamp, "%ld", now);
	} else if(strcmp(format, "iso8601") == 0) {
		ts = localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%Z", ts);
		asprintf(timestamp, "%s", buf);
	} else
		timestamp = NULL;
}

unsigned int get_next_period(time_t time_reference, int reporting_interval)
{
	unsigned int next_period;
	time_t now = time(NULL);

	if (now > time_reference)
		next_period = reporting_interval - ((now - time_reference) % reporting_interval);
	else
		next_period = (time_reference - now) % reporting_interval;

	if (next_period == 0)
		next_period = reporting_interval;

	return next_period;
}
