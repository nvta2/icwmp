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
#ifndef __DHCP_H
#define __DHCP_H

#include <json-c/json.h>

struct dhcp_args
{
	struct uci_section *dhcp_sec;
	char *interface;
};

struct dhcp_static_args
{
	struct uci_section *dhcpsection;
};

struct client_args
{
	json_object *client;
	char *key;
};


extern DMOBJ tDhcpv4Obj[];
extern DMOBJ tDhcpv4ServerObj[];
extern DMOBJ tDhcpServerPoolObj[];
extern DMLEAF tDhcpServerPoolParams[];
extern DMLEAF tDhcpServerPoolAddressParams[];
extern DMLEAF tDhcpServerPoolClientParams[];

int browseDhcpInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcpStaticInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcpClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interval_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interval_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_client_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_dhcp_client_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
