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

int nubus_call(char *path, char *method);

#endif //__NUBUS_H
