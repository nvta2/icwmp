/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/


#include <stdio.h>
#include <libubox/uloop.h>

#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmbbfcommon.h>

#include "ubus.h"
#include "uci.h"

#define DM_ENABLED_NOTIFY "/etc/icwmpd/.dm_enabled_notify"
static void parse_icwmp_active_notifications(struct uloop_timeout *timeout);
static struct uloop_timeout active_notif_timer = { .cb = parse_icwmp_active_notifications };
struct uci_context *uci_ctx;
char *polling_period = NULL;

static void polling_parse_icwmp_active_notifications()
{
	struct uci_element *e, *tmp;
	struct uci_list *list_notif;
	LIST_HEAD(listnotif);
	list_notif = &listnotif;
	struct uci_ptr ptr = {0};
	char *parameter = NULL, *notification = NULL, *value = NULL, *jval;
	struct dmctx dmctx = {0};
	FILE *fp = NULL;
	char buf[512];
	int fault;
	struct dm_parameter *dm_parameter;

	uci_ctx = uci_alloc_context();
	if (dmuci_lookup_ptr(uci_ctx, &ptr, "cwmp", "@notifications[0]", "active", NULL)) {
		goto end;
	}

	if (ptr.o && ptr.o->type == UCI_TYPE_LIST)
		list_notif = &ptr.o->v.list;

	uci_free_context(uci_ctx);
	if (!list_notif || uci_list_empty(list_notif))
		goto end;
	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL)
		return;

	while (fgets(buf, 512, fp) != NULL) {
		dm_ctx_init(&dmctx, DM_CWMP, 5, INSTANCE_MODE_NUMBER);
		int len = strlen(buf);
		if (len)
			buf[len-1] = '\0';

		dmjson_parse_init(buf);
		dmjson_get_var("parameter", &jval);
		parameter = strdup(jval);
		dmjson_get_var("value", &jval);
		value = strdup(jval);
		dmjson_get_var("notification", &jval);
		notification = strdup(jval);
		dmjson_parse_fini();

		if (list_notif && !uci_list_empty(list_notif)) {
			uci_foreach_element(list_notif, e) {
				if (e->name && strcmp(e->name, parameter) == 0) {
					fault = dm_entry_param_method(&dmctx, CMD_GET_VALUE, parameter, NULL, NULL);
					if (!fault && dmctx.list_parameter.next != &dmctx.list_parameter) {
						dm_parameter = list_entry(dmctx.list_parameter.next, struct dm_parameter, list);
						if (strcmp(dm_parameter->data, value) != 0) {
							if (pubus_call("tr069", "notify", 0, UBUS_ARGS{}) < 0){
								continue;
							}
						}
					}
				}
			}
		}
		if(parameter) {
			free(parameter);
			parameter = NULL;
		}
		if(notification) {
			free(notification);
			notification = NULL;
		}
		if(value) {
			free(value);
			value = NULL;
		}
	}
	end:
		uloop_timeout_set(&active_notif_timer, polling_period && atoi(polling_period)?atoi(polling_period)*1000:5000);
}

static void parse_icwmp_active_notifications(struct uloop_timeout *timeout) {
	polling_parse_icwmp_active_notifications();
}

int main() {
	load_uci_config(&polling_period);
	uloop_init();
	polling_parse_icwmp_active_notifications();
	uloop_run();
	uloop_done();
	return 0;
}
