/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#ifndef __BUCI_H
#define __BUCI_H

#include <uci.h>

int buci_init(void);
int buci_fini(void);
struct uci_section *buci_walk_section(char *package, char *section_type, struct uci_section *prev_section);
void buci_print_list(struct uci_list *uh, char **val, char *delimiter);
struct uci_element *buci_lookup_list(struct uci_list *list, const char *name);
int uci_lookup_ptr_bysection(struct uci_context *ctx, struct uci_ptr *ptr, struct uci_section *section, char *option, char *value);
char *buci_get_value_bysection(struct uci_section *section, char *option);
char *buci_get_value(char *package, char *section, char *option);

#define buci_foreach_section(package, section_type, section) \
	for (section = buci_walk_section(package, section_type, NULL); \
		section != NULL; \
		section = buci_walk_section(package, section_type, section))

#endif //__BUCI_H
