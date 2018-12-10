/*
 *  ubus.c -- This file conatains functions that allow to make ubus calls
 *
 *
 *  Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 *  Author: Omar Kallel <omar.kallel@pivasoftware.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 *  02110-1301 USA
 *
 */

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <json-c/json.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "ubus.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf b;

int subus_init(void)
{
	ubus_ctx = ubus_connect(NULL);
	if (!ubus_ctx) {
		return -1;
	}
	return 0;
}

int subus_fini(void)
{
	if (ubus_ctx) {
		ubus_free(ubus_ctx);
	}
	ubus_ctx = NULL;
	return 0;
}

void sadd_json_obj(json_object *json_obj_out, char *object, char *string)
{
	json_object *json_obj_tmp = json_object_new_string(string);
	json_object_object_add(json_obj_out, object, json_obj_tmp);
}

static int subus_call_req(char *path, char *method, int argc, struct sarg sarg[])
{
	uint32_t id;
	int i, r = 1;
	char *arg;

	json_object *json_obj_out = json_object_new_object();
	if (json_obj_out == NULL)
		return r;

	blob_buf_init(&b, 0);

	if (argc) {
		for (i = 0; i < argc; i++) {
			sadd_json_obj(json_obj_out, sarg[i].key, sarg[i].val);
		}
		arg = (char *)json_object_to_json_string(json_obj_out);

		if (!blobmsg_add_json_from_string(&b, arg)) {
			goto end;
		}
	}

	if (ubus_lookup_id(ubus_ctx, path, &id))
		goto end;

	r = ubus_invoke(ubus_ctx, id, method, b.head, NULL, NULL, 1);

end:
	json_object_put(json_obj_out);
	blob_buf_free(&b);
	return r;
}

int subus_call(char *path, char *method, int argc, struct sarg dmarg[])
{
	int r = -1;
	subus_init();
	if (ubus_ctx) {
		r = subus_call_req(path, method, argc, dmarg);
		if (r > 0) r = -1;
	}
	subus_fini();
	return r;
}
