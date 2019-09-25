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

#ifndef __LOG_H
#define __LOG_H

#define DEFAULT_LOGLEVEL SINFO

enum bulkdata_log_level_enum {
	SCRIT,
	SWARNING,
	SNOTICE,
	SINFO,
	SDEBUG,
	__MAX_SLOG
};

void bulkdata_log(int priority, const char *format, ...);

#endif //__LOG_H
