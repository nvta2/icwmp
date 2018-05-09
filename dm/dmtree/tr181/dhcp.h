/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#ifndef __DHCP_H
#define __DHCP_H

struct dhcp_args
{
	struct uci_section *dhcp_sec;
	char *interface;
};

struct dhcp_static_args
{
	struct uci_section *dhcpsection;
};

struct dhcp_option_args
{
	struct uci_section *dhcpoptionsection;
	struct uci_section *dhcppoolsection;
	char *poolinstance;
	char *poolinterface;
};

struct client_args
{
	json_object *client;
};

struct dhcp_client_ipv4address_args {
	char *mac;
	char *ip;
	unsigned int leasetime;
};

int entry_method_root_dhcp(struct dmctx *ctx);

#endif
