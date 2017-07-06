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

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "hosts.h"
#include "dmjson.h"

struct host_args cur_host_args = {0};

/*************************************************************
 * INIT
/*************************************************************/
inline int init_host_args(struct dmctx *ctx, json_object *clients, char *key)
{
	struct host_args *args = &cur_host_args;
	ctx->args = (void *)args;
	args->client = clients;
	args->key = key;
	return 0;
}
/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_host_ipaddress(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmjson_get_value(cur_host_args.client, 1, "ipaddr");
	return 0;
}

int get_host_hostname(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmjson_get_value(cur_host_args.client, 1, "hostname");
	return 0;
}

int get_host_active(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmjson_get_value(cur_host_args.client, 1, "connected");
	return 0;
}

int get_host_phy_address(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmjson_get_value(cur_host_args.client, 1, "macaddr");
	return 0;
}

int get_host_address_source(char *refparam, struct dmctx *ctx, char **value) {
	char *dhcp;

	dhcp = dmjson_get_value(cur_host_args.client, 1, "dhcp");
	if (strcasecmp(dhcp, "true") == 0)
		*value = "DHCP";
	else
		*value = "Static";
	return 0;
}

int get_host_leasetime_remaining(char *refparam, struct dmctx *ctx, char **value)
{
	char buf[80], *dhcp;
	FILE *fp;
	char line[MAX_DHCP_LEASES];
	struct tm ts;
	char *leasetime, *mac_f, *mac, *line1;
	char delimiter[] = " \t";

	dhcp = dmjson_get_value(cur_host_args.client, 1, "dhcp");
	if (strcmp(dhcp, "false") == 0) {
		*value = "0";
	}
	else {
		mac = dmjson_get_value(cur_host_args.client, 1, "macaddr");
		fp = fopen(ARP_FILE, "r");
		if ( fp != NULL)
		{
			while (fgets(line, MAX_DHCP_LEASES, fp) != NULL )
			{
				if (line[0] == '\n')
					continue;
				line1 = dmstrdup(line);
				leasetime = cut_fx(line, delimiter, 1);
				mac_f = cut_fx(line1, delimiter, 2);
				if (strcasecmp(mac, mac_f) == 0) {
					int rem_lease = atoi(leasetime) - time(NULL);
					if (rem_lease < 0)
						*value = "-1";
					else
						dmasprintf(value, "%d", rem_lease); // MEM WILL BE FREED IN DMMEMCLEAN
					fclose(fp) ;
					return 0;
				}
			}
			fclose(fp);
		}
	}
	return 0;
}

int  get_host_dhcp_client(char *refparam, struct dmctx *ctx, char **value)
{
	char *iface, *linker;
		dmastrcat(&linker, "linker_dhcp:", cur_host_args.key);
		adm_entry_get_linker_param(DMROOT"DHCPv4.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL) {
			*value = "";
		}
		dmfree(linker);
	return 0;
}

char *get_interface_type(char *mac, char *ndev)
{
	json_object *res;
	int wlctl_num;
	struct uci_section *s, *d;
	char *network, *device, *value, *wunit;
	char buf[8], *p;

	uci_foreach_sections("wireless", "wifi-device", d) {
		wlctl_num = 0;
		wunit = section_name(d);
		uci_foreach_option_eq("wireless", "wifi-iface", "device", wunit, s) {
			dmuci_get_value_by_section_string(s, "network", &network);
			if (strcmp(network, ndev) == 0) {
				if (wlctl_num != 0) {
					sprintf(buf, "%s.%d", wunit, wlctl_num);
					p = buf;
				}
				else {
					p = wunit;
				}
				dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", p, String}}, 1, &res);
				if(res) {
					json_object_object_foreach(res, key, val) {
						value = dmjson_get_value(res, 1, "macaddr");
						if (strcasecmp(value, mac) == 0)
							return "802.11";
					}
				}
				wlctl_num++;
			}
		}
	}
	return "Ethernet";
}

int get_host_interfacetype(char *refparam, struct dmctx *ctx, char **value)
{
	char *mac, *network;
	
	mac = dmjson_get_value(cur_host_args.client, 1, "macaddr");
	network = dmjson_get_value(cur_host_args.client, 1, "network");
	*value = get_interface_type(mac, network);
	return 0;
}

int get_host_nbr_entries(char *refparam, struct dmctx *ctx, char **value)
{
	int entries = 0;
	json_object *res;

	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, client_obj) {
		entries++;
	}
	dmasprintf(value, "%d", entries); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int entry_method_root_hosts(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"Hosts.") {
		DMOBJECT(DMROOT"Hosts.", ctx, "0", 0, NULL, NULL, NULL);
		DMPARAM("HostNumberOfEntries", ctx, "0", get_host_nbr_entries, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"Hosts.Host.", ctx, "0", 1, NULL, NULL, NULL);
		SUBENTRY(entry_host, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_host(struct dmctx *ctx)
{
	json_object *res, *client_obj;
	char *idx, *idx_last = NULL;
	int id = 0;
	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			init_host_args(ctx, client_obj, key);
			idx = handle_update_instance(2, ctx, &idx_last, update_instance_without_section, 1, ++id);
			SUBENTRY(entry_host_instance, ctx, idx);
		}
	}
	return 0;
}

inline int entry_host_instance(struct dmctx *ctx, char *int_num)
{
	IF_MATCH(ctx, DMROOT"Hosts.Host.%s.", int_num) {
		DMOBJECT(DMROOT"Hosts.Host.%s.", ctx, "0", NULL, NULL, NULL, NULL, int_num);
		DMPARAM("IPAddress", ctx, "0", get_host_ipaddress, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("HostName", ctx, "0", get_host_hostname, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("Active", ctx, "0", get_host_active, NULL, "xsd:boolean", 0, 0, UNDEF, NULL);
		DMPARAM("PhysAddress", ctx, "0", get_host_phy_address, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_InterfaceType", ctx, "0", get_host_interfacetype, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("AddressSource", ctx, "0", get_host_address_source, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("LeaseTimeRemaining", ctx, "0", get_host_leasetime_remaining, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("DHCPClient", ctx, "0", get_host_dhcp_client, NULL, NULL, 0, 1, UNDEF, NULL); //TO CHECK R/W
		return 0;
	}
	return FAULT_9005;
}
