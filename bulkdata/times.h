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

#ifndef __TIMES_H
#define __TIMES_H

char *bulkdata_get_time(void);
void get_time_stamp(char *format, char **timestamp);
unsigned int get_next_period(time_t time_reference, int reporting_interval);

#endif /* __TIMES_H */
