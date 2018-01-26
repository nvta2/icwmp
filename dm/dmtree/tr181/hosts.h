/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *
 */
#ifndef __HOSTS_H
#define __HOSTS_H

struct host_args
{
	json_object *client;
	char *key;
};

extern DMLEAF thostsParam[];
extern DMLEAF thostParam[];
extern DMOBJ thostsObj[];

int browsehostInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_host_nbr_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_associateddevice(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_layer3interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_interface_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_interfacename(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_hostname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_phy_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_interfacetype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_address_source(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_leasetime_remaining(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_host_dhcp_client(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif
