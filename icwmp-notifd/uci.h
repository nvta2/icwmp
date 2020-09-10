/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#ifndef __NUCI_H
#define __NUCI_H
#include <uci.h>
#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmbbfcommon.h>
#define UCI_OPTION_CPE_NOTIF_POLLING_PERIOD "cwmp.cpe.polling_period"
void nuci_init();
void nuci_end();
void load_uci_config(char **polling_period);
int nuci_get_list(char *package, char *section, char *list_opt, struct uci_list** list_val); //, struct uci_list *list_val
#endif
