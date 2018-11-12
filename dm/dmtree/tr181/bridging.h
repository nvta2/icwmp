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

#define BUF_SIZE 7
extern DMOBJ tBridgingObj[];
extern DMOBJ tDridgingBridgeObj[];
extern DMLEAF tDridgingBridgeParams[];
extern DMLEAF tBridgeVlanParams[];
extern DMLEAF tBridgePortParams[];
extern DMLEAF tBridgeVlanPortParams[];
extern DMOBJ tBridgePortObj[];
extern DMLEAF tBridgePortStatParams[];

int browseBridgeVlanPortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgeVlanInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgePortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseBridgeInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_bridge(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_bridge(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_br_vlan(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_br_vlan(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_br_port(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_br_port(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);


int get_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_management(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_port_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vlan_port_vlan_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_br_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_associated_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_vlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_vlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_vlan_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_vlan_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_vlan_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_br_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_port_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_vlan_port_port_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_linker_br_port(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_linker_br_vlan(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
