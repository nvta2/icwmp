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
{"Port", &DMWRITE, DMT_UNINT, get_userint_remoteaccesss_port, set_userint_remoteaccesss_port, NULL, NULL},
{"SupportedProtocols", &DMREAD, DMT_STRING, get_userint_remoteaccesss_supportedprotocols, NULL, NULL, NULL},
{"Protocol", &DMWRITE, DMT_STRING, get_userint_remoteaccesss_protocol, set_userint_remoteaccesss_protocol, NULL, NULL},
{0}
};

/**************************************************************************
* GET & SET PARAMETERS
***************************************************************************/
static void add_default_rule(char *port, char *enable, char *owsd)
{
	struct uci_section *ss;
	char *ret;

	dmuci_add_section("firewall", "rule", &ss, &ret);
	dmuci_set_value_by_section(ss, "name", "juci-remote-access");
	dmuci_set_value_by_section(ss, "src", "wan");
	dmuci_set_value_by_section(ss, "proto", "tcp");
	dmuci_set_value_by_section(ss, "target", "ACCEPT");
	dmuci_set_value_by_section(ss, "dest_port", port);
	dmuci_set_value_by_section(ss, "owsd", owsd);
	dmuci_set_value_by_section(ss, "enabled", enable);
}

int get_userint_remoteaccesss_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ss;
	char *rule_name, *rule_enabled;

	uci_foreach_sections("firewall", "rule", ss)
	{
		dmuci_get_value_by_section_string(ss, "name", &rule_name);
		if(strcmp(rule_name, "juci-remote-access") == 0)
		{
			dmuci_get_value_by_section_string(ss, "enabled", &rule_enabled);
			*value= (strcmp(rule_enabled, "0") == 0) ? "0": "1";
			return 0;
		}
	}

	*value = "0";
	return 0;
}

int set_userint_remoteaccesss_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ss;
	char *rule_name;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_sections("firewall", "rule", ss)
			{
				dmuci_get_value_by_section_string(ss, "name", &rule_name);
				if(strcmp(rule_name, "juci-remote-access") == 0)
				{
					dmuci_set_value_by_section(ss, "enabled", b ? "" : "0");
					return 0;
				}
			}

			add_default_rule("80", value, "wan");
			return 0;
	}
	return 0;
}

int get_userint_remoteaccesss_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ss;
	char *rule_name, *dest_port;

	uci_foreach_sections("firewall", "rule", ss) {
		dmuci_get_value_by_section_string(ss, "name", &rule_name);
		if(strcmp(rule_name, "juci-remote-access") == 0){
			dmuci_get_value_by_section_string(ss, "dest_port", &dest_port);
			*value= dest_port;
			return 0;
		}
	}

	*value = "80";
	return 0;
}

int set_userint_remoteaccesss_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{

	struct uci_section *ss;
	char *rule_name, *owsd;
	char *ret;

	switch (action)
	{
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_sections("firewall", "rule", ss)
			{
				dmuci_get_value_by_section_string(ss, "name", &rule_name);
				if(strcmp(rule_name, "juci-remote-access") == 0)
				{
					dmuci_set_value_by_section(ss, "dest_port", value);
					dmuci_get_value_by_section_string(ss, "owsd", &owsd);
					dmuci_set_value("owsd", owsd, "port", value);
					return 0;
				}
			}

			add_default_rule(value, "0", "wan");
			dmuci_set_value("owsd", "wan", "port", value);
			return 0;
	}
	return 0;
}

static int get_supportedprotocols(void)
{
	char *cert, *key, *ca;
	int found_https = 0;

	if ((dmuci_get_option_value_string("owsd", "wan_https", "cert", &cert) == 0 && *cert != '\0' && access(cert, F_OK) != -1) ||
		(dmuci_get_option_value_string("owsd", "wan_https", "key", &key) == 0 && *key != '\0' && access(key, F_OK) != -1) ||
		(dmuci_get_option_value_string("owsd", "wan_https", "ca", &ca) == 0 && *ca != '\0' && access(ca, F_OK) != -1))
		found_https = 1;

	return found_https;
}

int get_userint_remoteaccesss_supportedprotocols(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int found;

	found = get_supportedprotocols();
	if (found)
	{
		*value = "HTTP, HTTPS";
		return 0;
	}

	*value = "HTTP";
	return 0;
}

int get_userint_remoteaccesss_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ss;
	char *rule_name, *rule_owsd;

	uci_foreach_sections("firewall", "rule", ss)
	{
		dmuci_get_value_by_section_string(ss, "name", &rule_name);
		if(strcmp(rule_name, "juci-remote-access") == 0)
		{
			dmuci_get_value_by_section_string(ss, "owsd", &rule_owsd);
			if (strcmp(rule_owsd, "wan") == 0)
				*value = "HTTP";
			else
				*value = "HTTPS";
			return 0;
		}
	}

	*value = "HTTP";
	return 0;
}

int set_userint_remoteaccesss_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ss;
	char *rule_name, *name_http;
	int found;

	switch (action)
	{
		case VALUECHECK:
			found = get_supportedprotocols();
			if (found)
			{
				if ((strcmp(value, "HTTP") != 0) && (strcmp(value, "HTTPS") != 0))
					return FAULT_9007;
			}
			else
			{
				if (strcmp(value, "HTTP") != 0)
					return FAULT_9007;
			}
			return 0;
		case VALUESET:
			uci_foreach_sections("firewall", "rule", ss)
			{
				dmuci_get_value_by_section_string(ss, "name", &rule_name);
				if(strcmp(rule_name, "juci-remote-access") == 0)
				{
					if(strcmp(value, "HTTPS") == 0)
						dmuci_set_value_by_section(ss, "owsd", "wan_https");
					else
						dmuci_set_value_by_section(ss, "owsd", "wan");
					return 0;
				}
			}

			if(strcmp(value, "HTTPS") == 0)
				name_http = "wan_https";
			else
				name_http = "wan";
			add_default_rule("80", "0", name_http);
			return 0;
	}
	return 0;
}
