/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2018 Inteno Broadband Technology AB
 *	    Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *      Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#include <uci.h>
#include <ctype.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "xmpp.h"

/*** XMPP. ***/
DMOBJ tXMPPObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Connection", &DMWRITE, add_xmpp_connection, delete_xmpp_connection, NULL, browsexmpp_connectionInst, NULL, NULL, tXMPPConnectionObj, tXMPPConnectionParams, get_xmpp_connection_linker},
{0}
};

DMLEAF tXMPPParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ConnectionNumberOfEntries", &DMREAD, DMT_UNINT, get_xmpp_connection_nbr_entry, NULL, NULL, NULL},
{"SupportedServerConnectAlgorithms", &DMREAD, DMT_STRING, get_xmpp_connection_supported_server_connect_algorithms, NULL, NULL, NULL},
{0}
};

/*** XMPP.Connection. ***/
DMLEAF tXMPPConnectionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_connection_enable, set_connection_enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_xmpp_connection_alias, set_xmpp_connection_alias, NULL, NULL},
{"Username", &DMWRITE, DMT_STRING, get_xmpp_connection_username, set_xmpp_connection_username, NULL, NULL},
{"Password", &DMWRITE, DMT_STRING, get_xmpp_connection_password, set_xmpp_connection_password, NULL, NULL},
{"Domain", &DMWRITE, DMT_STRING, get_xmpp_connection_domain, set_xmpp_connection_domain, NULL, NULL},
{"Resource", &DMWRITE, DMT_STRING, get_xmpp_connection_resource, set_xmpp_connection_resource, NULL, NULL},
{"ServerConnectAlgorithm", &DMWRITE, DMT_STRING, get_xmpp_connection_server_connect_algorithm, set_xmpp_connection_server_connect_algorithm, NULL, NULL},
{"KeepAliveInterval", &DMWRITE, DMT_LONG, get_xmpp_connection_keepalive_interval, set_xmpp_connection_keepalive_interval, NULL, NULL},
{"ServerConnectAttempts", &DMWRITE, DMT_UNINT, get_xmpp_connection_server_attempts, set_xmpp_connection_server_attempts, NULL, NULL},
{"ServerRetryInitialInterval", &DMWRITE, DMT_UNINT, get_xmpp_connection_retry_initial_interval, set_xmpp_connection_retry_initial_interval, NULL, NULL},
{"ServerRetryIntervalMultiplier", &DMWRITE, DMT_UNINT, get_xmpp_connection_retry_interval_multiplier, set_xmpp_connection_retry_interval_multiplier, NULL, NULL},
{"ServerRetryMaxInterval", &DMWRITE, DMT_UNINT, get_xmpp_connection_retry_max_interval, set_xmpp_connection_retry_max_interval, NULL, NULL},
{"UseTLS", &DMWRITE, DMT_BOOL, get_xmpp_connection_server_usetls, set_xmpp_connection_server_usetls, NULL, NULL},
{"JabberID", &DMREAD, DMT_STRING, get_xmpp_connection_jabber_id, NULL, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_xmpp_connection_status, NULL, NULL, NULL},
{"ServerNumberOfEntries", &DMREAD, DMT_UNINT, get_xmpp_connection_server_number_of_entries, NULL, NULL, NULL},
{0}
};

DMOBJ tXMPPConnectionObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Server", &DMREAD, NULL, NULL, NULL, browsexmpp_connection_serverInst, NULL, NULL, NULL, tXMPPConnectionServerParams, NULL},
{0}
};

/*** XMPP.Connection.Server. ***/
DMLEAF tXMPPConnectionServerParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_xmpp_connection_server_enable, set_xmpp_connection_server_enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_xmpp_connection_server_alias, set_xmpp_connection_server_alias, NULL, NULL},
{"ServerAddress", &DMWRITE, DMT_STRING, get_xmpp_connection_server_server_address, set_xmpp_connection_server_server_address, NULL, NULL},
{"Port", &DMWRITE, DMT_UNINT, get_xmpp_connection_server_port, set_xmpp_connection_server_port, NULL, NULL},
{0}
};

int add_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	struct uci_section *xmpp_connection, *xmpp_connection_server;
	char *value1, *value2, *last_inst;

	last_inst = get_last_instance("cwmp_xmpp", "xmpp_connection", "connection_instance");
	dmuci_add_section_and_rename("cwmp_xmpp", "xmpp_connection", &xmpp_connection, &value1);
	dmuci_add_section_and_rename("cwmp_xmpp", "xmpp_connection_server", &xmpp_connection_server, &value2);
	dmasprintf(instancepara, "%d", atoi(last_inst)+1);
	dmuci_set_value_by_section(xmpp_connection, "connection_instance", *instancepara);
	dmuci_set_value_by_section(xmpp_connection, "enable", "0");
	dmuci_set_value_by_section(xmpp_connection, "interval", "30");
	dmuci_set_value_by_section(xmpp_connection, "attempt", "16");
	dmuci_set_value_by_section(xmpp_connection, "serveralgorithm", "DNS-SRV");
	dmuci_set_value_by_section(xmpp_connection_server, "id_connection", *instancepara);
	dmuci_set_value_by_section(xmpp_connection_server, "connection_server_instance", "1");
	dmuci_set_value_by_section(xmpp_connection_server, "enable", "0");
	dmuci_set_value_by_section(xmpp_connection_server, "port", "5222");
	return 0;
}

int delete_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	struct uci_section *s, *ss = NULL;
	struct uci_section *connsection = (struct uci_section *)data;
	char *prev_connection_instance;
	
	switch (del_action) {
		case DEL_INST:
			dmuci_get_value_by_section_string(connsection, "connection_instance", &prev_connection_instance);
			uci_foreach_option_eq("cwmp_xmpp", "xmpp_connection_server", "id_connection", prev_connection_instance, s) {
				dmuci_delete_by_section(s, NULL, NULL);
				break;
			}
			dmuci_delete_by_section(connsection, NULL, NULL);
			return 0;
		case DEL_ALL:
			uci_foreach_sections("cwmp_xmpp", "xmpp_connection", s) {
					if (found != 0) {
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL) {
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			uci_foreach_sections("cwmp_xmpp", "xmpp_connection_server", s) {
					if (found != 0) {
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL) {
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}

int get_xmpp_connection_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int cnt = 0;

	uci_foreach_sections("cwmp_xmpp", "xmpp_connection", s) {
		cnt++;
	}
	dmasprintf(value, "%d", cnt); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_xmpp_connection_supported_server_connect_algorithms(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "DNS-SRV , ServerTable";
	return 0;
}

int get_connection_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "enable", value);
	return 0;
}

int set_connection_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "enable", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "connection_alias", value);
	return 0;
}

int set_xmpp_connection_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "connection_alias", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "username", value);
	return 0;
}

int set_xmpp_connection_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "username", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	return 0;
}

int set_xmpp_connection_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "password", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "domain", value);
	return 0;
}

int set_xmpp_connection_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "domain", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_resource(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "resource", value);
	return 0;
}

int set_xmpp_connection_resource(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "resource", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_connect_algorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "serveralgorithm", value);
	return 0;
}

int set_xmpp_connection_server_connect_algorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(strcmp(value, "DNS-SRV") == 0 || strcmp(value, "ServerTable") == 0)
				dmuci_set_value_by_section(connsection, "serveralgorithm", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_keepalive_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "interval", value);
	return 0;
}

int set_xmpp_connection_keepalive_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "interval", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_attempts(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "attempt", value);
	return 0;
}

int set_xmpp_connection_server_attempts(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "attempt", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_retry_initial_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "initial_retry_interval", value);
	return 0;
}

int set_xmpp_connection_retry_initial_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "initial_retry_interval", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "retry_interval_multiplier", value);
	return 0;
}

int set_xmpp_connection_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "retry_interval_multiplier", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_retry_max_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "retry_max_interval", value);
	return 0;
}

int set_xmpp_connection_retry_max_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "retry_max_interval", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_usetls(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "usetls", value);
	return 0;
}

int set_xmpp_connection_server_usetls(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "usetls", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_jabber_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;
	char *resource, *domain, *username;

	dmuci_get_value_by_section_string(connsection, "resource", &resource);
	dmuci_get_value_by_section_string(connsection, "domain", &domain);
	dmuci_get_value_by_section_string(connsection, "username", &username);
	if(*resource != '\0' || *domain != '\0' || *username != '\0')
		dmasprintf(value, "%s@%s/%s", username, domain, resource);
	else
		*value = "";
	return 0;
}

int get_xmpp_connection_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;
	char *status;

	dmuci_get_value_by_section_string(connsection, "enable", &status);
	if(strcmp(status,"1") == 0)
			*value = "Enabled";
		else
			*value = "Disabled";
	return 0;
}

int get_xmpp_connection_server_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_xmpp_connection_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "enable", value);
	return 0;
}

int set_xmpp_connection_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "enable", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "connection_server_alias", value);
	return 0;
}

int set_xmpp_connection_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "connection_server_alias", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "server_address", value);
	return 0;
}

int set_xmpp_connection_server_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "server_address", value);
			return 0;
	}
	return 0;
}

int get_xmpp_connection_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *connsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(connsection, "port", value);
	return 0;
}

int set_xmpp_connection_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *connsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(connsection, "port", value);
			return 0;
	}
	return 0;
}

/**************************************************************************
* LINKER
***************************************************************************/
int  get_xmpp_connection_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	char *conn_instance;
	struct uci_section *connsection = (struct uci_section *)data;

	if (connsection)
	{
		dmuci_get_value_by_section_string(connsection, "connection_instance", &conn_instance);
		dmasprintf(linker,"xmppc:%s", conn_instance);
		return 0;
	}
	else
		*linker = "";
		return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browsexmpp_connectionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iconnection = NULL, *iconnection_last = NULL;
	struct uci_section *s = NULL;

	uci_foreach_sections("cwmp_xmpp", "xmpp_connection", s) {
		iconnection = handle_update_instance(1, dmctx, &iconnection_last, update_instance_alias, 3, s, "connection_instance", "connection_instance_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, iconnection) == DM_STOP)
			break;
	}
	return 0;
}

int browsexmpp_connection_serverInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iconnectionserver = NULL, *iconnectionserver_last = NULL, *prev_connection_instance;
	struct uci_section *s = NULL, *connsection = (struct uci_section *)prev_data;

	dmuci_get_value_by_section_string(connsection, "connection_instance", &prev_connection_instance);
	uci_foreach_option_eq("cwmp_xmpp", "xmpp_connection_server", "id_connection", prev_connection_instance, s) {
	iconnectionserver = handle_update_instance(1, dmctx, &iconnectionserver_last, update_instance_alias, 3, s, "connection_server_instance", "connection_server_instance_alias");
	if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, iconnectionserver) == DM_STOP)
		break;
	}
	return 0;
}

