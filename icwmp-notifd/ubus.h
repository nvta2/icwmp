/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#ifndef __PUBUS_H
#define __PUBUS_H

#include <libubox/blobmsg_json.h>
#include <json-c/json.h>
#include <libubus.h>

struct parg {
	char *key;
	char *val;
};

#define UBUS_ARGS (struct parg[])

int pubus_call(char *path, char *method, int argc, struct parg sarg[]);

#endif //__SUBUS_H
