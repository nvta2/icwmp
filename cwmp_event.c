/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include "cwmp_event.h"
#include "common.h"
#include "session.h"

struct event_container *__cwmp_add_event_container(int event_code, char *command_key)
{
	static int id;
	struct event_container *event_container;
	list_for_each_entry(event_container, &cwmp_main->session->events, list) {
		if (event_container->code == event_code && EVENT_CONST[event_code].TYPE == EVENT_TYPE_SINGLE) {
			return event_container;
		}
		if (event_container->code > event_code) {
			break;
		}
	}
	event_container = calloc(1, sizeof(struct event_container));
	if (event_container == NULL) {
		return NULL;
	}
	INIT_LIST_HEAD(&(event_container->head_dm_parameter));
	list_add_tail(&(event_container->list), &(cwmp_main->session->events));
	event_container->code = event_code;
	event_container->command_key = command_key ? strdup(command_key) : strdup("");
	if ((id < 0) || (id >= MAX_INT_ID)) {
		id = 0;
	}
	id++;
	event_container->id = id;
	return event_container;
}

struct event_container *cwmp_add_event_container(int event_code, char *command_key)
{
	pthread_mutex_lock(&add_event_mutext);
	struct event_container *event = __cwmp_add_event_container(event_code, command_key);
	pthread_mutex_unlock(&add_event_mutext);
	return event;
}
