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
#include "dhcp.h"
#include "dmjson.h"
#define DELIMITOR ","

/*** DHCPv4. ***/
DMOBJ tDhcpv4Obj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Server", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDhcpv4ServerObj, NULL, NULL},
{0}
};

/*** DHCPv4.Server. ***/
DMOBJ tDhcpv4ServerObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Pool", &DMWRITE, add_dhcp_server, delete_dhcp_server, NULL, browseDhcpInst, NULL, NULL, tDhcpServerPoolObj, tDhcpServerPoolParams, NULL},
{0}
};


/*** DHCPv4.Server.Pool.{i}. ***/
DMOBJ tDhcpServerPoolObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"StaticAddress", &DMWRITE, add_dhcp_staticaddress, delete_dhcp_staticaddress, NULL, browseDhcpStaticInst, NULL, NULL, NULL, tDhcpServerPoolAddressParams, NULL},
{"Client", &DMREAD, NULL, NULL, NULL, browseDhcpClientInst, NULL, NULL, tDhcpServerPoolClientObj, tDhcpServerPoolClientParams, get_dhcp_client_linker},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}. ***/
DMOBJ tDhcpServerPoolClientObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"IPv4Address", &DMREAD, NULL, NULL, NULL, browseDhcpClientIPv4Inst, NULL, NULL, NULL, tDhcpServerPoolClientIPv4AddressParams, NULL},
{0}
};

DMLEAF tDhcpServerPoolParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DNSServers", &DMWRITE, DMT_STRING,  get_dns_server, set_dns_server, NULL, NULL},
{"Status", &DMREAD, DMT_STRING,  get_dhcp_status, NULL, NULL, NULL},
{CUSTOM_PREFIX"DHCPServerConfigurable", &DMWRITE, DMT_BOOL, get_dhcp_configurable, set_dhcp_configurable, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL,  get_dhcp_enable, set_dhcp_enable, NULL, NULL},
{"MinAddress", &DMWRITE, DMT_STRING, get_dhcp_interval_address_min, set_dhcp_address_min, NULL, NULL},
{"MaxAddress", &DMWRITE, DMT_STRING,get_dhcp_interval_address_max, set_dhcp_address_max, NULL, NULL},
{"ReservedAddresses", &DMWRITE, DMT_STRING, get_dhcp_reserved_addresses, set_dhcp_reserved_addresses, NULL, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING,get_dhcp_subnetmask, set_dhcp_subnetmask, NULL, NULL},
{"IPRouters", &DMWRITE, DMT_STRING, get_dhcp_iprouters, set_dhcp_iprouters, NULL, NULL},
{"LeaseTime", &DMWRITE, DMT_INT, get_dhcp_leasetime, set_dhcp_leasetime, NULL, NULL},
{"DomainName", &DMWRITE, DMT_STRING, get_dhcp_domainname, set_dhcp_domainname, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_dhcp_interface, set_dhcp_interface_linker_parameter, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.StaticAddress.{i}. ***/
DMLEAF tDhcpServerPoolAddressParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_dhcp_static_alias, set_dhcp_static_alias, NULL, NULL},
{"Chaddr", &DMWRITE, DMT_STRING,  get_dhcp_staticaddress_chaddr, set_dhcp_staticaddress_chaddr, NULL, NULL},
{"Yiaddr", &DMWRITE, DMT_STRING,  get_dhcp_staticaddress_yiaddr, set_dhcp_staticaddress_yiaddr, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}. ***/
DMLEAF tDhcpServerPoolClientParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Chaddr", &DMREAD, DMT_STRING,  get_dhcp_client_chaddr, NULL, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}.IPv4Address.{i}. ***/
DMLEAF tDhcpServerPoolClientIPv4AddressParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"LeaseTimeRemaining", &DMREAD, DMT_TIME,  get_dhcp_client_ipv4address_leasetime, NULL, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_dhcp_client_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (data && ((struct client_args *)data)->key) {
		*linker = ((struct client_args *)data)->key;
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/*************************************************************
 * INIT
/*************************************************************/
inline int init_dhcp_args(struct dhcp_args *args, struct uci_section *s, char *interface)
{
	args->interface = interface;
	args->dhcp_sec = s;
	return 0;
}
inline int init_args_dhcp_host(struct dhcp_static_args *args, struct uci_section *s)
{
	args->dhcpsection = s;
	return 0;
}

inline int init_dhcp_client_args(struct client_args *args, json_object *client, char *key)
{
	args->client = client;
	args->key = key;
	return 0;
}

/*******************ADD-DEL OBJECT*********************/
int add_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL, *dmmap_dhcp= NULL;
	
	check_create_dmmap_package("dmmap_dhcp");
	instance = get_last_instance_icwmpd("dmmap_dhcp", "dhcp", "dhcp_instance");
	dmuci_add_section("dhcp", "dhcp", &s, &value);
	dmuci_set_value_by_section(s, "start", "100");
	dmuci_set_value_by_section(s, "leasetime", "12h");
	dmuci_set_value_by_section(s, "limit", "150");

	dmuci_add_section_icwmpd("dmmap_dhcp", "dhcp", &dmmap_dhcp, &v);
	dmuci_set_value_by_section(dmmap_dhcp, "section_name", section_name(s));
	*instancepara = update_instance_icwmpd(dmmap_dhcp, instance, "dhcp_instance");
	return 0;
}

int delete_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_section= NULL;

	switch (del_action) {
	case DEL_INST:
		if(is_section_unnamed(section_name(((struct dhcp_args *)data)->dhcp_sec))){
			LIST_HEAD(dup_list);
			delete_sections_save_next_sections("dmmap_dhcp", "dhcp", "dhcp_instance", section_name(((struct dhcp_args *)data)->dhcp_sec), atoi(instance), &dup_list);
			update_dmmap_sections(&dup_list, "dhcp_instance", "dmmap_dhcp", "dhcp");
			dmuci_delete_by_section_unnamed(((struct dhcp_args *)data)->dhcp_sec, NULL, NULL);
		} else {
			get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(((struct dhcp_args *)data)->dhcp_sec), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(((struct dhcp_args *)data)->dhcp_sec, NULL, NULL);
		}

		break;
	case DEL_ALL:
		uci_foreach_sections("dhcp", "dhcp", s) {
			if (found != 0){
				get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(s), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			ss = s;
			found++;
		}
		if (ss != NULL){
			get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(ss), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(ss, NULL, NULL);
		}
		break;
	}
	return 0;
}

int add_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL, *dmmap_dhcp_host= NULL;
	
	check_create_dmmap_package("dmmap_dhcp");
	instance = get_last_instance_lev2_icwmpd("dhcp", "host", "dmmap_dhcp", "ldhcpinstance", "dhcp", ((struct dhcp_args *)data)->interface);
	dmuci_add_section("dhcp", "host", &s, &value);
	dmuci_set_value_by_section(s, "dhcp", ((struct dhcp_args *)data)->interface);


	dmuci_add_section_icwmpd("dmmap_dhcp", "host", &dmmap_dhcp_host, &v);
	dmuci_set_value_by_section(dmmap_dhcp_host, "section_name", section_name(s));
	*instancepara = update_instance_icwmpd(dmmap_dhcp_host, instance, "ldhcpinstance");
	return 0;
}

int delete_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL, *dmmap_section = NULL;
	struct uci_section *ss = NULL;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name(dhcpargs->dhcpsection))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_dhcp", "host", "ldhcpinstance", section_name(dhcpargs->dhcpsection), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "ldhcpinstance", "dmmap_dhcp", "host");
				dmuci_delete_by_section_unnamed(dhcpargs->dhcpsection, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(dhcpargs->dhcpsection), &dmmap_section);
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(dhcpargs->dhcpsection, NULL, NULL);
			}

			break;
		case DEL_ALL:
			uci_foreach_option_eq("dhcp", "host", "dhcp", ((struct dhcp_args *)data)->interface, s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(ss), &dmmap_section);
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(ss), &dmmap_section);
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}
/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	int len;
	struct uci_section *s = NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
	if(res)
	{
		*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	}
	else
		*value = "";
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "dns", value);
		*value = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
		char *p = *value;
		while (*p) {
			if (*p == ' ' && p != *value && *(p-1) != ',')
				*p++ = ',';
			else
				p++;
		}
	}
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", value);
	}
	return 0;
}

int set_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *dup, *p;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dup = dmstrdup(value);
			p = dup;
			while (*p) {
				if (*p == ',')
					*p++ = ' ';
				else
					p++;
			}
			dmuci_set_value("network", ((struct dhcp_args *)data)->interface, "dns", dup);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		*value = "1";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				if (!b) {
					dmuci_delete_by_section(s, NULL, NULL);
				}
				break;
			}
			if (s == NULL && b) {
				dmuci_set_value("dhcp",((struct dhcp_args *)data)->interface, NULL, "dhcp");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "interface", ((struct dhcp_args *)data)->interface);
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "start", "100");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "limit", "150");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "leasetime", "12h");
			}
			return 0;
	}
	return 0;
}

int get_dhcp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	char *v;
	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", &v);
		*value = (*v == '1') ? "Disabled" : "Enabled";
	}
	return 0;
}

int get_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", value);
		if ((*value)[0] == '\0')
			*value = "1";
		else
			*value = "0";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				if (b)
					dmuci_set_value_by_section(s, "ignore", "");
				else
					dmuci_set_value_by_section(s, "ignore", "1");
				break;
			}
			return 0;
	}
	return 0;
}

enum enum_lanip_interval_address {
	LANIP_INTERVAL_START,
	LANIP_INTERVAL_END
};

int get_dhcp_interval_address(struct dmctx *ctx, void *data, char *instance, char **value, int option)
{
	json_object *res, *jobj;
	char *ipaddr = "" , *mask = "", *start , *limit;
	struct uci_section *s = NULL;
	char bufipstart[16], bufipend[16];

	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "start", &start);
		if (option == LANIP_INTERVAL_END)
			dmuci_get_value_by_section_string(s, "limit", &limit);
		break;
	}
	if (s == NULL) {
		return 0;
	}
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
	if (ipaddr[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		if (res)
		{
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			ipaddr = dmjson_get_value(jobj, 1, "address");			
		}
	}
	if (ipaddr[0] == '\0') {
		return 0;
	}
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		if (res) {
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			mask = dmjson_get_value(jobj, 1, "mask");
			if (mask[0] == '\0') {
				return 0;
			}
			mask = cidr2netmask(atoi(mask));
		}
	}
	if (mask[0] == '\0') {
		mask = "255.255.255.0";
	}
	if (option == LANIP_INTERVAL_START) {
		ipcalc(ipaddr, mask, start, NULL, bufipstart, NULL);
		*value = dmstrdup(bufipstart); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	else {
		ipcalc(ipaddr, mask, start, limit, bufipstart, bufipend);
		*value = dmstrdup(bufipend); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}
int get_dhcp_interval_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	get_dhcp_interval_address(ctx, data, instance, value, LANIP_INTERVAL_START);
	return 0;
}

int get_dhcp_interval_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	get_dhcp_interval_address(ctx, data, instance, value, LANIP_INTERVAL_END);
	return 0;
}

int set_dhcp_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start , *limit, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");					
				}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					mask = dmjson_get_value(jobj, 1, "mask");
					if (mask[0] == '\0')
						return 0;
					mask = cidr2netmask(atoi(mask));
				}
			}
			if (mask[0] == '\0')
				mask = "255.255.255.0";

			ipcalc_rev_start(ipaddr, mask, value, buf);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_set_value_by_section(s, "start", buf);
				break;
			}

			return 0;
	}
	return 0;
}

int set_dhcp_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int i_val;
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_get_value_by_section_string(s, "start", &start);
				break;
			}
			if (!s) return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");									}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					mask = dmjson_get_value(jobj, 1, "mask");
					if (mask[0] == '\0')
						return 0;
					mask = cidr2netmask(atoi(mask));
				}
			}
			if (mask[0] == '\0')
				mask = "255.255.255.0";

			ipcalc_rev_end(ipaddr, mask, start, value, buf);
			dmuci_set_value_by_section(s, "limit", buf);
			return 0;
	}
	return 0;
}


int get_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char val[512] = {0}, *p;
	struct uci_section *s = NULL;
	char *min, *max, *ip, *s_n_ip;
	unsigned int n_min, n_max, n_ip;
	*value = "";

	get_dhcp_interval_address(ctx, data, instance, &min, LANIP_INTERVAL_START);
	get_dhcp_interval_address(ctx, data, instance, &max, LANIP_INTERVAL_END);
	if (min[0] == '\0' || max[0] == '\0')
		return 0;
	n_min = inet_network(min);
	n_max = inet_network(max);
	p = val;
	uci_foreach_sections("dhcp", "host", s) {
		dmuci_get_value_by_section_string(s, "ip", &ip);
		if (ip[0] == '\0')
			continue;
		n_ip = inet_network(ip);
		if (n_ip >= n_min && n_ip <= n_max) {
			if (val[0] != '\0')
				dmstrappendchr(p, ',');
			dmstrappendstr(p, ip);
		}
	}
	dmstrappendend(p);
	*value = dmstrdup(val); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	struct uci_section *dhcp_section = NULL;
	char *min, *max, *ip, *val, *local_value;
	char *pch, *spch;
	unsigned int n_min, n_max, n_ip;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dhcp_interval_address(ctx, data, instance, &min, LANIP_INTERVAL_START);
			get_dhcp_interval_address(ctx, data, instance, &max, LANIP_INTERVAL_END);
			n_min = inet_network(min);
			n_max = inet_network(max);
			local_value = dmstrdup(value);

			for (pch = strtok_r(local_value, ",", &spch);
				pch != NULL;
				pch = strtok_r(NULL, ",", &spch)) {
				uci_foreach_option_eq("dhcp", "host", "ip", pch, s) {
					continue;
				}
				n_ip = inet_network(pch);
				if (n_ip < n_min && n_ip > n_max)
					continue;
				else {
					dmuci_add_section_and_rename("dhcp", "host", &dhcp_section, &val);
					dmuci_set_value_by_section(dhcp_section, "dhcp", ((struct dhcp_args *)data)->interface);
					dmuci_set_value_by_section(dhcp_section, "ip", pch);
				}
			}
			dmfree(local_value);
			return 0;
	}
	return 0;
}

int get_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mask;
	json_object *res, *jobj;
	struct uci_section *s = NULL;
	char *val;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "netmask", value);
		break;
	}
	if (s == NULL || (*value)[0] == '\0')
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", value);
	if ((*value)[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		mask = dmjson_get_value(jobj, 1, "mask");
		int i_mask = atoi(mask);
		val = cidr2netmask(i_mask);
		*value = dmstrdup(val);// MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_set_value_by_section(s, "netmask", value);
				return 0;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "gateway", value);
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", value);
	}
	return 0;
}

int set_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("network", ((struct dhcp_args *)data)->interface, "gateway", value);
			return 0;
	}
	return 0;
}

int get_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int len, mtime = 0;
	char *ltime = "", *pch, *spch, *ltime_ini, *tmp, *tmp_ini;
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "leasetime", &ltime);
		break;
	}
	if (ltime[0] == '\0') {
		*value = "-1";
		return 0;
	}
	ltime = dmstrdup(ltime);
	ltime_ini = dmstrdup(ltime);
	tmp = ltime;
	tmp_ini = ltime_ini;
	pch = strtok_r(ltime, "h", &spch);
	if (strcmp(pch, ltime_ini) != 0) {
		mtime = 3600 * atoi(pch);
		if(spch[0] != '\0') {
			ltime += strlen(pch)+1;
			ltime_ini += strlen(pch)+1;
			pch = strtok_r(ltime, "m", &spch);
			if (strcmp(pch, ltime_ini) != 0) {
				mtime += 60 * atoi(pch);
				if(spch[0] !='\0') {
					ltime += strlen(pch)+1;
					ltime_ini += strlen(pch)+1;
					pch = strtok_r(ltime, "s", &spch);
					if (strcmp(pch, ltime_ini) != 0) {
						mtime += atoi(pch);
					}
				}
			} else {
				pch = strtok_r(ltime, "s", &spch);
	if (strcmp(pch, ltime_ini) != 0)
				mtime +=  atoi(pch);
			}
		}
	}
	else {
		pch = strtok_r(ltime, "m", &spch);
		if (strcmp(pch, ltime_ini) != 0) {
		mtime += 60 * atoi(pch);
			if(spch[0] !='\0') {
				ltime += strlen(pch)+1;
				ltime_ini += strlen(pch)+1;
				pch = strtok_r(ltime, "s", &spch);
				if (strcmp(pch, ltime_ini) != 0) {
					mtime += atoi(pch);
				}
			}
		} else {
			pch = strtok_r(ltime, "s", &spch);
			if (strcmp(pch, ltime_ini) != 0)
				mtime +=  atoi(pch);
		}
	}
	dmfree(tmp);
	dmfree(tmp_ini);

	dmasprintf(value, "%d", mtime); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	char buf[32];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				int val = atoi(value);
				sprintf(buf, "%ds", val);
				dmuci_set_value_by_section(s, "leasetime",  buf);
				break;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	linker = dmstrdup(((struct dhcp_args *)data)->interface);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	dmfree(linker);
	return 0;
}

int set_dhcp_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(((struct dhcp_args *)data)->dhcp_sec, "interface", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *result, *str;
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *s = NULL;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_list(s, "dhcp_option", &val);
		if (val) {
			uci_foreach_element(val, e)
			{
				if ((str = strstr(e->name, "15,"))) {
					*value = dmstrdup(str + sizeof("15,") - 1); //MEM WILL BE FREED IN DMMEMCLEAN
					return 0;
				}
			}
		}
	}
	return 0;
}

int set_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *result, *dn, *pch;
	struct uci_list *val;
	struct uci_section *s = NULL;
	struct uci_element *e = NULL, *tmp;
	char *option = "dhcp_option", buf[64];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_get_value_by_section_list(s, option, &val);
				if (val) {
					uci_foreach_element_safe(val, e, tmp)
					{
						if (strstr(tmp->name, "15,")) {
							dmuci_del_list_value_by_section(s, "dhcp_option", tmp->name); //TODO test it
						}
					}
				}
				break;
			}
			goto end;
	}
end:
	sprintf(buf, "15,%s", value);
	dmuci_add_list_value_by_section(((struct dhcp_args *)data)->dhcp_sec, "dhcp_option", buf);
	return 0;
}

int get_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(((struct dhcp_static_args *)data)->dhcpsection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ldhcpalias", value);
	return 0;
}

int set_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(((struct dhcp_static_args *)data)->dhcpsection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "ldhcpalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac", &chaddr);
	if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac_orig", value);
	else 
		*value = chaddr;
	return 0;
}

int set_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
		
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac", &chaddr);
			if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
				dmuci_set_value_by_section(dhcpargs->dhcpsection, "mac_orig", value);
			else
				dmuci_set_value_by_section(dhcpargs->dhcpsection, "mac", value);
			return 0;
	}
	return 0;
}

int get_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "ip", value);
	return 0;
}

int set_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dhcpargs->dhcpsection, "ip", value);
			return 0;
	}
	return 0;
}

int get_dhcp_client_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmjson_get_value(((struct client_args *)data)->client, 1, "macaddr");
	return 0;
}

int get_dhcp_client_ipv4address_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char time_buf[26] = {0};
	struct tm *t_tm;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = *((struct dhcp_client_ipv4address_args*)data);

	*value = "0001-01-01T00:00:00Z";
	time_t t_time = current_dhcp_client_ipv4address_args.leasetime;
	t_tm = localtime(&t_time);
	if (t_tm == NULL)
		return 0;
	if(strftime(time_buf, sizeof(time_buf), "%FT%T%z", t_tm) == 0)
		return 0;

	time_buf[25] = time_buf[24];
	time_buf[24] = time_buf[23];
	time_buf[22] = ':';
	time_buf[26] = '\0';

	*value = dmstrdup(time_buf);
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseDhcpInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	char *interface, *idhcp = NULL, *idhcp_last = NULL;
	struct dhcp_args curr_dhcp_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("dhcp", "dhcp", "dmmap_dhcp", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "interface", &interface);
		init_dhcp_args(&curr_dhcp_args, p->config_section, interface);
		idhcp = handle_update_instance(1, dmctx, &idhcp_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "dhcp_instance", "dhcp_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_args, idhcp) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseDhcpStaticInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *sss = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;
	struct dhcp_static_args curr_dhcp_staticargs = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_cont("dhcp", "host", "dmmap_dhcp", "dhcp", ((struct dhcp_args *)prev_data)->interface, &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		idhcp = handle_update_instance(2, dmctx, &idhcp_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "ldhcpinstance", "ldhcpalias");
		init_args_dhcp_host(&curr_dhcp_staticargs, p->config_section);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_staticargs, idhcp) == DM_STOP)
			break;
	}

	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseDhcpClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *sss = NULL;
	char *idx = NULL, *idx_last = NULL;
	json_object *res = NULL, *client_obj = NULL;
	char *dhcp, *network;
	int id = 0;
	struct client_args curr_dhcp_client_args = {0};

	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			dhcp = dmjson_get_value(client_obj, 1, "dhcp");
			if(strcmp(dhcp, "true") == 0)
			{
				network = dmjson_get_value(client_obj, 1, "network");
				if(strcmp(network, ((struct dhcp_args *)prev_data)->interface) == 0)
				{
					init_dhcp_client_args(&curr_dhcp_client_args, client_obj, key);
					idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_client_args, idx) == DM_STOP)
						break;
				}
			}
		}
	}
	return 0;
}

int browseDhcpClientIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	unsigned int leasetime;
	char *macaddr;
	char mac[32], ip[32], buf[512];
	json_object *passed_args;
	FILE *fp;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = {0};
	int id = 0;
	char *idx = NULL, *idx_last = NULL;

	fp = fopen(DHCP_LEASES_FILE, "r");
	if (fp == NULL)
		return 0;
	while (fgets (buf , 256 , fp) != NULL) {
		sscanf(buf, "%u %s %s", &leasetime, mac, ip);
		passed_args= ((struct client_args*)prev_data)->client;
		macaddr=dmjson_get_value(passed_args, 1, "macaddr");
		if(!strcmp(mac, macaddr)){
			current_dhcp_client_ipv4address_args.ip= dmjson_get_value(passed_args, 1, "ipddr");
			current_dhcp_client_ipv4address_args.mac= dmstrdup(macaddr);
			current_dhcp_client_ipv4address_args.leasetime= leasetime;

			idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&current_dhcp_client_ipv4address_args, idx) == DM_STOP)
				break;
		}
	}

	return 0;
}
