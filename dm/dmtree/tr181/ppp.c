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
#include "dmjson.h"
#include "dmcommon.h"
#include "dmentry.h"
#include "ppp.h"

/*** PPP. ***/
DMLEAF tPPPParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"InterfaceNumberOfEntries", &DMREAD, DMT_UNINT, get_PPP_InterfaceNumberOfEntries, NULL, NULL, NULL},
{0}
};

DMOBJ tpppObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Interface", &DMWRITE, add_ppp_interface, delete_ppp_interface, NULL, browseInterfaceInst, NULL, NULL, tpppInterfaceObj, tpppInterfaceParam, get_linker_ppp_interface},
{0}
};

/*** PPP.Interface. ***/
DMOBJ tpppInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"PPPoE", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tPPPInterfacePPPoEParams, NULL},
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tStatsParam, NULL},
{0}
};

DMLEAF tpppInterfaceParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_ppp_alias, set_ppp_alias, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_PPPInterface_Status, NULL, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_ppp_enable, set_ppp_enable, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_PPPInterface_LastChange, NULL, NULL, NULL},
{"Reset", &DMWRITE, DMT_BOOL, get_PPPInterface_Reset, set_PPPInterface_Reset, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_ppp_name, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_ppp_lower_layer, set_ppp_lower_layer, NULL, NULL},
{"ConnectionStatus", &DMREAD, DMT_STRING, get_ppp_status, NULL, NULL, NULL},
{"Username", &DMWRITE, DMT_STRING, get_ppp_username, set_ppp_username, NULL, NULL},
{"Password", &DMWRITE, DMT_STRING, get_empty, set_ppp_password, NULL, NULL},
{0}
};

/*** PPP.Interface.Stats. ***/
DMLEAF tStatsParam[] = {
{"BytesReceived", &DMREAD, DMT_UNINT, get_ppp_eth_bytes_received, NULL, NULL, NULL},
{"BytesSent", &DMREAD, DMT_UNINT, get_ppp_eth_bytes_sent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_ppp_eth_pack_received, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_ppp_eth_pack_sent, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_ErrorsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_PPPInterfaceStats_UnknownProtoPacketsReceived, NULL, NULL, NULL},
{0}
};

DMLEAF tPPPInterfacePPPoEParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"SessionID", &DMREAD, DMT_UNINT, get_PPPInterfacePPPoE_SessionID, NULL, NULL, NULL},
{"ACName", &DMWRITE, DMT_STRING, get_PPPInterfacePPPoE_ACName, set_PPPInterfacePPPoE_ACName, NULL, NULL},
{"ServiceName", &DMWRITE, DMT_STRING, get_PPPInterfacePPPoE_ServiceName, set_PPPInterfacePPPoE_ServiceName, NULL, NULL},
{0}
};
/*************************************************************
 * GET SET ALIAS
/*************************************************************/
int get_ppp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name((struct uci_section *)data), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ppp_int_alias", value);
	return 0;
}

int set_ppp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name((struct uci_section *)data), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "ppp_int_alias", value);
			return 0;
	}
	return 0;
}

/**************************************************************************
* GET & SET PARAMETERS
***************************************************************************/
/*#Device.PPP.Interface.{i}.Enable!UBUS:network.interface/status/interface,@Name/up*/
int get_ppp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_interface_enable_ubus(section_name(((struct uci_section *)data)), refparam, ctx, value);
}

int set_ppp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_interface_enable_ubus(section_name(((struct uci_section *)data)), refparam, ctx, action, value);
}


int get_PPPInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status;
	get_interface_enable_ubus(section_name(((struct uci_section *)data)), refparam, ctx, &status);
	if(strcmp(status, "true") == 0)
		*value= "Up";
	else
		*value= "Down";
	return 0;
}


int get_PPPInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name((struct uci_section *)data), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "uptime");
	return 0;
}

int get_PPPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "false";
	return 0;
}

int set_PPPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				set_interface_enable_ubus(section_name((struct uci_section *)data), refparam, ctx, action, "0");
				set_interface_enable_ubus(section_name((struct uci_section *)data), refparam, ctx, action, "1");
			}
			break;
	}
	return 0;
}

int get_ppp_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct uci_section *)data)));
	return 0;
}

/*#Device.PPP.Interface.{i}.Status!UBUS:network.interface/status/interface,@Name/up*/
int get_ppp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status = NULL;
	char *uptime = NULL;
	char *pending = NULL;
	json_object *res = NULL, *jobj = NULL;
	bool bstatus = false, bpend = false;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct uci_section *)data)), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	jobj = dmjson_get_obj(res, 1, "up");
	if(jobj)
	{
		status = dmjson_get_value(res, 1, "up");
		string_to_bool(status, &bstatus);
		if (bstatus) {
			uptime = dmjson_get_value(res, 1, "uptime");
			pending = dmjson_get_value(res, 1, "pending");			
			string_to_bool(pending, &bpend);
		}
	}
	if (uptime && atoi(uptime) > 0)
		*value = "Connected";
	else if (pending && bpend)
		*value = "Pending Disconnect";
	else
		*value = "Disconnected";
	return 0;
}

/*#Device.PPP.Interface.{i}.Username!UCI:network/interface,@i-1/username*/
int get_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct uci_section *)data), "username", value);
	return 0;
}

int set_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct uci_section *)data), "username", value);
			return 0;
	}
	return 0;
}

/*#Device.PPP.Interface.{i}.Password!UCI:network/interface,@i-1/password*/
int set_ppp_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct uci_section *)data), "password", value);
			return 0;
	}
	return 0;
}

inline int ubus_get_wan_stats(void *data, char *instance, json_object *res, char **value, char *stat_mod)
{
	char *ifname, *proto;
	dmuci_get_value_by_section_string(((struct uci_section *)data), "ifname", &ifname);
	dmuci_get_value_by_section_string(((struct uci_section *)data), "proto", &proto);
	if (strcmp(proto, "pppoe") == 0) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", ifname, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	}
	return 0;
}

/*#Device.PPP.Interface.{i}.Stats.BytesReceived!UBUS:network.device/status/name,@Name/statistics.rx_bytes*/
int get_ppp_eth_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "rx_bytes");
	return 0;
}

/*#Device.PPP.Interface.{i}.Stats.BytesSent!UBUS:network.device/status/name,@Name/statistics.tx_bytes*/
int get_ppp_eth_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "tx_bytes");
	return 0;
}

/*#Device.PPP.Interface.{i}.Stats.PacketsReceived!UBUS:network.device/status/name,@Name/statistics.rx_packets*/
int get_ppp_eth_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "rx_packets");
	return 0;
}

/*#Device.PPP.Interface.{i}.Stats.PacketsSent!UBUS:network.device/status/name,@Name/statistics.tx_packets*/
int get_ppp_eth_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "tx_packets");
	return 0;
}


int get_PPPInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "tx_errors");
	return 0;
}

int get_PPPInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "rx_errors");
	return 0;
}

int get_PPPInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "unicast"));
	return 0;
}

int get_PPPInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "unicast"));
	return 0;
}

int get_PPPInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "tx_dropped");
	return 0;
}

int get_PPPInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "rx_dropped");
	return 0;
}

int get_PPPInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "multicast"));
	return 0;
}

int get_PPPInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "multicast"));
	return 0;
}

int get_PPPInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "broadcast"));
	return 0;
}

int get_PPPInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name((struct uci_section *)data));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "broadcast"));
	return 0;
}

int get_PPPInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(data, instance, res, value, "rx_over_errors");
	return 0;
}

int get_ppp_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	dmuci_get_value_by_section_string(((struct uci_section *)data), "ifname", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cATM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL) {
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cPTM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	}
	if (*value == NULL) {
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	}
	if (*value == NULL) {
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cSSID%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	}
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_ppp_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;
	char *newvalue= NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			if(linker) dmuci_set_value_by_section(((struct uci_section *)data), "ifname", linker);
			else return FAULT_9005;
			return 0;
	}
	return 0;
}

int get_PPP_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	char *proto;
	int nbre= 0;

	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "proto", &proto);
		if (!strstr(proto, "ppp"))
			continue;
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_PPPInterfacePPPoE_SessionID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_PPPInterfacePPPoE_ACName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *proto;
	dmuci_get_value_by_section_string(((struct uci_section *)data), "proto", &proto);
	if (strcmp(proto, "pppoe") == 0) {
		dmuci_get_value_by_section_string(((struct uci_section *)data), "ac", value);
		return 0;
	}
	*value= "";
	return 0;
}

int set_PPPInterfacePPPoE_ACName(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_string(((struct uci_section *)data), "proto", &proto);
			if (strcmp(proto, "pppoe") == 0)
				return 0;
			return FAULT_9001;
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct uci_section *)data), "ac", value);
			break;
	}
	return 0;
}

int get_PPPInterfacePPPoE_ServiceName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *proto;
	dmuci_get_value_by_section_string(((struct uci_section *)data), "proto", &proto);
	if (strcmp(proto, "pppoe") == 0) {
		dmuci_get_value_by_section_string(((struct uci_section *)data), "service", value);
		return 0;
	}
	*value= "";
	return 0;
}

int set_PPPInterfacePPPoE_ServiceName(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_string(((struct uci_section *)data), "proto", &proto);
			if (strcmp(proto, "pppoe") == 0)
				return 0;
			return FAULT_9001;
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct uci_section *)data), "service", value);
			break;
	}
	return 0;
}
/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_ppp_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {

	if(((struct uci_section *)data)) {
		dmasprintf(linker,"%s", section_name(((struct uci_section *)data)));
		return 0;
	}
	*linker = "";
	return 0;
}

/*************************************************************
 * ADD DEL OBJ
/*************************************************************/
int add_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char name[16] = {0};
	char *inst, *v;
	struct uci_section *s = NULL;
	struct uci_section *dmmap_ppp = NULL;

	check_create_dmmap_package("dmmap_network");
	inst = get_last_instance_lev2_icwmpd("network", "interface", "dmmap_network", "ppp_int_instance", "proto", "ppp");
	sprintf(name, "ppp_%d", inst ? (atoi(inst)+1) : 1);
	dmuci_set_value("network", name, "", "interface");
	dmuci_set_value("network", name, "proto", "ppp");
	dmuci_set_value("network", name, "username", name);
	dmuci_set_value("network", name, "password", name);
	dmuci_add_section_icwmpd("dmmap_network", "interface", &dmmap_ppp, &v);
	dmuci_set_value_by_section(dmmap_ppp, "section_name", name);
	*instance = update_instance_icwmpd(dmmap_ppp, inst, "ppp_int_instance");
	return 0;
}

int delete_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	struct uci_section *ppp_s = NULL;
	struct uci_section *ss = NULL, *dmmap_section = NULL;

	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct uci_section *)data)), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(((struct uci_section *)data), NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_option_eq("network", "interface", "proto", "ppp", ppp_s) {
				if (found != 0) {
					get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = ppp_s;
				found++;
			}
			if (ss != NULL) {
				get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
/*#Device.PPP.Interface.{i}.!UCI:network/interface/dmmap_network*/
int browseInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *net_sec = NULL;
	char *ppp_int = NULL, *ppp_int_last = NULL;
	char *proto;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "proto", &proto);
		if (!strstr(proto, "ppp"))
			continue;
		ppp_int = handle_update_instance(1, dmctx, &ppp_int_last, update_instance_alias, 3, p->dmmap_section, "ppp_int_instance", "ppp_int_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, ppp_int) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}
