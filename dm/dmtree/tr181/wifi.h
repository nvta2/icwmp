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

struct wifi_enp_args
{
	struct uci_section *wifi_enp_sec;
	char *ifname;
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
	char *wdev;
	int noise;
	int retransmissions;
	int assoctime;

};

struct wifi_neighboring_diagnostic_args
{
	char *ssid;
	char *bssid;
	char *operatingfrequencyband;
	int channel;
	int signalstrength;
	int noise;
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
extern DMLEAF tWifiAcessPointIEEE80211rParams[];
extern DMOBJ tWifiAcessPointAssociatedDeviceObj[];
extern DMLEAF tWifiAcessPointAssociatedDeviceStatsParams[];
extern DMLEAF tWifiRadioStatsParams[];
extern DMLEAF tWifiSsidStatsParams[];
extern DMOBJ tNeighboringWiFiDiagnosticObj[];
extern DMLEAF tNeighboringWiFiDiagnosticParams[];
extern DMLEAF tNeighboringWiFiDiagnosticResultParams[];
extern DMLEAF tWiFiAccessPointWPSParams[];
extern DMLEAF tWiFiAccessPointAccountingParams[];
extern DMOBJ tWiFiEndPointObj[];
extern DMLEAF tWiFiEndPointParams[];
extern DMLEAF tWiFiEndPointStatsParams[];
extern DMLEAF tWiFiEndPointSecurityParams[];
extern DMLEAF tWiFiEndPointWPSParams[];

int browseWifiSsidInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWifiAccessPointInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWifiRadioInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browse_wifi_associated_device(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWifiNeighboringWiFiDiagnosticResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseWiFiEndPointInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjWiFiEndPoint(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjWiFiEndPoint(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_WiFi_RadioNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFi_SSIDNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFi_AccessPointNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

static int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_radio_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
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
int get_WiFiRadio_AutoChannelSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_AutoChannelRefreshPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
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
int get_access_point_security_wepkey_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_ieee80211r_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_lastdatadownlinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_lastdatauplinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_signalstrength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_retrans_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_failed_retrans_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_retry_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_access_point_associative_device_statistics_multiple_retry_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_number_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_signal_strength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_operating_frequency_band(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_neighboring_wifi_diagnostics_result_noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_Wifi_Radio(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_Wifi_Ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_associated_device(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_WiFiRadio_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_MaxSupportedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_FragmentationThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_RTSThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_SupportedOperatingChannelBandwidths(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_OperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_CurrentOperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_BeaconPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_DTIMPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_ConfigMethodsSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_Version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAssociatedDevice_Noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_ServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_SecondaryServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_ServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_SecondaryServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_Secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_SecondarySecret(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAccounting_InterimInterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSID_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_UAPSDCapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_WMMCapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_UAPSDCapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_UAPSDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAssociatedDevice_Retransmissions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointSecurity_MFPConfig(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_MaxAllowedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_PreambleType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_IEEE80211hSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_IEEE80211hEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_TransmitPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadio_RegulatoryDomain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiRadioStats_Noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiSSIDStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPoint_IsolationEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiAccessPointAssociatedDevice_AssociationTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPoint_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPoint_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPoint_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPoint_ProfileReference(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPoint_SSIDReference(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointStats_LastDataDownlinkRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointStats_LastDataUplinkRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointStats_SignalStrength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointStats_Retransmissions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointSecurity_ModesSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_ConfigMethodsSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_Version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_WiFiEndPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

static int set_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_radio_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_AutoChannelRefreshPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
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
int set_access_point_security_wepkey_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_shared_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_security_radius_secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_access_point_ieee80211r_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_neighboring_wifi_diagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
static int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_FragmentationThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_RTSThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_OperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_BeaconPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_DTIMPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_ServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_SecondaryServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_ServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_SecondaryServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_Secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_SecondarySecret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointAccounting_InterimInterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPoint_UAPSDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPointSecurity_MFPConfig(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPoint_MaxAllowedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_PreambleType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_IEEE80211hEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_TransmitPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiRadio_RegulatoryDomain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiAccessPoint_IsolationEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPoint_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPoint_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPoint_ProfileReference(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_WiFiEndPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
