/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Anis Ellouze <anis.ellouze@pivasoftware.com>
 *
 */

#ifndef _CONFIG_H__
#define _CONFIG_H__

#include <pthread.h>
#include "common.h"

extern pthread_mutex_t  mutex_config_load;

int get_amd_version_config();
int get_instance_mode_config();
int get_session_timeout_config();
int cwmp_config_reload(struct cwmp *cwmp);
int global_conf_init (struct config *conf);
int cwmp_get_deviceid(struct cwmp *cwmp);
#endif
