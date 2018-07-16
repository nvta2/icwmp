/*	TR-069 STUN  client software
 *	Copyright (C) 2018 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef __SUBUS_H
#define __SUBUS_H

#include <libubox/blobmsg_json.h>
#include <json-c/json.h>
#include <libubus.h>

struct sarg {
	char *key;
	char *val;
};

#define UBUS_ARGS (struct sarg[])

int subus_call(char *path, char *method, int argc, struct sarg sarg[]);

#endif //__SUBUS_H
