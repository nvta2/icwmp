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

int global_conf_init();
int get_global_config();
int cwmp_config_reload();
int reload_networking_config();

#endif
