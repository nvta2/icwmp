/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2018 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "userinterface.h"

#include "dmjson.h"
#define DELIMITOR ","

int entry_userinterface_remoteaccess(struct dmctx *ctx);

int get_userinterface_remoteaccess_enable(char *refparam, struct dmctx *ctx, char **value){
	struct uci_section *ss;
	char *rule_name, *rule_target;
	int section_found= 0;
	uci_foreach_sections("firewall", "rule", ss) {
		dmuci_get_value_by_section_string(ss, "name", &rule_name);
		if(!strcmp(rule_name, "juci-remote-access")){
			section_found= 1;
			dmuci_get_value_by_section_string(ss, "target", &rule_target);
			*value= !strcmp(rule_target, "ACCEPT") ? "1": "0";
			return 0;
		}
	}

	*value = "0";
	return 0;
}

int set_userinterface_remoteaccess_enable(char *refparam, struct dmctx *ctx, int action, char *value){
	struct uci_section *ss;
	char *rule_name, *rule_target;
	int section_found= 0;
	char *ret;
	struct uci_ptr config;
	uci_foreach_sections("firewall", "rule", ss) {
		dmuci_get_value_by_section_string(ss, "name", &rule_name);
		if(!strcmp(rule_name, "juci-remote-access")){
			if(!strcmp(value, "1")) dmuci_set_value_by_section(ss, "target", "ACCEPT"); else dmuci_set_value_by_section(ss, "target", "REJECT");
			return 0;
		}
	}
	dmuci_add_section("firewall", "rule", &ss, &ret);
	dmuci_set_value_by_section(ss, "name", "juci-remote-access");
	dmuci_set_value_by_section(ss, "src", "wan");
	dmuci_set_value_by_section(ss, "dest_port", "80");
	dmuci_set_value_by_section(ss, "proto", "tcp");
	dmuci_set_value_by_section(ss, "fruleinstance", "10");
	if(!strcmp(value, "1")) dmuci_set_value_by_section(ss, "target", "ACCEPT"); else dmuci_set_value_by_section(ss, "target", "REJECT");
	dmuci_commit();
	return 0;
}

int entry_method_root_userinterface(struct dmctx *ctx){
	IF_MATCH(ctx, DMROOT"USerInterface.") {
		DMOBJECT(DMROOT"USerInterface.", ctx, "0", 1, NULL, NULL, NULL);
		DMOBJECT(DMROOT"USerInterface.RemoteAccess.", ctx, "0", 1, NULL, NULL, NULL);
		SUBENTRY(entry_userinterface_remoteaccess, ctx);
		return 0;
	}
	return FAULT_9005;
}

int entry_userinterface_remoteaccess(struct dmctx *ctx){
	IF_MATCH(ctx, DMROOT"USerInterface.RemoteAccess.") {
		DMOBJECT(DMROOT"USerInterface.RemoteAccess.", ctx, "0", NULL, NULL, NULL, NULL);
		DMPARAM("Enable", ctx, "1", get_userinterface_remoteaccess_enable, set_userinterface_remoteaccess_enable, NULL, 0, 0, UNDEF, NULL);
	}
}
