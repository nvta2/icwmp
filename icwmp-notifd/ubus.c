/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#include "ubus.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf b;

int nubus_init(void)
{
	ubus_ctx = ubus_connect(NULL);
	if (!ubus_ctx) {
		return -1;
	}
	return 0;
}

int nubus_end(void)
{
	if (ubus_ctx) {
		ubus_free(ubus_ctx);
	}
	ubus_ctx = NULL;
	return 0;
}

static int nubus_call_req(char *path, char *method)
{
	uint32_t id;
	int r = 1;

	json_object *json_obj_out = json_object_new_object();
	if (json_obj_out == NULL)
		return r;
	blob_buf_init(&b, 0);
	if (ubus_lookup_id(ubus_ctx, path, &id))
		goto end;
	r = ubus_invoke(ubus_ctx, id, method, b.head, NULL, NULL, 1);

end:
	json_object_put(json_obj_out);
	blob_buf_free(&b);
	return r;
}

int nubus_call(char *path, char *method)
{
	int r = -1;
	nubus_init();
	if (ubus_ctx) {
		r = nubus_call_req(path, method);
		if (r > 0) r = -1;
	}
	nubus_end();
	return r;
}
