/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#include <uci.h>
#include "udpechoserveruci.h"

static struct uci_context *uci_ctx = NULL;
static struct uci_context *uci_ctx_state = NULL;

int dmuci_init(void)
{
	uci_ctx = uci_alloc_context();
	if (!uci_ctx) {
		return -1;
	}
	uci_ctx_state = uci_alloc_context();
	if (!uci_ctx_state) {
		return -1;
	}
	uci_add_delta_path(uci_ctx_state, uci_ctx_state->savedir);
	uci_set_savedir(uci_ctx_state, VAR_STATE);
	return 0;
}

int dmuci_fini(void)
{
	if (uci_ctx) {
		uci_free_context(uci_ctx);
	}
	if (uci_ctx_state) {
		uci_free_context(uci_ctx_state);
	}
	uci_ctx = NULL;
	uci_ctx_state = NULL;
	return 0;
}

static bool dmuci_validate_section(const char *str)
{
	if (!*str)
		return false;

	for (; *str; str++) {
		unsigned char c = *str;

		if (isalnum(c) || c == '_')
			continue;

		return false;
	}
	return true;
}

static int dmuci_init_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value)
{
	char *last = NULL;
	char *tmp;

	memset(ptr, 0, sizeof(struct uci_ptr));

	/* value */
	if (value) {
		ptr->value = value;
	}
	ptr->package = package;
	if (!ptr->package)
		goto error;

	ptr->section = section;
	if (!ptr->section) {
		ptr->target = UCI_TYPE_PACKAGE;
		goto lastval;
	}

	ptr->option = option;
	if (!ptr->option) {
		ptr->target = UCI_TYPE_SECTION;
		goto lastval;
	} else {
		ptr->target = UCI_TYPE_OPTION;
	}

lastval:
	if (ptr->section && !dmuci_validate_section(ptr->section))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	return 0;

error:
	return -1;
}

void dmuci_print_list(struct uci_list *uh, char **val, char *delimiter)
{
	struct uci_element *e;
	static char buffer[512];
	int dlen = strlen(delimiter);
	int olen = 0;
	char *buf = buffer;
	*buf = '\0';

	uci_foreach_element(uh, e) {
		if (*buf) {
			strcat(buf, delimiter);
			strcat(buf, e->name);
		}
		else {
			strcpy(buf, e->name);
		}
	}
	*val = buf;
}

char *dmuci_get_value(char *package, char *section, char *option)
{
	struct uci_ptr ptr;
	char *val = "";

	if (!section || !option)
		return val;

	if (dmuci_init_ptr(uci_ctx, &ptr, package, section, option, NULL)) {
		return val;
	}
	if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
		return val;
	}

	if (!ptr.o)
		return val;

	if(ptr.o->type == UCI_TYPE_LIST) {
		dmuci_print_list(&ptr.o->v.list, &val, " ");
		return val;
	}

	if (ptr.o->v.string)
		return ptr.o->v.string;
	else
		return val;
}

char *dmuci_set_value(char *package, char *section, char *option, char *value)
{
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (!section)
		return "";

	if (dmuci_init_ptr(uci_ctx, &ptr, package, section, option, value)) {
		return "";
	}
	if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
		return "";
	}

	uci_set(uci_ctx, &ptr);

	if (ret == UCI_OK)
		ret = uci_save(uci_ctx, ptr.p);

	if (ptr.o && ptr.o->v.string)
		return ptr.o->v.string;

	return "";
}

/************************* /var/state ***************************/
char *dmuci_set_value_state(char *package, char *section, char *option, char *value)
{
	char *val;
	struct uci_context *save_uci_ctx = uci_ctx;
	uci_ctx = uci_ctx_state;
	val = dmuci_set_value(package, section, option, value);
	uci_ctx = save_uci_ctx;
	return val;
}
