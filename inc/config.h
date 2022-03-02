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

extern pthread_mutex_t mutex_config_load;

int global_conf_init(struct cwmp *cwmp);
int get_global_config(struct config *conf);
int cwmp_get_deviceid(struct cwmp *cwmp);
int cwmp_config_reload(struct cwmp *cwmp);
int reload_networking_config();
#endif
