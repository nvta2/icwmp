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
#ifndef __WIFI_H
#define __WIFI_H

struct wifi_radio_args
{
	struct uci_section *wifi_radio_sec;
};

struct wifi_ssid_args
{
	struct uci_section *wifi_ssid_sec;
	char *ifname;
	char *linker;
};
struct wifi_acp_args
{
	struct uci_section *wifi_acp_sec;
	char *ifname;
};

struct wifi_associative_device_args
{
	int active;
	int lastdatadownloadlinkrate;
	int lastdatauplinkrate;
	int signalstrength;
	char *macaddress;
};

extern DMOBJ tWifiObj[];
extern DMLEAF tWifiParams[];
extern DMOBJ tWifiRadioStatsObj[];
extern DMOBJ tAcessPointSecurityObj[];
extern DMOBJ tWifiSsidStatsObj[];
extern DMLEAF tWifiAcessPointParams[];
extern DMLEAF tWifiSsidParams[];
extern DMLEAF tWifiRadioParams[];
extern DMLEAF tWifiAcessPointSecurityParams[];
extern DMLEAF tWifiAcessPointAssociatedDeviceParams[];
extern DMLEAF tWifiRadioStatsParams[];
extern DMLEAF tWifiSsidStatsParams[];

int browseWifiSsidInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWifiAccessPointInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWifiRadioInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browse_wifi_associated_device(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int get_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

static int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_supported_frequency_bands(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_possible_channels(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_statistics_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ssid_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ap_ssid_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
static int get_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_total_associations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_supported_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_lastdatadownlinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_lastdatauplinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_signalstrength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_Wifi_Radio(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_Wifi_Ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_associated_device(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

static int set_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ssid_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_wepkey(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_shared_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
