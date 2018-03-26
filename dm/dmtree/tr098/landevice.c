/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */

#include <ctype.h>
#include <uci.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cwmp.h"
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmentry.h"
#include "landevice.h"
#include "wepkey.h"
#include "dmubus.h"
#include "dmjson.h"
#define DELIMITOR ","
#define TAILLE 10
#define MAX_PROC_ARP 256
#define DHCP_LEASE_FILE "/var/dhcp.leases"

inline int entry_landevice_sub_instance(struct dmctx *ctx, struct uci_section *landevice_section, char *idev);
inline int entry_landevice_ipinterface_instance (struct dmctx *ctx, char *idev, char *ilan);
inline int entry_landevice_dhcpstaticaddress_instance(struct dmctx *ctx, char *idev, char *idhcp);
inline int entry_landevice_wlanconfiguration_instance(struct dmctx *ctx, char *idev,char *iwlan);
inline int entry_landevice_wlanconfiguration_presharedkey_instance(struct dmctx *ctx, char *idev, char *iwlan, char *ipk);
inline int entry_landevice_wlanconfiguration_associateddevice(struct dmctx *ctx, char *idev, char *iwlan);
inline int entry_landevice_wlanconfiguration_associateddevice_instance(struct dmctx *ctx, char *idev, char *iwlan, char *idx);
inline int entry_landevice_lanethernetinterfaceconfig_instance(struct dmctx *ctx, char *idev, char *ieth);
inline int entry_landevice_host_instance(struct dmctx *ctx, char *idev, char *idx);

inline int entry_landevice_wlanconfiguration_wepkey_instance(struct dmctx *ctx, char *idev, char *iwlan, char *iwep);
inline int entry_landevice_dhcpconditionalservingpool_option_instance(struct dmctx *ctx, char * idev, char *icondpool, char *idx);
inline int entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance(struct dmctx *ctx, char * idev, char *icondpool);
inline int entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool(struct dmctx *ctx, char *idev);

struct ldlanargs cur_lanargs = {0};
struct ldipargs cur_ipargs = {0};
struct lddhcpargs cur_dhcpargs = {0};
struct ldwlanargs cur_wlanargs = {0};
struct ldethargs cur_ethargs = {0};
struct wlan_psk cur_pskargs = {0};
struct wlan_wep cur_wepargs = {0};
struct wl_clientargs cur_wl_clientargs = {0};
struct clientargs cur_clientargs = {0};
struct dhcppoolargs cur_dhcppoolargs = {0};
struct dhcppooloptionargs cur_dhcppooloptionargs = {0};

inline int init_ldargs_lan(struct dmctx *ctx, struct uci_section *s, char *iwan)
{
	struct ldlanargs *args = &cur_lanargs;
	ctx->args = (void *)args;
	args->ldlansection = s;
	args->ldinstance = iwan;
	return 0;
}

inline int init_ldargs_ip(struct dmctx *ctx, struct uci_section *s)
{
	struct ldipargs *args = &cur_ipargs;
	ctx->args = (void *)args;
	args->ldipsection = s;
	return 0;
}

inline int init_ldargs_dhcp(struct dmctx *ctx, struct uci_section *s)
{
	struct lddhcpargs *args = &cur_dhcpargs;
	ctx->args = (void *)args;
	args->lddhcpsection = s;
	return 0;
}

inline int init_args_dhcp_conditional_servingpool_entry(struct dmctx *ctx, struct uci_section *s)
{
	struct dhcppoolargs *args = &cur_dhcppoolargs;
	ctx->args = (void *)args;
	args->dhcppoolsection = s;
	return 0;
}

inline int init_args_pool_option(struct dmctx *ctx, struct uci_section *s, struct uci_section *ss)
{
	struct dhcppooloptionargs *args = &cur_dhcppooloptionargs;
	ctx->args = (void *)args;
	args->dhcppooloptionsection = s;
	args->dhcppoolsection = ss;
	return 0;
}

inline int init_ldargs_wlan(struct dmctx *ctx, struct uci_section *wifisection, int wlctl_num,
						struct uci_section *device_section, char *wunit, char *wiface, json_object *res, int pki)
{
	struct ldwlanargs *args = &cur_wlanargs;
	ctx->args = (void *)args;
	args->lwlansection = wifisection;
	args->device_section = device_section;
	args->res = res;
	args->wlctl_num = wlctl_num;
	args->wiface = wiface;
	args->wunit = wunit;
	args->pki = pki;
	return 0;
}

inline int init_ldargs_eth_cfg(struct dmctx *ctx, char *eth, struct uci_section *eth_section)
{
	struct ldethargs *args = &cur_ethargs;
	ctx->args = (void *)args;
	args->eth = eth;
	args->lan_ethsection = eth_section;
	return 0;
}

inline int init_client_args(struct dmctx *ctx, json_object *clients, char *lan_name)
{
	struct clientargs *args = &cur_clientargs;
	ctx->args = (void *)args;
	args->client = clients;
	args->lan_name = lan_name;
	return 0;
}

inline int init_wl_client_args(struct dmctx *ctx, char *value, char *wiface)
{
	struct wl_clientargs *args = &cur_wl_clientargs;
	ctx->args = (void *)args;
	args->mac = value;
	args->wiface = wiface;

	return 0;
}

inline int init_wlan_psk_args(struct dmctx *ctx, struct uci_section *s)
{
	struct wlan_psk *args = &cur_pskargs;
	ctx->args = (void *)args;
	args->wlanpsk = s;
	return 0;
}

inline int init_wlan_wep_args(struct dmctx *ctx, struct uci_section *s)
{
	struct wlan_wep *args = &cur_wepargs;
	ctx->args = (void *)args;
	args->wlanwep = s;
	return 0;
}

void update_dhcp_conf_start(int i, void *data)
{
		json_object *res, *jobj;
		struct dmctx dmctx = {0};
		struct dhcp_param *dhcp_param = (struct dhcp_param *)(data);
		char *mask, *start, *dhcp_name, *ipaddr, buf[16];
		
		dm_ctx_init(&dmctx);
		dmuci_get_option_value_string("network", dhcp_param->interface, "ipaddr", &ipaddr);
		if (ipaddr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipaddr = dmjson_get_value(jobj, 1, "address");
			}
		}
		if (ipaddr[0] == '\0')
			goto end;

		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask);
		if (mask[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				mask = dmjson_get_value(jobj, 1, "mask");
				if (mask[0] == '\0')
					goto end;
				mask = cidr2netmask(atoi(mask));
			}
		}
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "start", &start);
		ipcalc_rev_start(ipaddr, mask, start, buf);
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "dhcp_sec", &dhcp_name);
		dmuci_set_value("dhcp", dhcp_name, "start", buf);
		dmuci_set_varstate_value("cwmp", dhcp_param->state_sec, "start", "");
		dmuci_commit();
		dm_ctx_clean(&dmctx);
end:
		FREE(dhcp_param->state_sec);
		FREE(dhcp_param->interface);
		return;
}

void update_dhcp_conf_end(int i, void *data)
{
		json_object *res, *jobj;
		char *ipaddr, *mask, *start, *dhcp_name, *limit, buf[16], buf_start[16] = "";
		struct dhcp_param *dhcp_param = (struct dhcp_param *)(data);
		struct dmctx dmctx = {0};

		dm_ctx_init(&dmctx);
		dmuci_get_option_value_string("network", dhcp_param->interface, "ipaddr", &ipaddr);
		if (ipaddr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipaddr = dmjson_get_value(jobj, 1, "address");
			}
		}
		if (ipaddr[0] == '\0')
			goto end;

		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask);
		if (mask[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				mask = dmjson_get_value(jobj, 1, "mask");
				if (mask[0] == '\0')
					goto end;
				mask = cidr2netmask(atoi(mask));
			}
		}
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "dhcp_sec", &dhcp_name);
		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask); //TODO
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "start", &start);
		if (!start || start[0] == '\0')
			dmuci_get_option_value_string("dhcp", dhcp_name, "start", &start);
		else
			ipcalc_rev_start(ipaddr, mask, start, buf_start);
		if (!start || start[0] == '\0')
			goto end;
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "limit", &limit);
		if (buf_start[0] != '\0')
			ipcalc_rev_end(ipaddr, mask, buf_start, limit, buf);
		else
		ipcalc_rev_end(ipaddr, mask, start, limit, buf);
		dmuci_set_value("dhcp", dhcp_name, "limit", buf);
		dmuci_set_varstate_value("cwmp", dhcp_param->state_sec, "limit", "");
		dmuci_commit();
		dm_ctx_clean(&dmctx);
end:
		FREE(dhcp_param->state_sec);
		FREE(dhcp_param->interface);
		return;
}
int get_dhcp_option_last_inst(struct uci_section *ss)
{
	char *drinst = NULL, *tmp;
	int dr = 0;
	struct uci_section *s;

	uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), s) {
		dmuci_get_value_by_section_string(s, "optioninst", &tmp);
		if (tmp[0] == '\0')
			break;
		drinst = tmp;
	}
	if (drinst) dr = atoi(drinst);
	return dr;
}

char *dhcp_option_update_instance_alias_icwmpd(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	char *inst_opt = (char *) argv[1];
	char *alias_opt = (char *) argv[2];
	bool *find_max = (bool *) argv[3];

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			int m = get_dhcp_option_last_inst(s);
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, inst_opt, buf);
	}
	*last_inst = instance;
	if (action == INSTANCE_MODE_ALIAS) {
		dmuci_get_value_by_section_string(s, alias_opt, &alias);
		if (alias[0] == '\0') {
			sprintf(buf, "cpe-%s", instance);
			alias = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, alias_opt, buf);
		}
		sprintf(buf, "[%s]", alias);
		instance = dmstrdup(buf);
	}
	return instance;
}
/*******************ADD-DEL OBJECT*********************/
int add_dhcp_serving_pool_option(struct dmctx *ctx, char **instancepara)
{
	char val[64];
	char *value, *instance, *tmp;
	struct uci_section *s = NULL;
	struct dhcppoolargs *poolargs = (struct dhcppoolargs *)ctx->args;

	instance = get_last_instance(DMMAP, section_name(poolargs->dhcppoolsection), "optioninst");
	DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name(poolargs->dhcppoolsection), &s, &value);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "dhcp_option", "0");
	*instancepara = update_instance_icwmpd(s, instance, "optioninst");
	sprintf(val, "vendorclass%s", *instancepara);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "value", val);
	sprintf(val, "0,vendorclass%s", *instancepara);
	dmuci_add_list_value_by_section( poolargs->dhcppoolsection, "dhcp_option", val);
	return 0;
}

int delete_dhcp_serving_pool_option_all(struct dmctx *ctx)
{
	struct dhcppoolargs *poolargs = (struct dhcppoolargs *)ctx->args;
	char *value;
	char *instance;
	struct uci_list *val;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL;
	struct uci_element *e = NULL, *tmp;
	int dmmap = 0;
	uci_path_foreach_sections(icwmpd, "dmmap", section_name(poolargs->dhcppoolsection), dmmap_s)
	{
		if (dmmap)
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
		dmmap_ss = dmmap_s;
		dmmap++;
	}
	if (dmmap_ss != NULL)
		DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
	dmuci_get_value_by_section_list(poolargs->dhcppoolsection, "dhcp_option", &val);
	if (val) {
		uci_foreach_element_safe(val, e, tmp)
		{
			dmuci_del_list_value_by_section(poolargs->dhcppoolsection, "dhcp_option", tmp->name); //TODO test it
		}
	}
	return 0;
}

int delete_dhcp_serving_pool_option(struct dmctx *ctx)
{
	int dmmap = 0;
	char *value, *tag, *instance,  *bufopt;
	struct uci_list *val;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL;
	struct uci_element *e = NULL, *tmp;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &tag);
	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", &value);
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
	DMUCI_DELETE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, NULL, NULL);
	return 0;
}


int add_dhcp_conditional_serving_pool(struct dmctx *ctx, char **instancepara)
{
	char *value;
	char *instance;
	struct uci_section *s = NULL;

	instance = get_last_instance("dhcp", "vendorclass", "poulinstance");
	dmuci_add_section("dhcp", "vendorclass", &s, &value);
	dmuci_set_value_by_section(s, "dhcp_option", "");
	*instancepara = update_instance(s, instance, "poulinstance");
	return 0;
}

int delete_dhcp_conditional_serving_pool_all(struct dmctx *ctx)
{
	int found = 0;
	int dmmap = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL;

	uci_foreach_sections("dhcp", "vendorclass", s) {
		if (found != 0)
		{
			uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), dmmap_s)
			{
				if (dmmap != 0)
					DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
				dmmap_ss = dmmap_s;
				dmmap++;
			}
			if (dmmap_ss != NULL)
				DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
			dmmap = 0;
			dmmap_ss = NULL;
			dmuci_delete_by_section(ss, NULL, NULL);
		}
		ss = s;
		found++;
	}
	if (ss != NULL)
	{
		uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), dmmap_s)
		{
			if (dmmap != 0)
				DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
			dmmap_ss = dmmap_s;
			dmmap++;
		}
		if (dmmap_ss != NULL)
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
		dmuci_delete_by_section(ss, NULL, NULL);
	}
	return 0;
}

int delete_dhcp_conditional_serving_pool(struct dmctx *ctx)
{
	struct dhcppoolargs *poolargs = (struct dhcppoolargs *)ctx->args;
	int dmmap = 0;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL;


	uci_path_foreach_sections(icwmpd, "dmmap", section_name(poolargs->dhcppoolsection), dmmap_s)
		{
			if (dmmap)
				DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
			dmmap_ss = dmmap_s;
			dmmap++;
		}
		if (dmmap_ss != NULL)
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
	dmuci_delete_by_section(poolargs->dhcppoolsection, NULL, NULL);
	return 0;
}
int add_landevice_dhcpstaticaddress(struct dmctx *ctx, char **instancepara)
{
	char *value;
	char *instance;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	instance = get_last_instance_lev2("dhcp", "host", "ldhcpinstance", "interface", lan_name);
	dmuci_add_section("dhcp", "host", &s, &value);
	dmuci_set_value_by_section(s, "mac", DHCPSTATICADDRESS_DISABLED_CHADDR);
	dmuci_set_value_by_section(s, "interface", lan_name);
	*instancepara = update_instance(s, instance, "ldhcpinstance");
	return 0;
}

int delete_landevice_dhcpstaticaddress_all(struct dmctx *ctx)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	
	lan_name = section_name(lanargs->ldlansection);
	uci_foreach_option_eq("dhcp", "host", "interface", lan_name, s) {
		if (found != 0)
			dmuci_delete_by_section(ss, NULL, NULL);
		ss = s;
		found++;
	}
	if (ss != NULL)
		dmuci_delete_by_section(ss, NULL, NULL);
	return 0;	
}

int delete_landevice_dhcpstaticaddress(struct dmctx *ctx)
{
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;
	
	dmuci_delete_by_section(dhcpargs->lddhcpsection, NULL, NULL);
	return 0;
}


int add_landevice_wlanconfiguration(struct dmctx *ctx, char **instancepara)
{
	char *value;
	char ssid[16] = {0};
	char *instance;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	instance = get_last_instance_lev2("wireless", "wifi-iface", "lwlaninstance", "network", lan_name);
	sprintf(ssid, "Inteno_%s_%d", lan_name, instance ? (atoi(instance)+1) : 1);
	dmuci_add_section("wireless", "wifi-iface", &s, &value);
	dmuci_set_value_by_section(s, "device", "wl0");
	dmuci_set_value_by_section(s, "encryption", "none");
	dmuci_set_value_by_section(s, "macfilter", "0");
	dmuci_set_value_by_section(s, "mode", "ap");
	dmuci_set_value_by_section(s, "network", lan_name);
	dmuci_set_value_by_section(s, "ssid", ssid);
	*instancepara = update_instance(s, instance, "lwlaninstance");
	return 0;
}

int delete_landevice_wlanconfiguration_all(struct dmctx *ctx)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	
	lan_name = section_name(lanargs->ldlansection);
	uci_foreach_option_eq("wireless", "wifi-iface", "network", lan_name, s) {
		if (found != 0)
				dmuci_delete_by_section(ss, NULL, NULL);
		ss = s;
		found++;
	}
	if (ss != NULL)
		dmuci_delete_by_section(ss, NULL, NULL);
	return 0;
}

int delete_landevice_wlanconfiguration(struct dmctx *ctx)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_delete_by_section(wlanargs->lwlansection, NULL, NULL);
	return 0;
}
/******************************************************/
/************************************************************************** 
**** ****  function related to landevice_lanhostconfigmanagement  **** ****
***************************************************************************/

int get_lan_dns(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	int len;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	if ((*value)[0] == '\0') {
		dmuci_get_value_by_section_string(lanargs->ldlansection, "dns", value);
		*value = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
		char *p = *value;
		while (*p) {
			if (*p == ' ' && p != *value && *(p-1) != ',')
				*p++ = ',';
			else
				p++;
		}
	}
	return 0;
}

int set_lan_dns(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
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
			dmuci_set_value_by_section(lanargs->ldlansection, "dns", dup);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, char **value)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", section_name(lanargs->ldlansection), s) {
		*value = "1";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	bool b;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				if (!b) {
					dmuci_delete_by_section(s, NULL, NULL);
				}
				break;
			}
			if (s == NULL && b) {
				dmuci_set_value("dhcp",lan_name, NULL, "dhcp");
				dmuci_set_value("dhcp", lan_name, "interface", lan_name);
				dmuci_set_value("dhcp", lan_name, "start", "100");
				dmuci_set_value("dhcp", lan_name, "limit", "150");
				dmuci_set_value("dhcp", lan_name, "leasetime", "12h");
			}
			return 0;
	}
	return 0;
}

int get_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, char **value)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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

int set_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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

int get_lan_dhcp_interval_address(struct dmctx *ctx, char **value, int option)
{
	json_object *res, *jobj;
	char *ipaddr = "" , *mask = "", *start , *limit;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	struct uci_section *s = NULL;
	char bufipstart[16], bufipend[16];

	*value = "";
	
	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
		dmuci_get_value_by_section_string(s, "start", &start);
		if (option == LANIP_INTERVAL_END)
			dmuci_get_value_by_section_string(s, "limit", &limit);
		break;
	}
	if (s == NULL) {
		goto end;
	}
	dmuci_get_value_by_section_string(lanargs->ldlansection, "ipaddr", &ipaddr);
	if (ipaddr[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		if (res)
		{
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			ipaddr = dmjson_get_value(jobj, 1, "address");
		}
	}
	if (ipaddr[0] == '\0') {
		goto end;
	}
	dmuci_get_value_by_section_string(lanargs->ldlansection, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		if (res) {
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			mask = dmjson_get_value(jobj, 1, "mask");
			if (mask[0] == '\0') {
				goto end;
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

end:
	return 0;
}

int get_lan_dhcp_interval_address_start(char *refparam, struct dmctx *ctx, char **value)
{
	char *s_name, *tmp;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	dmasprintf(&s_name, "@%s[0]", lan_name);
	dmuci_get_varstate_string("cwmp", s_name, "start", &tmp);
	dmfree(s_name);
	if(tmp[0] != '\0')
	{
		*value = tmp;
		return 0;
	}
	int ret = get_lan_dhcp_interval_address(ctx, value, LANIP_INTERVAL_START);
	return ret;
}

int get_lan_dhcp_interval_address_end(char *refparam, struct dmctx *ctx, char **value)
{
	char *s_name, *tmp;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	dmasprintf(&s_name, "@%s[0]", lan_name);
	dmuci_get_varstate_string("cwmp", s_name, "limit", &tmp);
	dmfree(s_name);	
	if(tmp[0] != '\0')
	{
		*value = tmp;
		return 0;
	}
	int ret = get_lan_dhcp_interval_address(ctx, value, LANIP_INTERVAL_END);
	return ret;
}

int set_lan_dhcp_address_start(char *refparam, struct dmctx *ctx, int action, char *value)
{
	json_object *res;
	char *ipaddr = "", *mask = "", *start , *limit, buf[16];
	char *s_name = "", *tmp, *dhcp_name = NULL;
	struct uci_section *s = NULL;
	struct uci_section *curr_section = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	struct dhcp_param *dhcp_param_1;
				
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dhcp_name = section_name(s);
				break;
			}
			if (!s) return 0;
			dmasprintf(&s_name, "@%s[0]", lan_name);
			curr_section = dmuci_walk_state_section("cwmp", lan_name, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", lan_name, &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", s_name, "start", value);
			dmuci_set_varstate_value("cwmp", s_name, "dhcp_sec", dhcp_name);
			dmfree(s_name);
			dhcp_param_1 = calloc(1, sizeof(struct dhcp_param));
			dhcp_param_1->interface = strdup(lan_name);
			dhcp_param_1->state_sec = strdup((curr_section)->e.name);
			dm_add_end_session(&update_dhcp_conf_start, 0, (void*)(dhcp_param_1));
			return 0;
	}
	return 0;
}

int set_lan_dhcp_address_end(char *refparam, struct dmctx *ctx, int action, char *value)
{
	int i_val;
	json_object *res;
	char *ipaddr = "", *mask = "", *start, buf[16], *tmp, *s_name = NULL;
	struct uci_section *s = NULL;
	struct uci_section *curr_section = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	char *dhcp_name = NULL;
	struct dhcp_param *dhcp_param;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dmuci_get_value_by_section_string(s, "start", &start);
				dhcp_name = section_name(s);
				break;
			}
			if (!s) return 0;

			dmasprintf(&s_name, "@%s[0]", lan_name);
			curr_section = dmuci_walk_state_section("cwmp", lan_name, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", lan_name, &curr_section, &tmp);
				}
			dmuci_set_varstate_value("cwmp", s_name, "limit", value);
			dmuci_set_varstate_value("cwmp", s_name, "dhcp_sec", dhcp_name);
			dmfree(s_name);
			dhcp_param = calloc(1, sizeof(struct dhcp_param));
			dhcp_param->interface = strdup(lan_name);
			dhcp_param->state_sec = strdup((curr_section)->e.name);
			dm_add_end_session(&update_dhcp_conf_end, 0, (void*)(dhcp_param));
			return 0;
	}
	return 0;
}

int get_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, char **value) 
{	
	char val[512] = {0}, *p;
	struct uci_section *s = NULL;
	char *min, *max, *ip, *s_n_ip;
	unsigned int n_min, n_max, n_ip;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	*value = "";
	
	get_lan_dhcp_interval_address(ctx, &min, LANIP_INTERVAL_START);
	get_lan_dhcp_interval_address(ctx, &max, LANIP_INTERVAL_END);
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

int set_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *s = NULL;
	struct uci_section *dhcp_section = NULL;
	char *min, *max, *ip, *val, *local_value;
	char *pch, *spch;
	unsigned int n_min, n_max, n_ip;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_lan_dhcp_interval_address(ctx, &min, LANIP_INTERVAL_START);
			get_lan_dhcp_interval_address(ctx, &max, LANIP_INTERVAL_END);
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
					dmuci_set_value_by_section(dhcp_section, "mac", DHCPSTATICADDRESS_DISABLED_CHADDR);
					dmuci_set_value_by_section(dhcp_section, "interface", lan_name);
					dmuci_set_value_by_section(dhcp_section, "ip", pch);
				}
			}
			dmfree(local_value);
			uci_foreach_sections("dhcp", "host", s) {
				dmuci_get_value_by_section_string(s, "ip", &ip);
				n_ip =	inet_network(ip);
				if (n_ip >= n_min && n_ip <= n_max)
					dmuci_delete_by_section(s, "ip", NULL);
			}
			return 0;
	}
	return 0;
}

int get_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	char *mask;
	json_object *res, *jobj;
	struct uci_section *s = NULL;
	char *val;
	*value = "";
	
	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
		dmuci_get_value_by_section_string(s, "netmask", value);
		break;
	}
	if (s == NULL || (*value)[0] == '\0')
		dmuci_get_value_by_section_string(lanargs->ldlansection, "netmask", value);
	if ((*value)[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		mask = dmjson_get_value(jobj, 1, "mask");
		int i_mask = atoi(mask);
		val = cidr2netmask(i_mask);
		*value = dmstrdup(val);// MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dmuci_set_value_by_section(s, "netmask", value);
				return 0;
			}
			return 0;
	}
	return 0;
}

int get_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, char **value) 
{
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(lanargs->ldlansection, "gateway", value);
	if ((*value)[0] == '\0') {
		dmuci_get_value_by_section_string(lanargs->ldlansection, "ipaddr", value);
	}
	return 0;
}

int set_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, int action, char *value) 
{
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(lanargs->ldlansection, "gateway", value);
			return 0;
	}
	return 0;
}

int get_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, char **value)
{
	int len, mtime = 0;
	char *ltime = "", *pch, *spch, *ltime_ini, *tmp, *tmp_ini;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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
	if (strcmp(ltime, "infinite") == 0)
		mtime = -1;
	dmfree(tmp);
	dmfree(tmp_ini);

	dmasprintf(value, "%d", mtime); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, int action, char *value) 
{
	struct uci_section *s = NULL;
	char buf[32];
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				int val = atoi(value);
				if (val == -1)
					sprintf(buf, "infinite");
				else
					sprintf(buf, "%ds", val);
				dmuci_set_value_by_section(s, "leasetime",  buf);
				break;
			}
			return 0;
	}	
	return 0;
}

int get_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, char **value) 
{
	char *result, *str;
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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

int set_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *result, *dn, *pch;
	struct uci_list *val;
	struct uci_section *s = NULL;
	struct uci_element *e = NULL, *tmp;
	char *option = "dhcp_option", buf[64];
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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
	dmuci_add_list_value_by_section(s, "dhcp_option", buf);
	return 0;
}

int get_lan_host_nbr_entries(char *refparam, struct dmctx *ctx, char **value) 
{
	int entries = 0;
	char *network;
	json_object *res;
	struct ldlanargs *lanargs = (struct ldlanargs *)ctx->args;
	char *lan_name = section_name(lanargs->ldlansection);
	
	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, val) {
		network = dmjson_get_value(val, 1, "network");
		if (strcmp(network, lan_name) == 0)
			entries++;
	}
	dmasprintf(value, "%d", entries); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}
/***************************************************************************/


/************************************************************************** 
**** function related to landevice_lanhostconfigmanagement_ipinterface ****
***************************************************************************/

int get_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);
	return get_interface_enable_ubus(lan_name, refparam, ctx, value);
}

int set_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);
	return set_interface_enable_ubus(lan_name, refparam, ctx, action, value);
}

int get_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);
	return get_interface_firewall_enabled(lan_name, refparam, ctx, value);
}

int set_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);
	return set_interface_firewall_enabled(lan_name, refparam, ctx, action, value);
}

int get_interface_ipaddress(char *refparam, struct dmctx *ctx, char **value)
{
	char *proto;
	json_object *res, *jobj;
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;//TO CHECK
	char *lan_name = section_name(ipargs->ldipsection);
	
	dmuci_get_value_by_section_string(ipargs->ldipsection, "proto", &proto);
	if (strcmp(proto, "static") == 0)
		dmuci_get_value_by_section_string(ipargs->ldipsection, "ipaddr", value);
	else {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		*value = dmjson_get_value(jobj, 1, "address");
	}
	return 0;
}

int set_interface_ipaddress(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *lan_name = section_name(ipargs->ldipsection);
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
			dmuci_set_value_by_section(ipargs->ldipsection, "ipaddr", value);
			return 0;
	}
	return 0;
}

int get_interface_subnetmask(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	char *proto;
	char *val = NULL;
	json_object *res, *jobj;
	char *tmp;
	char *lan_name = section_name(ipargs->ldipsection);
	
	dmuci_get_value_by_section_string(ipargs->ldipsection, "proto", &proto);
	if (strcmp(proto, "static") == 0)
		dmuci_get_value_by_section_string(ipargs->ldipsection, "netmask", value);
	else {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
    	val = dmjson_get_value(jobj, 1, "mask");
		tmp = cidr2netmask(atoi(val));
		*value = dmstrdup(tmp); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_interface_subnetmask(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
			dmuci_set_value_by_section(ipargs->ldipsection, "netmask", value);
			return 0;
	}
	return 0;
}

int get_interface_addressingtype (char *refparam, struct dmctx *ctx, char **value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;
	*value = "";

	dmuci_get_value_by_section_string(ipargs->ldipsection, "proto", value);
	if (strcmp(*value, "static") == 0)
		*value = "Static";
	else if (strcmp(*value, "dhcp") == 0)
		*value = "DHCP";
	return 0;
}

int set_interface_addressingtype(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldipargs *ipargs = (struct ldipargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Static") == 0)
				dmuci_set_value_by_section(ipargs->ldipsection, "proto", "static");
			else if (strcmp(value, "DHCP") == 0)
				dmuci_set_value_by_section(ipargs->ldipsection, "proto", "dhcp");
			return 0;
	}
	return 0;
}

/************************************************************************************* 
**** function related to get_landevice_lanhostconfigmanagement_dhcpstaticaddress ****
**************************************************************************************/

int get_dhcpstaticaddress_enable (char *refparam, struct dmctx *ctx, char **value)
{
	char *mac;
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;

	dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac", &mac);
	if (strcmp (mac, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_dhcpstaticaddress_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *chaddr;
	bool b;
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac", &chaddr);
			if (b) {
				if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0) {
					char *orig_chaddr;
					dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac_orig", &orig_chaddr);
					dmuci_set_value_by_section(dhcpargs->lddhcpsection, "mac", orig_chaddr);
				} else {
					return 0;
				}
			} else {
				if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
					return 0;
				else {
					char *orig_chaddr;
					dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac", &orig_chaddr);
					dmuci_set_value_by_section(dhcpargs->lddhcpsection, "mac_orig", orig_chaddr);
					dmuci_set_value_by_section(dhcpargs->lddhcpsection, "mac", DHCPSTATICADDRESS_DISABLED_CHADDR);
				}
			}
			return 0;
	}
	return 0;
}

int get_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, char **value)
{
	char *chaddr;
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;
	
	dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac", &chaddr);
	if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac_orig", value);
	else 
		*value = chaddr;
	return 0;
}

int set_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	char *chaddr;
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;
		
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "mac", &chaddr);
			if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
				dmuci_set_value_by_section(dhcpargs->lddhcpsection, "mac_orig", value);
			else
				dmuci_set_value_by_section(dhcpargs->lddhcpsection, "mac", value);
			return 0;
	}
	return 0;
}

int get_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, char **value)
{
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;
	
	dmuci_get_value_by_section_string(dhcpargs->lddhcpsection, "ip", value);
	return 0;
}

int set_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct lddhcpargs *dhcpargs = (struct lddhcpargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dhcpargs->lddhcpsection, "ip", value);
			return 0;
	}
	return 0;
}
/*************************************/


/************************************************************************************* 
**** function related to get_landevice_ethernet_interface_config ****
**************************************************************************************/

int get_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
		
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 1, "up");
	return 0;
}

int set_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				DMCMD("ethctl", 3, ethargs->eth, "phy-power", "up"); //TODO wait ubus command
			}
			else {
				DMCMD("ethctl", 3, ethargs->eth, "phy-power", "down"); //TODO wait ubus command
			}
			return 0;
	}
	return 0;
}

int get_lan_eth_iface_cfg_status(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	bool b;
		
	get_lan_eth_iface_cfg_enable(refparam, ctx, value);
	string_to_bool(*value, &b);
	if (b)
		*value = "Up";
	else 
		*value = "Disabled";
	return 0;
}

int get_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *pch, *spch, *v;
	int len;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	struct uci_section *s;
	*value = "";
	
	uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
		dmuci_get_value_by_section_string(s, "speed", value);
	}
	if ((*value)[0] == '\0')
		return 0;
	else {
		if (strcmp(*value, "auto") == 0)
			*value = "Auto";
		else {
			v = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
			pch = strtok_r(v, "FHfh", &spch);
			len = strlen(pch) + 1;
			*value = pch;
		}
	}
	return 0;
}

int set_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *val = NULL;
	char *duplex;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	struct uci_section *s, *sec = NULL;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
				sec = s;
				break;
			}
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(sec, "speed", value);
				return 0;
			} else {
				dmuci_get_value_by_section_string(sec, "speed", &duplex);
				if (strcmp(duplex, "auto") == 0)
					duplex = "FD";
				else {
					if (strstr(duplex, "FD"))
						duplex = "FD";
					else if (strstr(duplex, "HD"))
						duplex = "HD";
					else
						duplex = "FD";
				}
				dmastrcat(&val, value, duplex);
				dmuci_set_value_by_section(sec, "speed", val);
				dmfree(val);
			}
			return 0;

	}
	return 0;
}

int get_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, char **value)
{
	char *tmp = "";
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	struct uci_section *s;

	uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
		dmuci_get_value_by_section_string(s, "speed", &tmp);
	}
	if (strcmp(tmp, "auto") == 0) {
		*value = "Auto";
	}
	else if (tmp[0] == '\0') {
		*value = "";
	}	
	else {
		if (strstr(tmp, "FD"))
			*value = "Full";
		else if (strstr(tmp, "HD"))
			*value = "Half";
		else
			*value = "";
	}
	return 0;
}

int set_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *m, *spch, *rate, *val = NULL;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	struct uci_section *s, *sec = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
				sec = s;
				break;
			}
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(sec, "speed", "auto");
				return 0;
			}
			dmuci_get_value_by_section_string(sec, "speed", &m);
			m = dmstrdup(m);
			rate = m;
			if (strcasecmp(rate, "auto") == 0)
				rate = "100";
			else {
				strtok_r(rate, "FHfh", &spch);
			}
			if (strcasecmp(value, "full") == 0)
				dmastrcat(&val, rate, "FD");
			else if (strcasecmp(value, "half") == 0)
				dmastrcat(&val, rate, "HD");
			else {
				dmfree(m);
				return 0;
			}
			dmuci_set_value_by_section(sec, "speed", val);
			dmfree(m);
			dmfree(val);
			return 0;
	}
	return 0;
}

int get_lan_eth_iface_cfg_stats_tx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_lan_eth_iface_cfg_stats_rx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_lan_eth_iface_cfg_stats_tx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_lan_eth_iface_cfg_stats_rx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)ctx->args;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

//HOST DYNAMIC
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
						value = dmjson_get_value(val, 1, "assoc_mac");
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

int get_lan_host_ipaddress(char *refparam, struct dmctx *ctx, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "ipaddr");
	return 0;	
}

int get_lan_host_hostname(char *refparam, struct dmctx *ctx, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "hostname");
	return 0;	
}

int get_lan_host_active(char *refparam, struct dmctx *ctx, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "connected");
	return 0;	
}

int get_lan_host_macaddress(char *refparam, struct dmctx *ctx, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "macaddr");
	return 0;	
}

int get_lan_host_interfacetype(char *refparam, struct dmctx *ctx, char **value)
{
	char *mac;
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	mac = dm_ubus_get_value(clienlargs->client, 1, "macaddr");
	*value = get_interface_type(mac, clienlargs->lan_name);
	return 0;	
}

int get_lan_host_addresssource(char *refparam, struct dmctx *ctx, char **value) {
	char *dhcp;
	struct clientargs *clienlargs = (struct clientargs *)ctx->args;
	
	dhcp = dm_ubus_get_value(clienlargs->client, 1, "dhcp");
	if (strcasecmp(dhcp, "true") == 0)
		*value = "DHCP";
	else 
		*value = "Static";
	return 0;	
}

int get_lan_host_leasetimeremaining(char *refparam, struct dmctx *ctx, char **value)
{
	char buf[80], *dhcp;
	FILE *fp;
	char line[MAX_DHCP_LEASES];
	struct tm ts;
	char *leasetime, *mac_f, *mac, *line1;
	struct clientargs *clientlargs = (struct clientargs *)ctx->args;
	char delimiter[] = " \t";
	
	dhcp = dm_ubus_get_value(clientlargs->client, 1, "dhcp");
	if (strcmp(dhcp, "false") == 0) {
		*value = "0";
	}
	else {
		mac = dm_ubus_get_value(clientlargs->client, 1, "macaddr");
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

/**************************************************************************************
**** **** ****function related to get_landevice_wlanconfiguration_generic **** ********
***************************************************************************************/
int get_wlan_enable(char *refparam, struct dmctx *ctx, char **value)
{
	int i;
	char *val;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "disabled", &val);

	if (val[0] == '1')
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_wlan_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "disabled", "0");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "disabled", "1");
			return 0;
	}
	return 0;
}

int get_wlan_status (char *refparam, struct dmctx *ctx, char **value)
{
	char *tmp;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	dmuci_get_value_by_section_string(wlanargs->device_section, "disabled", &tmp);
	if (tmp[0] == '0' || tmp[0] == '\0')
		*value = "Up";
	else if (tmp[0] == '1')
		*value = "Disabled";
	return 0;
}

int get_wlan_bssid(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	DM_ASSERT(wlanargs->res, *value = "");
	*value = dmjson_get_value(wlanargs->res, 1, "bssid");
	return 0;
}

int get_wlan_max_bit_rate (char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->device_section, "hwmode", value);
	return 0;
}

int set_wlan_max_bit_rate(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "hwmode", value);
			return 0;
	}
	return 0;
}

int get_wlan_channel(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0') {
		DM_ASSERT(wlanargs->res, *value ="");
		*value = dmjson_get_value(wlanargs->res, 1, "channel");
	}
	return 0;
}

int set_wlan_channel(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "channel", value);
			return 0;
	}
	return 0;
}

int get_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "auto";
			else {
				if(wlanargs->res == NULL)
					return 0;
				else
					value = dmjson_get_value(wlanargs->res, 1, "channel");
			}
			dmuci_set_value_by_section(wlanargs->device_section, "channel", value);
			return 0;
	}
	return 0;
}

int get_wlan_ssid(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "ssid", value);
	return 0;
}

int set_wlan_ssid(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->lwlansection, "ssid", value);
			return 0;
	}
	return 0;
}

int get_wlan_beacon_type(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", value);
	if (strcmp(*value, "none") == 0)
		*value = "None";
	else if (strcmp(*value, "wep-shared") == 0 || strcmp(*value, "wep-open") == 0)
		*value = "Basic";
	else if (strcmp(*value, "psk") == 0 || strcmp(*value, "psk+") == 0 || strcmp(*value, "wpa") == 0)
		*value = "WPA";
	else if (strcmp(*value, "psk2") == 0 || strcmp(*value, "psk2+") == 0 || strcmp(*value, "wpa2") == 0)
		*value = "11i";
	else if (strcmp(*value, "mixed-psk") == 0 || strcmp(*value, "mixed-psk+") == 0 || strcmp(*value, "mixed-wpa") == 0)
		*value = "WPAand11i";
	return 0;
}

int set_wlan_beacon_type(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *) ctx->args;
	char *encryption, *option;
	char strk64[4][11];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "None") == 0) {
				value = "none";
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if (strcmp(value, "Basic") == 0) {
				value = "wep-open";
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					wepkey64("Inteno", strk64);
					int i = 0;
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "WPA") == 0) {
				value = "psk";
				char *encryption;
				dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
				if (!strstr(encryption, "psk")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "tkip");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "11i") == 0) {
				value = "psk2";
				if (!strstr(encryption, "psk2")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "ccmp");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "WPAand11i") == 0) {
				value = "mixed-psk";
				if (!strstr(encryption, "mixed-psk")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "tkip+ccmp");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			return 0;
	}
	return 0;
}

int get_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	char *macfilter;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "macfilter", &macfilter);
	if (macfilter[0] != '0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "2";
			else
				value = "0";
			dmuci_set_value_by_section(wlanargs->lwlansection, "macfilter", value);
			return 0;
	}
	return 0;
}

int get_wlan_standard(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->device_section, "hwmode", value);
	if (strcmp(*value, "11b") == 0)
		*value = "b";
	else if (strcmp(*value, "11bg") == 0)
		*value = "g";
	else if (strcmp(*value, "11g") == 0 || strcmp(*value, "11gst") == 0 || strcmp(*value, "11lrs") == 0)
		*value = "g-only";
	else if (strcmp(*value, "11n") == 0 || strcmp(*value, "auto") == 0)
		*value = "n";
	return 0;
}

int set_wlan_standard(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "b") == 0)
				value = "11b";
			else if (strcmp(value, "g") == 0)
				value = "11bg";
			else if (strcmp(value, "g-only") == 0)
				value = "11g";
			else if (strcmp(value, "n") == 0)
				value = "auto";
			dmuci_set_value_by_section(wlanargs->device_section, "hwmode", value);
			return 0;
	}
	return 0;
}

int get_wlan_possible_channels(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res, *jobj;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	*value = "";
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res);
	if(res)
	*value = dmjson_get_value_array_all(res, DELIMITOR, 2, wlanargs->wunit, "channels");
	return 0;
}

int get_wlan_wep_key_index(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	char *encryption;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0)
		dmuci_get_value_by_section_string(wlanargs->lwlansection, "key", value);
	else
		*value = "";
	return 0;
}

int set_wlan_wep_key_index(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0)
			{
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
			}
			dmuci_set_value_by_section(wlanargs->lwlansection, "key", value);
			return 0;
	}	
	return 0;
}

int set_wlan_pre_shared_key(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (!strstr(encryption, "psk")) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk");
			}
			dmuci_set_value_by_section(wlanargs->lwlansection, "key", value);
			return 0;
	}
	return 0;
}

int set_wlan_key_passphrase(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "wep-open") == 0) {
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
			} else if (strcmp(encryption, "none") == 0)
				return 0;
			else
				return set_wlan_pre_shared_key(refparam, ctx, action, value);
			return 0;
	}
	return 0;
}

int get_wlan_wep_encryption_level(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "40-bit, 104-bit";
	return 0;
}

int get_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "wep-open") == 0)
		*value = "WEPEncryption";
	else 
		*value = "None";
	return 0;
}

int set_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "WEPEncryption") == 0) {
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					int i = 0;
					wepkey64("Inteno", strk64);
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
			} else if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			return 0;
	}
	return 0;
}

int get_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0)
		*value = "SharedAuthentication";
	else
		*value = "None";
	return 0;
}

int set_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption, *option;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "SharedAuthentication") == 0) {
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					int i = 0;
					wepkey64("Inteno", strk64);
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
			} else if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			return 0;
	}
	return 0;
}

int get_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	*value = "";
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "psk+tkip") == 0 || strcmp(encryption, "mixed-psk+tkip") == 0)
		*value = "TKIPEncryption";
	else if (strcmp(encryption, "psk+ccmp") == 0 || strcmp(encryption, "mixed-psk+ccmp") == 0)
		*value = "AESEncryption";
	else if (strcmp(encryption, "psk+tkip+ccmp") == 0 || strcmp(encryption, "mixed-psk+tkip+ccmp") == 0)
		*value = "TKIPandAESEncryption";
	return 0;
}

int set_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "TKIPEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+tkip");
			}
			else if (strcmp(value, "AESEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+ccmp");
			}
			else if (strcmp(value, "TKIPandAESEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+tkip+ccmp");
			}
			return 0;
	}
	return 0;
}

int get_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk") == 0 || strncmp(encryption, "psk+", 4) == 0 || strncmp(encryption, "mixed-psk", 9) == 0)
		*value = "PSKAuthentication";
	else if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "EAPAuthentication";
	return 0;
}

int set_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "PSKAuthentication") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk");
			}
			else if (strcmp(value, "EAPAuthentication") == 0) {
				if(strcmp(encryption, "wpa") != 0 && strcmp(encryption, "wpa2") != 0 && strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa");
			}
			return 0;
	}
	return 0;
}

int get_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk2+tkip") == 0 || strcmp(encryption, "mixed-psk+tkip") == 0)
		*value = "TKIPEncryption";
	else if (strcmp(encryption, "psk2+ccmp") == 0 || strcmp(encryption, "mixed-psk+ccmp") == 0)
		*value = "AESEncryption";
	else if (strcmp(encryption, "psk2+tkip+ccmp") == 0 || strcmp(encryption, "mixed-psk+tkip+ccmp") == 0)
		*value = "TKIPandAESEncryption";
	return 0;
}

int set_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "TKIPEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+tkip");
			}
			else if (strcmp(value, "AESEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+ccmp");
			}
			else if (strcmp(value, "TKIPandAESEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+tkip+ccmp");
			}
			return 0;
	}
	return 0;
}

int get_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk2") == 0 || strncmp(encryption, "psk2+", 5) == 0 || strncmp(encryption, "mixed-psk", 9) == 0 )
		*value = "PSKAuthentication";
	else if (strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "EAPAuthentication";
	return 0;
}

int set_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "PSKAuthentication") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2");
			}
			else if (strcmp(value, "EAPAuthentication") == 0) {
				if (strcmp(encryption, "wpa") != 0 && strcmp(encryption, "wpa2") != 0 && strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa2");
			}
			return 0;
	}	
	return 0;
}

int get_wlan_radio_enabled(char *refparam, struct dmctx *ctx, char **value)
{
	char *tmp;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->device_section, "disabled", &tmp);	
	if (tmp[0] == '0' || tmp[0] == '\0')
		*value = "1";
	else if (tmp[0] == '1')
		*value = "0";
	return 0;
}

int set_wlan_radio_enabled(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	char *wunit, buf[8];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->device_section, "disabled", "0");
			else
				dmuci_set_value_by_section(wlanargs->device_section, "disabled", "1");			
			return 0;
	}
	return 0;
}

int get_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "mode", value);
	if (strcmp(*value, "ap") == 0)
		*value = "InfrastructureAccessPoint";
	else 
		*value = "";
	return 0;
}

int set_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "InfrastructureAccessPoint") == 0)
				dmuci_set_value_by_section(wlanargs->lwlansection, "mode", "ap");
			return 0;
	}
	return 0;
}

int get_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "RadiusClient";
	else 
		*value = "None";
	return 0;
}

int set_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			else if (strcmp(value, "RadiusClient") == 0) {
				if (strcmp(encryption, "wpa") != 0 || strcmp(encryption, "wpa2") != 0 || strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
			}
			return 0;
	}
	return 0;
}

int get_wlan_total_associations(char *refparam, struct dmctx *ctx, char **value)
{
	int i = 0;
	json_object *res;
	char *wunit, buf[8];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, val) {
		if (strstr(key, "sta-"))
			i++;
	}
	dmasprintf(value, "%d", i); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_wlan_devstatus_statistics_tx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","tx_bytes");
	return 0;
}

int get_wlan_devstatus_statistics_rx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","rx_bytes");
	return 0;
}

int get_wlan_devstatus_statistics_tx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","tx_packets");
	return 0;
}

int get_wlan_devstatus_statistics_rx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *val = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","rx_packets");
	return 0;
}

int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, char **value)
{
	char *hidden;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "hidden", &hidden);
	if (hidden[0] == '1' && hidden[1] == '\0')
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "hidden", "");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "hidden", "1");
			return 0;

	}
	return 0;
}

int get_wlan_wps_enable(char *refparam, struct dmctx *ctx, char **value)
{
	char *wps_pbc;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "wps_pbc", &wps_pbc);
	if (wps_pbc[0] == '1' && wps_pbc[1] == '\0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_wps_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "");
			return 0;
	}
	return 0;
}

int get_x_inteno_se_channelmode(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0')
		*value = "Auto";
	else
		*value = "Manual";
	return 0;
}

int set_x_inteno_se_channelmode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *channel;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Auto") == 0)
				dmuci_set_value_by_section(wlanargs->device_section, "channel", "auto");
			else if (strcmp(value, "Manual") == 0) {
				if (wlanargs->res != NULL) {
					channel = dmjson_get_value(wlanargs->res, 1, "channel");
					if (channel[0] != '\0')
						dmuci_set_value_by_section(wlanargs->device_section, "channel", channel);
				}
			}
			return 0;
	}
	return 0;
}

int get_x_inteno_se_supported_standard(char *refparam, struct dmctx *ctx, char **value)
{
	char *freq;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	DM_ASSERT(wlanargs->res, *value = "b, g, n, gst, lrs");
	freq = dmjson_get_value(wlanargs->res, 1, "frequency");
	if (strcmp(freq, "5") == 0)
		*value = "a, n, ac";
	else
		*value = "b, g, n, gst, lrs";
	return 0;
}

int get_x_inteno_se_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, char **value)
{
	char *bandwith;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	dmuci_get_value_by_section_string(wlanargs->device_section, "bandwidth", value);
	if (value[0] == '\0')
	{
		DM_ASSERT(wlanargs->res, *value ="");
		bandwith = dmjson_get_value(wlanargs->res, 1, "bandwidth");
		dmastrcat(value, bandwith, "MHz"); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_x_inteno_se_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *pch, *spch, *dup;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dup = dmstrdup(value);
			pch = strtok_r(dup, "Mm", &spch);
			dmuci_set_value_by_section(wlanargs->device_section, "bandwidth", pch);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_x_inteno_se_maxssid(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "bss_max", value);
	return 0;
}

int set_x_inteno_se_maxssid(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->lwlansection, "bss_max", value);
			return 0;
	}

	return 0;
}

int set_wlan_wep_key(char *refparam, struct dmctx *ctx, int action, char *value, char *key_index)
{
	char *encryption, *option;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
			}
			dmuci_set_value_by_section(wlanargs->lwlansection, key_index, value);
			return 0;
	}
	return 0;
}

int set_wlan_wep_key1(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char buf[8];
	sprintf(buf, "key%d", cur_wepargs.key_index);
	return set_wlan_wep_key(refparam, ctx, action, value, buf);
}
/****************************************************************************************/

int get_wlan_associated_macaddress(char *refparam, struct dmctx *ctx, char **value) {
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)ctx->args;
	
	*value = dmstrdup(clientwlargs->mac);
	return 0;
}

int get_wlan_associated_ipddress(char *refparam, struct dmctx *ctx, char **value)
{
	FILE *fp;
	char *ip, *mac, *line1;
	char delimiter[] = " \t";
	char line[MAX_PROC_ARP];
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)ctx->args;
	
	fp = fopen(ARP_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ARP, fp);
		while (fgets(line, MAX_PROC_ARP, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			line1 = dmstrdup(line); // MEM WILL BE FREED IN DMMEMCLEAN
			mac = cut_fx(line, delimiter, 4);
			if (strcasecmp(mac, clientwlargs->mac) == 0) {
				*value = cut_fx(line1, delimiter, 1);
				fclose(fp) ;
				return 0;
			}
		}
		fclose(fp);
	}
	*value = "";
	return 0;
}

int get_wlan_associated_authenticationstate(char *refparam, struct dmctx *ctx, char **value) {
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)ctx->args;
	char buf[256];
	int pp, r;
	*value = "0";

	pp = dmcmd("/usr/sbin/wlctl", 3, "-i", clientwlargs->wiface, "authe_sta_list"); //TODO wait ubus command
	if (pp) {
		r = dmcmd_read(pp, buf, 256);
		close(pp);
		if (r > 0 && strcasestr(buf, clientwlargs->mac))
			*value = "1";
		else
			*value = "0";
	}
	return 0;
}

int get_wlan_psk_assoc_MACAddress(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *wunit, *encryption, buf[8];
	char sta_pki[8];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strstr(encryption, "psk")) {
		sprintf(sta_pki, "sta-%d", wlanargs->pki);
		dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", wlanargs->wiface, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		*value = dmjson_get_value(res, 2, sta_pki, "macaddr");
		return 0;
	}
	*value = "";
	return 0;
}

int get_x_inteno_se_scantimer(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	*value = "0";
	dmuci_get_value_by_section_string(wlanargs->device_section, "scantimer", value);
	return 0;
}

int set_x_inteno_se_scantimer(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "scantimer", value);
			return 0;
	}
	return 0;
}

int get_wmm_enabled(char *refparam, struct dmctx *ctx, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	bool b;

	dmuci_get_value_by_section_string(wlanargs->device_section, "wmm", value);
	string_to_bool(*value, &b);
		if (b)
			*value = "1";
		else
			*value = "0";

	return 0;
}

int get_x_inteno_se_frequency(char *refparam, struct dmctx *ctx, char **value)
{
	char *freq;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	DM_ASSERT(wlanargs->res, *value = "");
	freq = dmjson_get_value(wlanargs->res, 1, "frequency");
	dmastrcat(value, freq, "GHz");
	return 0;
}

int set_x_inteno_se_frequency(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *freq = NULL;
	json_object *res = NULL;
	struct uci_section *s = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(wlanargs->res)
				freq = dm_ubus_get_value(wlanargs->res, 1, "frequency");
			if (freq && strcmp(freq, value) == 0)
			{
				return 0;
			}
			else if (value[0] == '5' || value[0] == '2')
			{
				uci_foreach_sections("wireless", "wifi-device", s) {
					dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", section_name(s), String}}, 1, &res);
					if(res)
					{
						freq = dmjson_get_value(res, 1, "frequency");
						if (strcmp(freq, value) == 0)
						{
							dmuci_set_value_by_section(wlanargs->lwlansection, "device", section_name(s));
							return 0;
						}
					}
				}
			}
			else
				return 0;
	}
	return 0;
}
int set_wmm_enabled(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				dmuci_set_value_by_section(wlanargs->device_section, "wmm", "1");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_noack", "1");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_apsd", "1");
			}
			else {
				dmuci_set_value_by_section(wlanargs->device_section, "wmm", "0");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_noack", "");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_apsd", "");
			}
			return 0;
	}
	return 0;
}

void lan_eth_update_section_option_list (char *name, char *sec_name, char *wan_eth)
{
	char *pch, *spch, *ifname;

	if (name[0] == '\0') {
		update_section_option_list(DMMAP, "lan_eth", "ifname", "network", "", sec_name, name);
	}
	ifname = dmstrdup(name);
	for (pch = strtok_r(ifname, " ,", &spch);
		pch != NULL;
		pch = strtok_r(NULL, " ,", &spch)) {
		if (strncmp(pch, "eth", 3) != 0 || strncmp(pch, wan_eth, 4) == 0)
			continue;
		update_section_option_list(DMMAP, "lan_eth", "ifname", "network", pch, sec_name, name);
	}
	dmfree(ifname);
}
/////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_lan_dev_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_lanargs.ldlansection, "ldalias", value);
	return 0;
}

int set_lan_dev_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_lanargs.ldlansection, "ldalias", value);
			return 0;
	}
	return 0;
}

int get_lan_ip_int_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_ipargs.ldipsection, "lipalias", value);
	return 0;
}

int set_lan_ip_int_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_ipargs.ldipsection, "lipalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcpargs.lddhcpsection, "ldhcpalias", value);
	return 0;
}

int set_dhcp_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dhcpargs.lddhcpsection, "ldhcpalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_wlanargs.lwlansection, "lwlanalias", value);
	return 0;
}

int set_wlan_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_wlanargs.lwlansection, "lwlanalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_psk_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_pskargs.wlanpsk, "pskalias", value);
	return 0;
}

int set_wlan_psk_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_pskargs.wlanpsk, "pskalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_wep_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_wepargs.wlanwep, "wepalias", value);
	return 0;
}

int set_wlan_wep_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_wepargs.wlanwep, "wepalias", value);
			return 0;
	}
	return 0;
}

int get_lan_eth_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_ethargs.lan_ethsection, "ethalias", value);
	return 0;
}

int set_lan_eth_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_ethargs.lan_ethsection, "ethalias", value);
			return 0;
	}
	return 0;
}
int get_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcppoolargs.dhcppoolsection, "poulalias", value);
	return 0;
}

int set_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dhcppoolargs.dhcppoolsection, "poulalias", value);
			return 0;
	}
	return 0;
}


int get_dhcp_conditional_servingpool_enable(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcppoolargs.dhcppoolsection, "enable", value);
	return 0;
}

int set_dhcp_conditionalservingpool_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				dmuci_set_value_by_section(cur_dhcppoolargs.dhcppoolsection, "enable", "");
			}
			else {
				dmuci_set_value_by_section(cur_dhcppoolargs.dhcppoolsection, "enable", "0");
			}
			return 0;
	}
	return 0;
}

int get_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcppoolargs.dhcppoolsection, "vendorclass", value);
	return 0;
}

int set_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dhcppoolargs.dhcppoolsection, "vendorclass", value);
			return 0;
	}
	return 0;
}

int get_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dhcppoolargs.dhcppoolsection, "networkid", value);
	return 0;
}

int set_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dhcppoolargs.dhcppoolsection, "networkid", value);
			return 0;
	}
	return 0;
}

int get_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "optionalias", value);
	return 0;
}

int set_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "optionalias", value);
			return 0;
	}
	return 0;
}


int get_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", value);
	return 0;
}

int set_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *tmp = NULL;
	char *option_tmp = NULL;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", &tmp);
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &option_tmp);
			update_uci_dhcpserver_option(ctx, pooloptionargs->dhcppoolsection, option_tmp, value, tmp);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "dhcp_option", value);
			return 0;
	}
	return 0;
}

int get_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", value);
	return 0;
}

int set_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, int action, char *value)
{

	char *tmp;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &tmp);
			set_uci_dhcpserver_option(ctx, pooloptionargs->dhcppoolsection, tmp, value);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "value", value);

			return 0;
	}
	return 0;
}
/////////////SUB ENTRIES///////////////
inline int entry_landevice_sub(struct dmctx *ctx)
{
	struct uci_section *s = NULL;
	char *idev = NULL, *idev_last = NULL;

	uci_foreach_filter_func("network", "interface", NULL, &filter_lan_device_interface, s) {
		idev = handle_update_instance(1, ctx, &idev_last, update_instance_alias, 3, s, "ldinstance", "ldalias");
		init_ldargs_lan(ctx, s, idev);
		SUBENTRY(entry_landevice_sub_instance, ctx, s, idev);
	}
	return 0;
}

inline int entry_landevice_ipinterface_and_dhcpstaticaddress(struct dmctx *ctx, struct uci_section *landevice_section, char *idev)
{
	struct uci_section *ss = NULL;
	struct uci_section *sss = NULL;
	char *ilan = NULL, *ilan_last = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;

	uci_foreach_filter_func("network", "interface", landevice_section, filter_lan_ip_interface, ss) {
		ilan = handle_update_instance(2, ctx, &ilan_last, update_instance_alias, 3, ss, "lipinstance", "lipalias");
		init_ldargs_ip(ctx, ss);
		SUBENTRY(entry_landevice_ipinterface_instance, ctx, idev, ilan);
		uci_foreach_option_cont("dhcp", "host", "interface", section_name(ss), sss) {
			idhcp = handle_update_instance(2, ctx, &idhcp_last, update_instance_alias, 3, sss, "ldhcpinstance", "ldhcpalias");
			init_ldargs_dhcp(ctx, sss);
			SUBENTRY(entry_landevice_dhcpstaticaddress_instance, ctx, idev, idhcp);
		}
	}
	return 0;
}

inline int entry_landevice_wlanconfiguration(struct dmctx *ctx, struct uci_section *landevice_section, char *idev)
{
	struct uci_section *ss = NULL;
	struct uci_section *sss = NULL;
	json_object *res;
	char *iwlan = NULL, *iwlan_last = NULL;
	char *network , *wiface, buf[8];

	iwlan = get_last_instance_lev2("wireless", "wifi-iface", "lwlaninstance", "network", section_name(landevice_section));
	uci_foreach_sections("wireless", "wifi-device", ss) {
		int wlctl_num=0;		
		uci_foreach_option_eq("wireless", "wifi-iface", "device", section_name(ss), sss) {
			dmuci_get_value_by_section_string(sss, "network", &network);
			if (strcmp(network, section_name(landevice_section)) != 0)
				continue;
			iwlan = handle_update_instance(2, ctx, &iwlan_last, update_instance_alias, 3, sss, "lwlaninstance", "lwlanalias");
			wiface = section_name(ss);
			if (wlctl_num != 0) {
				sprintf(buf, "%s.%d", wiface, wlctl_num);
				wiface = buf;
			}
			dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wiface, String}}, 1, &res);
			init_ldargs_wlan(ctx, sss, wlctl_num, ss, section_name(ss), wiface, res, 0);
			wlctl_num++;
			SUBENTRY(entry_landevice_wlanconfiguration_instance, ctx, idev, iwlan);
		}
	}
	return 0;
}

inline int entry_landevice_wlanconfiguration_wepkey(struct dmctx *ctx, char *idev, char *iwlan)
{
	int i = 0;
	char *iwep = NULL, *iwep_last = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	struct uci_section *s = NULL;

	update_section_list(DMMAP,"wlan-wepkey", "wlan", 4, section_name(wlanargs->lwlansection), NULL, NULL, NULL, NULL);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "wlan-wepkey", "wlan", section_name(wlanargs->lwlansection), s) {
		//init_wlan_wep_args(ctx, s);
		cur_wepargs.wlanwep = s;
		cur_wepargs.key_index = ++i;
		iwep =  handle_update_instance(3, ctx, &iwep_last, update_instance_alias_icwmpd, 3, s, "wepinstance", "wepalias");
		SUBENTRY(entry_landevice_wlanconfiguration_wepkey_instance, ctx, idev, iwlan, iwep);
	}
	return 0;
}

inline int entry_landevice_wlanconfiguration_presharedkey(struct dmctx *ctx, char *idev, char *iwlan)
{
	char *ipk = NULL, *ipk_last = NULL ;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;
	struct uci_section *s = NULL;

	wlanargs->pki = 0;
	//update section list of wlan-psk before update instance
	update_section_list(DMMAP,"wlan-psk", "wlan", 10, section_name(wlanargs->lwlansection), NULL, NULL, NULL, NULL);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "wlan-psk", "wlan", section_name(wlanargs->lwlansection), s) {
		wlanargs->pki++;
		//init_wlan_psk_args(ctx, s);
		cur_pskargs.wlanpsk = s;
		ipk =  handle_update_instance(3, ctx, &ipk_last, update_instance_alias_icwmpd, 3, s, "pskinstance", "pskalias");
		SUBENTRY(entry_landevice_wlanconfiguration_presharedkey_instance, ctx, idev, iwlan, ipk); //"$wunit" "$wlctl_num" "$uci_num" are not needed
	}
	return 0;
}

inline int entry_landevice_wlanconfiguration_associateddevice(struct dmctx *ctx, char *idev, char *iwlan)
{
	int id = 0;
	json_object *res, *wl_client_obj;
	char *idx, *idx_last = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)ctx->args;

	dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", wlanargs->wiface, String}}, 1, &res);
	if (res) {
		char *value;
		json_object_object_foreach(res, key, wl_client_obj) {
			idx = handle_update_instance(3, ctx, &idx_last, update_instance_without_section, 1, ++id);
			value = dmjson_get_value(wl_client_obj, 1, "macaddr");
			init_wl_client_args(ctx, value, wlanargs->wiface);
			SUBENTRY(entry_landevice_wlanconfiguration_associateddevice_instance, ctx, idev, iwlan, idx);
		}
	}
	return 0;
}

inline int entry_landevice_lanethernetinterfaceconfig(struct dmctx *ctx, struct uci_section *landevice_section, char *idev)
{
	int i = 0;
	char *pch, *spch;
	char *ifname, *wan_eth, *baseifname;
	char *ieth = NULL, *ieth_last = NULL;
	struct uci_section *s = NULL;
#ifdef EX400
	uci_foreach_option_eq("ports", "ethport", "name", "WAN", s) {
		dmuci_get_value_by_section_string(s, "ifname", &wan_eth);
	}
#else
	dmuci_get_option_value_string("layer2_interface_ethernet", "ethernet_interface", "baseifname", &wan_eth);
#endif
	dmuci_get_value_by_section_string(landevice_section, "ifname", &ifname);
	lan_eth_update_section_option_list(ifname, section_name(landevice_section), wan_eth);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "lan_eth", "network", section_name(landevice_section), s) {
		dmuci_get_value_by_section_string(s, "ifname", &baseifname);
		init_ldargs_eth_cfg(ctx, baseifname, s);
		ieth =  handle_update_instance(2, ctx, &ieth_last, update_instance_alias_icwmpd, 3, s, "ethinstance", "ethalias");
		SUBENTRY(entry_landevice_lanethernetinterfaceconfig_instance, ctx, idev, ieth);
	}
	return 0;
}

inline int entry_landevice_host(struct dmctx *ctx, struct uci_section *landevice_section, char *idev)
{
	//HOST DYNAMIC
	json_object *res, *client_obj;
	char *network;
	char *idx, *idx_last = NULL;
	int id = 0;
	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			network = dmjson_get_value(client_obj, 1, "network");
			if (strcmp(network, section_name(landevice_section)) == 0) {
				init_client_args(ctx, client_obj, section_name(landevice_section));
				idx = handle_update_instance(2, ctx, &idx_last, update_instance_without_section, 1, ++id);
				SUBENTRY(entry_landevice_host_instance, ctx, idev, idx);
			}
		}
	}
	return 0;
}

inline int entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool(struct dmctx *ctx, char *idev)
{
	struct uci_section *s = NULL;
	char *icondpool = NULL, *icondpool_last = NULL;
	uci_foreach_sections("dhcp", "vendorclass", s) {
		init_args_dhcp_conditional_servingpool_entry(ctx, s);
		icondpool =  handle_update_instance(1, ctx, &icondpool_last, update_instance_alias, 3, s, "poulinstance", "poulalias");
		SUBENTRY(entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance, ctx, idev, icondpool);
	}
	return 0;
}

inline int entry_landevice_dhcpconditionalservingpool_option(struct dmctx *ctx, char *idev, char *icondpool)
{
	int id = 0;
	char *idx = NULL, *pch, *spch, *name, *value;
	char *idx_last = NULL;
	struct uci_list *val;
	struct uci_element *e = NULL, *tmp;
	struct uci_section *ss = NULL;
	struct dhcppoolargs *poolargs = (struct dhcppoolargs *)ctx->args;
	bool find_max = true;
	char *tt;

	int found = 0;
	dmuci_get_value_by_section_list(poolargs->dhcppoolsection, "dhcp_option", &val);
	if (val) {
		uci_foreach_element_safe(val, e, tmp)
		{
			tt = dmstrdup(tmp->name);
			pch = strtok_r(tt, ",", &spch);
			found = 0;
			uci_path_foreach_option_eq(icwmpd, "dmmap", section_name(poolargs->dhcppoolsection), "dhcp_option", pch, ss)
			{
				dmuci_get_value_by_section_string(ss, "value", &value);
				if (strcmp(spch, value) == 0)
				{
					dmuci_get_value_by_section_string(ss, "optioninst", &idx);
					found = 1;
				}
				else
					continue;
				init_args_pool_option(ctx, ss, poolargs->dhcppoolsection);
				SUBENTRY(entry_landevice_dhcpconditionalservingpool_option_instance, ctx, idev, icondpool, idx);
				dmfree(tt);
				break;
			}
			if (!found)
			{
				DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name(poolargs->dhcppoolsection), &ss, &name);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "dhcp_option", pch);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "value", spch);
				init_args_pool_option(ctx, ss, poolargs->dhcppoolsection);
				idx =  handle_update_instance(1, ctx, &idx_last, dhcp_option_update_instance_alias_icwmpd, 4, ss, "optioninst", "optionalias", &find_max);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "optioninst", idx);
				SUBENTRY(entry_landevice_dhcpconditionalservingpool_option_instance, ctx, idev, icondpool, idx);
				dmfree(tt);
			}
		}
	}
}
///////////////////////////////////////
/*************************************/
int entry_method_root_LANDevice(struct dmctx *ctx)
{
	//struct ldlanargs *(ctx->args) = (struct ldlanargs *)(ctx->args); //TO CHECK
	IF_MATCH(ctx, DMROOT"LANDevice.") {
		DMOBJECT(DMROOT"LANDevice.", ctx, "0", 0, NULL, NULL, NULL);
		SUBENTRY(entry_landevice_sub, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_sub_instance(struct dmctx *ctx, struct uci_section *landevice_section, char *idev)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.", idev) {
		DMOBJECT(DMROOT"LANDevice.%s.", ctx, "0", 0, NULL, NULL, NULL, idev);
		DMPARAM("Alias", ctx, "1", get_lan_dev_alias, set_lan_dev_alias, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.", ctx, "0", 1, NULL, NULL, NULL, idev);
		DMPARAM("DNSServers", ctx, "1", get_lan_dns, set_lan_dns, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DHCPServerConfigurable", ctx, "1", get_lan_dhcp_server_configurable, set_lan_dhcp_server_configurable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("DHCPServerEnable", ctx, "1", get_lan_dhcp_server_enable, set_lan_dhcp_server_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("MinAddress", ctx, "1", get_lan_dhcp_interval_address_start, set_lan_dhcp_address_start, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxAddress", ctx, "1", get_lan_dhcp_interval_address_end, set_lan_dhcp_address_end, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("ReservedAddresses", ctx, "1", get_lan_dhcp_reserved_addresses, set_lan_dhcp_reserved_addresses, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("SubnetMask", ctx, "1", get_lan_dhcp_subnetmask, set_lan_dhcp_subnetmask, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IPRouters", ctx, "1", get_lan_dhcp_iprouters, set_lan_dhcp_iprouters, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DHCPLeaseTime", ctx, "1", get_lan_dhcp_leasetime, set_lan_dhcp_leasetime, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DomainName", ctx, "1", get_lan_dhcp_domainname, set_lan_dhcp_domainname, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.Hosts.", ctx, "0", 0, NULL, NULL, NULL, idev);
		DMPARAM("HostNumberOfEntries", ctx, "0", get_lan_host_nbr_entries, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.Hosts.Host.", ctx, "0", 0, NULL, NULL, NULL, idev);
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.IPInterface.", ctx, "0", 1, NULL, NULL, NULL, idev);
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.", ctx, "1", 1, add_dhcp_conditional_serving_pool, delete_dhcp_conditional_serving_pool_all, NULL, idev);
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPStaticAddress.", ctx, "1", 1, add_landevice_dhcpstaticaddress, delete_landevice_dhcpstaticaddress_all, NULL, idev);
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.", ctx, "1", 0, add_landevice_wlanconfiguration, delete_landevice_wlanconfiguration_all, NULL, idev);
		DMOBJECT(DMROOT"LANDevice.%s.LANEthernetInterfaceConfig.", ctx, "0", 1, NULL, NULL, NULL, idev);/* TO CHECK */
		SUBENTRY(entry_landevice_ipinterface_and_dhcpstaticaddress, ctx, landevice_section, idev);
		SUBENTRY(entry_landevice_wlanconfiguration, ctx, landevice_section, idev);
		SUBENTRY(entry_landevice_lanethernetinterfaceconfig, ctx, landevice_section, idev);
		SUBENTRY(entry_landevice_host, ctx, landevice_section, idev);
		SUBENTRY(entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool, ctx, idev);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance(struct dmctx *ctx, char * idev, char *icondpool)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.%s.", idev, icondpool) {
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.%s.", ctx, "1", 1, NULL, delete_dhcp_conditional_serving_pool, NULL, idev, icondpool);
		DMPARAM("Alias", ctx, "1", get_dhcp_conditional_servingpool_alias, set_dhcp_conditional_servingpool_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_dhcp_conditional_servingpool_enable, set_dhcp_conditionalservingpool_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("VendorClassID", ctx, "1", get_dhcp_conditional_servingpool_vendorclassid, set_dhcp_conditional_servingpool_vendorclassid, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_COM_Networkid", ctx, "1", get_dhcp_conditional_servingpool_network_id, set_dhcp_conditional_servingpool_network_id, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.%s.DHCPOption.", ctx, "1", 0, add_dhcp_serving_pool_option, delete_dhcp_serving_pool_option_all, NULL, idev, icondpool);
		SUBENTRY(entry_landevice_dhcpconditionalservingpool_option, ctx, idev, icondpool);

		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_dhcpconditionalservingpool_option_instance(struct dmctx *ctx, char * idev, char *icondpool, char *idx)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.%s.DHCPOption.%s.", idev, icondpool, idx) {
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPConditionalServingPool.%s.DHCPOption.%s.", ctx, "1", 1, NULL, delete_dhcp_serving_pool_option, NULL, idev, icondpool, idx);
		DMPARAM("Alias", ctx, "1", get_dhcp_servingpool_alias, set_dhcp_servingpool_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Tag", ctx, "1", get_dhcp_servingpool_tag, set_dhcp_servingpool_tag, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("Value", ctx, "1", get_dhcp_servingpool_value, set_dhcp_servingpool_value, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_ipinterface_instance (struct dmctx *ctx, char *idev, char *ilan) //TODO CAN WE USE TYPE VOID
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.LANHostConfigManagement.IPInterface.%s.", idev, ilan) {
		struct ldipargs *ipargs = (struct ldipargs *)(ctx->args);
		char *linker;
		dmastrcat(&linker, "linker_interface:", section_name(ipargs->ldipsection));
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.IPInterface.%s.", ctx, "0", 1, NULL, NULL, linker, idev, ilan);
		DMPARAM("Alias", ctx, "1", get_lan_ip_int_alias, set_lan_ip_int_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_interface_enable_ipinterface, set_interface_enable_ipinterface, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("X_BROADCOM_COM_FirewallEnabled", ctx, "1", get_interface_firewall_enabled_ipinterface, set_interface_firewall_enabled_ipinterface, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("IPInterfaceIPAddress", ctx, "1", get_interface_ipaddress, set_interface_ipaddress, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IPInterfaceSubnetMask", ctx, "1", get_interface_subnetmask, set_interface_subnetmask, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IPInterfaceAddressingType", ctx, "1", get_interface_addressingtype, set_interface_addressingtype, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_dhcpstaticaddress_instance(struct dmctx *ctx, char *idev, char *idhcp) //TODO CAN WE USE TYPE VOID
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPStaticAddress.%s.", idev, idhcp) {
		DMOBJECT(DMROOT"LANDevice.%s.LANHostConfigManagement.DHCPStaticAddress.%s.", ctx, "1", 1, NULL, delete_landevice_dhcpstaticaddress, NULL, idev, idhcp);
		DMPARAM("Alias", ctx, "1", get_dhcp_alias, set_dhcp_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_dhcpstaticaddress_enable, set_dhcpstaticaddress_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Chaddr", ctx, "1", get_dhcpstaticaddress_chaddr, set_dhcpstaticaddress_chaddr, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Yiaddr", ctx, "1", get_dhcpstaticaddress_yiaddr, set_dhcpstaticaddress_yiaddr, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_wlanconfiguration_instance(struct dmctx *ctx, char *idev,char *iwlan)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.WLANConfiguration.%s.", idev, iwlan) {
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.", ctx, "1", 0, NULL, delete_landevice_wlanconfiguration, NULL, idev, iwlan);
		DMPARAM("Alias", ctx, "1", get_wlan_alias, set_wlan_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_wlan_enable, set_wlan_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Status", ctx, "0", get_wlan_status, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("BSSID", ctx, "0", get_wlan_bssid, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxBitRate", ctx, "1", get_wlan_max_bit_rate, set_wlan_max_bit_rate, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Channel", ctx, "1", get_wlan_channel, set_wlan_channel, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("AutoChannelEnable", ctx, "1", get_wlan_auto_channel_enable, set_wlan_auto_channel_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("SSID", ctx, "1", get_wlan_ssid, set_wlan_ssid, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("BeaconType", ctx, "1", get_wlan_beacon_type, set_wlan_beacon_type, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MACAddressControlEnabled", ctx, "1", get_wlan_mac_control_enable, set_wlan_mac_control_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("PossibleChannels", ctx, "0", get_wlan_possible_channels, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Standard", ctx, "1", get_wlan_standard, set_wlan_standard, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("WEPKeyIndex", ctx, "1", get_wlan_wep_key_index, set_wlan_wep_key_index, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("KeyPassphrase", ctx, "1", get_empty, set_wlan_key_passphrase, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("WEPEncryptionLevel", ctx, "0", get_wlan_wep_encryption_level, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("BasicEncryptionModes", ctx, "1", get_wlan_basic_encryption_modes, set_wlan_basic_encryption_modes, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("BasicAuthenticationMode", ctx, "1", get_wlan_basic_authentication_mode, set_wlan_basic_authentication_mode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("WPAEncryptionModes", ctx, "1", get_wlan_wpa_encryption_modes, set_wlan_wpa_encryption_modes, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("WPAAuthenticationMode", ctx, "1", get_wlan_wpa_authentication_mode, set_wlan_wpa_authentication_mode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IEEE11iEncryptionModes", ctx, "1", get_wlan_ieee_11i_encryption_modes, set_wlan_ieee_11i_encryption_modes, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("IEEE11iAuthenticationMode", ctx, "1", get_wlan_ieee_11i_authentication_mode, set_wlan_ieee_11i_authentication_mode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("RadioEnabled", ctx, "1", get_wlan_radio_enabled, set_wlan_radio_enabled, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("DeviceOperationMode", ctx, "1", get_wlan_device_operation_mode, set_wlan_device_operation_mode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("AuthenticationServiceMode", ctx, "1", get_wlan_authentication_service_mode, set_wlan_authentication_service_mode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("TotalAssociations", ctx, "0", get_wlan_total_associations, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("ChannelsInUse", ctx, "1", get_wlan_channel, set_wlan_channel, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("TotalBytesSent", ctx, "0", get_wlan_devstatus_statistics_tx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("TotalBytesReceived", ctx, "0", get_wlan_devstatus_statistics_rx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("TotalPacketsSent", ctx, "0", get_wlan_devstatus_statistics_tx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("TotalPacketsReceived", ctx, "0", get_wlan_devstatus_statistics_rx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("SSIDAdvertisementEnabled", ctx, "1", get_wlan_ssid_advertisement_enable, set_wlan_ssid_advertisement_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("WMMEnable", ctx, "1", get_wmm_enabled, set_wmm_enabled, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_ChannelMode", ctx, "1", get_x_inteno_se_channelmode, set_x_inteno_se_channelmode, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_SupportedStandards", ctx, "0", get_x_inteno_se_supported_standard, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_OperatingChannelBandwidth", ctx, "1", get_x_inteno_se_operating_channel_bandwidth, set_x_inteno_se_operating_channel_bandwidth, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_MaxSSID", ctx, "1", get_x_inteno_se_maxssid, set_x_inteno_se_maxssid, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_ScanTimer", ctx, "1", get_x_inteno_se_scantimer, set_x_inteno_se_scantimer, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_Frequency", ctx, "1", get_x_inteno_se_frequency, set_x_inteno_se_frequency, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.WPS.", ctx, "0", 1, NULL, NULL, NULL, idev, iwlan); //Check if we can move it
		DMPARAM("Enable", ctx, "1", get_wlan_wps_enable, set_wlan_wps_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.WEPKey.", ctx, "0", 1, NULL, NULL, NULL, idev, iwlan);
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.PreSharedKey.", ctx, "0", 1, NULL, NULL, NULL, idev, iwlan);
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.AssociatedDevice.", ctx, "0", 0, NULL, NULL, NULL, idev, iwlan);
		SUBENTRY(entry_landevice_wlanconfiguration_wepkey, ctx, idev, iwlan);
		SUBENTRY(entry_landevice_wlanconfiguration_presharedkey, ctx, idev, iwlan);
		SUBENTRY(entry_landevice_wlanconfiguration_associateddevice, ctx, idev, iwlan);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_wlanconfiguration_presharedkey_instance(struct dmctx *ctx, char *idev, char *iwlan, char *ipk)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.WLANConfiguration.%s.PreSharedKey.%s.", idev, iwlan, ipk) {
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.PreSharedKey.%s.", ctx, "0", 1, NULL, NULL, NULL, idev, iwlan, ipk);
		DMPARAM("Alias", ctx, "1", get_wlan_psk_alias, set_wlan_psk_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("PreSharedKey", ctx, "1", get_empty, set_wlan_pre_shared_key, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("KeyPassphrase", ctx, "1", get_empty, set_wlan_key_passphrase, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("AssociatedDeviceMACAddress", ctx, "0", get_wlan_psk_assoc_MACAddress, NULL, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_wlanconfiguration_associateddevice_instance(struct dmctx *ctx, char *idev, char *iwlan, char *idx)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.WLANConfiguration.%s.AssociatedDevice.%s.", idev, iwlan, idx) {
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.AssociatedDevice.%s.", ctx, "0", 0, NULL, NULL, NULL, idev, iwlan, idx);
		DMPARAM("AssociatedDeviceMACAddress", ctx, "0", get_wlan_associated_macaddress, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("AssociatedDeviceIPAddress", ctx, "0", get_wlan_associated_ipddress, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("AssociatedDeviceAuthenticationState", ctx, "0", get_wlan_associated_authenticationstate, NULL, "xsd:boolean", 0, 0, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_lanethernetinterfaceconfig_instance(struct dmctx *ctx, char *idev, char *ieth)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.LANEthernetInterfaceConfig.%s.", idev, ieth) {
		DMOBJECT(DMROOT"LANDevice.%s.LANEthernetInterfaceConfig.%s.", ctx, "0", 1, NULL, NULL, NULL, idev, ieth);
		DMPARAM("Alias", ctx, "1", get_lan_eth_alias, set_lan_eth_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_lan_eth_iface_cfg_enable, set_lan_eth_iface_cfg_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Status", ctx, "0", get_lan_eth_iface_cfg_status, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxBitRate", ctx, "1", get_lan_eth_iface_cfg_maxbitrate, set_lan_eth_iface_cfg_maxbitrate, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DuplexMode", ctx, "1", get_lan_eth_iface_cfg_duplexmode, set_lan_eth_iface_cfg_duplexmode, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"LANDevice.%s.LANEthernetInterfaceConfig.%s.Stats.", ctx, "0", 1, NULL, NULL, NULL, idev, ieth);
		DMPARAM("BytesSent", ctx, "0", get_lan_eth_iface_cfg_stats_tx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("BytesReceived", ctx, "0", get_lan_eth_iface_cfg_stats_rx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("PacketsSent", ctx, "0", get_lan_eth_iface_cfg_stats_tx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("PacketsReceived", ctx, "0", get_lan_eth_iface_cfg_stats_rx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_host_instance(struct dmctx *ctx, char *idev, char *idx)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.Hosts.Host.%s.", idev, idx) {
		DMOBJECT(DMROOT"LANDevice.%s.Hosts.Host.%s.", ctx, "0", 0, NULL, NULL, NULL, idev, idx);
		DMPARAM("IPAddress", ctx, "0", get_lan_host_ipaddress, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("HostName", ctx, "0", get_lan_host_hostname, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("Active", ctx, "0", get_lan_host_active, NULL, "xsd:boolean", 0, 0, UNDEF, NULL);
		DMPARAM("MACAddress", ctx, "0", get_lan_host_macaddress, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("InterfaceType", ctx, "0", get_lan_host_interfacetype, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("AddressSource", ctx, "0", get_lan_host_addresssource, NULL, NULL, 0, 0, UNDEF, NULL);
		DMPARAM("LeaseTimeRemaining", ctx, "0", get_lan_host_leasetimeremaining, NULL, NULL, 0, 0, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_landevice_wlanconfiguration_wepkey_instance(struct dmctx *ctx, char *idev, char *iwlan, char *iwep)
{
	IF_MATCH(ctx, DMROOT"LANDevice.%s.WLANConfiguration.%s.WEPKey.%s.", idev, iwlan, iwep) {
		DMOBJECT(DMROOT"LANDevice.%s.WLANConfiguration.%s.WEPKey.%s.", ctx, "0", 1, NULL, NULL, NULL, idev, iwlan, iwep);
		DMPARAM("Alias", ctx, "1", get_wlan_wep_alias, set_wlan_wep_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("WEPKey", ctx, "1", get_empty, set_wlan_wep_key1, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}
