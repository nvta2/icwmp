/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 *
 */

#ifndef _FREECWMP_EXTERNAL_H__
#define _FREECWMP_EXTERNAL_H__
#include <libubox/list.h>

#ifdef DUMMY_MODE
static char *fc_script = "./ext/openwrt/scripts/icwmp.sh";
#else
static char *fc_script = "/usr/sbin/icwmp";
#endif

void external_du_change_stateFaultResp (char *fault_code, char *version, char *name);
void external_downloadFaultResp (char *fault_code);
void external_fetch_downloadFaultResp (char **fault_code);
void external_uploadFaultResp (char *fault_code);
void external_fetch_uploadFaultResp (char **fault_code);
void external_uninstallFaultResp (char *fault_code);
void external_fetch_uninstallFaultResp (char **fault);
int external_simple(char *command, char *arg, int c);
int external_download(char *url, char *size, char *type, char *user, char *pass,time_t c);
int external_upload(char *url, char *type, char *user, char *pass, char *name);
int external_apply(char *action, char *arg,time_t c);
int external_change_du_state_uninstall(char *package_name);
void external_fetch_du_change_stateFaultResp (char **fault, char **version, char **name);
int external_change_du_state_download(char *url, char *user, char *pass);
int external_handle_action(int (*external_handler)(char *msg));
void external_add_list_paramameter(char *param_name, char *param_data, char *param_type, char *fault_code);
void external_free_list_parameter();
void external_init();
void external_exit();

#endif

