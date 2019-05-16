/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */
#ifndef __BRIDGING_H
#define __BRIDGING_H


struct bridging_args
{
	struct uci_section *bridge_sec;
	char *br_key;
	char *ifname;
	char *br_inst;
};

struct bridging_port_args
{
	struct uci_section *bridge_port_sec;
	struct uci_section *bridge_sec;
	bool vlan;
	char *ifname;
};

struct bridging_vlan_args
{
	struct uci_section *bridge_vlan_sec;
	struct uci_section *bridge_sec;
	char *vlan_port;
	char *br_inst;
	char *ifname;
};

extern DMOBJ tBridgingObj[];
extern DMLEAF tBridgingParams[];
extern DMOBJ tBridgingBridgeObj[];
extern DMLEAF tBridgingBridgeParams[];
extern DMLEAF tBridgeVlanParams[];
extern DMLEAF tBridgePortParams[];
extern DMLEAF tBridgeVlanPortParams[];
extern DMOBJ tBridgePortObj[];
extern DMLEAF tBridgePortStatParams[];

int browseBridgeVlanPortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgeVlanInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgePortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgeInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_linker_br_port(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_linker_br_vlan(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int add_bridge(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_bridge(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_br_vlan(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_br_vlan(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_br_port(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_br_port(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_Max_Bridge_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Max_DBridge_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Max_QBridge_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Max_VLAN_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Max_Provider_Bridge_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Max_Filter_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Bridge_Number_Of_Entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_port_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_last_change(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_management(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_management(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_default_user_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_default_user_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_priority_regeneration(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_priority_regeneration(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_port_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_pvid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_pvid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_port_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_port_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_br_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_unicast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_unicast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_discard_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_discard_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_multicast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_multicast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_broadcast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_broadcast_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_unknown_proto_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_vlan_port_vlan_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_vlan_port_vlan_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_br_vlan_untagged(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_br_vlan_untagged(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
