/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef _UDPECHOSERVERUCI_H__
#define _UDPECHOSERVERUCI_H__

#include <uci.h>

#define VAR_STATE "/var/state"

int dmuci_init(void);
int dmuci_fini(void);
void dmuci_print_list(struct uci_list *uh, char **val, char *delimiter);
char *dmuci_get_value(char *package, char *section, char *option);
char *dmuci_set_value(char *package, char *section, char *option, char *value);
char *dmuci_set_value_state(char *package, char *section, char *option, char *value);

#endif /* _UDPECHOSERVERUCI_H__ */
