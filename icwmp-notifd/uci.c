/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2020 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include "uci.h"
#include <stdio.h>

struct uci_context *uci_ctx;

struct uci_context *uci_ctx = NULL;

void nuci_init()
{
	uci_ctx = uci_alloc_context();
}

void nuci_end()
{
	if(uci_ctx) {
		uci_free_context(uci_ctx);
		uci_ctx = NULL;
	}
}
bool check_section_name(const char *str, bool name)
{
	if (!*str)
		return false;
	for (; *str; str++) {
		unsigned char c = *str;
		if (isalnum(c) || c == '_')
			continue;
		if (name || (c < 33) || (c > 126))
			return false;
	}
	return true;
}

int nuci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value)
{
	/*value*/
	ptr->value = value;

	/*package*/
	if (!package)
		return -1;
	ptr->package = package;
	/*section*/
	if (!section || !section[0]) {
		ptr->target = UCI_TYPE_PACKAGE;
		goto lookup;
	}
	ptr->section = section;
	if (ptr->section &&  !check_section_name(ptr->section , true))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	/*option*/
	if (!option || !option[0]) {
		ptr->target = UCI_TYPE_SECTION;
		goto lookup;
	}
	ptr->target = UCI_TYPE_OPTION;
	ptr->option = option;
lookup:
	if (uci_lookup_ptr(ctx, ptr, NULL, true) != UCI_OK || !UCI_LOOKUP_COMPLETE) {
		return -1;
	}
	return 0;
}

void load_uci_config(char **polling_period)
{
	struct  uci_context         *c = uci_alloc_context();
	struct  uci_ptr             ptr;
	char *s;

	s= strdup(UCI_OPTION_CPE_NOTIF_POLLING_PERIOD);
    if (uci_lookup_ptr(c, &ptr, s, true) != UCI_OK)
    {
        fprintf(stderr, "Error occurred in uci\n");
        goto end;
    }
    if(ptr.flags & UCI_LOOKUP_COMPLETE)
    {
        if (ptr.o==NULL || ptr.o->v.string==NULL)
        {
            fprintf(stderr, "icwmp_notifd.icwmp_notif.polling_period not found or empty value\n");
            *polling_period = NULL;
            goto end;
        }
        *polling_period = strdup(ptr.o->v.string);
    }
    end:
    	uci_free_context(c);
    	free(s);
}

int nuci_get_list(char *package, char *section, char *list_opt, struct uci_list** list_val)
{
	if(!uci_ctx)
		goto end_uci;

	if (list_val == NULL)
		goto end_uci;

	struct uci_ptr ptr = {0};

	if (nuci_lookup_ptr(uci_ctx, &ptr, package, section, list_opt, NULL))
		goto end_uci;

	if (ptr.o && ptr.o->type == UCI_TYPE_LIST) {
		*list_val = &ptr.o->v.list;
		return 1;
	}

	end_uci:
		return 0;
}
