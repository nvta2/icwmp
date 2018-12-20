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
#ifndef __ETHERNET_H
#define __ETHERNET_H

struct eth_port_args
{
	struct uci_section *eth_port_sec;
	char *ifname;
};

extern DMOBJ tEthernetObj[];
extern DMOBJ tEthernetStatObj[];
extern DMLEAF tEthernetParams[];
extern DMLEAF tEthernetStatParams[];
extern DMLEAF tVLANTermParams[];
extern DMLEAF tLinkParams[];

int browseEthIfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_eth_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_mac_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_port_stats_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_eth_port_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_eth_port_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_eth_port_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_eth_port_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);


int get_linker_val(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int get_linker_vlan_term(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int get_vlan_term_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_vlan_term_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_vlan_term_vlanid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_vlan_term_vlanid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_vlan_term_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_vlan_term_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_vlan_term_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_vlan_term_tpid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_vlan_term_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vlan_term_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vlan_term_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_vlan_term_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int add_vlan_term(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_vlan_term(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int browseVLANTermInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_linker_link(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_link_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_link_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_link_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_link_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_link_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_link_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_link_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_link_lowerlayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int add_link(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int browseLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

#endif
