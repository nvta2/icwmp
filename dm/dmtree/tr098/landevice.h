/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */

#ifndef __LAN_DEVICE_H
#define __LAN_DEVICE_H
#include <libubox/blobmsg_json.h>
#include <json-c/json.h>
#define NVRAM_FILE "/proc/nvram/WpaKey"

extern DMLEAF tLanhost_Config_ManagementParam[];
extern DMLEAF tDHCPStaticAddressParam[];
extern DMLEAF tIPInterfaceParam[];
extern DMLEAF tlanethernetinterfaceconfigParam[];
extern DMOBJ tLanhost_Config_ManagementObj[];
extern DMLEAF tWlanConfigurationParam[];
extern DMLEAF tWPSParam[];
extern DMLEAF tWepKeyParam[];
extern DMLEAF tpresharedkeyParam[];
extern DMLEAF tassociateddeviceParam[];
extern DMOBJ tWlanConfigurationObj[];
extern DMOBJ tLANDeviceinstObj[];
extern DMOBJ tLANDeviceObj[];
extern DMOBJ tlanethernetinterfaceconfigObj[];
extern DMLEAF tlanethernetinterfaceStatsParam[];
extern DMLEAF tLANDeviceParam[];
extern DMLEAF tlandevice_hostParam[];
extern DMOBJ tlandevice_hostObj[];
extern DMLEAF tlandevice_hostsParam[];
extern DMOBJ tDHCPConditionalServingPoolobj[];
extern DMLEAF tDHCPConditionalServingPoolParam[];
extern DMLEAF tDHCPOptionParam[];
struct wl_clientargs
{
	char *mac;
	char *wiface;
};

struct clientargs
{
	json_object *client;
	char *lan_name;
};

struct ldlanargs
{
	struct uci_section *ldlansection;
	char *ldinstance;
};

struct ldwlanargs
{
	struct uci_section *lwlansection;
	int wlctl_num;
	struct uci_section *device_section;
	char *wunit;
	char *wiface;
	json_object *res;
	int pki;
};

struct ldethargs
{
	struct uci_section *lan_ethsection;
	char *eth;
};

struct wlan_psk
{
	struct uci_section *wlanpsk;
	struct uci_section *lwlansection;
	char *wiface;
	int pki;
};

struct wlan_wep
{
	struct uci_section *wlanwep;
	struct uci_section *lwlansection;
	unsigned int key_index;
};

typedef struct dhcp_param
{
	char *interface;
	char *state_sec;
}dhcp_param;

struct dhcppooloptionargs
{
	struct uci_section *dhcppooloptionsection;
	struct uci_section *dhcppoolsection;
};

int browselandeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcp_static_addressInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browselanethernetinterfaceconfigInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWlanConfigurationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWepKeyInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsepresharedkeyInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseassociateddeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browselandevice_hostInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseentry_landevice_dhcpconditionalservingpool_option_instance(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browselandevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_landevice_wlanconfiguration(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_landevice_wlanconfiguration(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_landevice_dhcpstaticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_landevice_dhcpstaticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_dhcp_conditional_serving_pool(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_conditional_serving_pool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_dhcp_serving_pool_option(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_serving_pool_option(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_landevice(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_landevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_lan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dns(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_interval_address_start(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_interval_address_end(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_interface_addressingtype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcpstaticaddress_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_nbr_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_conditional_servingpool_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_hostname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_interfacetype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_addresssource(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_host_leasetimeremaining(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_eth_iface_cfg_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_beacon_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_possible_channels(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wep_key_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wep_encryption_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_radio_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_total_associations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_devstatus_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_devstatus_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_devstatus_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_devstatus_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_channelmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_maxssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_scantimer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wps_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_wep_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_psk_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_psk_assoc_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_associated_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_associated_ipddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_associated_authenticationstate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_lanhost_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_lan_dns(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_address_start(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_address_end(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_interface_addressingtype(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcpstaticaddress_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_conditionalservingpool_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_eth_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_beacon_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wep_key_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_key_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_radio_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_iopsys_eu_channelmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_iopsys_eu_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_iopsys_eu_maxssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_iopsys_eu_scantimer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_iopsys_eu_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wps_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wep_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_wep_key1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_psk_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_pre_shared_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_preshared_key_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
