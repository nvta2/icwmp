/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef CWMP_DU_STATE_H
#define CWMP_DU_STATE_H

int cwmp_du_install(char *url, char *uuid, char *user, char *pass, char *env, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code);
int cwmp_du_update(char *url, char *uuid, char *user, char *pass, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code);
int cwmp_du_uninstall(char *package_name, char *package_env, char **fault_code);

#endif
