/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
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
#include "wifi.h"
#include "dmjson.h"
#define DELIMITOR ","

/*** WiFi. ***/
DMOBJ tWifiObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Radio", &DMREAD, NULL, NULL, NULL, browseWifiRadioInst, NULL, NULL, tWifiRadioStatsObj, tWifiRadioParams, get_linker_Wifi_Radio},
{"SSID", &DMWRITE, add_wifi_ssid, delete_wifi_ssid, NULL, browseWifiSsidInst, NULL, NULL, tWifiSsidStatsObj, tWifiSsidParams, get_linker_Wifi_Ssid},
{"AccessPoint", &DMREAD, NULL, NULL, NULL, browseWifiAccessPointInst, NULL, NULL, tAcessPointSecurityObj, tWifiAcessPointParams, NULL},
{"NeighboringWiFiDiagnostic", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tNeighboringWiFiDiagnosticObj, tNeighboringWiFiDiagnosticParams, NULL},
{0}
};

DMLEAF tWifiParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"RadioNumberOfEntries", &DMREAD, DMT_UNINT, get_WiFi_RadioNumberOfEntries, NULL, NULL, NULL},
{"SSIDNumberOfEntries", &DMREAD, DMT_UNINT, get_WiFi_SSIDNumberOfEntries, NULL, NULL, NULL},
{"AccessPointNumberOfEntries", &DMREAD, DMT_UNINT, get_WiFi_AccessPointNumberOfEntries, NULL, NULL, NULL},
{CUSTOM_PREFIX"Bandsteering_Enable", &DMWRITE, DMT_BOOL, get_wifi_bandsteering_enable, set_wifi_bandsteering_enable, NULL, NULL},
{0}
};

/*** WiFi.Radio. ***/
DMOBJ tWifiRadioStatsObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiRadioStatsParams, NULL},
{0}
};

DMLEAF tWifiRadioParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_radio_alias, set_radio_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_radio_enable, set_radio_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_radio_status, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_WiFiRadio_LowerLayers, set_WiFiRadio_LowerLayers, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_WiFiRadio_Name, NULL, NULL, NULL},
{"MaxBitRate", &DMREAD, DMT_UNINT,get_radio_max_bit_rate, NULL, NULL, NULL},
{"OperatingFrequencyBand", &DMREAD, DMT_STRING, get_radio_frequency, NULL, NULL, NULL},
{"SupportedFrequencyBands", &DMREAD, DMT_STRING, get_radio_supported_frequency_bands, NULL, NULL, NULL},
{CUSTOM_PREFIX"MaxAssociations", &DMWRITE, DMT_STRING, get_radio_maxassoc, set_radio_maxassoc, NULL, NULL},
{CUSTOM_PREFIX"DFSEnable", &DMWRITE, DMT_BOOL, get_radio_dfsenable, set_radio_dfsenable, NULL, NULL},
{"SupportedStandards", &DMREAD, DMT_STRING, get_radio_supported_standard, NULL, NULL, NULL},
{"OperatingStandards", &DMWRITE, DMT_STRING, get_radio_operating_standard, set_radio_operating_standard, NULL, NULL},
{"ChannelsInUse", &DMREAD, DMT_STRING, get_radio_channel, NULL, NULL, NULL},
{"Channel", &DMWRITE, DMT_UNINT, get_radio_channel, set_radio_channel, NULL, NULL},
{"AutoChannelEnable", &DMWRITE, DMT_BOOL, get_radio_auto_channel_enable, set_radio_auto_channel_enable, NULL, NULL},
{"PossibleChannels", &DMREAD, DMT_STRING, get_radio_possible_channels, NULL, NULL, NULL},
{"AutoChannelSupported", &DMREAD, DMT_BOOL, get_WiFiRadio_AutoChannelSupported, NULL, NULL, NULL},
{"AutoChannelRefreshPeriod", &DMWRITE, DMT_UNINT, get_WiFiRadio_AutoChannelRefreshPeriod, set_WiFiRadio_AutoChannelRefreshPeriod, NULL, NULL},
{"MaxSupportedAssociations", &DMREAD, DMT_UNINT, get_WiFiRadio_MaxSupportedAssociations, NULL, NULL, NULL},
{"FragmentationThreshold", &DMWRITE, DMT_UNINT, get_WiFiRadio_FragmentationThreshold, set_WiFiRadio_FragmentationThreshold, NULL, NULL},
{"RTSThreshold", &DMWRITE, DMT_UNINT, get_WiFiRadio_RTSThreshold, set_WiFiRadio_RTSThreshold, NULL, NULL},
{"BeaconPeriod", &DMWRITE, DMT_UNINT, get_WiFiRadio_BeaconPeriod, set_WiFiRadio_BeaconPeriod, NULL, NULL},
{"DTIMPeriod", &DMWRITE, DMT_UNINT, get_WiFiRadio_DTIMPeriod, set_WiFiRadio_DTIMPeriod, NULL, NULL},
{"SupportedOperatingChannelBandwidths", &DMREAD, DMT_STRING, get_WiFiRadio_SupportedOperatingChannelBandwidths, NULL, NULL, NULL},
{"OperatingChannelBandwidth", &DMWRITE, DMT_STRING, get_WiFiRadio_OperatingChannelBandwidth, set_WiFiRadio_OperatingChannelBandwidth, NULL, NULL},
{"CurrentOperatingChannelBandwidth", &DMREAD, DMT_STRING, get_WiFiRadio_CurrentOperatingChannelBandwidth, NULL, NULL, NULL},
{"PreambleType", &DMWRITE, DMT_STRING, get_WiFiRadio_PreambleType, set_WiFiRadio_PreambleType, NULL, NULL},
{"IEEE80211hSupported", &DMREAD, DMT_BOOL, get_WiFiRadio_IEEE80211hSupported, NULL, NULL, NULL},
{"IEEE80211hEnabled", &DMWRITE, DMT_BOOL, get_WiFiRadio_IEEE80211hEnabled, set_WiFiRadio_IEEE80211hEnabled, NULL, NULL},
{"TransmitPower", &DMWRITE, DMT_INT, get_WiFiRadio_TransmitPower, set_WiFiRadio_TransmitPower, NULL, NULL},
{"RegulatoryDomain", &DMWRITE, DMT_STRING, get_WiFiRadio_RegulatoryDomain, set_WiFiRadio_RegulatoryDomain, NULL, NULL},
{0}
};

/*** WiFi.Radio.Stats. ***/
DMLEAF tWifiRadioStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_packets, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_errors, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_errors, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_discardpackets, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_discardpackets, NULL, NULL, NULL},
{"Noise", &DMREAD, DMT_INT, get_WiFiRadioStats_Noise, NULL, NULL, NULL},
{0}
};

/*** WiFi.SSID. ***/
DMOBJ tWifiSsidStatsObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiSsidStatsParams, NULL},
{0}
};


DMLEAF tWifiSsidParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_ssid_alias, set_ssid_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_wifi_enable, set_wifi_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_wifi_status, NULL, NULL, NULL},
{"SSID", &DMWRITE, DMT_STRING, get_wlan_ssid, set_wlan_ssid, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING,  get_wlan_ssid, set_wlan_ssid, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_ssid_lower_layer, set_ssid_lower_layer, NULL, NULL},
{"BSSID", &DMREAD, DMT_STRING, get_wlan_bssid, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_WiFiSSID_MACAddress, NULL, NULL, NULL},
{0}
};

/*** WiFi.SSID.Stats. ***/
DMLEAF tWifiSsidStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_ssid_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_ssid_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_ssid_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_ssid_statistics_rx_packets, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_ErrorsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_WiFiSSIDStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint. ***/
DMOBJ tAcessPointSecurityObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Security", &DMWRITE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiAcessPointSecurityParams, NULL},
{"AssociatedDevice", &DMREAD, NULL, NULL, NULL, browse_wifi_associated_device, NULL, NULL, tWifiAcessPointAssociatedDeviceObj, tWifiAcessPointAssociatedDeviceParams, get_linker_associated_device},
{"WPS", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWiFiAccessPointWPSParams, NULL},
{"Accounting", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWiFiAccessPointAccountingParams, NULL},
{CUSTOM_PREFIX"IEEE80211r", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiAcessPointIEEE80211rParams, NULL},
{0}
};

DMLEAF tWifiAcessPointParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_access_point_alias, set_access_point_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL,  get_wifi_enable, set_wifi_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_wifi_status, NULL, NULL, NULL},
{"SSIDReference", &DMREAD, DMT_STRING, get_ap_ssid_ref, NULL, NULL, NULL},
{"SSIDAdvertisementEnabled", &DMWRITE, DMT_BOOL, get_wlan_ssid_advertisement_enable, set_wlan_ssid_advertisement_enable, NULL, NULL},
{"WMMEnable", &DMWRITE, DMT_BOOL, get_wmm_enabled, set_wmm_enabled, NULL, NULL},
{"UAPSDEnable", &DMWRITE, DMT_BOOL, get_WiFiAccessPoint_UAPSDEnable, set_WiFiAccessPoint_UAPSDEnable, NULL, NULL},
{"AssociatedDeviceNumberOfEntries", &DMREAD, DMT_UNINT, get_access_point_total_associations, NULL, NULL, NULL},
{"MaxAssociatedDevices", &DMWRITE, DMT_UNINT, get_access_point_maxassoc, set_access_point_maxassoc, NULL, NULL},
{"MACAddressControlEnabled", &DMWRITE, DMT_BOOL, get_access_point_control_enable, set_access_point_control_enable, NULL, NULL},
{"UAPSDCapability", &DMREAD, DMT_BOOL, get_WiFiAccessPoint_UAPSDCapability, NULL, NULL, NULL},
{"WMMCapability", &DMREAD, DMT_BOOL, get_WiFiAccessPoint_WMMCapability, NULL, NULL, NULL},
{"MaxAllowedAssociations", &DMWRITE, DMT_UNINT, get_WiFiAccessPoint_MaxAllowedAssociations, set_WiFiAccessPoint_MaxAllowedAssociations, NULL, NULL},
{"IsolationEnable", &DMWRITE, DMT_BOOL, get_WiFiAccessPoint_IsolationEnable, set_WiFiAccessPoint_IsolationEnable, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint.Security. ***/
DMLEAF tWifiAcessPointSecurityParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ModesSupported", &DMREAD, DMT_STRING, get_access_point_security_supported_modes, NULL, NULL, NULL},
{"ModeEnabled", &DMWRITE, DMT_STRING ,get_access_point_security_modes, set_access_point_security_modes, NULL, NULL},
{"WEPKey", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_wepkey, NULL, NULL},
{CUSTOM_PREFIX"WEPKeyIndex", &DMWRITE, DMT_UNINT, get_access_point_security_wepkey_index, set_access_point_security_wepkey_index, NULL, NULL},
{"PreSharedKey", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_shared_key, NULL, NULL},
{"KeyPassphrase", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_passphrase, NULL, NULL},
{"RekeyingInterval", &DMWRITE, DMT_UNINT, get_access_point_security_rekey_interval, set_access_point_security_rekey_interval, NULL, NULL},
{"RadiusServerIPAddr", &DMWRITE, DMT_STRING, get_access_point_security_radius_ip_address, set_access_point_security_radius_ip_address, NULL, NULL},
{"RadiusServerPort", &DMWRITE, DMT_UNINT, get_access_point_security_radius_server_port, set_access_point_security_radius_server_port, NULL, NULL},
{"RadiusSecret", &DMWRITE, DMT_STRING,get_empty, set_access_point_security_radius_secret, NULL, NULL},
{"MFPConfig", &DMWRITE, DMT_STRING, get_WiFiAccessPointSecurity_MFPConfig, set_WiFiAccessPointSecurity_MFPConfig, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint.AssociatedDevice. ***/
DMLEAF tWifiAcessPointAssociatedDeviceParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Active", &DMREAD, DMT_BOOL, get_access_point_associative_device_active, NULL, NULL, NULL},
{"Noise", &DMREAD, DMT_INT, get_WiFiAccessPointAssociatedDevice_Noise, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING ,get_access_point_associative_device_mac, NULL, NULL, NULL},
{"LastDataDownlinkRate", &DMREAD, DMT_UNINT, get_access_point_associative_device_lastdatadownlinkrate, NULL, NULL, NULL},
{"LastDataUplinkRate", &DMREAD, DMT_UNINT, get_access_point_associative_device_lastdatauplinkrate, NULL, NULL, NULL},
{"SignalStrength", &DMREAD, DMT_INT, get_access_point_associative_device_signalstrength, NULL, NULL, NULL},
{"Retransmissions", &DMREAD, DMT_UNINT, get_WiFiAccessPointAssociatedDevice_Retransmissions, NULL, NULL, NULL},
{"AssociationTime", &DMREAD, DMT_TIME, get_WiFiAccessPointAssociatedDevice_AssociationTime, NULL, NULL, NULL},
{0}
};

/* *** Device.WiFi.AccessPoint.{i}.WPS. *** */
DMLEAF tWiFiAccessPointWPSParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_WiFiAccessPointWPS_Enable, set_WiFiAccessPointWPS_Enable, NULL, NULL},
{"ConfigMethodsSupported", &DMREAD, DMT_STRING, get_WiFiAccessPointWPS_ConfigMethodsSupported, NULL, NULL, NULL},
{"ConfigMethodsEnabled", &DMWRITE, DMT_STRING, get_WiFiAccessPointWPS_ConfigMethodsEnabled, set_WiFiAccessPointWPS_ConfigMethodsEnabled, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_WiFiAccessPointWPS_Status, NULL, NULL, NULL},
{"Version", &DMREAD, DMT_STRING, get_WiFiAccessPointWPS_Version, NULL, NULL, NULL},
{"PIN", &DMWRITE, DMT_STRING, get_WiFiAccessPointWPS_PIN, set_WiFiAccessPointWPS_PIN, NULL, NULL},
{0}
};

/* *** Device.WiFi.AccessPoint.{i}.Accounting. *** */
DMLEAF tWiFiAccessPointAccountingParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
//{"Enable", &DMWRITE, DMT_BOOL, get_WiFiAccessPointAccounting_Enable, set_WiFiAccessPointAccounting_Enable, NULL, NULL},
{"ServerIPAddr", &DMWRITE, DMT_STRING, get_WiFiAccessPointAccounting_ServerIPAddr, set_WiFiAccessPointAccounting_ServerIPAddr, NULL, NULL},
//{"SecondaryServerIPAddr", &DMWRITE, DMT_STRING, get_WiFiAccessPointAccounting_SecondaryServerIPAddr, set_WiFiAccessPointAccounting_SecondaryServerIPAddr, NULL, NULL},
{"ServerPort", &DMWRITE, DMT_UNINT, get_WiFiAccessPointAccounting_ServerPort, set_WiFiAccessPointAccounting_ServerPort, NULL, NULL},
//{"SecondaryServerPort", &DMWRITE, DMT_UNINT, get_WiFiAccessPointAccounting_SecondaryServerPort, set_WiFiAccessPointAccounting_SecondaryServerPort, NULL, NULL},
{"Secret", &DMWRITE, DMT_STRING, get_WiFiAccessPointAccounting_Secret, set_WiFiAccessPointAccounting_Secret, NULL, NULL},
//{"SecondarySecret", &DMWRITE, DMT_STRING, get_WiFiAccessPointAccounting_SecondarySecret, set_WiFiAccessPointAccounting_SecondarySecret, NULL, NULL},
//{"InterimInterval", &DMWRITE, DMT_UNINT, get_WiFiAccessPointAccounting_InterimInterval, set_WiFiAccessPointAccounting_InterimInterval, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint.X_IOPSYS_EU_IEEE80211r. ***/
DMLEAF tWifiAcessPointIEEE80211rParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_access_point_ieee80211r_enable, set_access_point_ieee80211r_enable, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint.AssociatedDevice.Stats. ***/
DMOBJ tWifiAcessPointAssociatedDeviceObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiAcessPointAssociatedDeviceStatsParams, NULL},
{0}
};

/*** WiFi.AccessPoint.AssociatedDevice.Stats. ***/
DMLEAF tWifiAcessPointAssociatedDeviceStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_rx_packets, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_tx_errors, NULL, NULL, NULL},
{"RetransCount", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_retrans_count, NULL, NULL, NULL},
{"FailedRetransCount", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_failed_retrans_count, NULL, NULL, NULL},
{"RetryCount", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_retry_count, NULL, NULL, NULL},
{"MultipleRetryCount", &DMREAD, DMT_UNINT, get_access_point_associative_device_statistics_multiple_retry_count, NULL, NULL, NULL},
{0}
};

/*** WiFi.NeighboringWiFiDiagnostic. ***/
DMOBJ tNeighboringWiFiDiagnosticObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{"Result", &DMREAD, NULL, NULL, NULL, browseWifiNeighboringWiFiDiagnosticResultInst, NULL, NULL, NULL, tNeighboringWiFiDiagnosticResultParams, NULL},
{0}
};

DMLEAF tNeighboringWiFiDiagnosticParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_neighboring_wifi_diagnostics_diagnostics_state, set_neighboring_wifi_diagnostics_diagnostics_state, NULL, NULL},
{"ResultNumberOfEntries", &DMREAD, DMT_UNINT, get_neighboring_wifi_diagnostics_result_number_entries, NULL, NULL, NULL},
{0}
};

/*** WiFi.NeighboringWiFiDiagnostic.Result. ***/
DMLEAF tNeighboringWiFiDiagnosticResultParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"SSID", &DMREAD, DMT_STRING, get_neighboring_wifi_diagnostics_result_ssid, NULL, NULL, NULL},
{"BSSID", &DMREAD, DMT_STRING, get_neighboring_wifi_diagnostics_result_bssid, NULL, NULL, NULL},
{"Channel", &DMREAD, DMT_UNINT, get_neighboring_wifi_diagnostics_result_channel, NULL, NULL, NULL},
{"SignalStrength", &DMREAD, DMT_INT, get_neighboring_wifi_diagnostics_result_signal_strength, NULL, NULL, NULL},
{"OperatingFrequencyBand", &DMREAD, DMT_STRING, get_neighboring_wifi_diagnostics_result_operating_frequency_band, NULL, NULL, NULL},
{"Noise", &DMREAD, DMT_INT, get_neighboring_wifi_diagnostics_result_noise, NULL, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_Wifi_Radio(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct wifi_radio_args *)data)->wifi_radio_sec) {
		*linker = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
		return 0;
	}
	*linker = "";
	return 0;
}

int get_linker_Wifi_Ssid(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct wifi_ssid_args *)data)->ifname) {
		*linker = ((struct wifi_ssid_args *)data)->ifname;
		return 0;
	}
	*linker = "";
	return 0;
}

int get_linker_associated_device(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	struct wifi_associative_device_args* cur_wifi_associative_device_args = (struct wifi_associative_device_args*)data;
	if(data && cur_wifi_associative_device_args->macaddress) {
		*linker= cur_wifi_associative_device_args->macaddress;
		return 0;
	}
	*linker = "";
	return 0;
}
/**************************************************************************
* INIT
***************************************************************************/
inline int init_wifi_radio(struct wifi_radio_args *args, struct uci_section *s)
{
	args->wifi_radio_sec = s;
	return 0;
}

inline int init_wifi_ssid(struct wifi_ssid_args *args, struct uci_section *s, char *wiface, char *linker)
{
	args->wifi_ssid_sec = s;
	args->ifname = wiface;
	args->linker = linker;
	return 0;
}

inline int init_wifi_acp(struct wifi_acp_args *args, struct uci_section *s, char *wiface)
{
	args->wifi_acp_sec = s;
	args->ifname = wiface;
	return 0;
}

/**************************************************************************
* SET & GET VALUE
***************************************************************************/
int get_WiFi_RadioNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;

	uci_foreach_sections("wireless", "wifi-device", s) {
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_WiFi_SSIDNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;

	uci_foreach_sections("wireless", "wifi-iface", s) {
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_WiFi_AccessPointNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;

	uci_foreach_sections("wireless", "wifi-iface", s) {
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "enabled", value);
	return 0;
}

int set_wifi_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				dmuci_set_value("wireless", "bandsteering", "enabled", "1");
			}
			else {
				dmuci_set_value("wireless", "bandsteering", "enabled", "0");
			}
			return 0;
	}
	return 0;
}

int get_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "disabled", &val);
	if ((val[0] == '\0') || (val[0] == '0'))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wifi_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "disabled", "0");
			else
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "disabled", "1");
			return 0;
	}
	return 0;
}

int get_wifi_status (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "disabled", value);
	if ((*value)[0] == '\0' || (*value)[0] == '0')
		*value = "Up";
	else
		*value = "Down";
	return 0;
}

static int get_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "ssid", value);
	return 0;
}

static int set_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "ssid", value);
			return 0;
	}
	return 0;
}

static int get_wlan_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *wlan_name;
	json_object *res;

	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "device", &wlan_name);
	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "bssid");
	return 0;
}

int get_WiFiSSID_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "macaddr");
	return 0;
}

int get_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "disabled", &val);

	if (val[0] == '1')
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "disabled", "0");
			else
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "disabled", "1");
			return 0;
	}
	return 0;
}

int get_radio_status (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "disabled", value);
	if ((*value)[0] == '1')
		*value = "Down";
	else
		*value = "Up";
	return 0;
}

int get_WiFiRadio_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value="";
	return 0;
}

int set_WiFiRadio_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			return FAULT_9007;
		case VALUESET:
			break;
	}
	return 0;
}

int get_WiFiRadio_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmasprintf(value, "%s", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec));
	return 0;
}

int get_radio_max_bit_rate (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *wlan_name, *rate;

	*value = "";
	wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res);
	if(res) {
		rate = dmjson_get_value(res, 2, wlan_name, "rate");
		*value = strtok(rate, " Mbps");
	}
	return 0;
}

int get_radio_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *freq;
	json_object *res;
	char *wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	freq = dmjson_get_value(res, 1, "frequency");
	if(strcmp(freq, "2") == 0 ) {
		dmastrcat(value, freq, ".4GHz");  // MEM WILL BE FREED IN DMMEMCLEAN
		return 0;
	}
	dmastrcat(value, freq, "GHz");  // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_radio_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *bandwith;
	json_object *res;
	char *wlan_name;
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "bandwidth", value);
	if (value[0] == '\0')
	{
		wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
		dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		bandwith = dmjson_get_value(res, 1, "bandwidth");
		dmastrcat(value, bandwith, "MHz"); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	else
		dmastrcat(value, *value, "MHz"); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_radio_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *spch, *dup;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dup = dmstrdup(value);
			pch = strtok_r(dup, "Mm", &spch);
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "bandwidth", pch);
			dmfree(dup);
			return 0;
	}
	return 0;
}


int get_radio_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "maxassoc", value);
	return 0;
}

int set_radio_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "maxassoc", value);
			return 0;
	}
	return 0;
}

int get_radio_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "band", &val);
	if (val[0] == 'a') {
		dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "dfsc", value);
		if ((*value)[0] == '\0')
			*value = "0";
	}
	return 0;
}

int set_radio_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *val;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "band", &val);
			if (val[0] == 'a') {
				string_to_bool(value, &b);
				if (b)
					dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "dfsc", "1");
				else
					dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "dfsc", "0");
			}
			return 0;
	}
	return 0;
}

int get_radio_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *freq, *wlan_name;
	json_object *res;

	wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res); //{"vif", wlan_name, String}
	json_object_object_foreach(res, key, radio_obj) {
		if(strcmp(wlan_name, key) == 0) {
			*value = dmjson_get_value_array_all(radio_obj, DELIMITOR, 1, "hwmodes");
		}
	}
	return 0;
}

int get_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res = NULL;
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "");
	json_object_object_foreach(res, key, radio_obj) {
		if(strcmp(section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), key) == 0) {
			*value = dmjson_get_value(radio_obj, 1, "opmode");
			break;
		}
	}
	return 0;
}

int set_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *freq, *wlan_name;
	json_object *res;
	switch (action) {
			case VALUECHECK:
				return 0;
			case VALUESET:
				wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
				dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
				freq = dmjson_get_value(res, 1, "frequency");
				if (strcmp(freq, "5") == 0) {
					 if (strcmp(value, "n") == 0)
						value = "11n"; 
					 else if (strcmp(value, "ac") == 0)
						value = "11ac";
					dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "hwmode", value);
				} else {
					if (strcmp(value, "b") == 0)
						value = "11b";
					else if (strcmp(value, "b,g") == 0 || strcmp(value, "g,b") == 0)
						value = "11bg";
					else if (strcmp(value, "g") == 0)
						value = "11g";
					 else if (strcmp(value, "n") == 0)
						value = "11n";
					dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "hwmode", value);
				}
				return 0;
		}
		return 0;
}

int get_radio_possible_channels(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *wlan_name;
	
	*value = "";
	wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res);
	if(res)
		*value = dmjson_get_value_array_all(res, DELIMITOR, 2, wlan_name, "channels");
	return 0;
}

int get_WiFiRadio_AutoChannelSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value="true";
	return 0;
}

int get_WiFiRadio_AutoChannelRefreshPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "scantimer", value);
	return 0;
}

int set_WiFiRadio_AutoChannelRefreshPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "scantimer", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_MaxSupportedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "maxassoc", value);
	return 0;
}

int get_WiFiRadio_FragmentationThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "frag", value);
	return 0;
}

int set_WiFiRadio_FragmentationThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "frag", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_RTSThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "rts", value);
	return 0;
}

int set_WiFiRadio_RTSThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "rts", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_BeaconPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "beacon_int", value);
	return 0;
}

int set_WiFiRadio_BeaconPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "beacon_int", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_DTIMPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "dtim_period", value);
	return 0;
}

int set_WiFiRadio_DTIMPeriod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "dtim_period", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_SupportedOperatingChannelBandwidths(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *jobj;

	dmubus_call("router.wireless", "radios", UBUS_ARGS{{}}, 0, &res);
	if(res)
		*value = dmjson_get_value_array_all(res, DELIMITOR, 2, section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), "bwcaps");

	return 0;
}

int get_WiFiRadio_OperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "bandwidth", value);
	if(*value[0] == '\0') {
		*value= "";
		return 0;
	}
	dmastrcat(value, *value, "MHz");
	return 0;
}

int set_WiFiRadio_OperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "bandwidth", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_CurrentOperatingChannelBandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	if(res)
		*value = dmjson_get_value(res, 1, "bandwidth");
	return 0;
}

int get_WiFiRadio_PreambleType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *preamble= NULL;

	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "short_preamble", &preamble);
	if (preamble[0] == '\0' || strlen(preamble) == 0 || strcmp(preamble, "1") != 0)
		*value= "long";
	else
		*value= "short";
	return 0;
}

int set_WiFiRadio_PreambleType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			printf("%s:%s line %d value: %s\n", __FILE__, __FUNCTION__, __LINE__, value);
			if(strcmp(value, "short") == 0)
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "short_preamble", "1");
			else
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "short_preamble", "0");
			break;
	}
	return 0;
}

int get_WiFiRadio_IEEE80211hSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "true";
	return 0;
}

int get_WiFiRadio_IEEE80211hEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "doth", value);
	return 0;
}

int set_WiFiRadio_IEEE80211hEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "doth", "1");
			else
				dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "doth", "0");
			break;
	}
	return 0;
}

int get_WiFiRadio_TransmitPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "txpower", value);
	return 0;
}

int set_WiFiRadio_TransmitPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "txpower", value);
			break;
	}
	return 0;
}

int get_WiFiRadio_RegulatoryDomain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *country, **arr;
	int length;

	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "country", &country);
	arr= strsplit(country, "/", &length);
	if(strlen(arr[0]) > 0)
		dmasprintf(value, "%s", arr[0]);
	else
		*value= "";

	return 0;
}

int set_WiFiRadio_RegulatoryDomain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *country, **arr;
	int length;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "country", &country);
			if(strlen(country)>0){
				arr= strsplit(country, "/", &length);
				dmasprintf(&country, "%s/%s", value, arr[1]);
			} else
				dmasprintf(&country, "%s/1", value);
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "country", country);
			break;
	}
	return 0;
}

int get_radio_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *wlan_name;
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0') {
		wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
		dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		*value = dmjson_get_value(res, 1, "channel");
	}
	return 0;
}

int set_radio_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "channel", value);
			return 0;
	}
	return 0;
}

int get_radio_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_radio_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	json_object *res;
	char *wlan_name;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "auto";
			else {
				wlan_name = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
				dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
				if(res == NULL)
					return 0;
				else
					value = dmjson_get_value(res, 1, "channel");
			}
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "channel", value);
			return 0;
	}
	return 0;
}

/*************************************************************
 * GET STAT
/*************************************************************/
int get_radio_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_radio_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_radio_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_radio_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

int get_radio_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_errors");
	return 0;
}

int get_radio_statistics_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_errors");
	return 0;
}

int get_radio_statistics_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_dropped");
	return 0;
}

int get_radio_statistics_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_dropped");
	return 0;
}

int get_WiFiRadioStats_Noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	if(res)
		*value = dmjson_get_value(res, 1, "noise");
	return 0;
	return 0;
}

int get_ssid_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_ssid_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_ssid_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_ssid_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

int get_WiFiSSIDStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_errors");
	return 0;
}

int get_WiFiSSIDStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_errors");
	return 0;
}

int get_WiFiSSIDStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "tx_dropped");
	return 0;
}

int get_WiFiSSIDStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", "rx_dropped");
	return 0;
}

int get_WiFiSSIDStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "TX", "unicast"));
	return 0;
}

int get_WiFiSSIDStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "RX", "unicast"));
	return 0;
}

int get_WiFiSSIDStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "TX", "multicast"));
	return 0;
}

int get_WiFiSSIDStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "RX", "multicast"));
	return 0;
}

int get_WiFiSSIDStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "TX", "broadcast"));
	return 0;
}

int get_WiFiSSIDStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->ifname && strlen(((struct wifi_ssid_args *)data)->ifname)>0)
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(((struct wifi_ssid_args *)data)->ifname, "RX", "broadcast"));
	return 0;
}

static char *get_associative_device_statistics(struct wifi_associative_device_args *wifi_associative_device, char *key)
{
	json_object *res, *jobj;
	char *macaddr, *stats = "0";
	int entries = 0;

	dmubus_call("wifix", "stations", UBUS_ARGS{{"vif", wifi_associative_device->wdev, String}}, 1, &res);
	while (res) {
		jobj = dmjson_select_obj_in_array_idx(res, entries, 1, "stations");
		if(jobj) {
			macaddr = dmjson_get_value(jobj, 1, "macaddr");
			if (!strcmp(macaddr, wifi_associative_device->macaddress)) {
				stats = dmjson_get_value(jobj, 2, "stats", key);
				if(*stats != '\0')
					return stats;
			}
			entries++;
		}
		else
			break;
	}
	return stats;
}

int get_access_point_associative_device_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_total_bytes");
	return 0;
}

int get_access_point_associative_device_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "rx_data_bytes");
	return 0;
}

int get_access_point_associative_device_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_total_pkts");
	return 0;
}

int get_access_point_associative_device_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "rx_data_pkts");
	return 0;
}

int get_access_point_associative_device_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_failures");
	return 0;
}

int get_access_point_associative_device_statistics_retrans_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_pkts_retries");
	return 0;
}

int get_access_point_associative_device_statistics_failed_retrans_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_pkts_retry_exhausted");
	return 0;
}

int get_access_point_associative_device_statistics_retry_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_pkts_retries");
	return 0;
}

int get_access_point_associative_device_statistics_multiple_retry_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr = (struct wifi_associative_device_args*)data;

	*value = get_associative_device_statistics(cur_wifi_associative_device_args_ptr, "tx_data_pkts_retried");
	return 0;
}

/**************************************************************************
* SET & GET VALUE
***************************************************************************/
static int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *hidden;
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "hidden", &hidden);
	if (hidden[0] == '1' && hidden[1] == '\0')
		*value = "0";
	else
		*value = "1";
	return 0;
}

static int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "hidden", "");
			else
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "hidden", "1");
			return 0;

	}
	return 0;
}

static int get_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	bool b;
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", value);
	dmuci_get_option_value_string("wireless", *value, "wmm", value);
	string_to_bool(*value, &b);
		if (b)
			*value = "true";
		else
			*value = "false";

	return 0;
}

static int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *device;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", &device);
			if (b) {
				dmuci_set_value("wireless", device, "wmm", "1");
				dmuci_set_value("wireless", device, "wmm_noack", "1");
				dmuci_set_value("wireless", device, "wmm_apsd", "1");
			}
			else {
				dmuci_set_value("wireless", device, "wmm", "0");
				dmuci_set_value("wireless", device, "wmm_noack", "");
				dmuci_set_value("wireless", device, "wmm_apsd", "");
			}
			return 0;
	}
	return 0;
}

int get_access_point_total_associations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *jobj;
	int entries = 0;
	dmubus_call("wifix", "stations", UBUS_ARGS{{"vif", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);

	while (1) {
		jobj = dmjson_select_obj_in_array_idx(res, entries, 1, "stations");
		if (jobj == NULL)
			break;
		entries++;
	}
	dmasprintf(value, "%d", entries); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_access_point_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "device", &device);
	dmuci_get_option_value_string("wireless", device, "maxassoc", value);
	return 0;
}

int set_access_point_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *device;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", &device);
			dmuci_set_value("wireless", device, "maxassoc", value);
			return 0;
	}
	return 0;
}

int get_access_point_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *macfilter;
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "macfilter", &macfilter);
	if (macfilter[0] != '0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int get_WiFiAccessPoint_WMMCapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "true";
	return 0;
}

int get_WiFiAccessPoint_MaxAllowedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *device;
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", &device);
	dmuci_get_option_value_string("wireless", device, "maxassoc", value);
	return 0;
}

int set_WiFiAccessPoint_MaxAllowedAssociations(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *device;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", &device);
			dmuci_set_value("wireless", device, "maxassoc", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPoint_IsolationEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "isolate", value);
	return 0;
}

int set_WiFiAccessPoint_IsolationEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "isolate", "1");
			else
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "isolate", "0");
			break;
	}
	return 0;
}

int get_WiFiAccessPoint_UAPSDCapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "true";
	return 0;
}

int set_access_point_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "2";
			else
				value = "0";
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "macfilter", value);
			return 0;
	}
	return 0;
}

int get_WiFiAccessPoint_UAPSDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "wmm_apsd", value);
	return 0;
}

int set_WiFiAccessPoint_UAPSDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "1";
			else
				value = "0";
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "wmm_apsd", value);
			break;
	}
	return 0;
}
int get_access_point_security_supported_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "None, WEP-64, WEP-128, WPA-Personal, WPA2-Personal, WPA-WPA2-Personal, WPA-Enterprise, WPA2-Enterprise, WPA-WPA2-Enterprise";
	return 0;
}

static void get_value_security_mode(char **value, char *encryption, char *cipher)
{
	if (strcmp(encryption, "none") == 0)
		*value = "None";
	else if (strcmp(encryption, "wep-open") == 0 || strcmp(encryption, "wep-shared") == 0)
		*value = "WEP-64";
	else if (strcmp(encryption, "psk") == 0)
		*value = "WPA-Personal";
	else if (strcmp(encryption, "wpa") == 0)
		*value = "WPA-Enterprise";
	else if ((strcmp(encryption, "psk2") == 0 && strcmp(cipher, "auto") == 0) || (strcmp(encryption, "psk2") == 0 && strcmp(cipher, "ccmp") == 0) || (strcmp(encryption, "psk2") == 0 && *cipher == '\0'))
		*value = "WPA2-Personal";
	else if (strcmp(encryption, "wpa2") == 0)
		*value = "WPA2-Enterprise";
	else if ((strcmp(encryption, "mixed-psk") == 0 && strcmp(cipher, "auto") == 0) || (strcmp(encryption, "mixed-psk") == 0 && strcmp(cipher, "ccmp") == 0) || (strcmp(encryption, "mixed-psk") == 0 && strcmp(cipher, "tkip+ccmp") == 0) || (strcmp(encryption, "mixed-psk") == 0 && *cipher == '\0'))
		*value = "WPA-WPA2-Personal";
	else if (strcmp(encryption, "wpa-mixed") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "WPA-WPA2-Enterprise";
}

int get_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption, *cipher, *mode;

	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", &cipher);
	if (*encryption == '\0' && *cipher == '\0') {
		*value = "None";
		return 0;
	}
	else
		get_value_security_mode(&mode, encryption, cipher);

	*value = mode;
	return 0;
}

int set_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *option, *gnw;
	char *encryption, *cipher, *mode;
	char strk64[4][11];

	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", &cipher);
	get_value_security_mode(&mode, encryption, cipher);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, mode) != 0) {
				if (strcmp(value, "None") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "none");
				}
				else if (strcmp(value, "WEP-64") == 0 || strcmp(value, "WEP-128") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "wep-open");
					wepkey64("Iopsys", strk64);
					int i = 0;
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, option, strk64[i]);
						dmfree(option);
						i++;
					}
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", "1");
				}
				else if (strcmp(value, "WPA-Personal") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "psk");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", gnw);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", "tkip");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				else if (strcmp(value, "WPA-Enterprise") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "wpa");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", "");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", "1812");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", "");
				}
				else if (strcmp(value, "WPA2-Personal") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "psk2");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", gnw);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", "ccmp");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "gtk_rekey", "3600");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "wps_pbc", "1");
					dmfree(gnw);
				}
				else if (strcmp(value, "WPA2-Enterprise") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "wpa2");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", "");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", "1812");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", "");
				}
				else if (strcmp(value, "WPA-WPA2-Personal") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "mixed-psk");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", gnw);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", "tkip+ccmp");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "gtk_rekey", "3600");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "wps_pbc", "1");
					dmfree(gnw);
				}
				else if (strcmp(value, "WPA-WPA2-Enterprise") == 0) {
					reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "wpa-mixed");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", "");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", "1812");
					dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", "");
				}
			}
			return 0;
	}
	return 0;
}

int set_access_point_security_wepkey(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *key_index, *encryption;
	char buf[8];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "wep-open") == 0 || strcmp(encryption, "wep-shared") == 0 ) {
				dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "key_index", &key_index);
				sprintf(buf,"key%s", key_index);
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, buf, value);
			}
			return 0;
	}
	return 0;
}

int get_access_point_security_wepkey_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *key_index;

	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "key_index", &key_index);
	if (*key_index == '\0')
		*value = "1";
	else
		*value = key_index;
	return 0;
}

int set_access_point_security_wepkey_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value)>=1 && atoi(value)<=4) {
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key_index", value);
			}
			return 0;
	}
	return 0;
}

int set_access_point_security_shared_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "psk") == 0 || strcmp(encryption, "psk2") == 0 || strcmp(encryption, "mixed-psk") == 0 ) {
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", value);
			}
			return 0;
	}
	return 0;
}

int set_access_point_security_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "psk") == 0 || strcmp(encryption, "psk2") == 0 || strcmp(encryption, "mixed-psk") == 0 ) {
				set_access_point_security_shared_key(refparam, ctx, data, instance, value, action);
			}
			return 0;
	}
	return 0;
}

int get_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "gtk_rekey", value);
	return 0;
}

int set_access_point_security_rekey_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *key_index, *encryption;
	char buf[8];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "wep-open") == 0 || strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "none") == 0)
				return 0;
			else {
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "gtk_rekey", value);
			}
			return 0;
	}
	return 0;
}

int get_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", value);
	return 0;
}

int set_access_point_security_radius_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *key_index, *encryption;
	char buf[8];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0 || strcmp(encryption, "wpa-mixed") == 0)
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", value);
			return 0;
	}
	return 0;
}

int get_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", value);
	return 0;
}

int set_access_point_security_radius_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *key_index, *encryption;
	char buf[8];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0 || strcmp(encryption, "wpa-mixed") == 0)
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", value);
			return 0;
	}
	return 0;
}

int set_access_point_security_radius_secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *key_index, *encryption;
	char buf[8];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0 || strcmp(encryption, "wpa-mixed") == 0)
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", value);
			return 0;
	}
	return 0;
}

int get_WiFiAccessPointSecurity_MFPConfig(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "ieee80211w", value);
	return 0;
}

int set_WiFiAccessPointSecurity_MFPConfig(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "ieee80211w", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("wireless", "status", "wps", value);
	return 0;
}

int set_WiFiAccessPointWPS_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *boolS;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value("wireless", "status", "wps", "1");
			else
				dmuci_set_value("wireless", "status", "wps", "0");
			break;
	}
	return 0;
}

int get_WiFiAccessPointWPS_ConfigMethodsSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "PushButton";
	return 0;
}

int get_WiFiAccessPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *pushbut;
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "wps_pushbutton", &pushbut);
	if(strcmp(pushbut, "1") == 0)
		*value= "PushButton";
	else
		*value= "";
	return 0;
}

int set_WiFiAccessPointWPS_ConfigMethodsEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_WiFiAccessPointWPS_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *wps_status;
	dmuci_get_option_value_string("wireless", "wifi-status", "wps", &wps_status);
	if(strcmp(wps_status, "0") == 0)
		*value= "Disabled";
	else
		*value= "Configured";
	return 0;
}

int get_WiFiAccessPointWPS_Version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_WiFiAccessPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "wps_pin", value);
	return 0;
}

int set_WiFiAccessPointWPS_PIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "wps_pin", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_WiFiAccessPointAccounting_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_ServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_server", value);
	return 0;
}

int set_WiFiAccessPointAccounting_ServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_server", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_SecondaryServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_WiFiAccessPointAccounting_SecondaryServerIPAddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_ServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_port", value);
	return 0;
}

int set_WiFiAccessPointAccounting_ServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_port", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_SecondaryServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_WiFiAccessPointAccounting_SecondaryServerPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_Secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_secret", value);
	return 0;
}

int set_WiFiAccessPointAccounting_Secret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "acct_secret", value);
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_SecondarySecret(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_WiFiAccessPointAccounting_SecondarySecret(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_WiFiAccessPointAccounting_InterimInterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_WiFiAccessPointAccounting_InterimInterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}


int get_radio_supported_frequency_bands(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "2.4GHz, 5GHz";
	return 0;
}

int get_access_point_associative_device_lastdatadownlinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%d", cur_wifi_associative_device_args_ptr->lastdatadownloadlinkrate);
	return 0;
}

int get_access_point_associative_device_lastdatauplinkrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%d", cur_wifi_associative_device_args_ptr->lastdatauplinkrate);
	return 0;
}

int get_access_point_associative_device_signalstrength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%d", cur_wifi_associative_device_args_ptr->signalstrength);
	return 0;
}

int get_WiFiAccessPointAssociatedDevice_Retransmissions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%d", cur_wifi_associative_device_args_ptr->retransmissions);
	return 0;
}

int get_WiFiAccessPointAssociatedDevice_AssociationTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%s", cur_wifi_associative_device_args_ptr->assoctime?int_period_to_date_time_format(cur_wifi_associative_device_args_ptr->assoctime):"0");
	return 0;
}

int get_access_point_associative_device_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, cur_wifi_associative_device_args_ptr->macaddress);
	return 0;
}

int get_access_point_associative_device_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%s", cur_wifi_associative_device_args_ptr->active?"true":"false");
	return 0;
}


int get_WiFiAccessPointAssociatedDevice_Noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_associative_device_args *cur_wifi_associative_device_args_ptr=(struct wifi_associative_device_args*)data;
	dmasprintf(value, "%d", cur_wifi_associative_device_args_ptr->noise);
	return 0;
}

int get_access_point_ieee80211r_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "ieee80211r", value);
	if ((*value)[0] == '\0')
		*value = "0";
	return 0;
}

int set_access_point_ieee80211r_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "ieee80211r", b?"1":"0");
			return 0;
	}
	return 0;
}

int get_neighboring_wifi_diagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ss;
	json_object *res, *neighboring_wifi_obj;

	uci_foreach_sections("wireless", "wifi-device", ss) {
		dmubus_call("router.wireless", "scanresults", UBUS_ARGS{{"radio", section_name(ss), String}}, 1, &res);
		neighboring_wifi_obj = dmjson_select_obj_in_array_idx(res, 0, 1, "access_points");
		if(neighboring_wifi_obj) {
			*value = "Complete";
			break;
		}
		else
			*value = "None";
	}
	return 0;
}

int set_neighboring_wifi_diagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ss;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				uci_foreach_sections("wireless", "wifi-device", ss) {
					dmubus_call_set("router.wireless", "scan", UBUS_ARGS{{"radio", section_name(ss), String}}, 1);
				}
				dmubus_call_set("tr069", "inform", UBUS_ARGS{{"event", "8 DIAGNOSTICS COMPLETE", String}}, 1);
			}
			return 0;
	}
	return 0;
}

int get_neighboring_wifi_diagnostics_result_number_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ss;
	json_object *res, *jobj;
	int entries = 0, result = 0;
	*value = "0";

	uci_foreach_sections("wireless", "wifi-device", ss) {
		dmubus_call("router.wireless", "scanresults", UBUS_ARGS{{"radio", section_name(ss), String}}, 1, &res);
		while (res) {
			jobj = dmjson_select_obj_in_array_idx(res, entries, 1, "access_points");
			if(jobj)
				entries++;
			else
				break;
		}
		result = result + entries;
		entries = 0;
	}
	dmasprintf(value, "%d", result); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_neighboring_wifi_diagnostics_result_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, cur_wifi_neighboring_diagnostic_args_ptr->ssid);
	return 0;
}

int get_neighboring_wifi_diagnostics_result_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, cur_wifi_neighboring_diagnostic_args_ptr->bssid);
	return 0;
}

int get_neighboring_wifi_diagnostics_result_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, "%d", cur_wifi_neighboring_diagnostic_args_ptr->channel);
	return 0;
}

int get_neighboring_wifi_diagnostics_result_signal_strength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, "%d", cur_wifi_neighboring_diagnostic_args_ptr->signalstrength);
	return 0;
}

int get_neighboring_wifi_diagnostics_result_operating_frequency_band(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, cur_wifi_neighboring_diagnostic_args_ptr->operatingfrequencyband);
	return 0;
}

int get_neighboring_wifi_diagnostics_result_noise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wifi_neighboring_diagnostic_args *cur_wifi_neighboring_diagnostic_args_ptr=(struct wifi_neighboring_diagnostic_args*)data;
	dmasprintf(value, "%d", cur_wifi_neighboring_diagnostic_args_ptr->noise);
	return 0;
}

/**************************************************************************
* SET AND GET ALIAS
***************************************************************************/

static int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_wireless", "wifi-device", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "radioalias", value);
	return 0;
}

static int set_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_wireless", "wifi-device", section_name(((struct wifi_radio_args *)data)->wifi_radio_sec), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "radioalias", value);
			return 0;
	}
	return 0;
}

int get_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(((struct wifi_ssid_args *)data)->wifi_ssid_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ssidalias", value);
	return 0;
}

int set_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(((struct wifi_ssid_args *)data)->wifi_ssid_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "ssidalias", value);
			return 0;
	}
	return 0;
}
int get_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(((struct wifi_acp_args *)data)->wifi_acp_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "accesspointalias", value);
	return 0;
}

int set_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(((struct wifi_acp_args *)data)->wifi_acp_sec), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "accesspointalias", value);
			return 0;
	}
	return 0;
}
/*************************************************************
 * GET & SET LOWER LAYER
/*************************************************************/
int get_ssid_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (((struct wifi_ssid_args *)data)->linker[0] != '\0') {
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cRadio%c", dmroot, dm_delim, dm_delim, dm_delim), ((struct wifi_ssid_args *)data)->linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL)
			*value = "";
	}
	return 0;
}

int set_ssid_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker, *newvalue= NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "device", linker);
				dmfree(linker);
			} else {
				return FAULT_9005;
			}
			return 0;
	}
	return 0;
}

int get_ap_ssid_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cSSID%c", dmroot, dm_delim, dm_delim, dm_delim), ((struct wifi_acp_args *)data)->ifname, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

/*************************************************************
 * ADD DEL OBJ
/*************************************************************/
int add_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value, *v;
	char ssid[16] = {0};
	char *inst;
	struct uci_section *s = NULL;
	struct uci_section *dmmap_wifi=NULL;

	check_create_dmmap_package("dmmap_wireless");
	inst = get_last_instance_icwmpd("dmmap_wireless", "wifi-iface", "ssidinstance");
	sprintf(ssid, "Iopsys_%d", inst ? (atoi(inst)+1) : 1);
	dmuci_add_section_and_rename("wireless", "wifi-iface", &s, &value);
	dmuci_set_value_by_section(s, "device", "wl0");
	dmuci_set_value_by_section(s, "encryption", "none");
	dmuci_set_value_by_section(s, "macfilter", "0");
	dmuci_set_value_by_section(s, "mode", "ap");
	dmuci_set_value_by_section(s, "ssid", ssid);

	dmuci_add_section_icwmpd("dmmap_wireless", "wifi-iface", &dmmap_wifi, &v);
	dmuci_set_value_by_section(dmmap_wifi, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_wifi, inst, "ssidinstance");
	return 0;
}

int delete_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_section= NULL;

	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(((struct wifi_ssid_args *)data)->wifi_ssid_sec), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("wireless", "wifi-iface", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseWifiRadioInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL;
	char buf[12];
	struct uci_section *s = NULL;
	struct wifi_radio_args curr_wifi_radio_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("wireless", "wifi-device", "dmmap_wireless", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		init_wifi_radio(&curr_wifi_radio_args, p->config_section);
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, p->dmmap_section, "radioinstance", "radioalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_radio_args, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseWifiSsidInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *ssid_last = NULL, *ifname, *acpt_last = NULL, *linker;
	struct uci_section *ss = NULL;
	json_object *res;
	struct wifi_ssid_args curr_wifi_ssid_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("wireless", "wifi-iface", "dmmap_wireless", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "ifname", &ifname);
		dmuci_get_value_by_section_string(p->config_section, "device", &linker);
		init_wifi_ssid(&curr_wifi_ssid_args, p->config_section, ifname, linker);
		wnum =  handle_update_instance(1, dmctx, &ssid_last, update_instance_alias, 3, p->dmmap_section, "ssidinstance", "ssidalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_ssid_args, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseWifiAccessPointInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *ssid_last = NULL, *ifname, *acpt_last = NULL;
	struct uci_section *ss = NULL;
	json_object *res;
	struct wifi_acp_args curr_wifi_acp_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("wireless", "wifi-iface", "dmmap_wireless", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "ifname", &ifname);
		init_wifi_acp(&curr_wifi_acp_args, p->config_section, ifname);
		wnum =  handle_update_instance(1, dmctx, &acpt_last, update_instance_alias, 3, p->dmmap_section, "accesspointinstance", "accesspointalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_acp_args, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browse_wifi_associated_device(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	json_object *res, *associated_client_obj;
	struct uci_section *ss = NULL;
	char *value, *ap_ifname, *idx, *idx_last = NULL;
	int id = 0, entries = 0;
	char *macaddr= NULL, *active= NULL, *lastdatadownloadlinkrate= NULL, *lastdatauplinkrate= NULL, *signalstrength= NULL, *noise= NULL, *retrans= NULL, *assoctimestr= NULL;
	struct wifi_associative_device_args cur_wifi_associative_device_args = {0};
	struct uci_section *dmmap_section;

	uci_foreach_sections("wireless", "wifi-iface", ss) {
		get_dmmap_section_of_config_section("dmmap_wireless", "wifi-iface", section_name(ss), &dmmap_section);
		dmuci_get_value_by_section_string(dmmap_section, "accesspointinstance", &value);
		if(!strcmp(value, prev_instance)){
			dmuci_get_value_by_section_string(ss, "ifname", &ap_ifname);
			break;
		}
	}

	dmubus_call("wifix", "stations", UBUS_ARGS{{"vif", ap_ifname, String}}, 1, &res);
	while (res) {
		associated_client_obj = dmjson_select_obj_in_array_idx(res, entries, 1, "stations");
		if(associated_client_obj) {
			cur_wifi_associative_device_args.wdev = ap_ifname;
			macaddr = dmjson_get_value(associated_client_obj, 1, "macaddr");
			if(macaddr!=NULL && strlen(macaddr)>0)
				dmasprintf(&(cur_wifi_associative_device_args.macaddress),dmjson_get_value(associated_client_obj, 1, "macaddr"));
			cur_wifi_associative_device_args.active = 1;
			lastdatadownloadlinkrate = dmjson_get_value(associated_client_obj, 2, "stats", "rate_of_last_rx_pkt");
			if(lastdatadownloadlinkrate!=NULL && strlen(lastdatadownloadlinkrate)>0)
				cur_wifi_associative_device_args.lastdatadownloadlinkrate = atoi(lastdatadownloadlinkrate);
			else
				cur_wifi_associative_device_args.lastdatadownloadlinkrate = 0;
			lastdatauplinkrate = dmjson_get_value(associated_client_obj, 2, "stats", "rate_of_last_tx_pkt");
			if(lastdatauplinkrate!=NULL && strlen(lastdatauplinkrate)>0)
				cur_wifi_associative_device_args.lastdatauplinkrate = atoi(lastdatauplinkrate);
			else
				cur_wifi_associative_device_args.lastdatauplinkrate = 0;
			signalstrength=dmjson_get_value(associated_client_obj, 1, "rssi");
			if(signalstrength!=NULL && strlen(signalstrength)>0)
				cur_wifi_associative_device_args.signalstrength = atoi(signalstrength);
			else
				cur_wifi_associative_device_args.signalstrength = 0;
			noise=dmjson_get_value(associated_client_obj, 1, "snr");
			if(noise!=NULL && strlen(noise)>0)
				cur_wifi_associative_device_args.noise = atoi(noise);
			else
				cur_wifi_associative_device_args.noise = 0;
			retrans= dmjson_get_value(associated_client_obj, 2, "stats", "tx_pkts_retries");
			cur_wifi_associative_device_args.retransmissions= atoi(retrans);

			assoctimestr=dmjson_get_value(associated_client_obj, 1, "in_network");
			if(assoctimestr!=NULL && strlen(assoctimestr)>0)
				cur_wifi_associative_device_args.assoctime = atoi(assoctimestr);
			else
				cur_wifi_associative_device_args.assoctime = 0;

			entries++;
			idx = handle_update_instance(3, dmctx, &idx_last, update_instance_without_section, 1, ++id);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_wifi_associative_device_args, idx) == DM_STOP)
				break;
		}
		else
			break;
	}
	return 0;
}

int browseWifiNeighboringWiFiDiagnosticResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct wifi_neighboring_diagnostic_args cur_wifi_neighboring_diagnostic_args = {0};
	json_object *res, *neighboring_wifi_obj;
	struct uci_section *ss;
	char *bssid, *ssid, *signalstrength, *channel, *frequency, *noise, *idx, *idx_last = NULL;
	int entries = 0, id = 0;

	uci_foreach_sections("wireless", "wifi-device", ss) {
		dmubus_call("router.wireless", "scanresults", UBUS_ARGS{{"radio", section_name(ss), String}}, 1, &res);
		while (res) {
			neighboring_wifi_obj = dmjson_select_obj_in_array_idx(res, entries, 1, "access_points");
			if(neighboring_wifi_obj) {
				bssid=dmjson_get_value(neighboring_wifi_obj, 1, "bssid");
				if(bssid!=NULL && strlen(bssid)>0)
					dmasprintf(&(cur_wifi_neighboring_diagnostic_args.bssid),dmjson_get_value(neighboring_wifi_obj, 1, "bssid"));
				ssid=dmjson_get_value(neighboring_wifi_obj, 1, "ssid");
				if(ssid!=NULL && strlen(ssid)>0)
					dmasprintf(&(cur_wifi_neighboring_diagnostic_args.ssid),dmjson_get_value(neighboring_wifi_obj, 1, "ssid"));
				channel=dmjson_get_value(neighboring_wifi_obj, 1, "channel");
				if(channel!=NULL && strlen(channel)>0)
					cur_wifi_neighboring_diagnostic_args.channel= atoi(channel);
				else
					cur_wifi_neighboring_diagnostic_args.channel = 0;
				signalstrength=dmjson_get_value(neighboring_wifi_obj, 1, "rssi");
				if(signalstrength!=NULL && strlen(signalstrength)>0)
					cur_wifi_neighboring_diagnostic_args.signalstrength= atoi(signalstrength);
				else
					cur_wifi_neighboring_diagnostic_args.signalstrength = 0;
				frequency=dmjson_get_value(neighboring_wifi_obj, 1, "frequency");
				if(frequency!=NULL && strlen(frequency)>0)
					dmasprintf(&(cur_wifi_neighboring_diagnostic_args.operatingfrequencyband),dmjson_get_value(neighboring_wifi_obj, 1, "frequency"));
				noise=dmjson_get_value(neighboring_wifi_obj, 1, "noise");
				if(noise!=NULL && strlen(noise)>0)
					cur_wifi_neighboring_diagnostic_args.noise= atoi(noise);
				else
					cur_wifi_neighboring_diagnostic_args.noise = 0;
				entries++;
				idx = handle_update_instance(3, dmctx, &idx_last, update_instance_without_section, 1, ++id);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_wifi_neighboring_diagnostic_args, idx) == DM_STOP)
					break;
			}
			else
				break;
		}
		entries = 0;
	}
	return 0;
}
