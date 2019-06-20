/*
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <unistd.h>
#include <string.h>
#include "xmppuci.h"
#include "xmppconfig.h"

char *get_xmppconnection_enable(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "enable");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmppconnection_server_enable(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection_server", s) {
		conn_inst = dmuci_get_value_bysection(s, "id_connection");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "enable");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_username(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "username");
			return v;
		}
	}
	v = "";
	return v;
}


char *get_xmpp_password(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "password");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_domain(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;

	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "domain");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_resource(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "resource");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_usetls(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "usetls");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_keepalive_interval(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "interval");
			return v;
		}
		else
			continue;
	}
	v = "";
	return v;
}

char *get_xmpp_connect_attempts(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "attempt");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_connect_initial_retry_interval(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "initial_retry_interval");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_connect_retry_interval_multiplier(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "retry_interval_multiplier");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_connect_retry_max_interval(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "retry_max_interval");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_serveralgorithm(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection", s) {
		conn_inst = dmuci_get_value_bysection(s, "connection_instance");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "serveralgorithm");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_server_address(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection_server", s) {
		conn_inst = dmuci_get_value_bysection(s, "id_connection");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "server_address");
			return v;
		}
	}
	v = "";
	return v;
}

char *get_xmpp_port(char *instance)
{
	struct uci_section *s;
	char *v, *conn_inst;
	dmuci_foreach_section("cwmp_xmpp", "xmpp_connection_server", s) {
		conn_inst = dmuci_get_value_bysection(s, "id_connection");
		if(strcmp(conn_inst, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, "port");
			return v;
		}
	}
	v = "";
	return v;
}
