/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *      Copyright (C) 2019 iopsys Software Solutions AB
 *      Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
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
#include "dmentry.h"

char *wan_ifname = NULL;

/* *** Device.Ethernet. *** */
DMOBJ tEthernetObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Interface", &DMREAD, NULL, NULL, NULL, browseEthernetInterfaceInst, NULL, NULL, tEthernetInterfaceObj, tEthernetInterfaceParams, get_linker_interface},
{"Link", &DMWRITE, addObjEthernetLink, delObjEthernetLink, NULL, browseEthernetLinkInst, NULL, NULL, tEthernetLinkObj, tEthernetLinkParams, get_linker_link},
{"VLANTermination", &DMWRITE, addObjEthernetVLANTermination, delObjEthernetVLANTermination, NULL, browseEthernetVLANTerminationInst, NULL, NULL, tEthernetVLANTerminationObj, tEthernetVLANTerminationParams, get_linker_vlan_term},
{0}
};

DMLEAF tEthernetParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"InterfaceNumberOfEntries", &DMREAD, DMT_UNINT, get_Ethernet_InterfaceNumberOfEntries, NULL, NULL, NULL},
{"LinkNumberOfEntries", &DMREAD, DMT_UNINT, get_Ethernet_LinkNumberOfEntries, NULL, NULL, NULL},
{"VLANTerminationNumberOfEntries", &DMREAD, DMT_UNINT, get_Ethernet_VLANTerminationNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.Ethernet.Interface.{i}. *** */
DMOBJ tEthernetInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetInterfaceStatsParams, NULL},
{0}
};

DMLEAF tEthernetInterfaceParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_EthernetInterface_Enable, set_EthernetInterface_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_EthernetInterface_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_EthernetInterface_Alias, set_EthernetInterface_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_EthernetInterface_Name, NULL, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_EthernetInterface_LastChange, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_EthernetInterface_LowerLayers, set_EthernetInterface_LowerLayers, NULL, NULL},
{"Upstream", &DMREAD, DMT_BOOL, get_EthernetInterface_Upstream, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_EthernetInterface_MACAddress, NULL, NULL, NULL},
{"MaxBitRate", &DMWRITE, DMT_INT, get_EthernetInterface_MaxBitRate, set_EthernetInterface_MaxBitRate, NULL, NULL},
{"CurrentBitRate", &DMREAD, DMT_UNINT, get_EthernetInterface_CurrentBitRate, NULL, NULL, NULL},
{"DuplexMode", &DMWRITE, DMT_STRING, get_EthernetInterface_DuplexMode, set_EthernetInterface_DuplexMode, NULL, NULL},
{"EEECapability", &DMREAD, DMT_BOOL, get_EthernetInterface_EEECapability, NULL, NULL, NULL},
{0}
};

/* *** Device.Ethernet.Interface.{i}.Stats. *** */
DMLEAF tEthernetInterfaceStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"BytesSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_BytesSent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_BytesReceived, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_PacketsSent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_PacketsReceived, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_ErrorsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetInterfaceStats_UnknownProtoPacketsReceived, NULL, NULL, NULL},
{0}
};

/* *** Device.Ethernet.Link.{i}. *** */
DMOBJ tEthernetLinkObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetLinkStatsParams, NULL},
{0}
};

DMLEAF tEthernetLinkParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_EthernetLink_Enable, set_EthernetLink_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_EthernetLink_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_EthernetLink_Alias, set_EthernetLink_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_EthernetLink_Name, NULL, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_EthernetLink_LastChange, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_EthernetLink_LowerLayers, set_EthernetLink_LowerLayers, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_EthernetLink_MACAddress, NULL, NULL, NULL},
{0}
};

/* *** Device.Ethernet.Link.{i}.Stats. *** */
DMLEAF tEthernetLinkStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"BytesSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_BytesSent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_BytesReceived, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_PacketsSent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_PacketsReceived, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_ErrorsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetLinkStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetLinkStats_UnknownProtoPacketsReceived, NULL, NULL, NULL},
{0}
};

/* *** Device.Ethernet.VLANTermination.{i}. *** */
DMOBJ tEthernetVLANTerminationObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetVLANTerminationStatsParams, NULL},
{0}
};

DMLEAF tEthernetVLANTerminationParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_EthernetVLANTermination_Enable, set_EthernetVLANTermination_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_EthernetVLANTermination_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_EthernetVLANTermination_Alias, set_EthernetVLANTermination_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_EthernetVLANTermination_Name, NULL, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_EthernetVLANTermination_LastChange, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_EthernetVLANTermination_LowerLayers, set_EthernetVLANTermination_LowerLayers, NULL, NULL},
{"VLANID", &DMWRITE, DMT_UNINT, get_EthernetVLANTermination_VLANID, set_EthernetVLANTermination_VLANID, NULL, NULL},
{"TPID", &DMWRITE, DMT_UNINT, get_EthernetVLANTermination_TPID, set_EthernetVLANTermination_TPID, NULL, NULL},
{0}
};

/* *** Device.Ethernet.VLANTermination.{i}.Stats. *** */
DMLEAF tEthernetVLANTerminationStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"BytesSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_BytesSent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_BytesReceived, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_PacketsSent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_PacketsReceived, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_ErrorsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_EthernetVLANTerminationStats_UnknownProtoPacketsReceived, NULL, NULL, NULL},
{0}
};

/*************************************************************
 * INIT
/*************************************************************/
inline int init_eth_port(struct eth_port_args *args, struct uci_section *s, char *ifname)
{
	args->eth_port_sec = s;
	args->ifname = ifname;
	return 0;
}

/*************************************************************
 * COMMON Functions
/*************************************************************/
static int is_mac_exist(char *macaddr)
{
	struct uci_section *s = NULL;
	char *mac;

	uci_path_foreach_sections(icwmpd, DMMAP, "link", s) {
		dmuci_get_value_by_section_string(s, "mac", &mac);
		if (strcmp(mac, macaddr) == 0)
			return 1;
	}
	return 0;
}

static void create_link(char *ifname)
{
	char *macaddr, *v, *device;
	struct uci_section *dmmap = NULL;

	macaddr = get_macaddr(ifname);
	if (macaddr[0] == '\0')
		return;

	device = get_device(ifname);
	if (device[0] == '\0')
		return;

	/* Interfaces might share the same mac address */
	if (is_mac_exist(macaddr))
		return;

	dmuci_add_section_icwmpd(DMMAP, "link", &dmmap, &v);
	dmuci_set_value_by_section(dmmap, "mac", macaddr);
	dmuci_set_value_by_section(dmmap, "device", device);
	dmuci_set_value_by_section(dmmap, "section_name", ifname);
}

static int dmmap_synchronizeEthernetLink(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *type, *ifname;

	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "type", &type);
		if (strcmp(type, "alias") == 0 || strcmp(section_name(s), "loopback") == 0)
			continue;

		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		if (*ifname == '\0' || *ifname == '@')
			continue;

		create_link(section_name(s));
	}
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseEthernetInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *int_num = NULL, *int_num_last = NULL, *ifname;
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

int browseEthernetLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct dm_args args = {0};
	struct uci_section *s = NULL;
	char *id_last = NULL, *id = NULL;

	dmmap_synchronizeEthernetLink(dmctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, DMMAP, "link", s) {
		args.section = s;
		id = handle_update_instance(1, dmctx, &id_last, update_instance_alias_icwmpd, 3, s, "link_instance", "link_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&args, id) == DM_STOP) {
			break;
		}
	}
	return 0;
}

int browseEthernetVLANTerminationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *vlan_term = NULL, *vlan_term_last = NULL, *type= NULL, *vlan_method= NULL;
	struct dm_args curr_vlan_term_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "device", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "type", &type);
		dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
		if ((strcmp(vlan_method, "2") != 0 && strcmp(vlan_method, "1") != 0) || (strcmp(vlan_method, "1") == 0 && strcmp(type, "untagged") == 0) )
			continue;
		curr_vlan_term_args.section = p->config_section;
		if(strcmp(vlan_method, "2") == 0)
			vlan_term = handle_update_instance(1, dmctx, &vlan_term_last, update_instance_alias, 3, p->dmmap_section, "all_vlan_term_instance", "all_vlan_term_alias");
		else
			vlan_term = handle_update_instance(1, dmctx, &vlan_term_last, update_instance_alias, 3, p->dmmap_section, "only_tagged_vlan_term_instance", "vlan_term_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_vlan_term_args, vlan_term) == DM_STOP)
			break;
	}

	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

/*************************************************************
* LINKER
**************************************************************/
int get_linker_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker)
{
	if (data && ((struct eth_port_args *)data)->ifname) {
		*linker = ((struct eth_port_args *)data)->ifname;
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

int get_linker_link(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker)
{
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "mac", linker);
	return 0;
}

int get_linker_vlan_term(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker)
{
	if(((struct dm_args *)data)->section) {
		dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "name", linker);
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/*************************************************************
 * ADD & DEL OBJ
/*************************************************************/
int addObjEthernetLink(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *inst, *v;
	struct uci_section *dmmap_network= NULL;

	inst = get_last_instance_icwmpd(DMMAP, "link", "link_instance");

	dmuci_add_section_icwmpd(DMMAP, "link", &dmmap_network, &v);
	*instance = update_instance_icwmpd(dmmap_network, inst, "link_instance");
	return 0;
}

int delObjEthernetLink(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	switch (del_action) {
		case DEL_INST:
			dmuci_delete_by_section(((struct dm_args *)data)->section, NULL, NULL);
			break;
		case DEL_ALL:
			DMUCI_DEL_SECTION(icwmpd, DMMAP, "link", NULL, NULL);
			break;
	}

	return 0;
}

int addObjEthernetVLANTermination(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *inst, *v, *eth_wan, *vid, *name, *vlan_name, *vlan_method = NULL;
	struct uci_section *dmmap_network= NULL;

	check_create_dmmap_package("dmmap_network");
	dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
	if(strcmp(vlan_method, "2") == 0)
		inst = get_vlan_last_instance_icwmpd("dmmap_network", "device", "all_vlan_term_instance", vlan_method);
	else
		inst = get_vlan_last_instance_icwmpd("dmmap_network", "device", "only_tagged_vlan_term_instance", vlan_method);

	dmuci_get_option_value_string("ports", "WAN", "ifname", &eth_wan);
	dmasprintf(&vid, "%d", inst?atoi(inst)+4:4);
	dmasprintf(&vlan_name, "vlan_%s", vid);
	dmuci_set_value("network", vlan_name, "", "device");
	dmuci_set_value("network", vlan_name, "ifname", eth_wan);
	dmuci_set_value("network", vlan_name, "type", "8021q");
	dmuci_set_value("network", vlan_name, "vid", vid);
	dmasprintf(&name, "%s.%s", eth_wan, vid);
	dmuci_set_value("network", vlan_name, "name", name);

	dmuci_add_section_icwmpd("dmmap_network", "device", &dmmap_network, &v);
	dmuci_set_value_by_section(dmmap_network, "section_name", vlan_name);
	if(strcmp(vlan_method, "2") == 0)
		*instance = update_instance_icwmpd(dmmap_network, inst, "all_vlan_term_instance");
	else
		*instance = update_instance_icwmpd(dmmap_network, inst, "only_tagged_vlan_term_instance");

	return 0;
}

int delObjEthernetVLANTermination(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	struct uci_section *s = NULL, *ss = NULL, *dmmap_section = NULL, *vlan_method = NULL;

	switch (del_action) {
	case DEL_INST:
		dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
		if(is_section_unnamed(section_name(((struct dm_args *)data)->section))){
			LIST_HEAD(dup_list);
			if(strcmp(vlan_method, "2") == 0){
				delete_sections_save_next_sections("dmmap_network", "device", "all_vlan_term_instance", section_name((struct uci_section *)data), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "all_vlan_term_instance", "dmmap_network", "device");
			}
			else{
				delete_sections_save_next_sections("dmmap_network", "device", "only_tagged_vlan_term_instance", section_name((struct uci_section *)data), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "only_tagged_vlan_term_instance", "dmmap_network", "device");
			}
			dmuci_delete_by_section_unnamed(((struct dm_args *)data)->section, NULL, NULL);
		} else {
			get_dmmap_section_of_config_section("dmmap_dropbear", "dropbear", section_name(((struct dm_args *)data)->section), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(((struct dm_args *)data)->section, NULL, NULL);
		}
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
 * GET & SET PARAM
/*************************************************************/
int get_Ethernet_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	uci_foreach_sections("ports", "ethport", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_Ethernet_LinkNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	dmmap_synchronizeEthernetLink(ctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, DMMAP, "link", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_Ethernet_VLANTerminationNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	char *type, *vlan_method;
	int cnt = 0;

	uci_foreach_sections("network", "device", s)
	{
		dmuci_get_value_by_section_string(s, "type", &type);
		dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
		if ((strcmp(vlan_method, "2") != 0 && strcmp(vlan_method, "1") != 0) || (strcmp(vlan_method, "1") == 0 && strcmp(type, "untagged") == 0))
			continue;
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_EthernetInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_EthernetInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
			if (strstr(((struct eth_port_args *)data)->ifname, wan_ifname))
				ifname = dmstrdup(wan_ifname);
			else
				ifname = dmstrdup(((struct eth_port_args *)data)->ifname);

			DMCMD("ethctl", 3, ifname, "phy-power", b ? "up" : "down");
			dmfree(ifname);
			return 0;
	}
	return 0;
}

int get_EthernetInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	bool b;

	get_EthernetInterface_Enable(refparam, ctx, data, instance, value);
	string_to_bool(*value, &b);
	*value = b ? "Up" : "Down";
	return 0;
}

int get_EthernetInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_ports", "ethport", section_name(((struct eth_port_args *)data)->eth_port_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "eth_port_alias", value);
	if(*value == NULL || strlen(*value)<1)
		dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "name", value);
	return 0;
}

int set_EthernetInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_ports", "ethport", section_name(((struct eth_port_args *)data)->eth_port_sec), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "eth_port_alias", value);
			return 0;
	}
	return 0;
}

int get_EthernetInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "name", value);
	return 0;
}

int get_EthernetInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct uci_section *s = NULL;
	char *ifname;

	*value ="0";
	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		if (strstr(ifname, ((struct eth_port_args *)data)->ifname)) {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &res);
			DM_ASSERT(res, *value = "0");
			*value = dmjson_get_value(res, 1, "uptime");
			if((*value)[0] == '\0')
				*value = "0";
			break;
		}
	}
	return 0;
}

int get_EthernetInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	return 0;
}

int set_EthernetInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_EthernetInterface_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	dmuci_get_option_value_string("network", "lan", "ifname", &ifname);
	if (strstr(ifname, ((struct eth_port_args *)data)->ifname))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int get_EthernetInterface_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "macaddr");
	return 0;
}

int get_EthernetInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *pch, *spch, *speed;

	dmuci_get_value_by_section_string(((struct eth_port_args *)data)->eth_port_sec, "speed", &speed);
	if (speed[0] == '\0' || strcmp(speed, "disabled") == 0 )
		*value = "0";
	else {
		if (strcmp(speed, "auto") == 0)
			*value = "-1";
		else {
			pch = strtok_r(speed, "FHfh", &spch);
			*value = dmstrdup(pch);
		}
	}
	return 0;
}

int set_EthernetInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *duplex, *val = "", *p = "";

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "0") == 0 ) {
				dmuci_set_value_by_section(((struct eth_port_args *)data)->eth_port_sec, "speed", "disabled");
			} else if (strcmp(value, "-1") == 0) {
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

int get_EthernetInterface_CurrentBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *speed, *pch;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	speed = dmjson_get_value(res, 1, "speed");
	if(speed[0] != '\0') {
		pch = strtok(speed, "FHfh");
		*value = dmstrdup(pch);
	} else
		*value = "0";
	return 0;
}

int get_EthernetInterface_DuplexMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
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

int set_EthernetInterface_DuplexMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_EthernetInterface_EEECapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	return 0;
}

inline int get_ubus_ethernet_interface_stats(json_object *res, char **value, char *stat_mod, void *data)
{
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_EthernetInterfaceStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_interface_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_EthernetInterfaceStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_EthernetInterfaceStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_EthernetInterfaceStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

int get_EthernetInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_errors");
	return 0;
}

int get_EthernetInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_errors");
	return 0;
}

int get_EthernetInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "TX", "unicast"));
	return 0;
}

int get_EthernetInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "RX", "unicast"));
	return 0;
}

int get_EthernetInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_dropped");
	return 0;
}

int get_EthernetInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_dropped");
	return 0;
}

int get_EthernetInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "TX", "multicast"));
	return 0;
}

int get_EthernetInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "RX", "multicast"));
	return 0;
}

int get_EthernetInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "TX", "broadcast"));
	return 0;
}

int get_EthernetInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct eth_port_args *)data)->ifname, "RX", "broadcast"));
	return 0;
}

int get_EthernetInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct eth_port_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_over_errors");
	return 0;
}

int get_EthernetLink_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

int set_EthernetLink_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_EthernetLink_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Up";
	return 0;
}

int get_EthernetLink_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "link_alias", value);
	return 0;
}

int set_EthernetLink_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct dm_args *)data)->section, "link_alias", value);
			break;
	}
	return 0;
}

int get_EthernetLink_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct dm_args *)data)->section));
	return 0;
}

int get_EthernetLink_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *interface;

	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "section_name", &interface);
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 1, "uptime");
	if((*value)[0] == '\0')
		*value = "0";
	return 0;
}

int get_EthernetLink_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	char *link_mac, *type, *ifname, *mac, *br_inst, *mg, *wanifname, *wanlinker, linker[64] = "";
	struct uci_section *dmmap_section, *port;

	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "mac", &link_mac);
	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "type", &type);
		if (strcmp(type, "alias") == 0 || strcmp(section_name(s), "loopback") == 0)
			continue;

		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		if (*ifname == '\0' || *ifname == '@')
			continue;

		mac = get_macaddr(section_name(s));
		if (mac[0] == '\0' || strcasecmp(mac, link_mac) != 0)
			continue;

		if (strcmp(type, "bridge") == 0) {
			get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(s), &dmmap_section);
			if (dmmap_section != NULL) {
				dmuci_get_value_by_section_string(dmmap_section, "bridge_instance", &br_inst);
				uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key", br_inst, port) {
					dmuci_get_value_by_section_string(port, "mg_port", &mg);
					if (strcmp(mg, "true") == 0)
						sprintf(linker, "%s+", section_name(port));
					adm_entry_get_linker_param(ctx, dm_print_path("%s%cBridging%cBridge%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
					if (*value == NULL)
						*value = "";
				}
			}
		}
		else {
			dmuci_get_option_value_string("ports", "WAN", "ifname", &wanifname);
			/* for upstream interface, set the lowerlayer to wan port of Ethernet.Interface */
			if(strstr(ifname, wanifname)) {
				dmasprintf(&wanlinker, "%s.1", wanifname);
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), wanlinker, value);
				dmfree(wanlinker);
				if (*value == NULL)
					*value = "";
			}
		}
	}
	return 0;
}

int set_EthernetLink_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_EthernetLink_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "mac", value);
	return 0;
}

inline int get_ubus_ethernet_link_stats(json_object *res, char **value, char *stat_mod, void *data)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_EthernetLinkStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_EthernetLinkStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "rx_bytes", data);
	return 0;
}

int get_EthernetLinkStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "tx_packets", data);
	return 0;
}

int get_EthernetLinkStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "rx_packets", data);
	return 0;
}

int get_EthernetLinkStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "tx_errors", data);
	return 0;
}

int get_EthernetLinkStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "rx_errors", data);
	return 0;
}

int get_EthernetLinkStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "unicast"));
	return 0;
}

int get_EthernetLinkStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "unicast"));
	return 0;
}

int get_EthernetLinkStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "tx_dropped", data);
	return 0;
}

int get_EthernetLinkStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "rx_dropped", data);
	return 0;
}

int get_EthernetLinkStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "multicast"));
	return 0;
}

int get_EthernetLinkStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "multicast"));
	return 0;
}

int get_EthernetLinkStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "broadcast"));
	return 0;
}

int get_EthernetLinkStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "device", &device);
	dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "broadcast"));
	return 0;
}

int get_EthernetLinkStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_link_stats(res, value, "rx_over_errors", data);
	return 0;
}

int get_EthernetVLANTermination_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

int set_EthernetVLANTermination_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_EthernetVLANTermination_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Up";
	return 0;
}

int get_EthernetVLANTermination_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	char *vlan_method= NULL;

	get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct dm_args *)data)->section), &dmmap_section);
	dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
	if(strcmp(vlan_method, "2") == 0)
		dmuci_get_value_by_section_string(dmmap_section, "all_vlan_term_alias", value);
	else
		dmuci_get_value_by_section_string(dmmap_section, "vlan_term_alias", value);

	return 0;
}

int set_EthernetVLANTermination_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;
	char *vlan_method= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_network", "device", section_name(((struct dm_args *)data)->section), &dmmap_section);
			dmuci_get_option_value_string("cwmp", "cpe", "vlan_method", &vlan_method);
			if(strcmp(vlan_method, "2") == 0)
				dmuci_set_value_by_section(dmmap_section, "all_vlan_term_alias", value);
			else
				dmuci_set_value_by_section(dmmap_section, "vlan_term_alias", value);
			return 0;
	}
	return 0;
}

int get_EthernetVLANTermination_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct dm_args *)data)->section));
	return 0;
}

int get_EthernetVLANTermination_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct uci_section *s = NULL;
	char *ifname, *devifname;

	*value ="0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "name", &devifname);
	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		if (strstr(ifname, devifname)) {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &res);
			DM_ASSERT(res, *value = "0");
			*value = dmjson_get_value(res, 1, "uptime");
			if((*value)[0] == '\0')
				*value = "0";
			break;
		}
	}
	return 0;
}

int get_EthernetVLANTermination_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *pch, *spch, *devifname, *ifname, *mac;
	struct uci_section *section;
	
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "name", &devifname);
	uci_foreach_sections("network", "interface", section) {
		dmuci_get_value_by_section_string(section, "ifname", &ifname);
		for (pch = strtok_r(ifname, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
			if(strcmp(pch, devifname) == 0) {
				mac = get_macaddr(section_name(section));
				if (mac[0] != '\0') {
					adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), mac, value);
					if (*value == NULL)
						*value = "";
					break;
				}
			}
		}
	}
	return 0;
}

int set_EthernetVLANTermination_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *iface_list, *linker = NULL, *newvalue = NULL, *vlan_name = NULL;
	struct uci_section *s;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);

			if (linker == NULL || *linker == '\0')
				return -1;

			dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "name", &vlan_name);
			uci_foreach_sections("network", "interface", s) {
				dmuci_get_value_by_section_string(s, "ifname", &iface_list);
				if(strcmp(section_name(s), linker) != 0 && is_elt_exit_in_str_list(iface_list, vlan_name)) {
					remove_elt_from_str_list(&iface_list, vlan_name);
					dmuci_set_value_by_section(s, "ifname", iface_list);
				} else if (strcmp(section_name(s), linker) == 0 && !is_elt_exit_in_str_list(iface_list, vlan_name)) {
					add_elt_to_str_list(&iface_list, vlan_name);
					dmuci_set_value_by_section(s, "ifname", iface_list);
				}
			}
	}
	return 0;
}

int get_EthernetVLANTermination_VLANID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "vid", value);
	return 0;
}

int set_EthernetVLANTermination_VLANID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *ifname, *name, *vid, *curr_ifname;
	struct uci_section *s;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET: {
			dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
			dmasprintf(&name, "%s.%s", ifname, value);
			dmuci_set_value_by_section(((struct dm_args *)data)->section, "name", name);
			dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "vid", &vid);
			dmuci_set_value_by_section(((struct dm_args *)data)->section, "vid", value);
			dmasprintf(&curr_ifname, "%s.%s", ifname, vid);
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

int get_EthernetVLANTermination_TPID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *type;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "type", &type);
	if (strcmp(type, "8021q") == 0 || strcmp(type, "untagged") == 0)
		*value = "33024";
	else if (strcmp(type, "8021ad") == 0)
		*value = "34984";
	else
		return -1;
	return 0;
}

int set_EthernetVLANTermination_TPID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "33024") == 0)
				dmuci_set_value_by_section(((struct dm_args *)data)->section, "type", "8021q");
			else if (strcmp(value, "34984") == 0)
				dmuci_set_value_by_section(((struct dm_args *)data)->section, "type", "8021ad");
			else
				return -1;
			return 0;
	}
	return 0;
}

inline int get_ubus_ethernet_vlan_termination_stats(json_object *res, char **value, char *stat_mod, void *data)
{
	char *ifname;
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_EthernetVLANTerminationStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_EthernetVLANTerminationStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_EthernetVLANTerminationStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "tx_packets", data);
	return 0;
}

int get_EthernetVLANTerminationStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "rx_packets", data);
	return 0;
}

int get_EthernetVLANTerminationStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "tx_errors", data);
	return 0;
}

int get_EthernetVLANTerminationStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "rx_errors", data);
	return 0;
}

int get_EthernetVLANTerminationStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm"))
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "TX", "unicast"));
	return 0;
}

int get_EthernetVLANTerminationStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm"))
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "RX", "unicast"));
	return 0;
}

int get_EthernetVLANTerminationStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "tx_dropped", data);
	return 0;
}

int get_EthernetVLANTerminationStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "rx_dropped", data);
	return 0;
}

int get_EthernetVLANTerminationStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm")) {
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "TX", "multicast"));
	}
	return 0;
}

int get_EthernetVLANTerminationStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm"))
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "RX", "multicast"));
	return 0;
}

int get_EthernetVLANTerminationStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm"))
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "TX", "broadcast"));
	return 0;
}

int get_EthernetVLANTerminationStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *ifname;
	*value = "0";
	dmuci_get_value_by_section_string(((struct dm_args *)data)->section, "ifname", &ifname);
	if(!strstr(ifname, "atm") && !strstr(ifname, "ptm"))
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(ifname, "RX", "broadcast"));
	return 0;
}

int get_EthernetVLANTerminationStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	get_ubus_ethernet_vlan_termination_stats(res, value, "rx_over_errors", data);
	return 0;
}
