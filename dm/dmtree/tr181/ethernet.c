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

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "ethernet.h"
#include "dmjson.h"

struct eth_port_args cur_eth_port_args = {0};
char *wan_ifname = NULL;

inline int init_eth_port(struct dmctx *ctx, struct uci_section *s, char *ifname)
{
	struct eth_port_args *args = &cur_eth_port_args;
	ctx->args = (void *)args;
	args->eth_port_sec = s;
	args->ifname = ifname;
	return 0;
}

///////////////////SET & GET ALIAS////////////////////////////////////
int get_eth_port_alias(char *refparam, struct dmctx *ctx, char **value)
{
	char *eth_instance;
	dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "eth_port_alias", value);
	if(*value == NULL || strlen(*value)<1) {
		dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "eth_port_instance", &eth_instance);
		dmasprintf(value, "cpe-%s", eth_instance);
	}
	return 0;
}

int set_eth_port_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "eth_port_alias", value);
			return 0;
	}
	return 0;
}
//////////////////////////GET & SET ETH PARAM////////////////:
int get_eth_port_enable(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *ifname;

	if (strstr(cur_eth_port_args.ifname, wan_ifname)) {
		ifname = dmstrdup(wan_ifname);
	} else
		ifname = dmstrdup(cur_eth_port_args.ifname);

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "up");
	dmfree(ifname);
	return 0;
}

int set_eth_port_enable(char *refparam, struct dmctx *ctx, int action, char *value)
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
			if (strstr(cur_eth_port_args.ifname, wan_ifname)) {
				ifname = dmstrdup(wan_ifname);
			} else
				ifname = dmstrdup(cur_eth_port_args.ifname);

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

int get_eth_port_status(char *refparam, struct dmctx *ctx, char **value)
{
	bool b;

	get_eth_port_enable(refparam, ctx, value);
	string_to_bool(*value, &b);
	if (b)
		*value = "Up";
	else
		*value = "Down";
	return 0;
}

int get_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *pch, *spch, *v;

	dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "speed", value);
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

int set_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *val = "", *p = "";
	char *duplex;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "disabled") == 0 ) {
				dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "speed", "disabled");
			} else if (strcasecmp(value, "auto") == 0 || strcmp(value, "-1") == 0) {
				dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "speed", "auto");
			} else {
				dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "speed", &duplex);
				if (strcmp(duplex, "auto") == 0 || strcmp(duplex, "disabled") == 0)
					p = "FDAUTO";
				else {
					p = strchr(duplex, 'F') ? strchr(duplex, 'F') : strchr(duplex, 'H');
				}
				if (p) dmastrcat(&val, value, p);
				dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "speed", val);
				dmfree(val);
			}
			return 0;
	}
	return 0;
}

int get_eth_port_name(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "name", value);
	return 0;
}

int get_eth_port_mac_address(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_eth_port_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "macaddr");
	return 0;
}

int get_eth_port_duplexmode(char *refparam, struct dmctx *ctx, char **value)
{
	char *tmp, *name;

	dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "speed", value);
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

int set_eth_port_duplexmode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *m, *spch, *rate, *val = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "speed", "auto");
				return 0;
			}
			dmuci_get_value_by_section_string(cur_eth_port_args.eth_port_sec, "speed", &m);
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
			dmuci_set_value_by_section(cur_eth_port_args.eth_port_sec, "speed", val);
			dmfree(m);
			dmfree(val);
			return 0;
	}
	return 0;
}

int get_eth_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_eth_port_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_eth_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_eth_port_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_eth_port_stats_tx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_eth_port_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_eth_port_stats_rx_packets(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_eth_port_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

/////////////////////ENTRY METHOD //////////////////////////
int entry_method_root_Ethernet(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"Ethernet.") {
		DMOBJECT(DMROOT"Ethernet.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"Ethernet.Interface.", ctx, "0", 1, NULL, NULL, NULL);
		SUBENTRY(entry_method_eth_interface, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_method_eth_interface(struct dmctx *ctx)
{
	char *int_num = NULL, *int_num_last = NULL, *ifname;
	struct uci_section *ss = NULL;
#ifndef EX400
	dmuci_get_option_value_string("layer2_interface_ethernet", "Wan", "baseifname", &wan_ifname);
#else
	dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_ifname);
#endif
	uci_foreach_sections("ports", "ethport", ss) {
		dmuci_get_value_by_section_string(ss, "ifname", &ifname);
		if (strcmp(ifname, wan_ifname) == 0) {
			dmasprintf(&ifname, "%s.1", ifname);
		}
		init_eth_port(ctx, ss, ifname);
		int_num =  handle_update_instance(1, ctx, &int_num_last, update_instance_alias, 3, ss, "eth_port_instance", "eth_port_alias");
		SUBENTRY(entry_eth_interface_instance, ctx, int_num);
	}
	return 0;
}

inline int entry_eth_interface_instance(struct dmctx *ctx, char *int_num)
{
	IF_MATCH(ctx, DMROOT"Ethernet.Interface.%s.", int_num) {
		DMOBJECT(DMROOT"Ethernet.Interface.%s.", ctx, "0", 1, NULL, NULL, cur_eth_port_args.ifname, int_num);
		DMPARAM("Alias", ctx, "1", get_eth_port_alias, set_eth_port_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Enable", ctx, "1", get_eth_port_enable, set_eth_port_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Status", ctx, "0", get_eth_port_status, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxBitRate", ctx, "1", get_eth_port_maxbitrate, set_eth_port_maxbitrate, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Name", ctx, "0", get_eth_port_name, NULL, NULL, 0, 1, UNDEF, NULL); //TO CHECK R/W
		DMPARAM("MACAddress", ctx, "0", get_eth_port_mac_address, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DuplexMode", ctx, "1", get_eth_port_duplexmode, set_eth_port_duplexmode, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"Ethernet.Interface.%s.Stats.", ctx, "0", 1, NULL, NULL, NULL, int_num);
		DMPARAM("BytesSent", ctx, "0", get_eth_port_stats_tx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("BytesReceived", ctx, "0", get_eth_port_stats_rx_bytes, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("PacketsSent", ctx, "0", get_eth_port_stats_tx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("PacketsReceived", ctx, "0", get_eth_port_stats_rx_packets, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}
