/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */

#ifndef __WAN_DEVICE_H
#define __WAN_DEVICE_H
#include <libubox/blobmsg_json.h>
#include <json-c/json.h>

struct wanargs
{
/* WANDevice */
	struct uci_section *wandevsection;
	int instance;
	char *fdev;
/* WANConnectionDevice */
	struct uci_section *wancdsection;
	char *fwan;
	char *iwan;
	char *wan_ifname;
/* WAN###Connection */
	struct uci_section *wancprotosection;
};

extern struct dm_permession_s DMWANConnectionDevice;
extern struct dm_permession_s DMWANExternalIPPerm;
extern struct dm_notif_s DMWANConnectionDevicenotif;
extern struct dm_forced_inform_s DMWANConnectionProtocolinform;

extern struct dm_notif_s DMWANConnectionDevicenotif;
extern DMOBJ tWANDeviceObj[];
extern DMLEAF tWANDeviceParam[];
extern DMOBJ tWANConnectionDeviceObj[];
extern DMLEAF tWANConnectionDeviceParam[];
extern DMOBJ tWANConnectionObj[];
extern DMLEAF tWANIPConnectionParam[];
extern DMLEAF tWANPPPConnectionParam[];
extern DMLEAF tWANConnectionStatsParam[];
extern DMLEAF tWANConnection_VLANParam[];
extern DMLEAF tWANDSLLinkConfigParam[];
extern DMLEAF tWANCommonInterfaceConfigParam[];
extern DMLEAF tWANDSLInterfaceConfigParam[];
extern DMOBJ tWANEthernetInterfaceConfigObj[];
extern DMLEAF tWANEthernetInterfaceConfigParam[];
extern DMLEAF tWANEthernetInterfaceConfigStatsParam[];

int get_wan_interface (char *fwan, char **wan_interface);
int browsewandeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
unsigned char get_wan_protocol_connection_forced_inform(char *refparam, struct dmctx *dmctx, void *data, char *instance);
int browsewanconnectiondeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsewanprotocolconnectionipInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsewanprotocolconnectionpppInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_wan_wanconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_wan_wanconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_wan_wanipconnection(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_wan_wanipconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_wan_wanpppconnection(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_wan_wanpppconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

bool check_wan_is_ethernet(struct dmctx *dmctx, void *data);
bool check_wan_is_atm(struct dmctx *dmctx, void *data);

int get_wan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_con_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_enable_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_mng_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_mng_interface_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_mng_interface_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_connection_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_nat_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_firewall_enabled_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_igmp_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_dns_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ppp_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_ppp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_link_connection_eth_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_link_connection_eth_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_link_connection_eth_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_link_connection_eth_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_vpriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_ip_link_connection_layer2_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_dsl_link_config_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_dsl_link_config_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_dsl_link_config_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_wan_access_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_wan_dsl_interface_config_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_wan_dsl_interface_config_modulation_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_datapath(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_downstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_downstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_downstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_downstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_upstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_upstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_upstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_device_dsl_upstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wan_eth_intf_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_protocol_connection_linker(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_wan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_con_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_enable_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_device_mng_interface_mac (char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_connection_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_nat_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_firewall_enabled_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_igmp_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_dns_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_dns_override(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_connection_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ppp_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_enable_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_device_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_device_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_vpriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_ip_link_connection_layer2_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_dsl_link_config_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_dsl_link_config_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_annexm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wan_eth_intf_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
