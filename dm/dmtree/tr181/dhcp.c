/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
 *		Author Feten Besbes <feten.besbes@pivasoftware.com>
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "dhcp.h"
#include "cwmpmem.h"

#include "dmjson.h"
#define DELIMITOR ","

struct dhcp_args cur_dhcp_args = {0};
struct client_args cur_dhcp_client_args = {0};
struct dhcp_static_args cur_dhcp_staticargs = {0};
struct dhcp_option_args cur_dhcp_optionargs = {0};
struct dhcp_client_ipv4address_args cur_dhcp_client_ipv4address_args = {0};

/*************************************************************
 * INIT
/*************************************************************/
inline int init_dhcp_args(struct dmctx *ctx, struct uci_section *s, char *interface)
{
	struct dhcp_args *args = &cur_dhcp_args;
	ctx->args = (void *)args;
	args->interface = interface;
	args->dhcp_sec = s;
	return 0;
}
inline int init_args_dhcp_host(struct dmctx *ctx, struct uci_section *s)
{
	struct dhcp_static_args *args = &cur_dhcp_staticargs;
	ctx->args = (void *)args;
	args->dhcpsection = s;
	return 0;
}

inline int init_args_option(struct dmctx *ctx, struct uci_section *pool_option_section, struct uci_section *pool_section, char *ipool, char *interface)
{
	struct dhcp_option_args *args = &cur_dhcp_optionargs;
	ctx->args = (void *)args;
	args->dhcpoptionsection = pool_option_section;
	args->dhcppoolsection = pool_section;
	args->poolinstance = ipool;
	args->poolinterface = interface;
	return 0;
}


inline int init_dhcp_client_args(struct dmctx *ctx, json_object *client)
{
	struct client_args *args = &cur_dhcp_client_args;
	ctx->args = (void *)args;
	args->client = client;
	return 0;
}

/*******************ADD-DEL OBJECT*********************/
int add_dhcp_server(struct dmctx *ctx, char **instancepara)
{
	char *value;
	char *instance;
	struct uci_section *s = NULL;
	
	instance = get_last_instance("dhcp", "dhcp", "dhcp_instance");
	dmuci_add_section("dhcp", "dhcp", &s, &value);
	dmuci_set_value_by_section(s, "start", "100");
	dmuci_set_value_by_section(s, "leasetime", "12h");
	dmuci_set_value_by_section(s, "limit", "150");
	*instancepara = update_instance(s, instance, "dhcp_instance");
	return 0;
}

int delete_dhcp_server(struct dmctx *ctx)
{
	dmuci_delete_by_section(cur_dhcp_args.dhcp_sec, NULL, NULL);
	return 0;
}

int delete_dhcp_server_all(struct dmctx *ctx)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	uci_foreach_sections("dhcp", "dhcp", s) {
		if (found != 0)
			dmuci_delete_by_section(ss, NULL, NULL);
		ss = s;
		found++;
	}
	if (ss != NULL)
		dmuci_delete_by_section(ss, NULL, NULL);
	return 0;	
}

int add_dhcp_staticaddress(struct dmctx *ctx, char **instancepara)
{
	char *value;
	char *instance;
	struct uci_section *s = NULL;
	
	instance = get_last_instance_lev2("dhcp", "host", "ldhcpinstance", "dhcp", cur_dhcp_args.interface);
	dmuci_add_section("dhcp", "host", &s, &value);
	dmuci_set_value_by_section(s, "dhcp", cur_dhcp_args.interface);
	*instancepara = update_instance(s, instance, "ldhcpinstance");
	return 0;
}

int delete_dhcp_staticaddress_all(struct dmctx *ctx)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	uci_foreach_option_eq("dhcp", "host", "dhcp", cur_dhcp_args.interface, s) {
		if (found != 0)
			dmuci_delete_by_section(ss, NULL, NULL);
		ss = s;
		found++;
	}
	if (ss != NULL)
		dmuci_delete_by_section(ss, NULL, NULL);
	return 0;	
}

int delete_dhcp_staticaddress(struct dmctx *ctx)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)ctx->args;
	
	dmuci_delete_by_section(dhcpargs->dhcpsection, NULL, NULL);
	return 0;
}

int add_dhcp_option(struct dmctx *ctx, char **instancepara)
{
	char val[64];
	char *value, *instance;
	struct uci_section *s = NULL, *lan_s = NULL;
	struct dhcp_args *dhcpargs = (struct dhcp_args *)ctx->args;
	char *itf_name = dhcpargs->interface;

	instance = get_last_instance(DMMAP, section_name(dhcpargs->dhcp_sec), "option_instance");
	DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name(dhcpargs->dhcp_sec), &s, &value);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "tag", "0");
	*instancepara = update_instance_icwmpd(s, instance, "option_instance");
	sprintf(val, "option_%s", *instancepara);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "value", val);
	sprintf(val, "0,option_%s", *instancepara);
	dmuci_add_list_value_by_section(dhcpargs->dhcp_sec, "dhcp_option", val);
	return 0;
}

int delete_dhcp_option_all(struct dmctx *ctx)
{
	struct dhcp_args *poolargs = (struct dhcp_args *)ctx->args;
	char *value;
	char *instance;
	struct uci_list *val;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL;
	struct uci_section *lan_s = NULL;
	struct uci_element *e = NULL, *tmp;
	int dmmap = 0;
	uci_path_foreach_sections(icwmpd, "dmmap", section_name(poolargs->dhcp_sec), dmmap_s)
	{
		if (dmmap)
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
		dmmap_ss = dmmap_s;
		dmmap++;
	}
	if (dmmap_ss != NULL)
		DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
	dmuci_get_value_by_section_list(poolargs->dhcp_sec, "dhcp_option", &val);
	if (val) {
		uci_foreach_element_safe(val, e, tmp)
		{
			dmuci_del_list_value_by_section(poolargs->dhcp_sec, "dhcp_option", tmp->name);
		}
	}
	return 0;
}

int delete_dhcp_option(struct dmctx *ctx)
{
	int dmmap = 0;
	char *value, *tag, *instance,  *bufopt;
	struct uci_list *val;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL, *lan_s = NULL;
	struct uci_element *e = NULL, *tmp;
	struct dhcp_option_args *pooloptionargs = (struct dhcp_option_args *)ctx->args;
	//dmmap section
	dmuci_get_value_by_section_string(pooloptionargs->dhcpoptionsection, "tag", &tag);
	dmuci_get_value_by_section_string(pooloptionargs->dhcpoptionsection, "value", &value);
	dmasprintf(&bufopt, "%s,%s", tag, value);
	dmuci_get_value_by_section_list(pooloptionargs->dhcppoolsection, "dhcp_option", &val);
	if (val) {
		uci_foreach_element_safe(val, e, tmp)
		{
			if (strcmp(tmp->name, bufopt) == 0) {
				dmuci_del_list_value_by_section(pooloptionargs->dhcppoolsection, "dhcp_option", tmp->name); //TODO test it
				break;
			}
		}
	}
	dmfree(bufopt);
	DMUCI_DELETE_BY_SECTION(icwmpd, pooloptionargs->dhcpoptionsection, NULL, NULL);
	return 0;
}
/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_dns_server(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char buf[256] = "";
	int len;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
	if(res)
	{
		*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	}	
	else
		*value = "";
	if ((*value)[0] == '\0') {
		if (get_uci_dhcpserver_option(cur_dhcp_args.dhcp_sec, "6", buf) == 0) {
			*value = dmstrdup(buf);
		}
	}
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", cur_dhcp_args.interface, "ipaddr", value);
	}
	return 0;
}

int set_dns_server(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *dup, *p;
	struct uci_section *dmmap_s = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			set_uci_dhcpserver_option(cur_dhcp_args.dhcp_sec, "6", value);
			uci_path_foreach_option_eq(icwmpd, "dmmap", section_name(cur_dhcp_args.dhcp_sec), "tag", "6", dmmap_s) {
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_s, "value", value);
				break;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_configurable(char *refparam, struct dmctx *ctx, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
		*value = "1";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_dhcp_configurable(char *refparam, struct dmctx *ctx, int action, char *value)
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
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
				if (!b) {
					dmuci_delete_by_section(s, NULL, NULL);
				}
				break;
			}
			if (s == NULL && b) {
				dmuci_set_value("dhcp",cur_dhcp_args.interface, NULL, "dhcp");
				dmuci_set_value("dhcp", cur_dhcp_args.interface, "interface", cur_dhcp_args.interface);
				dmuci_set_value("dhcp", cur_dhcp_args.interface, "start", "100");
				dmuci_set_value("dhcp", cur_dhcp_args.interface, "limit", "150");
				dmuci_set_value("dhcp", cur_dhcp_args.interface, "leasetime", "12h");
			}
			return 0;
	}
	return 0;
}

int get_dhcp_status(char *refparam, struct dmctx *ctx, char **value){
	struct uci_section *s = NULL;
	char v[3];
	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", v);
		*value = (v[0] == '1') ? "Disabled" : "Enabled";
	}
	return 0;
}

int get_dhcp_enable(char *refparam, struct dmctx *ctx, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
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

int set_dhcp_enable(char *refparam, struct dmctx *ctx, int action, char *value)
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
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
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

int get_dhcp_interval_address(struct dmctx *ctx, char **value, int option)
{
	json_object *res, *jobj;
	char *ipaddr = "" , *mask = "", *start , *limit;
	struct uci_section *s = NULL;
	char bufipstart[16], bufipend[16];

	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
		dmuci_get_value_by_section_string(s, "start", &start);
		if (option == LANIP_INTERVAL_END)
			dmuci_get_value_by_section_string(s, "limit", &limit);
		break;
	}
	if (s == NULL) {
		return 0;
	}
	dmuci_get_option_value_string("network", cur_dhcp_args.interface, "ipaddr", &ipaddr);
	if (ipaddr[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
		if (res)
		{
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			ipaddr = dmjson_get_value(jobj, 1, "address");			
		}
	}
	if (ipaddr[0] == '\0') {
		return 0;
	}
	dmuci_get_option_value_string("network", cur_dhcp_args.interface, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
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

int get_dhcp_interval_address_min(char *refparam, struct dmctx *ctx, char **value)
{
	get_dhcp_interval_address(ctx, value, LANIP_INTERVAL_START);
	return 0;
}

int get_dhcp_interval_address_max(char *refparam, struct dmctx *ctx, char **value)
{
	get_dhcp_interval_address(ctx, value, LANIP_INTERVAL_END);
	return 0;
}

int set_dhcp_address_min(char *refparam, struct dmctx *ctx, int action, char *value)
{
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start , *limit, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("network", cur_dhcp_args.interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");					
				}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", cur_dhcp_args.interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
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
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
				dmuci_set_value_by_section(s, "start", buf);
				break;
			}

			return 0;
	}
	return 0;
}

int set_dhcp_address_max(char *refparam, struct dmctx *ctx, int action, char *value)
{
	int i_val;
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
				dmuci_get_value_by_section_string(s, "start", &start);
				break;
			}
			if (!s) return 0;

			dmuci_get_option_value_string("network", cur_dhcp_args.interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");									}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", cur_dhcp_args.interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
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


int get_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, char **value)
{
	char val[512] = {0}, *p;
	struct uci_section *s = NULL;
	char *min, *max, *ip, *s_n_ip;
	unsigned int n_min, n_max, n_ip;
	*value = "";

	get_dhcp_interval_address(ctx, &min, LANIP_INTERVAL_START);
	get_dhcp_interval_address(ctx, &max, LANIP_INTERVAL_END);
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

int set_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, int action, char *value)
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
			get_dhcp_interval_address(ctx, &min, LANIP_INTERVAL_START);
			get_dhcp_interval_address(ctx, &max, LANIP_INTERVAL_END);
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
					dmuci_add_section("dhcp", "host", &dhcp_section, &val);
					dmuci_set_value_by_section(dhcp_section, "dhcp", cur_dhcp_args.interface);
					dmuci_set_value_by_section(dhcp_section, "ip", pch);
				}
			}
			dmfree(local_value);
			return 0;
	}
	return 0;
}

int get_dhcp_subnetmask(char *refparam, struct dmctx *ctx, char **value)
{
	char *mask;
	json_object *res, *jobj;
	struct uci_section *s = NULL;
	char *val;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
		dmuci_get_value_by_section_string(s, "netmask", value);
		break;
	}
	if (s == NULL || (*value)[0] == '\0')
	dmuci_get_option_value_string("network", cur_dhcp_args.interface, "netmask", value);
	if ((*value)[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", cur_dhcp_args.interface, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		mask = dmjson_get_value(jobj, 1, "mask");
		int i_mask = atoi(mask);
		val = cidr2netmask(i_mask);
		*value = dmstrdup(val);// MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_dhcp_subnetmask(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
				dmuci_set_value_by_section(s, "netmask", value);
				return 0;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_iprouters(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("network", cur_dhcp_args.interface, "gateway", value);
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", cur_dhcp_args.interface, "ipaddr", value);
	}
	return 0;
}

int set_dhcp_iprouters(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("network", cur_dhcp_args.interface, "gateway", value);
			return 0;
	}
	return 0;
}

int get_dhcp_leasetime(char *refparam, struct dmctx *ctx, char **value)
{
	int len, mtime = 0;
	char *ltime = "", *pch, *spch, *ltime_ini, *tmp, *tmp_ini;
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
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

int set_dhcp_leasetime(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *s = NULL;
	char buf[32];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
				int val = atoi(value);
				sprintf(buf, "%ds", val);
				dmuci_set_value_by_section(s, "leasetime",  buf);
				break;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_interface(char *refparam, struct dmctx *ctx, char **value)
{
	char *linker;
	linker = dmstrdup(cur_dhcp_args.interface);
	adm_entry_get_linker_param(DMROOT"IP.Interface.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	dmfree(linker);
	return 0;
}

int set_dhcp_interface_linker_parameter(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *linker;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(value, &linker);
			if (linker) {
				dmuci_set_value_by_section(cur_dhcp_args.dhcp_sec, "interface", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_dhcp_domainname(char *refparam, struct dmctx *ctx, char **value)
{
	char *result, *str;
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *s = NULL;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
		dmuci_get_value_by_section_list(s, "dhcp_option", &val);
		if (val) {
			uci_foreach_element(val, e)
			{
				if (str = strstr(e->name, "15,")) {
					*value = dmstrdup(str + sizeof("15,") - 1); //MEM WILL BE FREED IN DMMEMCLEAN
					return 0;
				}
			}
		}
	}
	return 0;
}

int set_dhcp_domainname(char *refparam, struct dmctx *ctx, int action, char *value)
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
			uci_foreach_option_eq("dhcp", "dhcp", "interface", cur_dhcp_args.interface, s) {
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
	dmuci_add_list_value_by_section(cur_dhcp_args.dhcp_sec, "dhcp_option", buf);
	return 0;
}

int get_dhcp_static_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcp_staticargs.dhcpsection, "ldhcpalias", value);
	return 0;
}

int set_dhcp_static_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dhcp_staticargs.dhcpsection, "ldhcpalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, char **value)
{
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)ctx->args;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac", &chaddr);
	if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac_orig", value);
	else 
		*value = chaddr;
	return 0;
}

int set_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)ctx->args;
		
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

int get_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, char **value)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)ctx->args;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "ip", value);
	return 0;
}

int set_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dhcpargs->dhcpsection, "ip", value);
			return 0;
	}
	return 0;
}

int get_dhcp_client_ipv4address_leasetime(char *refparam, struct dmctx *ctx, char **value){
	char time_buf[26] = {0};
	struct tm *t_tm;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = *((struct dhcp_client_ipv4address_args*)ctx->args);

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

int get_dhcp_client_chaddr(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmjson_get_value(cur_dhcp_client_args.client, 1, "macaddr");
	return 0;
}


int get_dhcp_option_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcp_optionargs.dhcpoptionsection, "option_alias", value);
	return 0;
}

int set_dhcp_option_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(cur_dhcp_optionargs.dhcpoptionsection, "option_alias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_option_tag(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcp_optionargs.dhcpoptionsection, "tag", value);
	return 0;
}

int set_dhcp_option_tag(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *cur_tag, *cur_value, *instance;
	struct dhcp_option_args *pooloptionargs = (struct dhcp_option_args *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcpoptionsection, "value", &cur_value);
			dmuci_get_value_by_section_string(pooloptionargs->dhcpoptionsection, "tag", &cur_tag);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcpoptionsection, "tag", value);
			update_uci_dhcpserver_option(ctx, pooloptionargs->dhcppoolsection, cur_tag, value, cur_value);
			return 0;
	}
	return 0;
}

int get_dhcp_option_value(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcp_optionargs.dhcpoptionsection, "value", value);
	return 0;
}

int set_dhcp_option_value(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *cur_tag, *cur_value;
	struct dhcp_option_args *pooloptionargs = (struct dhcp_option_args *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcpoptionsection, "tag", &cur_tag);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcpoptionsection, "value", value);
			set_uci_dhcpserver_option(pooloptionargs->dhcppoolsection, cur_tag, value);
			return 0;
	}
	return 0;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int entry_dhcp_client_ipv4address(struct dmctx *ctx, char* num1,char* num2){
	unsigned int leasetime;
	char *macaddr;
	char mac[32], ip[32], buf[512];
	json_object *passed_args;
	FILE *fp;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = {0};
	int id = 0;
	char *idx = NULL, *idx_last = NULL;

	fp = fopen("/tmp/dhcp.leases", "r");
	if (fp == NULL)
		return 0;
	while (fgets (buf , 256 , fp) != NULL) {
		sscanf(buf, "%u %s %s", &leasetime, mac, ip);
		struct client_args current_client_args = {0};
		passed_args= ((struct client_args*)(ctx->args))->client;
		macaddr=dmjson_get_value(passed_args, 1, "macaddr");
		if(!strcmp(mac, macaddr)){
			current_dhcp_client_ipv4address_args.ip= dmjson_get_value(passed_args, 1, "ipddr");
			current_dhcp_client_ipv4address_args.mac= dmstrdup(macaddr);
			current_dhcp_client_ipv4address_args.leasetime= leasetime;
			ctx->args=(void*)&current_dhcp_client_ipv4address_args;
			idx = handle_update_instance(2, ctx, &idx_last, update_instance_without_section, 1, ++id);
			SUBENTRY(entry_dhcp_client_ipv4address_instance, ctx, num1, num2, idx);
		}

	}
}

int entry_method_root_dhcp(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"DHCPv4.") {
		DMOBJECT(DMROOT"DHCPv4.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"DHCPv4.Server.", ctx, "0", 1, NULL, NULL, NULL);
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.", ctx, "0", 1, add_dhcp_server, delete_dhcp_server_all, NULL);
		SUBENTRY(entry_dhcp, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dhcp(struct dmctx *ctx)
{
	struct uci_section *s;
	char *interface, *idhcp = NULL, *idhcp_last = NULL;
	uci_foreach_sections("dhcp","dhcp", s) {
		dmuci_get_value_by_section_string(s, "interface", &interface);
		init_dhcp_args(ctx, s, interface);
		idhcp = handle_update_instance(1, ctx, &idhcp_last, update_instance_alias, 3, s, "dhcp_instance", "dhcp_alias");
		SUBENTRY(entry_dhcp_instance, ctx, interface, idhcp);
	}

	return 0;
}

inline int entry_dhcp_instance(struct dmctx *ctx, char *interface, char *int_num)
{
	IF_MATCH(ctx, DMROOT"DHCPv4.Server.Pool.%s.", int_num) {
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.", ctx, "0", NULL, NULL, delete_dhcp_server, NULL, int_num);
		DMPARAM("Status", ctx, "0", get_dhcp_status, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DNSServers", ctx, "1", get_dns_server, set_dns_server, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_DHCPServerConfigurable", ctx, "1", get_dhcp_configurable, set_dhcp_configurable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_dhcp_enable, set_dhcp_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("MinAddress", ctx, "1", get_dhcp_interval_address_min, set_dhcp_address_min, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxAddress", ctx, "1", get_dhcp_interval_address_max, set_dhcp_address_max, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("ReservedAddresses", ctx, "1", get_dhcp_reserved_addresses, set_dhcp_reserved_addresses, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("SubnetMask", ctx, "1", get_dhcp_subnetmask, set_dhcp_subnetmask, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IPRouters", ctx, "1", get_dhcp_iprouters, set_dhcp_iprouters, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("LeaseTime", ctx, "1", get_dhcp_leasetime, set_dhcp_leasetime, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DomainName", ctx, "1", get_dhcp_domainname, set_dhcp_domainname, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Interface", ctx, "1", get_dhcp_interface, set_dhcp_interface_linker_parameter, NULL, 0, 1, UNDEF, NULL); // refer to  IP.Interface
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.StaticAddress.", ctx, "0", NULL, add_dhcp_staticaddress, delete_dhcp_staticaddress_all, NULL, int_num); //TODO
		SUBENTRY(entry_dhcp_static_address, ctx, interface, int_num);
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Client.", ctx, "0", NULL, NULL, NULL, NULL, int_num); //TODO
		SUBENTRY(entry_dhcp_client, ctx, interface, int_num);
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Option.", ctx, "0", NULL, add_dhcp_option, delete_dhcp_option_all, NULL, int_num);
		SUBENTRY(entry_dhcp_option, ctx, interface, int_num);
		return 0;
	}
	return FAULT_9005;
}


inline int entry_dhcp_static_address(struct dmctx *ctx, char *interface, char *idev)
{
	struct uci_section *sss = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;
	uci_foreach_option_cont("dhcp", "host", "dhcp", interface, sss) {
		idhcp = handle_update_instance(2, ctx, &idhcp_last, update_instance_alias, 3, sss, "ldhcpinstance", "ldhcpalias");
		init_args_dhcp_host(ctx, sss);
		SUBENTRY(entry_dhcp_static_address_instance, ctx, idev, idhcp);
	}
}

inline int entry_dhcp_option (struct dmctx *ctx, char *interface, char *pool_instance)
{
	int id = 0;
	char *idx = NULL, *pch, *spch, *name, *value;
	char *idx_last = NULL;
	struct uci_list *val;
	struct uci_element *e = NULL, *tmp;
	struct uci_section *dmmap_s = NULL, *s= NULL;
	struct dhcp_args *poolargs = (struct dhcp_args *)ctx->args;
	bool find_max = true;
	char *tt;

	int found = 0;
	dmuci_get_value_by_section_list(poolargs->dhcp_sec, "dhcp_option", &val);
	if (val) {
		uci_foreach_element_safe(val, e, tmp)
		{
			tt = dmstrdup(tmp->name);
			pch = strtok_r(tt, ",", &spch);
			found = 0;
			uci_path_foreach_option_eq(icwmpd, "dmmap", section_name(poolargs->dhcp_sec), "tag", pch, dmmap_s)
			{
				dmuci_get_value_by_section_string(dmmap_s, "value", &value);
				if (strcmp(spch, value) == 0)
				{
					dmuci_get_value_by_section_string(dmmap_s, "option_instance", &idx);
					found = 1;
				}
				else
					continue;
				init_args_option(ctx, dmmap_s, poolargs->dhcp_sec, pool_instance, interface);
				SUBENTRY(entry_dhcp_option_instance, ctx, pool_instance, idx);
				dmfree(tt);
				break;
			}
			if (!found)
			{
				if(idx!=NULL)
					idx_last=dmstrdup(idx);
				DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name(poolargs->dhcp_sec), &s, &value);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "tag", pch);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "value", spch);
				init_args_option(ctx, s, poolargs->dhcp_sec, pool_instance, interface);
				idx = handle_update_instance(2, ctx, &idx_last, update_instance_alias, 3, s, "option_instance", "option_alias");
				SUBENTRY(entry_dhcp_option_instance, ctx, pool_instance, idx);
				dmfree(tt);
			}
		}
	}
}

inline int entry_dhcp_option_instance(struct dmctx *ctx, char * pool_instance, char *option_instance)
{
	IF_MATCH(ctx, DMROOT"DHCPv4.Server.Pool.%s.Option.%s.", pool_instance, option_instance) {
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Option.%s.", ctx, "1", 1, NULL, delete_dhcp_option, NULL, pool_instance, option_instance);
		DMPARAM("Alias", ctx, "1", get_dhcp_option_alias, set_dhcp_option_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Tag", ctx, "1", get_dhcp_option_tag, set_dhcp_option_tag, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("Value", ctx, "1", get_dhcp_option_value, set_dhcp_option_value, "xsd:string", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dhcp_static_address_instance(struct dmctx *ctx, char *int_num, char *st_address)
{
	IF_MATCH(ctx, DMROOT"DHCPv4.Server.Pool.%s.StaticAddress.%s.", int_num, st_address) {
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.StaticAddress.%s.", ctx, "1", NULL, NULL, delete_dhcp_staticaddress, NULL, int_num, st_address);
		DMPARAM("Alias", ctx, "1", get_dhcp_static_alias, set_dhcp_static_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Chaddr", ctx, "1", get_dhcp_staticaddress_chaddr, set_dhcp_staticaddress_chaddr, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Yiaddr", ctx, "1", get_dhcp_staticaddress_yiaddr, set_dhcp_staticaddress_yiaddr, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dhcp_client(struct dmctx *ctx, char *interface, char *idev )
{
	struct uci_section *sss = NULL;
	char *idx = NULL, *idx_last = NULL;
	json_object *res = NULL, *client_obj = NULL;
	char *dhcp, *network;
	int id = 0;
	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			dhcp = dmjson_get_value(client_obj, 1, "dhcp");
			if(strcmp(dhcp, "true") == 0)
			{
				network = dmjson_get_value(client_obj, 1, "network");
				if(strcmp(network, interface) == 0)
				{
					init_dhcp_client_args(ctx, client_obj);
					idx = handle_update_instance(2, ctx, &idx_last, update_instance_without_section, 1, ++id);
					SUBENTRY(entry_dhcp_client_instance, ctx, idev, idx, key);
				}
			}
		}
	}
	return 0;
}

inline int entry_dhcp_client_instance(struct dmctx *ctx, char *int_num, char *idx, char *key)
{
	char linker[32] = "linker_dhcp:";
	strcat(linker, key);
	IF_MATCH(ctx, DMROOT"DHCPv4.Server.Pool.%s.Client.%s.", int_num, idx) {
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Client.%s.", ctx, "1", NULL, NULL, NULL, linker, int_num, idx);
		DMPARAM("Chaddr", ctx, "0", get_dhcp_client_chaddr, NULL, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Client.%s.IPv4Address.", ctx, "0", 1, NULL, NULL, NULL, int_num, idx);
		SUBENTRY(entry_dhcp_client_ipv4address, ctx, int_num, idx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dhcp_client_ipv4address_instance(struct dmctx *ctx, char* int_num1, char *int_num2, char *idx){
	IF_MATCH(ctx, DMROOT"DHCPv4.Server.Pool.%s.Client.%s.IPv4Address.%s.", int_num1, int_num2, idx) {
		DMOBJECT(DMROOT"DHCPv4.Server.Pool.%s.Client.%s.IPv4Address.%s.", ctx, "0", NULL, NULL, NULL, NULL, int_num1, int_num2, idx);
		DMPARAM("LeaseTimeRemaining", ctx, "0", get_dhcp_client_ipv4address_leasetime, NULL, NULL, 0, 1, UNDEF, NULL);
	}
}

