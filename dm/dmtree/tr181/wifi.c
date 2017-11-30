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
{"Radio", &DMWRITE, NULL, NULL, NULL, browseWifiRadioInst, NULL, NULL, tWifiRadioStatsObj, tWifiRadioParams, get_linker_Wifi_Radio},
{"SSID", &DMWRITE, add_wifi_ssid, delete_wifi_ssid, NULL, browseWifiSsidInst, NULL, NULL, tWifiSsidStatsObj, tWifiSsidParams, get_linker_Wifi_Ssid},
{"AccessPoint", &DMREAD, NULL, NULL, NULL, browseWifiAccessPointInst, NULL, NULL, tAcessPointSecurityObj, tWifiAcessPointParams, NULL},
{0}
};

DMLEAF tWifiParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"X_INTENO_SE_Bandsteering_Enable", &DMWRITE, DMT_BOOL, get_wifi_bandsteering_enable, set_wifi_bandsteering_enable, NULL, NULL},
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
{"MaxBitRate", &DMWRITE, DMT_STRING,get_radio_max_bit_rate, set_radio_max_bit_rate, NULL, NULL},
{"OperatingFrequencyBand", &DMREAD, DMT_STRING, get_radio_frequency, NULL, NULL, NULL},
{"SupportedFrequencyBands", &DMREAD, DMT_STRING, get_radio_supported_frequency_bands, NULL, NULL, NULL},
{"OperatingChannelBandwidth", &DMWRITE, DMT_STRING,  get_radio_operating_channel_bandwidth, set_radio_operating_channel_bandwidth, NULL, NULL},
{"X_INTENO_SE_MaxAssociations", &DMWRITE, DMT_STRING, get_radio_maxassoc, set_radio_maxassoc, NULL, NULL},
{"X_INTENO_SE_DFSEnable", &DMWRITE, DMT_BOOL, get_radio_dfsenable, set_radio_dfsenable, NULL, NULL},
{"SupportedStandards", &DMREAD, DMT_STRING, get_radio_supported_standard, NULL, NULL, NULL},
{"OperatingStandards", &DMWRITE, DMT_STRING, get_radio_operating_standard, set_radio_operating_standard, NULL, NULL},
{"ChannelsInUse", &DMREAD, DMT_STRING, get_radio_channel, NULL, NULL, NULL},
{"Channel", &DMWRITE, DMT_UNINT, get_radio_channel, set_radio_channel, NULL, NULL},
{"AutoChannelEnable", &DMWRITE, DMT_BOOL, get_radio_auto_channel_enable, set_radio_auto_channel_enable, NULL, NULL},
{"PossibleChannels", &DMREAD, DMT_STRING, get_radio_possible_channels, NULL, NULL, NULL},
{0}
};

/*** WiFi.Radio.Stats. ***/
DMLEAF tWifiRadioStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_radio_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_radio_statistics_rx_packets, NULL, NULL, NULL},
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
{0}
};

/*** WiFi.SSID.Stats. ***/
DMLEAF tWifiSsidStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_ssid_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_ssid_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_ssid_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_ssid_statistics_rx_packets, NULL, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint. ***/

DMOBJ tAcessPointSecurityObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Security", &DMWRITE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWifiAcessPointSecurityParams, NULL},
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
{"AssociatedDeviceNumberOfEntries", &DMREAD, DMT_UNINT, get_access_point_total_associations, NULL, NULL, NULL},
{"MaxAssociatedDevices", &DMWRITE, DMT_UNINT, get_access_point_maxassoc, set_access_point_maxassoc, NULL, NULL},
{"MACAddressControlEnabled", &DMWRITE, DMT_BOOL, get_access_point_control_enable, set_access_point_control_enable, NULL, NULL},
{0}
};

/*** WiFi.AccessPoint.Security. ***/
DMLEAF tWifiAcessPointSecurityParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ModesSupported", &DMREAD, DMT_STRING, get_access_point_security_supported_modes, NULL, NULL, NULL},
{"ModeEnabled", &DMWRITE, DMT_STRING ,get_access_point_security_modes, set_access_point_security_modes, NULL, NULL},
{"WEPKey", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_wepkey, NULL, NULL},
{"PreSharedKey", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_shared_key, NULL, NULL},
{"KeyPassphrase", &DMWRITE, DMT_STRING, get_empty, set_access_point_security_passphrase, NULL, NULL},
{"RekeyingInterval", &DMWRITE, DMT_UNINT, get_access_point_security_rekey_interval, set_access_point_security_rekey_interval, NULL, NULL},
{"RadiusServerIPAddr", &DMWRITE, DMT_STRING, get_access_point_security_radius_ip_address, set_access_point_security_radius_ip_address, NULL, NULL},
{"RadiusServerPort", &DMWRITE, DMT_UNINT, get_access_point_security_radius_server_port, set_access_point_security_radius_server_port, NULL, NULL},
{"RadiusSecret", &DMWRITE, DMT_STRING,get_empty, set_access_point_security_radius_secret, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_Wifi_Radio(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(((struct wifi_radio_args *)data)->wifi_radio_sec) {
		*linker = section_name(((struct wifi_radio_args *)data)->wifi_radio_sec);
		return 0;
	}
	*linker = "";
	return 0;
}

int get_linker_Wifi_Ssid(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(((struct wifi_ssid_args *)data)->ifname) {
		*linker = ((struct wifi_ssid_args *)data)->ifname;
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
	int i;
	char *val;
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "disabled", &val);
	if (val[0] == '0')
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

int get_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "ssid", value);
	return 0;
}

int set_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_wlan_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *wlan_name;
	json_object *res;

	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "device", &wlan_name);
	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "bssid");
	return 0;
}

int get_radio_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int i;
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
int get_radio_max_bit_rate (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "hwmode", value);
	return 0;
}

int set_radio_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "hwmode", value);
			return 0;
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
	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "b, g, n");
	freq = dmjson_get_value(res, 1, "frequency");
	if (strcmp(freq, "5") == 0)
		*value = "a, n, ac";
	else
		*value = "b, g, n";
	return 0;
}

int get_radio_operating_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "hwmode", value);
	if (strcmp(*value, "11b") == 0)
		*value = "b";
	else if (strcmp(*value, "11bg") == 0)
		*value = "b,g";
	else if (strcmp(*value, "11g") == 0 || strcmp(*value, "11gst") == 0 || strcmp(*value, "11lrs") == 0)
		*value = "g";
	else if (strcmp(*value, "11n") == 0 || strcmp(*value, "auto") == 0)
		*value = "n";
	else if (strcmp(*value, "11ac") == 0)
		*value = "ac";
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

/**************************************************************************
* SET & GET VALUE
***************************************************************************/
int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *hidden;
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "hidden", &hidden);
	if (hidden[0] == '1' && hidden[1] == '\0')
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	bool b;
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "device", value);
	dmuci_get_option_value_string("wireless", *value, "wmm", value);
	string_to_bool(*value, &b);
		if (b)
			*value = "Enabled";
		else
			*value = "Disabled";

	return 0;
}

int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
	int i = 0;
	json_object *res;
	char *wunit, buf[8];
	dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", ((struct wifi_ssid_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, val) {
		if (strstr(key, "sta-"))
			i++;
	}
	dmasprintf(value, "%d", i); // MEM WILL BE FREED IN DMMEMCLEAN
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

int get_access_point_security_supported_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "None, WEP-64, WEP-128, WPA-Personal, WPA2-Personal, WPA-WPA2-Personal, WPA-Enterprise, WPA2-Enterprise, WPA-WPA2-Enterprise";
	return 0;
}

int get_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption, *cipher;

	*value = "";
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", &encryption);
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "cipher", &cipher);
	if (strcmp(encryption, "none") == 0)
		*value = "None";
	else if (strcmp(encryption, "wep-open") == 0 || strcmp(encryption, "wep-shared") == 0)
		*value = "WEP-64";
	else if (strcmp(encryption, "psk") == 0)
		*value = "WPA-Personal";
	else if (strcmp(encryption, "wpa") == 0)
		*value = "WPA-Enterprise";
	else if (strcmp(encryption, "psk2") == 0 && strcmp(cipher, "ccmp") == 0)
		*value = "WPA2-Personal";
	else if (strcmp(encryption, "wpa2") == 0)
		*value = "WPA2-Enterprise";
	else if (strcmp(encryption, "mixed-psk") == 0 && strcmp(cipher, "tkip+ccmp") == 0)
		*value = "WPA-WPA2-Personal";
	else if (strcmp(encryption, "mixed-wpa") == 0)
		*value = "WPA-WPA2-Enterprise";
	return 0;
}

int set_access_point_security_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *option, *gnw;
	char strk64[4][11];
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "None") == 0) {
				reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "none");
			}
			else if (strcmp(value, "WEP-64") == 0 || strcmp(value, "WEP-128") == 0) {
				reset_wlan(((struct wifi_acp_args *)data)->wifi_acp_sec);
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "wep-open");
				wepkey64("Inteno", strk64);
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
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "encryption", "mixed-wpa");
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_server", "");
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_port", "1812");
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", "");
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
				dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "key", &key_index);
				sprintf(buf,"key%s", key_index);
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, buf, value);
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
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
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
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
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
			if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
				dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "radius_secret", value);
			return 0;
	}
	return 0;
}

int get_radio_supported_frequency_bands(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "2.4GHz, 5GHz";
	return 0;
}
/**************************************************************************
* SET AND GET ALIAS
***************************************************************************/

int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_radio_args *)data)->wifi_radio_sec, "radioalias", value);
	return 0;
}

int set_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_radio_args *)data)->wifi_radio_sec, "radioalias", value);
			return 0;
	}
	return 0;
}

int get_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "ssidalias", value);
	return 0;
}

int set_ssid_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "ssidalias", value);
			return 0;
	}
	return 0;
}
int get_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wifi_acp_args *)data)->wifi_acp_sec, "accesspointalias", value);
	return 0;
}

int set_access_point_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wifi_acp_args *)data)->wifi_acp_sec, "accesspointalias", value);
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
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cRadio%c", DMROOT, dm_delim, dm_delim, dm_delim), ((struct wifi_ssid_args *)data)->linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL)
			*value = "";
	}
	return 0;
}

int set_ssid_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, "device", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_ap_ssid_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cSSID%c", DMROOT, dm_delim, dm_delim, dm_delim), ((struct wifi_acp_args *)data)->ifname, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

/*************************************************************
 * ADD DEL OBJ
/*************************************************************/
int add_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value;
	char ssid[16] = {0};
	char *inst;
	struct uci_section *s = NULL;

	inst = get_last_instance("wireless", "wifi-iface", "ssidinstance");
	sprintf(ssid, "Inteno_%d", inst ? (atoi(inst)+1) : 1);
	dmuci_add_section("wireless", "wifi-iface", &s, &value);
	dmuci_set_value_by_section(s, "device", "wl0");
	dmuci_set_value_by_section(s, "encryption", "none");
	dmuci_set_value_by_section(s, "macfilter", "0");
	dmuci_set_value_by_section(s, "mode", "ap");
	dmuci_set_value_by_section(s, "ssid", ssid);
	*instance = update_instance(s, inst, "ssidinstance");
	return 0;
}

int delete_wifi_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	switch (del_action) {
		case DEL_INST:
			dmuci_delete_by_section(((struct wifi_ssid_args *)data)->wifi_ssid_sec, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("wireless", "wifi-iface", s) {
				if (found != 0)
						dmuci_delete_by_section(ss, NULL, NULL);
				ss = s;
				found++;
			}
			if (ss != NULL)
				dmuci_delete_by_section(ss, NULL, NULL);
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

	uci_foreach_sections("wireless", "wifi-device", s) {
		init_wifi_radio(&curr_wifi_radio_args, s);
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, s, "radioinstance", "radioalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_radio_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}

int browseWifiSsidInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *ssid_last = NULL, *ifname, *acpt_last = NULL, *linker;
	struct uci_section *ss = NULL;
	json_object *res;
	struct wifi_ssid_args curr_wifi_ssid_args = {0};

	uci_foreach_sections("wireless", "wifi-iface", ss) {
		dmuci_get_value_by_section_string(ss, "ifname", &ifname);
		dmuci_get_value_by_section_string(ss, "device", &linker);
		init_wifi_ssid(&curr_wifi_ssid_args, ss, ifname, linker);
		wnum =  handle_update_instance(1, dmctx, &ssid_last, update_instance_alias, 3, ss, "ssidinstance", "ssidalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_ssid_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}

int browseWifiAccessPointInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *ssid_last = NULL, *ifname, *acpt_last = NULL;
	struct uci_section *ss = NULL;
	json_object *res;
	struct wifi_acp_args curr_wifi_acp_args = {0};

	uci_foreach_sections("wireless", "wifi-iface", ss) {
		dmuci_get_value_by_section_string(ss, "ifname", &ifname);
		init_wifi_acp(&curr_wifi_acp_args, ss, ifname);
		wnum =  handle_update_instance(1, dmctx, &acpt_last, update_instance_alias, 3, ss, "accesspointinstance", "accesspointalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wifi_acp_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}
