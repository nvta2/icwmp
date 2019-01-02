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
#include "x_iopsys_eu_wifi.h"
#include "dmjson.h"

/*** X_IOPSYS_EU_Wifi. ***/
DMLEAF tsewifiParam[] = {
{"Bandsteering_Enable", &DMWRITE, DMT_BOOL, get_bandsteering_enable, set_bandsteering_enable, NULL, NULL},
{"Bandsteering_Rssi_Threshold", &DMWRITE, DMT_BOOL, get_bandsteering_rssi_threshold, set_bandsteering_rssi_threshold, NULL, NULL},
{"Bandsteering_Policy", &DMWRITE, DMT_BOOL, get_bandsteering_policy, set_bandsteering_policy, NULL, NULL},
{"Bandsteering_Bw_Util", &DMWRITE, DMT_BOOL, get_bandsteering_bw_util, set_bandsteering_bw_util, NULL, NULL},
{0}
};

DMOBJ tsewifiObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Radio", &DMREAD, NULL, NULL, NULL, browsesewifiradioInst, NULL, NULL, NULL, tsewifiradioParam, NULL},
{0}
};

/*** X_IOPSYS_EU_Wifi.Radio.{i}. ***/
DMLEAF tsewifiradioParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_radio_alias, set_radio_alias, NULL, NULL},
{"Frequency", &DMREAD, DMT_STRING, get_wifi_frequency, NULL, NULL, NULL},
{"MaxAssociations", &DMWRITE, DMT_STRING, get_wifi_maxassoc, set_wifi_maxassoc, NULL, NULL},
{"DFSEnable", &DMWRITE, DMT_BOOL, get_wifi_dfsenable, set_wifi_dfsenable, NULL, NULL},
{0}
};

int get_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("wireless", "bandsteering", "enabled", value);
	return 0;
}

int set_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
        dmuci_get_option_value_string("wireless", "bandsteering", "rssi_threshold", value);
        return 0;
}

int set_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_bandsteering_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
        dmuci_get_option_value_string("wireless", "bandsteering", "policy", value);
        return 0;
}

int set_bandsteering_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_bandsteering_bw_util(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
        dmuci_get_option_value_string("wireless", "bandsteering", "bw_util", value);
        return 0;
}

int set_bandsteering_bw_util(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_wifi_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *freq;
	json_object *res;
	struct uci_section *sewifisection = (struct uci_section *)data;
	char *wlan_name = section_name(sewifisection);
	
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

int get_wifi_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *sewifisection = (struct uci_section *)data;
	
	dmuci_get_value_by_section_string(sewifisection, "maxassoc", value);
	return 0;
}

int set_wifi_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *sewifisection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(sewifisection, "maxassoc", value);
			return 0;
	}
	return 0;
}

int get_wifi_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	struct uci_section *sewifisection = (struct uci_section *)data;
	char *wlan_name = section_name(sewifisection);
	*value = "";
	
	dmuci_get_value_by_section_string(sewifisection, "band", &val);
	if (val[0] == 'a') {
		dmuci_get_value_by_section_string(sewifisection, "dfsc", value);
		if ((*value)[0] == '\0')
			*value = "0";
	}
	return 0;
}

int set_wifi_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	bool b;
	char *val;
	struct uci_section *sewifisection = (struct uci_section *)data;
	char *wlan_name = section_name(sewifisection);
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(sewifisection, "band", &val);
			if (val[0] == 'a') {
				string_to_bool(value, &b);
				if (b)
					dmuci_set_value_by_section(sewifisection, "dfsc", "1");
				else
					dmuci_set_value_by_section(sewifisection, "dfsc", "0");
			}
			return 0;
	}
	return 0;
}

////////////////////////SET AND GET ALIAS/////////////////////////////////
static int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_wireless", "wifi-device", section_name((struct uci_section *)data), &dmmap_section);
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
			get_dmmap_section_of_config_section("dmmap_wireless", "wifi-device", section_name((struct uci_section *)data), &dmmap_section);
			dmuci_set_value_by_section(dmmap_section, "radioalias", value);
			return 0;
	}
	return 0;
}


int browsesewifiradioInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL;
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("wireless", "wifi-device", "dmmap_wireless", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, p->dmmap_section, "radioinstance", "radioalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}
