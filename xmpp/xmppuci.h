/*
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
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

#ifndef _XMPPUCI_H__
#define _XMPPUCI_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uci.h>

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

#endif /* _XMPPUCI_H__ */
