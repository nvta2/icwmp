/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2015 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#include <libubox/blobmsg_json.h>
#include <json-c/json.h>

#include "dmcwmp.h"
#include "dmuci.h"
#include "dmmem.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmjson.h"
#include "layer_2_bridging.h"

inline int entry_layer2_availableinterface_instance(struct dmctx *ctx, char *int_instance);
inline int entry_layer2_marking_instance(struct dmctx *ctx, char *marking_br_instance);
inline int entry_layer2_bridge_instance(struct dmctx *ctx, char *bridge_instance, char *bridge_instance_last);
inline int entry_layer2_bridge_vlan(struct dmctx *ctx, char *bridge_instance, char *bridge_instance_last);
inline int entry_layer2_bridge_vlan_instance(struct dmctx *ctx,char *bridge_instance, char *vlan_instance);
int set_marking_bridge_key_sub(char *refparam, struct dmctx *ctx, char *value);
int set_marking_interface_key_sub(char *refparam, struct dmctx *ctx, char *value);
int set_bridge_vlan_enable_sub(char *refparam, struct dmctx *ctx, bool b);
int set_bridge_vlan_vid_sub(struct uci_section *vb, char *value);
int set_bridge_vlanid_sub(char *refparam, struct dmctx *ctx, char *value);
int set_bridge_vlan_priority_sub(struct uci_section *vb, char *value);
void remove_config_interfaces(char *baseifname, char *bridge_key, struct uci_section *bridge_s, char *mbi);

struct wan_interface 
{
	char *instance;
	char *name;
	char *package;
	char *section;		
};

struct args_layer2
{
	struct uci_section *layer2section;
	struct uci_section *layer2sectionlev2;
	char *bridge_instance;
	char *availableinterface_instance;
	char *interface_type;
	char *oface;
};
const char *vlan_ifname[3] = {"eth","atm", "ptm"};

#ifndef EX400
struct wan_interface wan_interface_tab[3] = {
{"1", "ethernet", "layer2_interface_ethernet", "ethernet_interface"},
{"2", "adsl", "layer2_interface_adsl", "atm_bridge"},
{"3", "vdsl", "layer2_interface_vdsl", "vdsl_interface"}
};
#endif

struct args_layer2 cur_args = {0};
char *wan_baseifname = NULL;

inline void init_args_layer2(struct dmctx *ctx, struct uci_section *s, struct uci_section *ss,
							char *availableinterface_instance, char *bridge_instance,
							char *interface_type, char *oface)
{
	struct args_layer2 *args = &cur_args;
	ctx->args = (void *)args;
	args->layer2section = s;
	args->layer2sectionlev2 = ss;
	args->interface_type = interface_type;
	args->oface = oface;
	args->availableinterface_instance = availableinterface_instance;
	args->bridge_instance = bridge_instance;
}

inline void init_args_layer2_vlan(struct dmctx *ctx, struct uci_section *ss)
{
	struct args_layer2 *args = &cur_args;
	ctx->args = (void *)args;
	args->layer2sectionlev2 = ss;
}

char *layer2_get_last_section_instance(char *package, char *section, char *opt_inst)
{
	struct uci_section *s, *last = NULL;
	char *inst = NULL;

	uci_path_foreach_sections(icwmpd, package, section, s)
	{
		last = s;
	}
	if (last) {
		dmuci_get_value_by_section_string(last, opt_inst, &inst);
		inst = update_instance_icwmpd(last, inst, opt_inst);
	}
	return inst;
}

int update_bridge_vlan_config(char *vid, char *vlan_name, char *priority ,char *bridge_key)
{
	struct uci_section *s, *ss;
	char *add_value, *instance, *p;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", bridge_key, s)
	{
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "name", vlan_name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "vid", vid);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "priority", priority);
		return 0;
	}

	instance = get_last_instance_lev2(DMMAP, "vlan_bridge", "vlan_instance", "bridgekey", bridge_key);
	DMUCI_ADD_SECTION(icwmpd, "dmmap", "vlan_bridge", &ss, &add_value);
	instance = update_instance_icwmpd(ss, instance, "vlan_instance");
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "bridgekey", bridge_key);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "name", vlan_name);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "vid", vid);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "priority", priority);
	return 0;
}

int update_bridge_all_vlan_config_bybridge(struct dmctx *ctx)
{
	char *ifname, *pch, *spch, *vid, *priority, *vname;
	struct uci_section *s, *ss;

	dmuci_get_value_by_section_string(cur_args.layer2section, "ifname", &ifname);
	ifname = dmstrdup(ifname);
	for (pch = strtok_r(ifname, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
		if (strncmp(pch, wan_baseifname, 4) == 0 || strncmp(pch, "ptm", 3) == 0 || strncmp(pch, "atm", 3) == 0) {
			uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", pch, s) {
				dmuci_get_value_by_section_string(s, "name", &vname);
				dmuci_get_value_by_section_string(s, "vlan8021q", &vid);
				dmuci_get_value_by_section_string(s, "vlan8021p", &priority);
				update_bridge_vlan_config(vid, vname, priority, cur_args.bridge_instance);
				break;
			}
		}
	}
	dmfree(ifname);
	return 0;
}

void update_add_vlan_interfaces(char *bridge_key, char *vid)
{
	char *baseifname, *add_value;
	char baseifname_dup[16];
	char *p;
	bool found;
	struct uci_section *s, *vlan_interface_s, *vi_sec;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "bridgekey", bridge_key, s) {
		dmuci_get_value_by_section_string(s, "baseifname", &baseifname);
		p = baseifname_dup;
		dmstrappendstr(p, baseifname);
		dmstrappendchr(p, '.');
		dmstrappendstr(p, vid);
		dmstrappendend(p);
		found = false;
		uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", baseifname_dup, vi_sec) {
			found = true;
			break;
		}
		if(found)
			continue;

		if(strncmp(baseifname, wan_baseifname, 4) == 0 ||
			strncmp(baseifname, "ptm", 3) == 0 ||
			strncmp(baseifname, "atm", 3) == 0) {
			dmuci_add_section("layer2_interface_vlan", "vlan_interface", &vlan_interface_s, &add_value);
			dmuci_set_value_by_section(vlan_interface_s, "baseifname", baseifname);
			dmuci_set_value_by_section(vlan_interface_s, "bridge", bridge_key);
			dmuci_set_value_by_section(vlan_interface_s, "ifname", baseifname_dup);
			dmuci_set_value_by_section(vlan_interface_s, "name", baseifname_dup);
			dmuci_set_value_by_section(vlan_interface_s, "vlan8021q", vid);
		}
	}
}

void update_add_vlan_to_bridge_interface(char *bridge_key, struct uci_section *dmmap_s)
{
	char *vid, *ifname, *baseifname;
	struct uci_section *interface_s, *marking_bridge_s;
	char baseifname_dup[16];
	char *p;
	char ifname_dup[128];
	char *ptr;

	dmuci_get_value_by_section_string(dmmap_s, "vid", &vid);
	if(vid[0] == '\0')
		return ;

	uci_foreach_option_eq("network", "interface", "bridge_instance", bridge_key, interface_s)
	{
		dmuci_get_value_by_section_string(interface_s, "ifname", &ifname);
		ifname_dup[0] = '\0';
		ptr = ifname_dup;
		dmstrappendstr(ptr, ifname);
		dmstrappendend(ptr);
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "bridgekey", bridge_key, marking_bridge_s)
		{
			dmuci_get_value_by_section_string(marking_bridge_s, "baseifname", &baseifname);
			if (strncmp(baseifname, wan_baseifname, 4) == 0
				|| strncmp(baseifname, "ptm", 3) == 0
				|| strncmp(baseifname, "atm", 3) == 0) {
				p = baseifname_dup;
				dmstrappendstr(p, baseifname);
				dmstrappendchr(p, '.');
				dmstrappendstr(p, vid);
				dmstrappendend(p);
				if (is_strword_in_optionvalue(ifname_dup, baseifname_dup)) continue;
				if (ifname_dup[0] != '\0')
					dmstrappendchr(ptr, ' ');
				dmstrappendstr(ptr, baseifname_dup);
				dmstrappendend(ptr);
			}
		}
		dmuci_set_value_by_section(interface_s, "ifname", ifname_dup);
	}
}

int get_marking_bridge_reference(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2section, "bridgekey", value);
	return 0;
}

void get_baseifname_from_ifname(char *ifname, char *baseifname)
{
	while (*ifname != '.' && *ifname != '\0') {
		*baseifname++ = *ifname++;
	}
	*baseifname = '\0';
}

void update_markinginterface_list(struct uci_section *interface_section, char *bridge_key)
{
	char *ifname, *dupifname, *ifname_element, *bridgekey, *instance;
	char *add_value, *interfacekey="", *spch;
	struct uci_section *marking_section, *new_marking_section, *wireless_section;
	struct uci_section *ciface;
	bool found;
	char baseifname[8];

	dmuci_get_value_by_section_string(interface_section, "ifname", &ifname);
	dupifname = dmstrdup(ifname);
	ifname_element = strtok_r(dupifname, " ", &spch);
	while (ifname_element != NULL) {
		get_baseifname_from_ifname(ifname_element, baseifname);
		found = false;
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", baseifname, marking_section)
		{
			dmuci_get_value_by_section_string(marking_section, "bridgekey", &bridgekey);
			if (strcmp(bridgekey, bridge_key) == 0) {
				found = true;
				break;
			}
		}
		if (found){
			goto nextifname;
		}
		uci_path_foreach_option_eq(icwmpd, "dmmap", "available-bridge", "baseifname", baseifname, ciface)
		{
			dmuci_get_value_by_section_string(ciface, "key", &interfacekey);
			break;
		}
		if (!ciface) goto nextifname;
		instance = layer2_get_last_section_instance("dmmap", "marking-bridge", "marking_instance");
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "marking-bridge", &new_marking_section, &add_value);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "baseifname", baseifname);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "bridgekey", bridge_key);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "interfacekey", interfacekey);
		update_instance_icwmpd(new_marking_section, instance, "marking_instance");

nextifname:
		ifname_element = strtok_r(NULL, " ", &spch);
	}
	dmfree(dupifname);
	new_marking_section = NULL;
	uci_foreach_option_eq("wireless", "wifi-iface", "network",  section_name(interface_section), wireless_section) {
		found = false;
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", section_name(wireless_section), marking_section)
		{
			dmuci_get_value_by_section_string(marking_section, "bridgekey", &bridgekey);
			if (strcmp(bridgekey, bridge_key) == 0) {
				found = true;
				break;
			}
		}
		if (found) continue;
		uci_path_foreach_option_eq(icwmpd, "dmmap", "available-bridge", "baseifname", section_name(wireless_section), ciface)
		{
			dmuci_get_value_by_section_string(ciface, "key", &interfacekey);
			break;
		}
		if (!ciface) continue;
		instance = layer2_get_last_section_instance("dmmap", "marking-bridge", "marking_instance");
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "marking-bridge", &new_marking_section, &add_value);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "baseifname", section_name(wireless_section));
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "bridgekey", bridge_key);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, new_marking_section, "interfacekey", interfacekey);
		update_instance_icwmpd(new_marking_section, instance, "marking_instance");
	}
}

//set_marking_bridgekey
int set_marking_bridge_key(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
			set_marking_bridge_key_sub(refparam, ctx, value);
			return 0;
	}
	return 0;
}

int set_marking_bridge_key_sub(char *refparam, struct dmctx *ctx, char *value)
{
	char *old_bridge_key, *baseifname, *ifname, *vid, *enable, *bridgekey, *p;
	char new_ifname[128];
	struct uci_section *s, *ss;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	char iface[16];
	bool found;

	dmuci_get_value_by_section_string(args->layer2section, "bridgekey", &old_bridge_key);
	dmuci_set_value_by_section(args->layer2section, "bridgekey", value);
	dmuci_get_value_by_section_string(args->layer2section, "baseifname", &baseifname);

	if (strncmp(baseifname, "cfg", 3) == 0) {
		uci_foreach_option_eq("network", "interface", "bridge_instance", value, s)
		{
			dmuci_set_value("wireless", baseifname, "network", section_name(s));
			break;
		}
		if (!s) return 0;
	}
	else if (strncmp(baseifname, wan_baseifname, 4) == 0
			|| strncmp(baseifname, "ptm", 3) == 0
			|| strncmp(baseifname, "atm", 3) == 0) {
		found = false;
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", baseifname, s)
		{
			dmuci_get_value_by_section_string(s, "bridgekey", &bridgekey);
			if (strcmp(bridgekey, old_bridge_key) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", old_bridge_key, s)
			{
				dmuci_get_value_by_section_string(s, "vid", &vid);
				if (vid[0] == '\0') continue;
				uci_foreach_option_eq("network", "interface", "bridge_instance", old_bridge_key, ss)
				{
					break;
				}
				if (!ss) return 0;
				dmuci_get_value_by_section_string(ss, "ifname", &ifname);
				p = iface;
				dmstrappendstr(p, baseifname);
				dmstrappendchr(p, '.');
				dmstrappendstr(p, vid);
				dmstrappendend(p);
				remove_interface_from_ifname(iface, ifname, new_ifname);
				dmuci_set_value_by_section(ss, "ifname", new_ifname);
			}
		}
		uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", value, s)
		{
			dmuci_get_value_by_section_string(s, "vid", &vid);
			if (vid[0] == '\0') continue;
			dmuci_get_value_by_section_string(s, "enable", &enable);
			if (enable[0] == '\0' || enable[0] == '0') continue;
			update_remove_vlan_from_bridge_interface(value, s);
			update_add_vlan_interfaces(value, vid);
			update_add_vlan_to_bridge_interface(value, s);
		}
	}
	else if (strncmp(baseifname, "eth", 3) == 0) {
		uci_foreach_option_eq("network", "interface", "bridge_instance", old_bridge_key, s)
		{
			dmuci_get_value_by_section_string(s, "ifname", &ifname);
			if (strlen(ifname) > 5) {
				remove_interface_from_ifname(baseifname, ifname, new_ifname);
			}
			else {
			remove_interface_from_ifname(baseifname, ifname, new_ifname);
			}
			dmuci_set_value_by_section(s, "ifname",  new_ifname);
		}
		uci_foreach_option_eq("network", "interface", "bridge_instance", value, ss)
		{
			dmuci_get_value_by_section_string(ss, "ifname", &ifname);
			p = new_ifname;
			if (ifname[0] != '\0') {
				dmstrappendstr(p, ifname);
				dmstrappendchr(p, ' ');
			}
			dmstrappendstr(p, baseifname);
			dmstrappendend(p);
			dmuci_set_value_by_section(ss, "ifname",  new_ifname);
		}

	}
	return 0;
}

int get_marking_interface_key(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2section, "interfacekey", value);
	return 0;
}

//set_marking_interfacekey
int set_marking_interface_key(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
		set_marking_interface_key_sub(refparam, ctx, value);
			return 0;
	}
	return 0;
}

int set_marking_interface_key_sub(char *refparam, struct dmctx *ctx, char *value)
{
	char *vid, *ifname, *baseifname, *bkey, *p, *enable;
	char new_ifname[128];
	char *obifname, *instance;
	struct uci_section *s = NULL, *ss = NULL, *ab, *mb, *sbridge = NULL;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	mb = args->layer2section;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "available-bridge", "key", value, ab) {
		break;
	}
	if (!ab) return 0;

	dmuci_get_value_by_section_string(mb, "bridgekey", &bkey);
	if (bkey[0] != '\0') {
		uci_foreach_option_eq("network", "interface", "bridge_instance", bkey, sbridge) {
			break;
		}
		if (sbridge) {
			dmuci_get_value_by_section_string(mb, "baseifname", &obifname);
			dmuci_get_value_by_section_string(mb, "marking_instance", &instance);
			if (obifname[0] != '\0') {
				remove_config_interfaces(obifname, bkey, sbridge, instance);
			}
		}
	}

	dmuci_get_value_by_section_string(ab, "baseifname", &baseifname);
	if (strncmp(baseifname, "cfg", 3) == 0) {
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", baseifname, s)
		{
			break;
		}
		if (s) return 0;
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "interfacekey",  value);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "baseifname",  baseifname);
		if (bkey[0] == '\0' || !sbridge) return 0;
		dmuci_set_value("wireless", baseifname, "network",  section_name(sbridge));
	}
	else if (strncmp(baseifname, wan_baseifname, 4) == 0
			|| strncmp(baseifname, "ptm", 3) == 0
			|| strncmp(baseifname, "atm", 3) == 0) {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "interfacekey",  value);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "baseifname",  baseifname);
		if (bkey[0] == '\0') return 0;
		uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", bkey, s)
		{
			dmuci_get_value_by_section_string(s, "vid", &vid);
			if (vid[0] == '\0') continue;
			dmuci_get_value_by_section_string(s, "enable", &enable);
			if (enable[0] == '\0' || enable[0] == '0') continue;
			update_remove_vlan_from_bridge_interface(bkey, s);
			update_add_vlan_interfaces(bkey, vid);
			update_add_vlan_to_bridge_interface(bkey, s);
		}
	}
	else if (strncmp(baseifname, "eth", 3) == 0) {
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", baseifname, s)
		{
			break;
		}
		if (s) return 0;
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "interfacekey",  value);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, mb, "baseifname",  baseifname);
		if (bkey[0] == '\0' || !sbridge) return 0;
		dmuci_get_value_by_section_string(sbridge, "ifname", &ifname);
		p = new_ifname;
		if (ifname[0] != '\0') {
			dmstrappendstr(p, ifname);
			dmstrappendchr(p, ' ');
		}
		dmstrappendstr(p, baseifname);
		dmstrappendend(p);
		dmuci_set_value_by_section(sbridge, "ifname",  new_ifname);
	}
	return 0;
}

int get_bridge_vlan_enable(char *refparam,struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2sectionlev2, "enable", value);
	return 0;
}

//set_bridgevlan_enable
int set_bridge_vlan_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	int error = string_to_bool(value, &b);
	switch (action) {
		case VALUECHECK:
			if (error)
				return FAULT_9007;
			return 0;
		case VALUESET:
			set_bridge_vlan_enable_sub(refparam, ctx, b);
			return 0;
	}
	return 0;
}

int set_bridge_vlan_enable_sub(char *refparam, struct dmctx *ctx, bool b)
{
	char *value, *vid, *bkey, *cval;
	struct uci_section *vb;
	bool bcval;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;

	vb = args->layer2sectionlev2;
	dmuci_get_value_by_section_string(vb, "vid", &vid);
	dmuci_get_value_by_section_string(vb, "bridgekey", &bkey);
	dmuci_get_value_by_section_string(vb, "enable", &cval);
	string_to_bool(cval, &bcval);
	if (b && !bcval) {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "enable", "1");
		if (vid[0] == '\0') return 0;
		update_add_vlan_interfaces(bkey, vid);
		update_add_vlan_to_bridge_interface(bkey, vb);
	}
	else if (!b && bcval) {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "enable", "0");
		if (vid[0] == '\0') return 0;
		update_remove_vlan_from_bridge_interface(bkey, vb);
	}
	return 0;
}

int get_bridge_vlan_name(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2sectionlev2, "name", value);
	return 0;
}

int set_bridge_vlan_name(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct args_layer2 *args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, args->layer2sectionlev2, "name", value);
			return 0;
	}
	return 0;
}

int get_bridge_vlan_vid(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2sectionlev2, "vid", value);
	return 0;
}

//set_bridgevlan_vid
int set_bridge_vlan_vid(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *vb;
	struct args_layer2 *args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			vb = args->layer2sectionlev2;
			set_bridge_vlan_vid_sub(vb, value);
			return 0;
	}
	return 0;
}

int get_bridge_vlan_priority(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2sectionlev2, "priority", value);
	return 0;
}

//set_bridgevlan_vid
int set_bridge_vlan_priority(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct uci_section *vb;
	struct args_layer2 *args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			vb = args->layer2sectionlev2;
			set_bridge_vlan_priority_sub(vb, value);
			return 0;
	}
	return 0;
}

int set_bridge_vlan_vid_sub(struct uci_section *vb, char *value)
{
	char *enable, *bkey, *cval;
	dmuci_get_value_by_section_string(vb, "vid", &cval);
	if (strcmp(cval, value) == 0) return 0;
	dmuci_get_value_by_section_string(vb, "enable", &enable);
	if (enable[0] == '1') {
		dmuci_get_value_by_section_string(vb, "bridgekey", &bkey);
		update_remove_vlan_from_bridge_interface(bkey, vb);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "vid", value);
		update_add_vlan_interfaces(bkey, value);
		update_add_vlan_to_bridge_interface(bkey, vb);
	}
	else {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "vid", value);
	}

	return 0;
}

int set_bridge_vlan_priority_sub(struct uci_section *vb, char *value)
{
	char *enable, *cval, *vname;
	struct uci_section *vlan_s = NULL;
	dmuci_get_value_by_section_string(vb, "priority", &cval);
	dmuci_get_value_by_section_string(vb, "name", &vname);
	if (strcmp(cval, value) == 0) 
		return 0;
	uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "name", vname, vlan_s) {
		dmuci_set_value_by_section(vlan_s, "vlan8021p", value);
		break;
	}
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "priority", value);
	return 0;
}

int get_bridge_status(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(args->layer2section), String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 1, "up");
	return 0;
}

int set_bridge_status(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;

	int error = string_to_bool(value, &b);
	switch (action) {
		case VALUECHECK:
			if (error)
				return FAULT_9007;
			return 0;
		case VALUESET:
			if (b) {
				dmubus_call_set("network.interface", "up", UBUS_ARGS{{"interface", section_name(args->layer2section), String}}, 1);
			}
			else {
				dmubus_call_set("network.interface", "down", UBUS_ARGS{{"interface", section_name(args->layer2section), String}}, 1);
			}
			return 0;
	}
	return 0;
}

int get_bridge_key(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = args->bridge_instance;
	return 0;
}

int get_bridge_name(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = dmstrdup(section_name(args->layer2section));
	return 0;
}

int set_bridge_name(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct args_layer2 *args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			dmuci_rename_section_by_section(args->layer2section, value);
			return 0;
	}
	return 0;
}

int get_bridge_vlanid(char *refparam, struct dmctx *ctx, char **value)
{
	char *bridge_i;
	struct uci_section *s = NULL, *vlan_s = NULL;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = "";

	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", args->bridge_instance, s)
	{
		dmuci_get_value_by_section_string(s, "vid", value);
		break;
	}
	return 0;
}

int set_bridge_vlanid(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	struct uci_section *vb;

	switch(action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			vb = args->layer2section;
			set_bridge_vlanid_sub(refparam, ctx, value);
			return 0;
	}
	return 0;
}

int set_bridge_vlanid_sub(char *refparam, struct dmctx *ctx, char *value)
{
	char *add_value, *name, *instance;
	struct uci_section *s = NULL, *vb;;
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", args->bridge_instance, s)
	{
		break;
	}
	if (s == NULL) {
		instance = get_last_instance_lev2(DMMAP, "vlan_bridge", "vlan_instance", "bridgekey", args->bridge_instance);
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "vlan_bridge", &vb, &add_value);
		instance = update_instance_icwmpd(vb, instance, "vlan_instance");
		dmasprintf(&name, "vlan_%s.%s", args->bridge_instance, instance);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "bridgekey", args->bridge_instance);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "name", name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "enable", "0");
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, vb, "vid", value);
		dmfree(name);
	}
	else {
		set_bridge_vlan_vid_sub(s, value);
	}
	return 0;
}

int get_associated_interfaces(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	dmuci_get_value_by_section_string(args->layer2section, "ifname", value);
	return 0;
}

int set_associated_interfaces(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct args_layer2 *args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct args_layer2 *)ctx->args;
			dmuci_set_value_by_section(args->layer2section, "ifname", value);
			return 0;
	}
	return 0;
}

int get_available_interface_key(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = args->availableinterface_instance;
	return 0;
}

int get_interface_reference(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = args->oface;
	return 0;
}

int get_interfaces_type(char *refparam, struct dmctx *ctx, char **value)
{
	struct args_layer2 *args = (struct args_layer2 *)ctx->args;
	*value = args->interface_type;
	return 0;
}

struct uci_section *update_availableinterface_list(struct dmctx *ctx, char *iface, char **instance, char **instance_last)
{
	struct uci_section *s = NULL;
	struct uci_section *available_bridge = NULL;
	char *add_value;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "available-bridge", "baseifname", iface, s)
	{
		*instance =  handle_update_instance(1, ctx, instance_last, update_instance_alias_icwmpd, 3, s, "key", "avbralias");
		return s;
	}
	DMUCI_ADD_SECTION(icwmpd, "dmmap", "available-bridge", &available_bridge, &add_value);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, available_bridge, "baseifname", iface);
	*instance =  handle_update_instance(1, ctx, instance_last, update_instance_alias_icwmpd, 3, available_bridge, "key", "avbralias");
	return available_bridge;
}

/*************************************************************
 * ADD DELETE OBJECT
/*************************************************************/

int add_layer2bridging_bridge(struct dmctx *ctx, char **instance)
{
	char *last_instance;
	char bridge_name[16], ib[8];
	char *p = bridge_name;

	last_instance = get_last_instance_lev2("network", "interface", "bridge_instance", "type", "bridge");
	sprintf(ib, "%d", last_instance ? atoi(last_instance)+1 : 1);
	dmstrappendstr(p, "bridge_0_");
	dmstrappendstr(p, ib);
	dmstrappendend(p);
	dmuci_set_value("network", bridge_name, "", "interface");
	dmuci_set_value("network", bridge_name, "type", "bridge");
	dmuci_set_value("network", bridge_name, "proto", "dhcp");
	*instance = dmuci_set_value("network", bridge_name, "bridge_instance", ib);
	return 0;
}

int delete_layer2bridging_bridge(struct dmctx *ctx)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	char *bridge_instance;
	struct uci_section *vlan_s = NULL, *prev_s = NULL;
	dmuci_get_value_by_section_string(args_bridge->layer2section, "bridge_instance", &bridge_instance);
	dmuci_set_value_by_section(args_bridge->layer2section, "type", "");
	dmuci_set_value_by_section(args_bridge->layer2section, "bridge_instance", "");
	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", bridge_instance, vlan_s) {
		if (prev_s)
			DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
		prev_s = vlan_s;
	}
	if (prev_s)
		DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
	return 0;
}

int delete_layer2bridging_bridge_all(struct dmctx *ctx)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	struct uci_section *bridge_s, *vlan_s, *prev_s = NULL;
	char *bridgekey = NULL;

	uci_foreach_option_eq("network", "interface", "type", "bridge", bridge_s) {
		dmuci_set_value_by_section(bridge_s, "type", "");
		dmuci_set_value_by_section(bridge_s, "bridge_instance", "");
	}
	uci_path_foreach_sections(icwmpd, "dmmap", "vlan_bridge", vlan_s) {
		if(prev_s != NULL && bridgekey[0] != '\0')
			DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
		prev_s = vlan_s;
		dmuci_get_value_by_section_string(vlan_s, "bridgekey", &bridgekey);
	}
	if(prev_s != NULL && bridgekey[0] != '\0')
		DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
	return 0;
}

int add_layer2bridging_marking(struct dmctx *ctx, char **instance)
{
	char *last_instance;
	char *value;
	struct uci_section *marking_s;

	last_instance = get_last_instance(DMMAP, "marking-bridge", "marking_instance");
	DMUCI_ADD_SECTION(icwmpd, "dmmap", "marking-bridge", &marking_s, &value);
	*instance = update_instance_icwmpd(marking_s, last_instance, "marking_instance");
	return 0;
}

void remove_config_interfaces(char *baseifname, char *bridge_key, struct uci_section *bridge_s, char *mbi)
{
	struct uci_section *marking_s, *vlan_s;
	char *b_key, *m_instance, *vid, *ifname;
	bool found = false;
	char new_ifname[128];
	char *p, iface[16];

	if (strncmp(baseifname, "cfg", 3) == 0) {
		dmuci_delete("wireless", baseifname , "network", NULL);
	}
	else if (strncmp(baseifname, wan_baseifname, 4) == 0
			|| strncmp(baseifname, "ptm", 3) == 0
			|| strncmp(baseifname, "atm", 3) == 0)
	{
		uci_path_foreach_option_eq(icwmpd, "dmmap", "marking-bridge", "baseifname", baseifname, marking_s) {
			dmuci_get_value_by_section_string(marking_s, "bridgekey", &b_key);
			dmuci_get_value_by_section_string(marking_s, "marking_instance", &m_instance);
			if(strcmp(b_key, bridge_key) == 0 && strcmp(m_instance, mbi) != 0) {
				found = true;
				break;
			}
		}
		if (!found)
		{
			uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", bridge_key, vlan_s) {
				dmuci_get_value_by_section_string(vlan_s, "vid", &vid);
				if(vid[0] == '\0')
					continue;
				dmuci_get_value_by_section_string(bridge_s, "ifname", &ifname);
				p = iface;
				dmstrappendstr(p, baseifname);
				dmstrappendchr(p, '.');
				dmstrappendstr(p, vid);
				dmstrappendend(p);
				remove_interface_from_ifname(iface, ifname, new_ifname);
				dmuci_set_value_by_section(bridge_s, "ifname", new_ifname);
			}
		}
	}
	else if (strncmp(baseifname, "eth", 3) == 0) {
		dmuci_get_value_by_section_string(bridge_s, "ifname", &ifname);
		remove_interface_from_ifname(baseifname, ifname, new_ifname);
		dmuci_set_value_by_section(bridge_s, "ifname", new_ifname);
	}
}

int delete_layer2bridging_marking(struct dmctx *ctx)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	char *b_key, *bifname, *m_instance;
	struct uci_section *bridge_s;

	dmuci_get_value_by_section_string(args_bridge->layer2section, "bridgekey", &b_key);
	dmuci_get_value_by_section_string(args_bridge->layer2section, "baseifname", &bifname);
	dmuci_get_value_by_section_string(args_bridge->layer2section, "marking_instance", &m_instance);
	dmuci_delete_by_section(args_bridge->layer2section, NULL, NULL);
	if(b_key[0] == '\0')
		return 0;
	uci_foreach_option_eq("network", "interface", "bridge_instance", b_key, bridge_s) {
		remove_config_interfaces(bifname, b_key, bridge_s, m_instance);
		break;
	}
	return 0;
}

int delete_layer2bridging_marking_all(struct dmctx *ctx)
{
	struct uci_section *mark_s, *prev_s = NULL;
	char *bifname, *b_key, *m_instance;
	struct uci_section *bridge_s;

	uci_path_foreach_sections(icwmpd, "dmmap", "marking-bridge", mark_s) {
		dmuci_get_value_by_section_string(mark_s, "bridgekey", &b_key);
		dmuci_get_value_by_section_string(mark_s, "baseifname", &bifname);
		dmuci_get_value_by_section_string(mark_s, "marking_instance", &m_instance);
		if (prev_s != NULL) {
			DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
		}
		prev_s = mark_s;
		if (b_key[0] == '\0')
			continue;
		uci_foreach_option_eq("network", "interface", "bridge_instance", b_key, bridge_s) {
			remove_config_interfaces(bifname, b_key, bridge_s, m_instance);
			break;
		}
	}
	if(prev_s != NULL)
		DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
	return 0;
}

int add_layer2bridging_bridge_vlan(struct dmctx *ctx, char **instance)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	char *value, *last_instance ;
	struct uci_section *vlan_s;
	char buf[16];
	char *v_name = buf;

	last_instance = get_last_instance_lev2(DMMAP, "vlan_bridge", "vlan_instance", "bridgekey", args_bridge->bridge_instance);
	DMUCI_ADD_SECTION(icwmpd, "dmmap", "vlan_bridge", &vlan_s, &value);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, vlan_s, "bridgekey", args_bridge->bridge_instance);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, vlan_s, "enable", "0");
	*instance = update_instance_icwmpd(vlan_s, last_instance, "vlan_instance");
	dmstrappendstr(v_name, "vlan_");
	dmstrappendstr(v_name, args_bridge->bridge_instance);
	dmstrappendchr(v_name, '.');
	dmstrappendstr(v_name, *instance);
	dmstrappendend(v_name);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, vlan_s, "name", v_name);
	return 0;
}

int delete_layer2bridging_bridge_vlan(struct dmctx *ctx)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	char *vid, *ifname;
	char new_ifname[128];

	dmuci_get_value_by_section_string(args_bridge->layer2sectionlev2, "vid", &vid);
	dmuci_get_value_by_section_string(args_bridge->layer2section, "ifname", &ifname);
	remove_vid_interfaces_from_ifname(vid, ifname, new_ifname);
	dmuci_set_value_by_section(args_bridge->layer2section, "ifname", new_ifname);
	DMUCI_DELETE_BY_SECTION(icwmpd, args_bridge->layer2sectionlev2, NULL, NULL);
	return 0;
}

int delete_layer2bridging_bridge_vlan_all(struct dmctx *ctx)
{
	struct args_layer2 *args_bridge = (struct args_layer2 *)ctx->args;
	char *vid, *ifname;
	struct uci_section *vlan_s, *prev_s = NULL ;
	char new_ifname[128];

	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", args_bridge->bridge_instance, vlan_s) {
		dmuci_get_value_by_section_string(vlan_s, "vid", &vid);
		dmuci_get_value_by_section_string(args_bridge->layer2section, "ifname", &ifname);
		remove_vid_interfaces_from_ifname(vid, ifname, new_ifname);
		dmuci_set_value_by_section(args_bridge->layer2section, "ifname", new_ifname);
		if (prev_s != NULL)
			DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
		prev_s = vlan_s;
	}
	if (prev_s != NULL)
		DMUCI_DELETE_BY_SECTION(icwmpd, prev_s, NULL, NULL);
	return 0;
}

////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_avai_int_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_args.layer2section, "avbralias", value);
	return 0;
}

int set_avai_int_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_args.layer2section, "avbralias", value);
			return 0;
	}
	return 0;
}

int get_marking_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_args.layer2section, "marking_alias", value);
	return 0;
}

int set_marking_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_args.layer2section, "marking_alias", value);
			return 0;
	}
	return 0;
}

int get_bridge_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_args.layer2section, "bridge_alias", value);
	return 0;
}

int set_bridge_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_args.layer2section, "bridge_alias", value);
			return 0;
	}
	return 0;
}
int get_brvlan_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_args.layer2section, "vlan_alias", value);
	return 0;
}

int set_brvlan_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_args.layer2section, "vlan_alias", value);
			return 0;
	}
	return 0;
}

/*************************************************************
 * SUB ENTRIES
/*************************************************************/

inline int entry_layer2_availableinterface(struct dmctx *ctx)
{
	int i = 0;
	char *oface, *phy_interface, *ch_ptr, *saveptr, *waninstance = NULL;
	char *base_ifname, *available_inst = NULL;
	struct uci_section *wifi_s , *wan_s, *ai_s;
	char *instance_last = NULL;
#ifndef EX400
	for (i=0; i<3; i++) {
		uci_foreach_sections(wan_interface_tab[i].package, wan_interface_tab[i].section, wan_s) {
			waninstance = update_instance(wan_s, waninstance, "waninstance");
			dmasprintf(&oface, DMROOT"WANDevice.%s.WANConnectionDevice.%s.", wan_interface_tab[i].instance, waninstance); // MEM WILL BE FREED IN DMMEMCLEAN
			dmuci_get_value_by_section_string(wan_s, "baseifname", &base_ifname);
			ai_s = update_availableinterface_list(ctx, base_ifname, &available_inst, &instance_last);
			init_args_layer2(ctx, ai_s, NULL, instance_last, NULL, "WANInterface", oface);
			SUBENTRY(entry_layer2_availableinterface_instance, ctx, available_inst);
		}
	}
#else
	uci_foreach_sections("ports", "ethport", wan_s) {
		if(!strcmp(wan_s->e.name, "WAN")){
			waninstance = update_instance(wan_s, waninstance, "waninstance");
			dmasprintf(&oface, DMROOT"WANDevice.1.WANConnectionDevice.%s.", waninstance); // MEM WILL BE FREED IN DMMEMCLEAN
			dmuci_get_value_by_section_string(wan_s, "baseifname", &base_ifname);
			ai_s = update_availableinterface_list(ctx, base_ifname, &available_inst, &instance_last);
			init_args_layer2(ctx, ai_s, NULL, instance_last, NULL, "WANInterface", oface);
			SUBENTRY(entry_layer2_availableinterface_instance, ctx, available_inst);
		}
	}
#endif
	db_get_value_string("hw", "board", "ethernetLanPorts", &phy_interface);
	ch_ptr = strtok_r(phy_interface, " ", &saveptr);
	i = 0;
	while (ch_ptr != NULL)
	{
		dmasprintf(&oface, DMROOT"LANInterfaces.LANEthernetInterfaceConfig.%d.", ++i); // MEM WILL BE FREED IN DMMEMCLEAN
		ai_s = update_availableinterface_list(ctx, ch_ptr, &available_inst, &instance_last);
		init_args_layer2(ctx, ai_s, NULL, instance_last, NULL, "LANInterface", oface);
		SUBENTRY(entry_layer2_availableinterface_instance, ctx, available_inst);
		ch_ptr = strtok_r(NULL, " ", &saveptr);
	}
	i = 0;
	uci_foreach_sections("wireless", "wifi-iface", wifi_s) {
		dmasprintf(&oface, DMROOT"LANInterfaces.WLANConfiguration.%d.", ++i); // MEM WILL BE FREED IN DMMEMCLEAN
		ai_s = update_availableinterface_list(ctx, section_name(wifi_s), &available_inst, &instance_last);
		init_args_layer2(ctx, ai_s, NULL, instance_last, NULL, "LANInterface", oface);
		SUBENTRY(entry_layer2_availableinterface_instance, ctx, available_inst);
	}
	return 0;
}

inline int entry_layer2_bridge(struct dmctx *ctx)
{
	char *bridge_instance = NULL, *bridge_instance_last = NULL;
	struct uci_section *bridge_s;
	uci_foreach_option_eq("network", "interface", "type", "bridge", bridge_s) {
		dmuci_delete_by_section(bridge_s, "bridge_instance", NULL);
	}
	uci_foreach_option_eq("network", "interface", "type", "bridge", bridge_s) {
		bridge_instance =  handle_update_instance(1, ctx, &bridge_instance_last, update_instance_alias, 3, bridge_s, "bridge_instance", "bridge_alias");
		update_markinginterface_list(bridge_s, bridge_instance_last);
		init_args_layer2(ctx, bridge_s, NULL, NULL, bridge_instance_last, NULL, NULL);
		update_bridge_all_vlan_config_bybridge(ctx);
		SUBENTRY(entry_layer2_bridge_instance, ctx, bridge_instance, bridge_instance_last);
	}
	return 0;
}

inline int entry_layer2_marking(struct dmctx *ctx)
{
	char *marking_instance = NULL, *marking_instance_last = NULL;
	struct uci_section *marking_s = NULL;

	uci_path_foreach_sections(icwmpd, "dmmap", "marking-bridge", marking_s) {
		marking_instance =  handle_update_instance(1, ctx, &marking_instance_last, update_instance_alias_icwmpd, 3, marking_s, "marking_instance", "marking_alias");
		init_args_layer2(ctx, marking_s, NULL, NULL, NULL, NULL, NULL);
		SUBENTRY(entry_layer2_marking_instance, ctx, marking_instance);
	}
	return 0;
}

inline int entry_layer2_bridge_vlan(struct dmctx *ctx, char *bridge_instance, char *bridge_instance_last)
{
	struct uci_section *ss = NULL;
	char *vlan_instance = NULL, *vlan_instance_last = NULL;

	update_bridge_all_vlan_config_bybridge(ctx);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "vlan_bridge", "bridgekey", bridge_instance_last, ss) {
		vlan_instance =  handle_update_instance(2, ctx, &vlan_instance_last, update_instance_alias_icwmpd, 3, ss, "vlan_instance", "vlan_alias");
		init_args_layer2_vlan(ctx, ss);
		SUBENTRY(entry_layer2_bridge_vlan_instance, ctx, bridge_instance, vlan_instance);
	}
	return 0;
}
/*************************************************************
* LAYER2-DM OBJ & PARAM
/ * ***********************************************************/

int entry_method_root_Layer2Bridging(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"Layer2Bridging.") {
#ifdef EX400
		dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_baseifname);
#else
		dmuci_get_option_value_string("layer2_interface_ethernet", "Wan", "baseifname", &wan_baseifname);
#endif
		DMOBJECT(DMROOT"Layer2Bridging.", ctx, "0", 1, NULL, NULL, NULL);
		DMOBJECT(DMROOT"Layer2Bridging.AvailableInterface.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"Layer2Bridging.Marking.", ctx, "1", 1, add_layer2bridging_marking, delete_layer2bridging_marking_all, NULL);
		DMOBJECT(DMROOT"Layer2Bridging.Bridge.", ctx, "1", 1, add_layer2bridging_bridge, delete_layer2bridging_bridge_all, NULL);
		SUBENTRY(entry_layer2_availableinterface, ctx);
		SUBENTRY(entry_layer2_bridge, ctx);
		SUBENTRY(entry_layer2_marking, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_layer2_availableinterface_instance(struct dmctx *ctx, char *int_instance)
{
	IF_MATCH(ctx, DMROOT"Layer2Bridging.AvailableInterface.%s.", int_instance) {
		DMOBJECT(DMROOT"Layer2Bridging.AvailableInterface.%s.", ctx, "0", 0, NULL, NULL, NULL, int_instance);
		DMPARAM("Alias", ctx, "1", get_avai_int_alias, set_avai_int_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("AvailableInterfaceKey", ctx, "0", get_available_interface_key, NULL, "xsd:unsignedInt", 0, 0, 0, NULL);
		DMPARAM("InterfaceReference", ctx, "0", get_interface_reference, NULL, NULL, 0, 0, 0, NULL);
		DMPARAM("InterfaceType", ctx, "0", get_interfaces_type, NULL, NULL, 0, 0, 0, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_layer2_marking_instance(struct dmctx *ctx, char *marking_br_instance)
{
	IF_MATCH(ctx, DMROOT"Layer2Bridging.Marking.%s.", marking_br_instance) {
		DMOBJECT(DMROOT"Layer2Bridging.Marking.%s.", ctx, "1", 1, NULL, delete_layer2bridging_marking, NULL, marking_br_instance);
		DMPARAM("Alias", ctx, "1", get_marking_alias, set_marking_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MarkingBridgeReference", ctx, "1", get_marking_bridge_reference, set_marking_bridge_key, "xsd:int", 0, 1, UNDEF, NULL);
		DMPARAM("MarkingInterface", ctx, "1", get_marking_interface_key, set_marking_interface_key, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_layer2_bridge_instance(struct dmctx *ctx, char *bridge_instance, char *bridge_instance_last)
{
	IF_MATCH(ctx, DMROOT"Layer2Bridging.Bridge.%s.", bridge_instance) {
		DMOBJECT(DMROOT"Layer2Bridging.Bridge.%s.", ctx, "1", 1, NULL, delete_layer2bridging_bridge, NULL, bridge_instance);
		DMPARAM("Alias", ctx, "1", get_bridge_alias, set_bridge_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("BridgeEnable", ctx, "1", get_bridge_status, set_bridge_status, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("BridgeKey", ctx, "0", get_bridge_key, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("BridgeName", ctx, "1", get_bridge_name, set_bridge_name, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("VLANID", ctx, "1", get_bridge_vlanid, set_bridge_vlanid, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_COM_AssociatedInterfaces", ctx, "1", get_associated_interfaces, set_associated_interfaces, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"Layer2Bridging.Bridge.%s.VLAN.", ctx, "1", 1, add_layer2bridging_bridge_vlan, delete_layer2bridging_bridge_vlan_all, NULL, bridge_instance);
		SUBENTRY(entry_layer2_bridge_vlan, ctx, bridge_instance, bridge_instance_last);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_layer2_bridge_vlan_instance(struct dmctx *ctx,char *bridge_instance, char *vlan_instance)
{
	IF_MATCH(ctx, DMROOT"Layer2Bridging.Bridge.%s.VLAN.%s.", bridge_instance, vlan_instance) {
		DMOBJECT(DMROOT"Layer2Bridging.Bridge.%s.VLAN.%s.", ctx, "1", 1, NULL, delete_layer2bridging_bridge_vlan, NULL, bridge_instance, vlan_instance);
		DMPARAM("Alias", ctx, "1", get_brvlan_alias, set_brvlan_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("VLANEnable", ctx, "1", get_bridge_vlan_enable, set_bridge_vlan_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("VLANName", ctx, "1", get_bridge_vlan_name, set_bridge_vlan_name, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("VLANID", ctx, "1", get_bridge_vlan_vid, set_bridge_vlan_vid, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_COM_VLANPriority", ctx, "1", get_bridge_vlan_priority, set_bridge_vlan_priority, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}
