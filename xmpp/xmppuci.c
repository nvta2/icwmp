/*
 * Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 * Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <uci.h>
#include "xmppuci.h"

struct uci_context *uci_ctx;

int dmuci_init(void)
{
	uci_ctx = uci_alloc_context();
	if (!uci_ctx) {
		return -1;
	}
	return 0;
}

int dmuci_fini(void)
{
	if (uci_ctx) {
		uci_free_context(uci_ctx);
	}

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

struct uci_section *dmuci_walk_section(char *package, char *section_type, struct uci_section *prev_section)
{
	struct uci_ptr ptr;
	struct uci_element *e;
	struct uci_section *next_section;

	if (section_type == NULL) {
		if (prev_section) {
			e = &prev_section->e;
			if (e->list.next == &prev_section->package->sections)
				return NULL;
			e = container_of(e->list.next, struct uci_element, list);
			next_section = uci_to_section(e);
			return next_section;
		}
		else {
			if (dmuci_init_ptr(uci_ctx, &ptr, package, NULL, NULL, NULL)) {
				return NULL;
			}
			if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
				return NULL;
			}
			if (ptr.p->sections.next == &ptr.p->sections)
				return NULL;
			e = container_of(ptr.p->sections.next, struct uci_element, list);
			next_section = uci_to_section(e);

			return next_section;
		}
	}
	else {
		struct uci_list *ul, *u = NULL;
		struct uci_list *shead = NULL;

		if (prev_section) {
			ul = &prev_section->e.list;
			shead = &prev_section->package->sections;
		}
		else {
			if (dmuci_init_ptr(uci_ctx, &ptr, package, NULL, NULL, NULL)) {
				return NULL;
			}
			if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
				return NULL;
			}
			ul = &ptr.p->sections;
			shead = &ptr.p->sections;
		}
		while (ul->next != shead) {
			e = container_of(ul->next, struct uci_element, list);
			next_section = uci_to_section(e);
			if (strcmp(next_section->type, section_type) == 0)
				return next_section;
			ul = ul->next;
		}
		return NULL;
	}
	return NULL;
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

struct uci_element *dmuci_lookup_list(struct uci_list *list, const char *name)
{
	struct uci_element *e;

	uci_foreach_element(list, e) {
		if (!strcmp(e->name, name))
			return e;
	}
	return NULL;
}

int uci_lookup_ptr_bysection(struct uci_context *ctx, struct uci_ptr *ptr, struct uci_section *section, char *option, char *value)
{
	struct uci_element *e;
	memset(ptr, 0, sizeof(struct uci_ptr));

	ptr->package = section->package->e.name;
	ptr->section = section->e.name;
	ptr->option = option;
	ptr->value = value;
	ptr->flags |= UCI_LOOKUP_DONE;

	ptr->p = section->package;
	ptr->s = section;

	if (ptr->option) {
		e = dmuci_lookup_list(&ptr->s->options, ptr->option);
		if (!e)
			return UCI_OK;
		ptr->o = uci_to_option(e);
		ptr->last = e;
		ptr->target = UCI_TYPE_OPTION;
	}
	else {
		ptr->last = &ptr->s->e;
		ptr->target = UCI_TYPE_SECTION;
	}

	ptr->flags |= UCI_LOOKUP_COMPLETE;

	return UCI_OK;
}

char *dmuci_get_value_bysection(struct uci_section *section, char *option)
{
	struct uci_ptr ptr;
	char *val = "";

	if (uci_lookup_ptr_bysection(uci_ctx, &ptr, section, option, NULL) != UCI_OK) {
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

