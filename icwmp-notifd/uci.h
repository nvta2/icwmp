/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#ifndef __PUCI_H
#define __PUCI_H
#include <uci.h>
#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmbbfcommon.h>
bool check_section_name(const char *str, bool name);
int dmuci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value);
#define UCI_OPTION_CPE_NOTIF_POLLING_PERIOD "cwmp.cpe.polling_period"
#endif
