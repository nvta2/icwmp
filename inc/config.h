/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013 Inteno Broadband Technology AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Anis Ellouze <anis.ellouze@pivasoftware.com>
 *
 */

#ifndef _CONFIG_H__
#define _CONFIG_H__


int uci_get_state_value(char *cmd,char **value);
int uci_set_state_value(char *cmd);
int save_acs_bkp_config(struct cwmp *cwmp);
int uci_get_value(char *cmd,char **value);
int get_amd_version_config();
int get_instance_mode_config();
int get_session_timeout_config();

#endif
