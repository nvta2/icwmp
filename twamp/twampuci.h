/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef _TWAMPUCI_H__
#define _TWAMPUCI_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int dmuci_init(void);
int dmuci_fini(void);
struct uci_section *dmuci_walk_section(char *package, char *section_type, struct uci_section *prev_section);
void dmuci_print_list(struct uci_list *uh, char **val, char *delimiter);
struct uci_element *dmuci_lookup_list(struct uci_list *list, const char *name);
int uci_lookup_ptr_bysection(struct uci_context *ctx, struct uci_ptr *ptr, struct uci_section *section, char *option, char *value);
char *dmuci_get_value_bysection(struct uci_section *section, char *option);
char *dmuci_get_value(char *package, char *section, char *option);

#define dmuci_foreach_section(package, section_type, section) \
	for (section = dmuci_walk_section(package, section_type, NULL); \
		section != NULL; \
		section = dmuci_walk_section(package, section_type, section))

#endif /* _TWAMPUCI_H__ */
