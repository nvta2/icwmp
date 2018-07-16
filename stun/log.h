/*	TR-069 STUN  client software
 *	Copyright (C) 2018 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef __SLOG_H
#define __SLOG_H

enum stun_log_level_enum {
	SCRIT,
	SWARNING,
	SNOTICE,
	SINFO,
	SDEBUG,
	__MAX_SLOG
};

void stun_log(int priority, const char *format, ...);

#endif //__SLOG_H
