/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#ifndef __ETHERNET_H
#define __ETHERNET_H

struct eth_port_args
{
	struct uci_section *eth_port_sec;
	char *ifname;
};


int entry_method_root_Ethernet(struct dmctx *ctx);

#endif
