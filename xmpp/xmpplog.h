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

#ifndef _XMPPLOG_H_
#define _XMPPLOG_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_LOGLEVEL SINFO

enum xmpp_log_level_enum {
	SCRIT,
	SWARNING,
	SNOTICE,
	SINFO,
	SDEBUG,
	__MAX_SLOG
};

void xmpp_log(int priority, const char *format, ...);

#endif /* _XMPPLOG_H_ */
