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
#include <ctype.h>
#include <uci.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "bridging.h"
#include "dmjson.h"

static char *wan_baseifname = NULL;

/*** Bridging. ***/
DMOBJ tBridgingObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Bridge", &DMWRITE, add_bridge, delete_bridge, NULL, browseBridgeInst, NULL, NULL, tDridgingBridgeObj, tDridgingBridgeParams, NULL},
{0}
};

/*** Bridging.Bridge.{i}. ***/
DMOBJ tDridgingBridgeObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"VLAN", &DMWRITE, add_br_vlan, delete_br_vlan, NULL, browseBridgeVlanInst, NULL, NULL, NULL, tBridgeVlanParams, get_linker_br_vlan},
{"Port", &DMWRITE, add_br_port, delete_br_port, NULL, browseBridgePortInst, NULL, NULL, tBridgePortObj, tBridgePortParams, get_linker_br_port},
{"VLANPort", &DMREAD, NULL, NULL, NULL, browseBridgeVlanPortInst, NULL, NULL, NULL, tBridgeVlanPortParams, NULL},
{0}
};

DMLEAF tDridgingBridgeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_br_alias, set_br_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_br_enable, set_br_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_br_status, NULL, NULL, NULL},
{CUSTOM_PREFIX"AssociatedInterfaces", &DMWRITE, DMT_STRING, get_br_associated_interfaces, set_br_associated_interfaces, NULL, NULL},
{0}
};

/*** Bridging.Bridge.{i}.VLAN.{i}. ***/
DMLEAF tBridgeVlanParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING,get_br_vlan_alias, set_br_vlan_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_br_vlan_enable, set_br_vlan_enable, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_br_vlan_name, set_br_vlan_name, NULL, NULL},
{"VLANID", &DMWRITE, DMT_STRING, get_br_vlan_vid, set_br_vlan_vid, NULL, NULL},
{CUSTOM_PREFIX"VLANPriority", &DMWRITE, DMT_STRING, get_br_vlan_priority, set_br_vlan_priority, NULL, NULL},
{0}
};

/*** Bridging.Bridge.{i}.Port.{i}. ***/
DMOBJ tBridgePortObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tBridgePortStatParams, NULL},
{0}
};

DMLEAF tBridgePortParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_br_port_alias, set_br_port_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_br_port_enable, set_br_port_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_br_port_status, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_br_port_name, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_port_lower_layer, set_port_lower_layer, NULL, NULL},
{"ManagementPort", &DMREAD, DMT_STRING, get_br_port_management, NULL, NULL, NULL},
{0}
};

/*** Bridging.Bridge.{i}.Port.{i}.Stats. ***/
DMLEAF tBridgePortStatParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_br_port_stats_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_br_port_stats_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_br_port_stats_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_br_port_stats_rx_packets, NULL, NULL, NULL},
{0}
};

/*** Bridging.Bridge.{i}.VLANPort.{i}. ***/
DMLEAF tBridgeVlanPortParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMREAD, DMT_STRING,  get_br_vlan_alias, NULL, NULL, NULL},
{"Enable", &DMREAD, DMT_BOOL, get_br_vlan_enable, NULL, NULL, NULL},
{"VLAN", &DMREAD, DMT_STRING,  get_vlan_port_vlan_ref, NULL, NULL, NULL},
{"Port", &DMWRITE, DMT_STRING, get_vlan_port_port_ref, set_vlan_port_port_ref, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_br_port(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct bridging_port_args *)data)->bridge_port_sec) {
		dmasprintf(linker,"%s+%s", section_name(((struct bridging_port_args *)data)->bridge_port_sec), ((struct bridging_port_args *)data)->ifname);
		return 0;
	}
	*linker = "";
	return 0;
}

int get_linker_br_vlan(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct bridging_vlan_args *)data)->vlan_port) {
		dmasprintf(linker,"vlan%s_%s", ((struct bridging_vlan_args *)data)->vlan_port, ((struct bridging_vlan_args *)data)->br_inst);
		return 0;
	}
	*linker = "";
	return 0;
}
/**************************************************************************
* INIT
***************************************************************************/
inline int init_bridging_args(struct bridging_args *args, struct uci_section *s, char *last_instance, char *ifname, char *br_instance)
{
	args->bridge_sec = s;
	args->br_key = last_instance;
	args->ifname = ifname;
	args->br_inst = br_instance;
	return 0;
}

inline int init_bridging_port_args(struct bridging_port_args *args, struct uci_section *s, struct uci_section *bs, bool vlan, char *ifname)
{
	args->bridge_port_sec = s;
	args->bridge_sec = bs;
	args->vlan = vlan;
	args->ifname = ifname;
	return 0;
}

inline int init_bridging_vlan_args(struct bridging_vlan_args *args, struct uci_section *s, struct uci_section *bs, char *vlan_port, char *br_inst)
{
	args->bridge_vlan_sec = s;
	args->bridge_sec = bs;
	args->vlan_port = vlan_port;
	args->br_inst = br_inst;
	return 0;
}

/**************************************************************************
* INSTANCE MG
***************************************************************************/
char *get_last_vid(void)
{
	struct uci_section *vlan_s;
	char *vid = NULL, *type;
	int num_vid = 0;

	uci_foreach_sections("network", "device", vlan_s) {
		if(!vlan_s)
			break;
		dmuci_get_value_by_section_string(vlan_s, "type", &type);
		if (strcmp(type, "untagged")==0)
			continue;
		dmuci_get_value_by_section_string(vlan_s, "vid", &vid);
		if( atoi(vid)> num_vid )
			num_vid = atoi(vid);
		dmasprintf(&vid, "%d", num_vid);
	}
	return vid;
}

int check_ifname_exist_in_br_ifname_list(char *ifname)
{
	char *br_ifname_list, *br_ifname_dup, *pch, *spch;
	struct uci_section *s;
	uci_foreach_option_eq("network", "interface", "type", "bridge", s) {
		dmuci_get_value_by_section_string(s, "ifname", &br_ifname_list);
		if(br_ifname_list[0] == '\0')
			return 0;
		br_ifname_dup = dmstrdup(br_ifname_list);
		for (pch = strtok_r(br_ifname_dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
			if (strcmp(pch, ifname) == 0)
				return 1;
		}
	}
	return 0;
}

int get_br_port_last_inst(char *br_key)
{
	char *tmp;
	int max=1;
	struct uci_section *s;
	int instance;

	uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key", br_key, s) {
		dmuci_get_value_by_section_string(s, "bridge_port_instance", &tmp);
		if (tmp[0] == '\0')
			continue;
		instance = atoi(tmp);
		if(instance>max) max=instance;
	}
	return max;
}

char *br_port_update_instance_alias_icwmpd(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	char *inst_opt = (char *) argv[1];
	char *alias_opt = (char *) argv[2];
	bool *find_max = (bool *) argv[3];
	char *br_key = (char *) argv[4];

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			int m = get_br_port_last_inst(br_key);
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

char *br_port_update_instance_alias(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	char *inst_opt = (char *) argv[1];
	char *alias_opt = (char *) argv[2];
	bool *find_max = (bool *) argv[3];
	char *br_key = (char *) argv[4];
	int m;

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			m = get_br_port_last_inst(br_key);
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance = dmuci_set_value_by_section(s, inst_opt, buf);
	}
	*last_inst = instance;
	if (action == INSTANCE_MODE_ALIAS) {
		dmuci_get_value_by_section_string(s, alias_opt, &alias);
		if (alias[0] == '\0') {
			sprintf(buf, "cpe-%s", instance);
			alias = dmuci_set_value_by_section(s, alias_opt, buf);
		}
		sprintf(buf, "[%s]", alias);
		instance = dmstrdup(buf);
	}
	return instance;
}

int reset_br_port(char *br_key)
{
	struct uci_section *s, *prev_s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key", br_key, s){
		if (prev_s)
			dmuci_delete_by_section(prev_s, NULL, NULL);
		prev_s = s;
	}

	if (prev_s)
		dmuci_delete_by_section(prev_s, NULL, NULL);
	return 0;
}

int check_ifname_is_not_lan_port(char *ifname)
{
	struct uci_section *s;
	if (!strstr(ifname, wan_baseifname)) {
		uci_foreach_option_eq("ports", "ethport", "ifname", ifname, s) {
			return 0;
		}
	}
	return 1;
}

int update_port_parameters(char *linker, char *br_key, char *br_pt_inst, char *mg_port)
{
	struct uci_section *s, *dmmap_section;
	if (check_ifname_is_vlan(linker)) {
		uci_foreach_option_eq("network", "device", "ifname", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	} else if (strncmp(linker, "ptm", 3) == 0) {
		uci_foreach_option_eq("dsl", "ptm-device", "device", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	} else if (strncmp(linker, "atm", 3) == 0) {
		uci_foreach_option_eq("dsl", "atm-device", "device", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	} else if (strncmp(linker, "wl", 2) == 0) {
		uci_foreach_option_eq("wireless", "wifi-iface", "ifname", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	} else if (strncmp(linker, "eth0", 4) == 0) {
		uci_foreach_option_eq("network", "device", "name", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	} else {
		uci_foreach_option_eq("ports", "ethport", "ifname", linker, s) {
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(s), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "bridge_key", br_key);
			dmuci_set_value_by_section(dmmap_section, "bridge_port_instance", br_pt_inst);
			dmuci_set_value_by_section(dmmap_section, "mg_port", mg_port);
			break;
		}
	}
	return 0;
}
/**************************************************************************
*SET & GET BRIDGING PARAMETERS
***************************************************************************/
int get_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct bridging_args *)data)->bridge_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "false");
	*value = dmjson_get_value(res, 1, "up");
	return 0;
}

int get_br_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct bridging_args *)data)->bridge_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "Disabled");
	*value = dmjson_get_value(res, 1, "up");
	if(strcmp(*value,"true") == 0)
		*value = "Enabled";
	else
		*value = "Disabled";
	return 0;
}

int set_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	int error = string_to_bool(value, &b);

	switch (action) {
		case VALUECHECK:
			if (error)
				return FAULT_9007;
			return 0;
		case VALUESET:
			if (b) {
				dmubus_call_set("network.interface", "up", UBUS_ARGS{{"interface", section_name(((struct bridging_args *)data)->bridge_sec), String}}, 1);
			}
			else {
				dmubus_call_set("network.interface", "down", UBUS_ARGS{{"interface", section_name(((struct bridging_args *)data)->bridge_sec), String}}, 1);
			}
			return 0;
	}
	return 0;
}

int get_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct bridging_args *)data)->bridge_sec, "ifname", value);
	return 0;
}

int set_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct bridging_args *)data)->bridge_sec, "ifname", value);
			return 0;
	}
	return 0;
}

int get_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *speed, *val;
	struct uci_section *wifi_device_s;

	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", value);
	if(strncmp(*value, "wl", 2) == 0 || strncmp(*value, "ra", 2) == 0 || strncmp(*value, "apclii", 6) == 0) {
		uci_foreach_option_eq("wireless", "wifi-iface", "ifname", *value, wifi_device_s) {
			dmuci_get_value_by_section_string(wifi_device_s, "disabled", &val);
			if ((val[0] == '\0') || (val[0] == '0'))
				*value = "true";
			else
				*value = "false";
			return 0;
		}
	}
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", *value, String}}, 1, &res);
	DM_ASSERT(res, *value = "false");
	speed = dmjson_get_value(res, 1, "speed");
	if(*speed != '\0')
		*value = "true";
	else
		*value = "false";
	return 0;
}

int set_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			return 0;
	}
	return 0;
}

int get_br_port_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	bool b;

	get_br_port_enable(refparam, ctx, data, instance, value);
	string_to_bool(*value, &b);
	if (b)
		*value = "Up";
	else
		*value = "Down";
	return 0;
}

int get_br_port_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "name", value);
	return 0;
}

int get_br_port_management(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section= NULL;

	get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(((struct bridging_port_args *)data)->bridge_port_sec), &dmmap_section);
	if(dmmap_section==NULL || dmmap_section==0)
		dmmap_section= ((struct bridging_port_args *)data)->bridge_port_sec;
	dmuci_get_value_by_section_string(dmmap_section, "mg_port", value);
	return 0;
}

/**************************************************************************
* GET STAT
***************************************************************************/
int get_br_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", value);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", *value, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_br_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", value);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", *value, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_br_port_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", value);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", *value, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_br_port_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", value);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", *value, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

int is_bridge_vlan_enabled(struct bridging_vlan_args *curr_arg)
{
	struct uci_section *vlan_sec = curr_arg->bridge_vlan_sec, *br_sec = curr_arg->bridge_sec;
	char *ifname, *br_ifname, *ifname_dup, *pch, *spch;

	dmuci_get_value_by_section_string(br_sec, "ifname", &br_ifname);
	dmuci_get_value_by_section_string(vlan_sec, "name", &ifname);
	ifname_dup = dmstrdup(br_ifname);
	if(ifname!=NULL && ifname[0]!='\0'){
		if (is_strword_in_optionvalue(ifname_dup, ifname))
			return 1;
	}
	return 0;
}

static int update_br_vlan_ifname(struct bridging_vlan_args *curr_arg, int status)
{
	char ifname_dup[128], *ptr, *baseifname, *ifname, *start, *end;
	struct uci_section *vlan_sec = curr_arg->bridge_vlan_sec, *br_sec = curr_arg->bridge_sec;
	int pos=0;
	dmuci_get_value_by_section_string(br_sec, "ifname", &ifname);
	dmuci_get_value_by_section_string(vlan_sec, "name", &baseifname);
	ptr = ifname_dup;
	dmstrappendstr(ptr, ifname);
	dmstrappendend(ptr);
	if(status){
		if (is_strword_in_optionvalue(ifname_dup, baseifname)) return 0;
		if (ifname_dup[0] != '\0') dmstrappendchr(ptr, ' ');
		dmstrappendstr(ptr, baseifname);
		dmstrappendend(ptr);
	}else{
		if (is_strword_in_optionvalue(ifname_dup, baseifname)){
			start = strstr(ifname_dup, baseifname);
			end = start + strlen(baseifname);
			if(start != ifname_dup){
				start--;
				pos=1;
			}
			memmove(start, start + strlen(baseifname)+pos, strlen(end) + 1);
		}
	}
	dmuci_set_value_by_section(br_sec, "ifname", ifname_dup);
	return 0;
}

int get_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "false";
	int status;
	status = is_bridge_vlan_enabled((struct bridging_vlan_args *)data);
	if (status)
		*value = "true";
	return 0;
}

int set_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *vlan_ifname, *br_ifname, *vid, *p;
	char new_ifname[256];
	char pr_linker[32];
	int is_enabled;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			is_enabled = is_bridge_vlan_enabled((struct bridging_vlan_args *)data);
			if (b && !is_enabled) {
				update_br_vlan_ifname((struct bridging_vlan_args *)data, 1);
			}
			if (!b && is_enabled){
				update_br_vlan_ifname((struct bridging_vlan_args *)data, 0);
			}
			return 0;
	}
	return 0;
}

int get_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	*value = dmstrdup(section_name(((struct bridging_vlan_args *)data)->bridge_vlan_sec));
	return 0;
}

int set_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			 dmuci_rename_section_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec,value);
			return 0;
	}
	return 0;
}

int get_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "vid", value);
	return 0;
}

int set_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *name, *br_ifname, *ifname;
	int is_enabled;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "vid", value);
			dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "ifname", &ifname);
			dmasprintf(&name, "%s.%s", ifname, value);
			is_enabled = is_bridge_vlan_enabled((struct bridging_vlan_args *)data);
			if(is_enabled)
				update_br_vlan_ifname((struct bridging_vlan_args *)data, 0);
			dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "name", name);
			if(is_enabled)
				update_br_vlan_ifname((struct bridging_vlan_args *)data, 1);
			return 0;
	}
	return 0;
}

int get_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "priority", value);
	return 0;
}

int set_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *ifname, *p, *vifname, *linker, *n_ifname;
	char buf[256];
	char tmp[8];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "priority", value);
			return 0;
	}
	return 0;
}
/*************************************************************
 * GET SET ALIAS
/*************************************************************/

int get_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct bridging_args *)data)->bridge_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "bridge_alias", value);
	return 0;
}

int set_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct bridging_args *)data)->bridge_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "bridge_alias", value);
			return 0;
	}
	return 0;
}

int get_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section= NULL;

	get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(((struct bridging_port_args *)data)->bridge_port_sec), &dmmap_section);
	if(dmmap_section==NULL || dmmap_section==0)
		dmmap_section= ((struct bridging_port_args *)data)->bridge_port_sec;

	dmuci_get_value_by_section_string(dmmap_section, "bridge_port_alias", value);
	return 0;
}

int set_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section= NULL;

	get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(((struct bridging_port_args *)data)->bridge_port_sec), &dmmap_section);
	if(dmmap_section==NULL || dmmap_section==0)
		dmmap_section= ((struct bridging_port_args *)data)->bridge_port_sec;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "bridge_port_alias", value);
			return 0;
	}
	return 0;
}

int get_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct bridging_vlan_args *)data)->bridge_vlan_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "bridge_vlan_alias", value);
	return 0;
}

int set_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct bridging_vlan_args *)data)->bridge_vlan_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "bridge_vlan_alias", value);
			return 0;
	}
	return 0;
}
/*************************************************************
 * ADD DELETE OBJECT
/*************************************************************/
int add_bridge(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *last_inst, *v;
	char bridge_name[16], ib[8];
	char *p = bridge_name;
	struct uci_section* dmmap_bridge= NULL;

	last_inst = get_last_instance_lev2_icwmpd("network", "interface", "dmmap_network", "bridge_instance", "type", "bridge");
	sprintf(ib, "%d", last_inst ? atoi(last_inst)+1 : 1);
	dmstrappendstr(p, "bridge_0_");
	dmstrappendstr(p, ib);
	dmstrappendend(p);
	dmuci_set_value("network", bridge_name, "", "interface");
	dmuci_set_value("network", bridge_name, "type", "bridge");
	dmuci_set_value("network", bridge_name, "proto", "dhcp");

	dmuci_add_section_icwmpd("dmmap_network", "interface", &dmmap_bridge, &v);
	dmuci_set_value_by_section(dmmap_bridge, "section_name", bridge_name);
	*instance = update_instance_icwmpd(dmmap_bridge, last_inst, "bridge_instance");

	update_section_list(DMMAP,"bridge_port", "bridge_key", 1, ib, "mg_port", "true", "bridge_port_instance", "1");
	return 0;
}

int delete_bridge(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL, *prev_s = NULL;
	struct uci_section *bridge_s, *vlan_s, *dmmap_section= NULL;
	char *bridgekey = NULL;

	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct bridging_args *)data)->bridge_sec), &dmmap_section);
			dmuci_set_value_by_section(((struct bridging_args *)data)->bridge_sec, "type", "");
			dmuci_set_value_by_section(dmmap_section, "bridge_instance", "");
			dmuci_set_value_by_section(dmmap_section, "ip_int_instance", "");
			dmuci_set_value_by_section(dmmap_section, "ipv4_instance", "");
			uci_path_foreach_option_eq(icwmpd, "dmmap", "bridge_port", "bridge_key", ((struct bridging_args *)data)->br_key, s) {
				if (prev_s)
					DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
				prev_s = s;
			}
			if (prev_s)
				DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
			reset_br_port( ((struct bridging_args *)data)->br_key);
			dmuci_set_value_by_section(((struct bridging_args *)data)->bridge_sec, "ifname", "");
			break;
		case DEL_ALL:
			uci_foreach_option_eq("network", "interface", "type", "bridge", bridge_s) {
				get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(bridge_s), &dmmap_section);
				dmuci_set_value_by_section(bridge_s, "type", "");
				dmuci_get_value_by_section_string(dmmap_section, "bridge_instance", &bridgekey);
				dmuci_set_value_by_section(dmmap_section, "bridge_instance", "");
				dmuci_set_value_by_section(dmmap_section, "ip_int_instance", "");
				dmuci_set_value_by_section(dmmap_section, "ipv4_instance", "");
				uci_path_foreach_option_eq(icwmpd, "dmmap", "bridge_port", "bridge_key", bridgekey, s) {
					prev_s = s;
				}
				if (prev_s)
					DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
				reset_br_port(bridgekey);
				dmuci_set_value_by_section(bridge_s, "ifname", "");
			}
			break;
	}
	return 0;
}

int add_br_vlan(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value, *last_instance, *v, *vlan_name, *name, *last_vid, *vid;
	struct uci_section *vlan_s, *dmmap_bridge_vlan;

	check_create_dmmap_package("dmmap_network");
	last_instance = get_last_instance_lev2_icwmpd_dmmap_opt("network", "device", "dmmap_network", "bridge_vlan_instance", "bridge_key", ((struct bridging_args *)data)->br_key);
	dmasprintf(&vlan_name, "vlan%d", last_instance ? atoi(last_instance)+ 1 : 1);
	last_vid = get_last_vid();
	dmasprintf(&name, "%s.%d", wan_baseifname, last_vid ? atoi(last_vid)+ 1 : 1001);
	dmasprintf(&vid, "%d", last_vid ? atoi(last_vid)+ 1 : 1001);
	dmuci_add_section_and_rename("network", "device", &vlan_s, &value);
	dmuci_rename_section_by_section(vlan_s, vlan_name);
	dmuci_set_value_by_section(vlan_s, "priority", "0");
	dmuci_set_value_by_section(vlan_s, "type", "8021q");
	dmuci_set_value_by_section(vlan_s, "vid", vid);
	dmuci_set_value_by_section(vlan_s, "name", name);
	dmuci_set_value_by_section(vlan_s, "ifname", wan_baseifname);

	dmuci_add_section_icwmpd("dmmap_network", "device", &dmmap_bridge_vlan, &v);
	dmuci_set_value_by_section(dmmap_bridge_vlan, "section_name", vlan_name);
	dmuci_set_value_by_section(dmmap_bridge_vlan, "bridge_key", ((struct bridging_args *)data)->br_key);
	*instance = update_instance_icwmpd(dmmap_bridge_vlan, last_instance, "bridge_vlan_instance");
	return 0;
}

int delete_br_vlan(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	char *vid, *ifname, *type;
	struct uci_section *prev_s = NULL, *vlan_s=NULL, *dmmap_section;
	char new_ifname[128];
	int is_enabled;

	switch (del_action) {
	case DEL_INST:
		is_enabled = is_bridge_vlan_enabled((struct bridging_vlan_args *)data);
		if(is_enabled)
			update_br_vlan_ifname((struct bridging_vlan_args *)data, 0);
		get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct bridging_vlan_args *)data)->bridge_vlan_sec), &dmmap_section);
		dmuci_delete_by_section(dmmap_section, NULL, NULL);
		dmuci_delete_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, NULL, NULL);
		break;
	case DEL_ALL:
		uci_foreach_sections("network", "device", vlan_s) {
			dmuci_get_value_by_section_string(vlan_s, "type", &type);
			if (strcmp(type, "untagged")==0)
				continue;
			dmuci_get_value_by_section_string(vlan_s, "vid", &vid);
			dmuci_get_value_by_section_string(((struct bridging_args *)data)->bridge_sec, "ifname", &ifname);
			if(ifname[0] != '\0' && vid[0] != '\0'){
				remove_vid_interfaces_from_ifname(vid, ifname, new_ifname);
				dmuci_set_value_by_section(((struct bridging_args *)data)->bridge_sec, "ifname", new_ifname);
			}
			if (prev_s != NULL){
				get_dmmap_section_of_config_section("dmmap_network", "device", section_name(prev_s), &dmmap_section);
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(prev_s, NULL, NULL);
			}
			prev_s = vlan_s;
		}
		if (prev_s != NULL){
			get_dmmap_section_of_config_section("dmmap_network", "device", section_name(prev_s), &dmmap_section);
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(prev_s, NULL, NULL);
		}
		break;
	}
	return 0;
}

int add_br_port(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value;
	struct uci_section *br_port_s;

	int m = get_br_port_last_inst(((struct bridging_args *)data)->br_key);
	dmasprintf(instance, "%d", m+1);
	DMUCI_ADD_SECTION(icwmpd, "dmmap_bridge_port", "bridge_port", &br_port_s, &value);
	dmuci_set_value_by_section(br_port_s, "bridge_key", ((struct bridging_args *)data)->br_key);
	dmuci_set_value_by_section(br_port_s, "bridge_port_instance", *instance);
	dmuci_set_value_by_section(br_port_s, "mg_port", "false");
	return 0;
}

int delete_br_port(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	char *ifname;
	char new_ifname[128];
	struct uci_section *s = NULL, *prev_s = NULL, *dmmap_section= NULL;

	switch (del_action) {
	case DEL_INST:
		get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(((struct bridging_port_args *)data)->bridge_port_sec), &dmmap_section);
		if(dmmap_section==NULL || dmmap_section==0){
			dmmap_section= ((struct bridging_port_args *)data)->bridge_port_sec;
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_section, NULL, NULL);
		}
		else{
			dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_sec, "ifname", &ifname);
			if(ifname[0] != '\0'){
				remove_interface_from_ifname(((struct bridging_port_args *)data)->ifname, ifname, new_ifname);
				dmuci_set_value_by_section(((struct bridging_port_args *)data)->bridge_sec, "ifname", new_ifname);
			}
			get_dmmap_section_of_config_section("dmmap_bridge_port", "bridge_port", section_name(((struct bridging_port_args *)data)->bridge_port_sec), &dmmap_section);
			DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_section, NULL, NULL);
		}
		break;
	case DEL_ALL:
		uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key", ((struct bridging_args *)data)->br_key, s) {
			if (prev_s)
				DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
			prev_s = s;
		}
		if (prev_s)
			DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
		dmuci_set_value_by_section(((struct bridging_args *)data)->bridge_sec, "ifname", ""); // TO CHECK
		break;
	}
	return 0;
}
/*************************************************************
 * LOWER LAYER
/*************************************************************/
int check_port_with_ifname (char *ifname, struct uci_section **ss)
{
	struct uci_section *sss, *s;
	char *file_config_name;
	char *atm_device, *ptm_device;

	dmasprintf(&file_config_name, "%s","/etc/config/dsl");

	if (check_ifname_is_vlan(ifname)) {
		uci_foreach_option_eq("network", "device", "name", ifname, s) {
			*ss = s;
			break;
		}
	}
	else if (strncmp(ifname, "ptm", 3) == 0) {
		if(access( file_config_name, F_OK ) != -1){
			uci_foreach_sections("dsl", "ptm-device", sss) {
				dmuci_get_value_by_section_string(sss, "device", &ptm_device);
				dmasprintf(&ptm_device, "%s.1", ptm_device);
				if(strcmp(ifname, ptm_device)==0) {
					uci_foreach_option_eq("network", "device", "name", ifname, s) {
						*ss = s;
						break;
					}
				}
			}
		}
	}
	else if (strncmp(ifname, "atm", 3) == 0) {
		if(access( file_config_name, F_OK ) != -1){
			uci_foreach_sections("dsl", "atm-device", sss) {
				dmuci_get_value_by_section_string(sss, "device", &atm_device);
				dmasprintf(&atm_device, "%s.1", atm_device);
				if(strcmp(ifname, atm_device)==0) {
					uci_foreach_option_eq("network", "device", "name", ifname, s) {
						*ss = s;
						break;
					}
				}
			}
		}
	}
	else if(strncmp(ifname, wan_baseifname, strlen(wan_baseifname))==0) {
		uci_foreach_option_eq("network", "device", "name", ifname, s) {
			*ss = s;
			break;
		}
	}
	else if(strncmp(ifname, "wl", 2) == 0 || strncmp(ifname, "ra", 2) == 0 || strncmp(ifname, "apclii", 6) == 0) {
		uci_foreach_option_eq("wireless", "wifi-iface", "ifname", ifname, s) {
			*ss = s;
			break;
		}
	}
	else {
		uci_foreach_option_eq("ports", "ethport", "ifname", ifname, s) {
			*ss = s;
			break;
		}
	}
	return 0;
}

int get_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mg_port, *pch, *spch, *ifname, *ifname_dup, *p, *linker = "";
	char buf[16], plinker[32], lbuf[512];
	struct uci_section *s = NULL;

	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "mg_port", &mg_port);
	dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_sec, "ifname", &ifname);
	if (ifname[0] != '\0' && strcmp(mg_port, "true") ==  0) {
		ifname_dup = dmstrdup(ifname);
		p = lbuf;
		for (pch = strtok_r(ifname_dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
			check_port_with_ifname(pch, &s);
			if(s == NULL)
				continue;
			sprintf(plinker, "%s+%s", section_name(s), pch);
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cBridging%cBridge%c", dmroot, dm_delim, dm_delim, dm_delim), plinker, value);
			if (*value == NULL)
				*value = "";
			dmstrappendstr(p, *value);
			dmstrappendchr(p, ',');
		}
		p = p -1;
		dmstrappendend(p);
		*value = dmstrdup(lbuf);
		return 0;
	} else {
		dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "ifname", &linker);
		if (strcmp(linker, wan_baseifname) == 0) {
			dmasprintf(&linker, "%s.1", linker);
		}
		if(((struct bridging_port_args *)data)->vlan) {
			strncpy(buf, linker, 5);
			buf[5] = '\0';
			strcat(buf, "1");
			linker = buf;
		}
	}
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx,dm_print_path("%s%cWiFi%cSSID%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cATM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cPTM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		*value = "";
	return 0;
}

int set_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker, *iface, *ifname, *p, *br_key, *br_pt_inst = "", *mg_port = "false", *br_port_ifname, *vid = NULL;
	char new_ifname[256];
	char tmp[16];
	char pr_linker[32];
	struct uci_section *s;
	switch (action) {
		case VALUECHECK:
			dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "mg_port", &mg_port);
			adm_entry_get_linker_value(ctx, value, &linker);
			if (strcmp(mg_port, "false") && linker && check_ifname_exist_in_br_ifname_list(linker))
				return FAULT_9001;
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			 //check ifname(linker) doesn't exit in bridges
			if (linker && !check_ifname_exist_in_br_ifname_list(linker)) {
				//save param of current port and copy it to new port
				dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "bridge_key", &br_key);
				dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "bridge_port_instance", &br_pt_inst);
				dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_port_sec, "mg_port", &mg_port);
				//remove old port (ifname) from bridge
				if (((struct bridging_port_args *)data)->ifname[0] != 0 && strcmp(((struct bridging_port_args *)data)->ifname, linker) != 0) {
					delete_br_port(NULL, ctx, data, instance, DEL_INST);
				}
				// check if the current port is already linked with VLAN
				sprintf(pr_linker,"%s+%s", section_name(((struct bridging_port_args *)data)->bridge_port_sec), ((struct bridging_port_args *)data)->ifname);
				uci_foreach_option_eq("network", "device", "br_port_linker", pr_linker, s) {
					dmuci_get_value_by_section_string(s, "vlan8021q", &vid);
					break;
				}
				dmuci_get_value_by_section_string(((struct bridging_port_args *)data)->bridge_sec, "ifname", &ifname);
				p = new_ifname;
				if (ifname[0] != '\0') {
					dmstrappendstr(p, ifname);
					dmstrappendchr(p, ' ');
				}
				if(vid && check_ifname_is_not_lan_port(linker) && !strstr (linker, "wl")) {
					strncpy(tmp, linker, 5);
					tmp[5] = '\0';
					strcat(tmp, vid);
					linker = tmp;
					dmstrappendstr(p, tmp);
					dmstrappendend(p);
					uci_foreach_option_eq("network", "device", "br_port_linker", pr_linker, s) {
						sprintf(pr_linker,"%s+%s", section_name(s), linker);
						dmuci_set_value_by_section(s, "br_port_linker", pr_linker);
						dmuci_set_value_by_section(s, "ifname", linker);
						dmuci_set_value_by_section(s, "penable", "1");
					}
				} else {
					dmstrappendstr(p, linker);
					dmstrappendend(p);
				}
				dmuci_set_value_by_section(((struct bridging_port_args *)data)->bridge_sec, "ifname", new_ifname);
				//remove old br_port param to the new one
				update_port_parameters(linker, br_key, br_pt_inst, mg_port);
				if(((struct bridging_port_args *)data)->ifname[0] == '\0')
					DMUCI_DELETE_BY_SECTION(icwmpd,((struct bridging_port_args *)data)->bridge_port_sec, NULL, NULL);// delete dmmap section after remove br_port_instance to adequate config
			}
			return 0;
	}
	return 0;
}

int get_vlan_port_vlan_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char linker[8];
	char *name;
	dmasprintf(&name,"%s%cBridging%cBridge%c%s%c", dmroot, dm_delim, dm_delim, dm_delim, ((struct bridging_vlan_args *)data)->br_inst, dm_delim);
	sprintf(linker,"vlan%s_%s", ((struct bridging_vlan_args *)data)->vlan_port, ((struct bridging_vlan_args *)data)->br_inst);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cBridging%cBridge%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int get_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "br_port_linker", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cBridging%cBridge%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker = NULL, *vid, *enable, *vifname, tmp[8], *pch, *p, *br_ifname;
	char new_ifname[16];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (!linker) {
				dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "br_port_linker", "");
				set_br_vlan_enable(refparam, ctx, data, instance, "false", action);
				return 0;
			}
			dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "br_port_linker", linker);
			dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "vlan8021q", &vid);
			pch = strchr(linker, '+') + 1;
			if (pch[0] == '\0') {
				dmfree(linker);
				return 0;
			}
			if (vid[0] == '\0') {
				if (strstr(pch, "atm") || strstr(pch, "ptm") || strstr(pch, wan_baseifname)) {
					strncpy(tmp, pch, 4);
					tmp[4] ='\0';
					dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "ifname", tmp);
				}
			} else {
				if (strstr(pch, "atm") || strstr(pch, "ptm") || strstr(pch, wan_baseifname)) {
					p = new_ifname;
					strncpy(tmp, pch, 4);
					tmp[4] ='\0';
					dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "ifname", tmp);
					dmstrappendstr(p,  tmp);
					dmstrappendchr(p, '.');
					dmstrappendstr(p, vid);
					dmstrappendend(p);
					dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "ifname", new_ifname);
					dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_vlan_sec, "penable", &enable);///TO CHECK
					// add to bridge ifname if enable = 1
					if (enable[0] == '1') {
						vifname = dmstrdup(new_ifname);
						dmuci_get_value_by_section_string(((struct bridging_vlan_args *)data)->bridge_sec, "ifname", &br_ifname);
						p = new_ifname;
						if (br_ifname[0] != '\0') {
							dmstrappendstr(p, br_ifname);
							dmstrappendchr(p, ' ');
						}
						dmstrappendstr(p, vifname);
						dmstrappendend(p);
						dmuci_set_value_by_section(((struct bridging_vlan_args *)data)->bridge_sec, "ifname", new_ifname);
						dmfree(vifname);
					}
				}
			}
			dmfree(linker);
			return 0;
	}
	return 0;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseBridgeInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *br_s = NULL;
	char *br_inst = NULL, *br_inst_last = NULL, *ifname;
	struct bridging_args curr_bridging_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_baseifname);

	synchronize_specific_config_sections_with_dmmap_eq("network", "interface", "dmmap_network", "type", "bridge", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		br_inst = handle_update_instance(1, dmctx, &br_inst_last, update_instance_alias, 3, p->dmmap_section, "bridge_instance", "bridge_alias");
		dmuci_get_value_by_section_string(p->config_section, "ifname", &ifname);
		init_bridging_args(&curr_bridging_args, p->config_section, br_inst_last, ifname, br_inst);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_bridging_args, br_inst) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

void set_bridge_port_parameters(struct uci_section *dmmap_section, char* bridge_key){
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_section, "bridge_key", bridge_key);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_section, "mg_port", "false");
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_section, "penable", "1");
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_section, "is_dmmap", "false");
}

int browseBridgePortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance){
	struct uci_section *new_port = NULL, *ss_atm = NULL, *ss_ptm = NULL;
	char *port = NULL, *port_last = NULL;
	char *ifname_dup = NULL, *pch, *spch, *type, *is_dmmap, *file_config_name, *deviceatm, *deviceptm, *atm_device, *ptm_device;
	bool find_max = true, found = false;
	struct bridging_port_args curr_bridging_port_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	dmasprintf(&file_config_name, "%s","/etc/config/dsl");
	check_create_dmmap_package("dmmap_bridge_port");
	update_section_list_icwmpd("dmmap_bridge_port","bridge_port", "bridge_key", 1, ((struct bridging_args *)prev_data)->br_key, "mg_port", "true", "bridge_port_instance", "1");
	uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key",  ((struct bridging_args *)prev_data)->br_key, new_port) {
		dmuci_get_value_by_section_string(new_port, "is_dmmap", &is_dmmap);
		if(strcmp(is_dmmap, "false")!=0) {
			init_bridging_port_args(&curr_bridging_port_args, new_port, ((struct bridging_args *)prev_data)->bridge_sec, false, "");
			port = handle_update_instance(2, dmctx, &port_last, update_instance_alias_icwmpd, 5, new_port, "bridge_port_instance", "bridge_port_alias",  &find_max, ((struct bridging_args *)prev_data)->br_key);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_bridging_port_args, port) == DM_STOP)
				goto end;
		}
	}

	if (((struct bridging_args *)prev_data)->ifname[0] == '\0')
		return 0;
	ifname_dup = dmstrdup(((struct bridging_args *)prev_data)->ifname);
	for (pch = strtok_r(ifname_dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
		found = false;

		if(!found)
			found= synchronize_multi_config_sections_with_dmmap_eq("ports", "ethport", "dmmap_bridge_port", "bridge_port", "ifname", pch, pch, &dup_list);

		if(!found)
			found= synchronize_multi_config_sections_with_dmmap_eq("wireless", "wifi-iface", "dmmap_bridge_port", "bridge_port", "ifname", pch, pch, &dup_list);

		if(access( file_config_name, F_OK ) != -1){
			uci_foreach_sections("dsl", "atm-device", ss_atm) {
				dmuci_get_value_by_section_string(ss_atm, "device", &deviceatm);
				dmasprintf(&atm_device, "%s.1", deviceatm);
				if(!found) {
					if(strncmp(pch, atm_device, strlen(atm_device))==0) {
						found= synchronize_multi_config_sections_with_dmmap_eq("network", "device", "dmmap_bridge_port", "bridge_port", "name", pch, pch, &dup_list);
					}
				}
			}

			uci_foreach_sections("dsl", "ptm-device", ss_ptm) {
				dmuci_get_value_by_section_string(ss_ptm, "device", &deviceptm);
				dmasprintf(&ptm_device, "%s.1", deviceptm);
				if(!found) {
					if(strncmp(pch, ptm_device, strlen(ptm_device))==0) {
						found= synchronize_multi_config_sections_with_dmmap_eq("network", "device", "dmmap_bridge_port", "bridge_port", "name", pch, pch, &dup_list);
					}
				}
			}
		}

		if(!found){
			if(strncmp(pch, wan_baseifname, strlen(wan_baseifname))==0) {
				found= synchronize_multi_config_sections_with_dmmap_eq("network", "device", "dmmap_bridge_port", "bridge_port", "name", pch, pch, &dup_list);
			}
		}

		if(!found){
			if(strncmp(pch, wan_baseifname, 4) == 0 || strncmp(pch, "ptm", 3) == 0 || strncmp(pch, "atm", 3) == 0){
				found= synchronize_multi_config_sections_with_dmmap_eq_diff("network", "device", "dmmap_bridge_port", "bridge_port", "name", pch, "type", "untagged", pch, &dup_list);
			}
		}
	}

	list_for_each_entry(p, &dup_list, list) {
		set_bridge_port_parameters(p->dmmap_section, ((struct bridging_args *)prev_data)->br_key);
		init_bridging_port_args(&curr_bridging_port_args, p->config_section, ((struct bridging_args *)prev_data)->bridge_sec, false, (char*)p->additional_attribute);
		port = handle_update_instance(2, dmctx, &port_last, update_instance_alias_icwmpd, 5, p->dmmap_section, "bridge_port_instance", "bridge_port_alias", &find_max, ((struct bridging_args *)prev_data)->br_key);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_bridging_port_args, port) == DM_STOP)
			goto end;
	}
	end:
		dmfree(ifname_dup);
		return 0;
}

int browseBridgeVlanInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *vlan = NULL, *vlan_last = NULL, *type, *is_lan= NULL;
	struct bridging_vlan_args curr_bridging_vlan_args = {0};
	struct bridging_args *br_args = (struct bridging_args *)prev_data;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	dmuci_get_value_by_section_string(br_args->bridge_sec, "is_lan", &is_lan);
	if(is_lan==NULL || strcmp(is_lan, "1")!=0){
		synchronize_specific_config_sections_with_dmmap("network", "device", "dmmap_network", &dup_list);
		list_for_each_entry(p, &dup_list, list) {
			if(!p->config_section)
				goto end;
			//Check if VLAN or NOT
			dmuci_get_value_by_section_string(p->config_section, "type", &type);
			if (strcmp(type, "untagged")!=0) {
				dmuci_set_value_by_section(p->dmmap_section, "bridge_key", br_args->br_key);
				vlan =  handle_update_instance(2, dmctx, &vlan_last, update_instance_alias, 3, p->dmmap_section, "bridge_vlan_instance", "bridge_vlan_alias");
				init_bridging_vlan_args(&curr_bridging_vlan_args, p->config_section, br_args->bridge_sec, vlan_last, br_args->br_key);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_bridging_vlan_args, vlan) == DM_STOP)
					goto end;
			}
		}
		free_dmmap_config_dup_list(&dup_list);
	}
end:
	return 0;
}

int browseBridgeVlanPortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *vlan = NULL, *vlan_last = NULL, *type, *is_lan= NULL;
	struct bridging_vlan_args curr_bridging_vlan_args = {0};
	struct bridging_args *br_args = (struct bridging_args *)prev_data;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	dmuci_get_value_by_section_string(br_args->bridge_sec, "is_lan", &is_lan);
	if(is_lan==NULL || strcmp(is_lan, "1")!=0){
		synchronize_specific_config_sections_with_dmmap("network", "device", "dmmap_network", &dup_list);
		list_for_each_entry(p, &dup_list, list) {
			if(!p->config_section)
				goto end;
			//Check if VLAN or NOT
			dmuci_get_value_by_section_string(p->config_section, "type", &type);
			if (strcmp(type, "untagged")!=0) {
				dmuci_set_value_by_section(p->dmmap_section, "bridge_key", br_args->br_key);
				vlan =  handle_update_instance(2, dmctx, &vlan_last, update_instance_alias, 3, p->dmmap_section, "bridge_vlan_instance", "bridge_vlan_alias");
				init_bridging_vlan_args(&curr_bridging_vlan_args, p->config_section, br_args->bridge_sec, vlan_last, br_args->br_key);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_bridging_vlan_args, vlan) == DM_STOP)
					goto end;
			}
		}
		free_dmmap_config_dup_list(&dup_list);
	}
end:
	return 0;
}
