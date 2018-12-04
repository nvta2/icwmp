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
#include "ethernet.h"
#include "dmjson.h"
#include "log.h"

char *wan_ifname = NULL;

/*** Ethernet. ***/
DMOBJ tEthernetObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, linker*/
{"Interface", &DMREAD, NULL, NULL, NULL, browseEthIfaceInst, NULL, NULL, tEthernetStatObj, tEthernetParams, get_linker_val},
{"VLANTermination", &DMWRITE, add_vlan_term, delete_vlan_term, NULL, browseVLANTermInst, NULL, NULL, NULL, tVLANTermParams, get_linker_vlan_term},
{0}
};

DMLEAF tVLANTermParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_vlan_term_enable, set_vlan_term_enable, NULL, NULL},
{"VLANID", &DMWRITE, DMT_UNINT, get_vlan_term_vlanid, set_vlan_term_vlanid, NULL, NULL},
{"TPID", &DMWRITE, DMT_UNINT, get_vlan_term_tpid, set_vlan_term_tpid, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_vlan_term_status, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_vlan_term_name, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_vlan_term_alias, set_vlan_term_alias, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_vlan_term_lowerlayers, set_vlan_term_lowerlayers, NULL, NULL},
{0}
};

DMLEAF tEthernetParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"Alias", &DMWRITE, DMT_STRING, get_eth_port_alias, set_eth_port_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_eth_port_enable, set_eth_port_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_eth_port_status, NULL, NULL, NULL},
{"MaxBitRate", &DMWRITE, DMT_INT, get_eth_port_maxbitrate, set_eth_port_maxbitrate, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_eth_port_name, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_eth_port_mac_address, NULL, NULL, NULL},
{"DuplexMode", &DMWRITE, DMT_STRING, get_eth_port_duplexmode, set_eth_port_duplexmode, NULL, NULL},
{0}
};

/*** Ethernet.Stats. ***/
DMOBJ tEthernetStatObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetStatParams, NULL},
{0}
};

DMLEAF tEthernetStatParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_eth_port_stats_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_eth_port_stats_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_eth_port_stats_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_eth_port_stats_rx_packets, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_eth_port_stats_tx_errors, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_eth_port_stats_rx_errors, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_eth_port_stats_tx_discardpackets, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_eth_port_stats_rx_discardpackets, NULL, NULL, NULL},
{0}
};


/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_val(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (data && ((struct eth_port_args *)data)->ifname) {
		*linker = ((struct eth_port_args *)data)->ifname;
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}
/**************************************************************************
* INIT
***************************************************************************/
inline int init_eth_port(struct eth_port_args *args, struct uci_section *s, char *ifname)
{
	args->eth_port_sec = s;
	args->ifname = ifname;
	return 0;
}
/**************************************************************************
* SET & GET ALIAS
***************************************************************************/
int get_eth_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_ports", "ethport", section_name(((struct eth_port_args *)data)->eth_port_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "eth_port_alias", value);
	if(*value == NULL || strlen(*value)<1)
	{
		dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "name", value);
	}
	return 0;
}

int set_eth_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_ports", "ethport", section_name(((struct eth_port_args *)data)->eth_port_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "eth_port_alias", value);
			return 0;
	}
	return 0;
}
/**************************************************************************
* GET & SET ETH PARAM
***************************************************************************/
int get_eth_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *ifname;

	if (strstr(((struct eth_port_args *)data)->ifname, wan_ifname)) {
		ifname = dmstrdup(wan_ifname);
	} else
		ifname = dmstrdup(((struct eth_port_args *)data)->ifname);

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "carrier");
	dmfree(ifname);
	return 0;
}

int set_eth_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *ifname;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (strstr(((struct eth_port_args *)data)->ifname, wan_ifname)) {
				ifname = dmstrdup(wan_ifname);
			} else
				ifname = dmstrdup(((struct eth_port_args *)data)->ifname);

			if (b) {
				DMCMD("ethctl", 3, ifname, "phy-power", "up"); //TODO wait ubus command
			}
			else {
				DMCMD("ethctl", 3, ifname, "phy-power", "down"); //TODO wait ubus command
			}
			dmfree(ifname);
			return 0;
	}
	return 0;
}

int get_eth_port_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	bool b;

	get_eth_port_enable(refparam, ctx, data, instance, value);
	string_to_bool(*value, &b);
	if (b)
		*value = "Up";
	else
		*value = "Down";
	return 0;
}

int get_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *pch, *spch, *v;

	dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "speed", value);
	if ((*value)[0] == '\0' || strcmp(*value, "disabled") == 0 )
		return 0;
	else {
		if (strcmp(*value, "auto") == 0)
			*value = "-1";
		else {
			v = dmstrdup(*value);
			pch = strtok_r(v, "FHfh", &spch);
			*value = dmstrdup(pch);
		}
	}
	return 0;
}

int set_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *val = "", *p = "";
	char *duplex;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "disabled") == 0 ) {
				dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", "disabled");
			} else if (strcasecmp(value, "auto") == 0 || strcmp(value, "-1") == 0) {
				dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", "auto");
			} else {
				dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "speed", &duplex);
				if (strcmp(duplex, "auto") == 0 || strcmp(duplex, "disabled") == 0)
					p = "FDAUTO";
				else {
					p = strchr(duplex, 'F') ? strchr(duplex, 'F') : strchr(duplex, 'H');
				}
				if (p) dmastrcat(&val, value, p);
				dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", val);
				dmfree(val);
			}
			return 0;
	}
	return 0;
}

int get_eth_port_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "name", value);
	return 0;
}

int get_eth_port_mac_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "macaddr");
	return 0;
}

int get_eth_port_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp, *name;

	dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "speed", value);
	if (*value[0] == '\0') {
		*value = "";
	} else if (strcmp(*value, "auto") == 0) {
		*value = "Auto";
	} else {
		if (strchr(*value, 'F'))
			*value = "Full";
		else if (strchr(*value, 'H'))
			*value = "Half";
		else
			*value = "";
	}
	return 0;
}

int set_eth_port_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *m, *spch, *rate, *val = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", "auto");
				return 0;
			}
			dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "speed", &m);
			m = dmstrdup(m);
			rate = m;
			if (strcmp(rate, "auto") == 0)
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
			dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", val);
			dmfree(m);
			dmfree(val);
			return 0;
	}
	return 0;
}

int get_eth_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_eth_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_eth_port_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_eth_port_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

int get_eth_port_stats_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_errors");
	return 0;
}

int get_eth_port_stats_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_errors");
	return 0;
}

int get_eth_port_stats_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_dropped");
	return 0;
}

int get_eth_port_stats_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_dropped");
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseEthIfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *int_num = NULL, *int_num_last = NULL, *ifname;
	struct uci_section *ss = NULL;
	struct eth_port_args curr_eth_port_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("ports", "ethport", "dmmap_ports", &dup_list);
	dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_ifname);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "ifname", &ifname);
		if (strcmp(ifname, wan_ifname) == 0) {
			if(strchr(ifname, '.')== NULL)
				dmasprintf(&ifname, "%s.1", ifname);
		}
		init_eth_port(&curr_eth_port_args, p->config_section, ifname);
		int_num =  handle_update_instance(1, dmctx, &int_num_last, update_instance_alias, 3, p->dmmap_section, "eth_port_instance", "eth_port_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_eth_port_args, int_num) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_vlan_term(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(((struct vlan_term_args *)data)->device_sec) {
		dmuci_get_value_by_section_string(((struct vlan_term_args *)data)->device_sec, "name", linker);
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/**************************************************************************
* SET & GET Enable
***************************************************************************/
int get_vlan_term_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

int set_vlan_term_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	// TODO
	return 0;
}

/**************************************************************************
* SET & GET VLANID
***************************************************************************/
int get_vlan_term_vlanid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	dmuci_get_value_by_section_string(((struct vlan_term_args *)data)->device_sec, "vid", value);
	return 0;
}

int set_vlan_term_vlanid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *ifname, *name;
	char *vid, *curr_ifname;
	struct uci_section *s;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET: {
			dmuci_get_value_by_section_string(((struct vlan_term_args *)data)->device_sec, "ifname", &ifname);
			dmasprintf(&name, "%s.%s", ifname, value);
			dmuci_set_value_by_section(((struct vlan_term_args *)data)->device_sec, "name", name);

			dmuci_get_value_by_section_string(((struct vlan_term_args *)data)->device_sec, "vid", &vid);
			dmuci_set_value_by_section(((struct vlan_term_args *)data)->device_sec, "vid", value);
			dmasprintf(&curr_ifname, "%s.%s", ifname, vid);

			// Update the interface related to this device
			uci_foreach_option_eq("network", "interface", "ifname", curr_ifname, s) {
				dmuci_set_value_by_section(s, "ifname", name);
			}
			dmfree(name);
			dmfree(curr_ifname);
			return 0;
		}
	}
	return 0;
}

/**************************************************************************
* SET & GET Alias
***************************************************************************/
int get_vlan_term_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct vlan_term_args *)data)->device_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "vlan_term_alias", value);

	return 0;
}

int set_vlan_term_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct vlan_term_args *)data)->device_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "vlan_term_alias", value);
			return 0;
	}
	return 0;
}

/**************************************************************************
* SET & GET TPID
***************************************************************************/
int get_vlan_term_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *type;
	dmuci_get_value_by_section_string(((struct vlan_term_args *)data)->device_sec, "type", &type);
	if (strcmp(type, "8021q") == 0 || strcmp(type, "untagged") == 0)
		// 0x8100
		*value = "33024";
	else if (strcmp(type, "8021ad") == 0)
		// 0x88a8
		*value = "34984";
	else
		return -1;

	return 0;
}

int set_vlan_term_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "33024") == 0)
				dmuci_set_value_by_section(((struct vlan_term_args *)data)->device_sec, "type", "8021q");
			else if (strcmp(value, "34984") == 0)
				dmuci_set_value_by_section(((struct vlan_term_args *)data)->device_sec, "type", "8021ad");
			else
				return -1;

			return 0;
	}

	return 0;
}

/**************************************************************************
* GET Status
***************************************************************************/
int get_vlan_term_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	// always use "Up"
	*value = "Up";
	return 0;
}

/**************************************************************************
* GET Name
***************************************************************************/
int get_vlan_term_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct vlan_term_args *)data)->device_sec));
	return 0;
}

/**************************************************************************
* SET & GET Lowerlayers
***************************************************************************/
int get_vlan_term_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	// TODO
	return 0;
}

int set_vlan_term_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	// TODO
	return 0;
}

/*******************ADD-DEL OBJECT*********************/
int add_vlan_term(char *refparam, struct dmctx *ctx, void *data, char **instance_para)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL, *dmmap_network= NULL;

	check_create_dmmap_package("dmmap_network");
	instance = get_last_instance_icwmpd("dmmap_network", "device", "vlan_term_instance");
	dmuci_add_section("network", "device", &s, &value);
	dmuci_set_value_by_section(s, "ifname", "eth0");
	dmuci_set_value_by_section(s, "type", "8021q");

	dmuci_add_section_icwmpd("dmmap_network", "device", &dmmap_network, &v);
	dmuci_set_value_by_section(dmmap_network, "section_name", section_name(s));
	*instance_para = update_instance_icwmpd(dmmap_network, instance, "vlan_term_instance");
	return 0;
}

int delete_vlan_term(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_section= NULL;

	switch (del_action) {
	case DEL_INST:
		get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct vlan_term_args *)data)->device_sec), &dmmap_section);
		if(dmmap_section != NULL)
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
		dmuci_delete_by_section(((struct vlan_term_args *)data)->device_sec, NULL, NULL);
		break;
	case DEL_ALL:
		uci_foreach_sections("network", "device", s) {
			if (found != 0){
				get_dmmap_section_of_config_section("dmmap_network", "device", section_name(s), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			ss = s;
			found++;
		}
		if (ss != NULL){
			get_dmmap_section_of_config_section("dmmap_network", "device", section_name(ss), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(ss, NULL, NULL);
		}
		break;
	}
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseVLANTermInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *vlan_term = NULL, *vlan_term_last = NULL;
	struct vlan_term_args curr_vlan_term_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "device", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		curr_vlan_term_args.device_sec = p->config_section;
		vlan_term = handle_update_instance(1, dmctx, &vlan_term_last, update_instance_alias, 3, p->dmmap_section, "vlan_term_instance", "vlan_term_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_vlan_term_args, vlan_term) == DM_STOP)
			break;
	}
	return 0;
}

