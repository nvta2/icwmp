/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2018 Inteno Broadband Technology AB
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmjson.h"
#include "dmcommon.h"
#include "userinterface.h"

/*** USerInterface. ***/
DMOBJ tUserInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker(10)*/
{"RemoteAccess", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tUserIntRemoteAccessParam, NULL},
{0}
};

/*** USerInterface.RemoteAccess. ***/
DMLEAF tUserIntRemoteAccessParam[] = {
/* PARAM, permission, type, getvalue, setvalue, forced_inform, notification(7)*/
{"Enable", &DMWRITE, DMT_BOOL, get_userint_remoteaccesss_enable, set_userint_remoteaccesss_enable, NULL, NULL},
{0}
};

/**************************************************************************
* GET & SET PARAMETERS
***************************************************************************/

int get_userint_remoteaccesss_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
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

int set_userint_remoteaccesss_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
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

