/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/


#include <uci.h>
#include <ctype.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "x_iopsys_eu_wifilife.h"

/*** DMROOT.X_IOPSYS_EU_WiFiLife. ****/
DMOBJ X_IOPSYS_EU_WiFiLifeObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{"Steering", &DMREAD, NULL, NULL, NULL, browseWifiLifeSteeringObj, NULL, NULL, NULL, WiFiLifeSteeringParams, NULL},
{0}
};

DMLEAF X_IOPSYS_EU_WiFiLifeParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_x_iopsys_eu_wifilife_enable, set_x_iopsys_eu_wifilife_enable, NULL, NULL},
{0}
};

DMLEAF WiFiLifeSteeringParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_wifilife_steering_enable, set_wifilife_steering_enable, NULL, NULL},
{"LegacyFallback", &DMWRITE, DMT_BOOL, get_wifilife_steering_legacy_fallback, set_wifilife_steering_legacy_fallback, NULL, NULL},
{0}
};

int browseWifiLifeSteeringObj(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *isteer = NULL, *isteer_last = NULL;
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("wifilife", "steer", "dmmap_wifilife", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		isteer =  handle_update_instance(1, dmctx, &isteer_last, update_instance_alias, 3, p->dmmap_section, "steer_instance", "steer_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, isteer) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

/*************************************************************************************
**** GET / SET function ****
**************************************************************************************/
int get_x_iopsys_eu_wifilife_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("wifilife", "@wifilife[0]", "enabled", value);
	if ((*value)[0] == '\0') {
		*value = "1";
	}
	return 0;
}

int set_x_iopsys_eu_wifilife_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value("wifilife", "@wifilife[0]", "enabled", b?"1":"0");
			return 0;
	}
	return 0;
}


int get_wifilife_steering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "enabled", value);
	if ((*value)[0] == '\0')
		*value = "0";
	return 0;
}

int set_wifilife_steering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value_by_section((struct uci_section *)data, "enabled", b?"1":"0");
			return 0;
	}
	return 0;
}

int get_wifilife_steering_legacy_fallback(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "fallback_legacy", value);
	if ((*value)[0] == '\0')
		*value = "0";
	return 0;
}

int set_wifilife_steering_legacy_fallback(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value_by_section((struct uci_section *)data, "fallback_legacy", b?"1":"0");
			return 0;
	}
	return 0;
}
