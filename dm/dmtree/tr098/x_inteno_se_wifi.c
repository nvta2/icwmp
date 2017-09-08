/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *		Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "x_inteno_se_wifi.h"
#include "dmjson.h"

struct sewifiargs cur_wifiargs = {0};

inline int entry_sewifi_radio(struct dmctx *ctx);
inline int entry_sewifi_radio_instance(struct dmctx *ctx, char *wnum);
inline int init_se_wifi(struct dmctx *ctx, struct uci_section *s)
{
	struct sewifiargs *args = &cur_wifiargs;
	ctx->args = (void *)args;
	args->sewifisection = s;
	return 0;
}

int get_bandsteering_enable(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "enabled", value);
	return 0;
}

int set_bandsteering_enable(char *refparam, struct dmctx *ctx, int action, char *value)
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

int get_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "rssi_threshold", value);
	return 0;
}

int set_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			if (value == NULL)
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value("wireless", "bandsteering", "rssi_threshold", value);
			return 0;
	}
	return 0;
}

int get_bandsteering_policy(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "policy", value);
	return 0;
}

int set_bandsteering_policy(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			if (value == NULL)
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value("wireless", "bandsteering", "policy", value);
			return 0;
	}
	return 0;
}

int get_bandsteering_bw_util(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "bw_util", value);
	return 0;
}

int set_bandsteering_bw_util(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (value==NULL)
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value("wireless", "bandsteering", "bw_util", value);
			return 0;
	}
	return 0;
}

int get_wifi_frequency(char *refparam, struct dmctx *ctx, char **value)
{
	char *freq;
	json_object *res;
	struct sewifiargs *wifiargs = (struct sewifiargs *)ctx->args;
	char *wlan_name = section_name(wifiargs->sewifisection);
	
	dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wlan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	freq = dmjson_get_value(res, 1, "frequency");
	dmastrcat(value, freq, "GHz");  // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_wifi_maxassoc(char *refparam, struct dmctx *ctx, char **value)
{
	struct sewifiargs *wifiargs = (struct sewifiargs *)ctx->args;
	
	dmuci_get_value_by_section_string(wifiargs->sewifisection, "maxassoc", value);
	return 0;
}

int set_wifi_maxassoc(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct sewifiargs *wifiargs = (struct sewifiargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wifiargs->sewifisection, "maxassoc", value);
			return 0;
	}
	return 0;
}

int get_wifi_dfsenable(char *refparam, struct dmctx *ctx, char **value)
{
	char *val;
	struct sewifiargs *wifiargs = (struct sewifiargs *)ctx->args;
	char *wlan_name = section_name(wifiargs->sewifisection);
	*value = "";
	
	dmuci_get_value_by_section_string(wifiargs->sewifisection, "band", &val);
	if (val[0] == 'a') {
		dmuci_get_value_by_section_string(wifiargs->sewifisection, "dfsc", value);
		if ((*value)[0] == '\0')
			*value = "0";
	}
	return 0;
}

int set_wifi_dfsenable(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	bool b;
	char *val;
	struct sewifiargs *wifiargs = (struct sewifiargs *)ctx->args;
	char *wlan_name = section_name(wifiargs->sewifisection);
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wifiargs->sewifisection, "band", &val);
			if (val[0] == 'a') {
				string_to_bool(value, &b);
				if (b)
					dmuci_set_value_by_section(wifiargs->sewifisection, "dfsc", "1");
				else
					dmuci_set_value_by_section(wifiargs->sewifisection, "dfsc", "0");
			}
			return 0;
	}
	return 0;
}

////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_radio_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_wifiargs.sewifisection, "radioalias", value);
	return 0;
}

int set_radio_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_wifiargs.sewifisection, "radioalias", value);
			return 0;
	}
	return 0;
}
/////////////SUB ENTRIES///////////////
inline int entry_sewifi_radio(struct dmctx *ctx)
{
	char *wnum = NULL, *wnum_last = NULL;
	struct uci_section *s = NULL;
	uci_foreach_sections("wireless", "wifi-device", s) {
		init_se_wifi(ctx, s);
		wnum =  handle_update_instance(1, ctx, &wnum_last, update_instance_alias, 3, s, "radioinstance", "radioalias");
		SUBENTRY(entry_sewifi_radio_instance, ctx, wnum);
	}
	return 0;
}
//////////////////////////////////////

int entry_method_root_SE_Wifi(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"X_INTENO_SE_Wifi.") {
		DMOBJECT(DMROOT"X_INTENO_SE_Wifi.", ctx, "0", 1, NULL, NULL, NULL);
		DMPARAM("Bandsteering_Enable", ctx, "1", get_bandsteering_enable, set_bandsteering_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Bandsteering_Rssi_Threshold", ctx, "1", get_bandsteering_rssi_threshold, set_bandsteering_rssi_threshold, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Bandsteering_Policy", ctx, "1", get_bandsteering_policy, set_bandsteering_policy, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("Bandsteering_Bw_Util", ctx, "1", get_bandsteering_bw_util, set_bandsteering_bw_util, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"X_INTENO_SE_Wifi.Radio.", ctx, "0", 1, NULL, NULL, NULL);
		SUBENTRY(entry_sewifi_radio, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_sewifi_radio_instance(struct dmctx *ctx, char *wnum)
{
	IF_MATCH(ctx, DMROOT"X_INTENO_SE_Wifi.Radio.%s.", wnum) {
		DMOBJECT(DMROOT"X_INTENO_SE_Wifi.Radio.%s.", ctx, "0", 1, NULL, NULL, NULL, wnum);
		DMPARAM("Alias", ctx, "1", get_radio_alias, set_radio_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Frequency", ctx, "0", get_wifi_frequency, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("MaxAssociations", ctx, "1", get_wifi_maxassoc, set_wifi_maxassoc, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DFSEnable", ctx, "1", get_wifi_dfsenable, set_wifi_dfsenable, "xsd:boolean", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}
