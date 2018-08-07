/*
 * Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
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

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <strophe.h>
#include "xmpp.h"
#include "xmppcmd.h"
#include "xmppuci.h"
#include "xmpplog.h"
#include "xmppconfig.h"

struct xmpp_config cur_xmpp_conf = {0};
struct xmpp_connection cur_xmpp_con = {0};

static int send_stanza_cr_response(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
	xmpp_stanza_t *reply;
	xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;

    reply = xmpp_stanza_new(ctx);
	if (!reply) {
		xmpp_log(SCRIT,"XMPP CR response Error");
		return -1;
	}
	xmpp_stanza_set_name(reply, "iq");
	xmpp_stanza_set_type(reply, "result");
	xmpp_stanza_set_attribute(reply, "from", xmpp_stanza_get_attribute(stanza, "to"));
	xmpp_stanza_set_attribute(reply, "id", xmpp_stanza_get_attribute(stanza, "id"));
	xmpp_stanza_set_attribute(reply, "to", xmpp_stanza_get_attribute(stanza, "from"));
	xmpp_send(conn, reply);
	xmpp_stanza_release(reply);
	return 0;
}

static int send_stanza_cr_error(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata, int xmpp_error)
{
	xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
	xmpp_stanza_t *cr_stanza, *child, *child1, *child2, *child3, *child4, *child5, *stext, *stext2, *mech;
	char *username, *password;

	cr_stanza = xmpp_stanza_new(ctx);
	if (!cr_stanza) {
		xmpp_log(SCRIT,"XMPP CR response Error");
		return -1;
	}
	xmpp_stanza_set_name(cr_stanza, "iq");
	xmpp_stanza_set_type(cr_stanza, "error");
	xmpp_stanza_set_attribute(cr_stanza, "id", xmpp_stanza_get_attribute(stanza, "id"));
	xmpp_stanza_set_attribute(cr_stanza, "to", xmpp_stanza_get_attribute(stanza, "from"));
	xmpp_stanza_set_attribute(cr_stanza, "from", xmpp_stanza_get_attribute(stanza, "to"));
	child1 = xmpp_stanza_get_child_by_name(stanza, "connectionRequest");
	if(child1) {
		mech = xmpp_stanza_get_child_by_name(child1, "username");
		if (mech)
			username = xmpp_stanza_get_text(mech);
		mech = xmpp_stanza_get_next(mech);
		if (strcmp(xmpp_stanza_get_name(mech), "password") == 0)
			password = xmpp_stanza_get_text(mech);
	}
	child3 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name( child3, "connectionRequest");
	xmpp_stanza_set_ns(child3, XMPP_CR_NS);
	child4 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name( child4, "username");
	stext = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(stext, username);
	xmpp_stanza_add_child(child4, stext);
	xmpp_stanza_add_child(child3, child4);
	child5 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name( child5, "password");
	stext2 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(stext2, password);
	xmpp_stanza_add_child(child5, stext2);
	xmpp_stanza_add_child(child3, child5);
	xmpp_stanza_add_child(cr_stanza, child3);
	child = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(child, "error");
	if (xmpp_error == XMPP_SERVICE_UNAVAILABLE)
		xmpp_stanza_set_attribute(child, "code", "503");
	xmpp_stanza_set_type(child, "cancel");
	child2 = xmpp_stanza_new(ctx);
	if (xmpp_error == XMPP_SERVICE_UNAVAILABLE)
		xmpp_stanza_set_name(child2, "service-unavailable");
	else if (xmpp_error == XMPP_NOT_AUTHORIZED)
		xmpp_stanza_set_name(child2, "not-autorized");
	xmpp_stanza_set_attribute(child2, "xmlns", XMPP_ERROR_NS);
	xmpp_stanza_add_child(child, child2);
	xmpp_stanza_add_child(cr_stanza, child);
	xmpp_send(conn, cr_stanza);
	xmpp_stanza_release(cr_stanza);
	return 0;
}

int check_xmpp_authorized(char *from)
{
	char *pch, *spch, *str;
	int len;

	if (cur_xmpp_conf.xmpp_allowed_jid == NULL || cur_xmpp_conf.xmpp_allowed_jid[0] == '\0')
	{
		xmpp_log(SDEBUG,"xmpp connection request handler : allowed jid is empty");
		return 1;
	}
	else
	{
		str = strdup(cur_xmpp_conf.xmpp_allowed_jid);
		pch = strtok_r(str, ",", &spch);
		len = strlen(pch);
		while (pch != NULL) {
			if(strncmp(pch, from, len) == 0){
				free(str);
				return 1;
			}
			pch = strtok_r(NULL, ",", &spch);
		}
		free(str);
		return 0;
	}
}

static int cr_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza1, void * const userdata)
{
	time_t current_time;
	static int request = 0;
    static time_t restrict_start_time = 0;
	xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
	char *name_space, *text, *from, *username, *password;
	xmpp_stanza_t *child, *mech;
	bool valid_ns = true, auth_status = false, service_available = false, permitted = true;

	if(xmpp_stanza_get_child_by_name(stanza1, "connectionRequest")) {
		from = xmpp_stanza_get_attribute(stanza1, "from");
		request++;
		current_time = time(NULL);
        if ((restrict_start_time==0) ||
            ((current_time-restrict_start_time) > CONNECTION_REQUEST_RESTRICT_PERIOD))
        {
            restrict_start_time = current_time;
            request  = 1;
        }
        else
        {
            request++;
            if (request > CONNECTION_REQUEST_RESTRICT_REQUEST)
            {
                restrict_start_time = current_time;
                permitted = false;
				xmpp_log(SINFO,"Permitted CR Request Exceeded");
				goto xmpp_end;
            }
        }
	}
	else {
		xmpp_log(SDEBUG,"xmpp connection request handler does not contain an iq type");
		return 1;
	}
	if (!check_xmpp_authorized(from))
	{
		service_available = false;
		xmpp_log(SDEBUG,"xmpp connection request handler not authorized by allowed jid");
		goto xmpp_end;
	}

	child = xmpp_stanza_get_child_by_name(stanza1, "connectionRequest");
	if(child) {
		service_available = true;
		name_space = xmpp_stanza_get_attribute(child, "xmlns");
		if(strcmp(name_space, XMPP_CR_NS) != 0)
		{
			valid_ns = false;
			goto xmpp_end; //send error response
		}
		mech = xmpp_stanza_get_child_by_name(child, "username");
		if (mech) {
			text = xmpp_stanza_get_text(mech);
			dmuci_init();
			username = dmuci_get_value("cwmp", "cpe", "userid");
			dmuci_fini();
			if(strcmp(text, username) == 0) {
				mech = xmpp_stanza_get_next(mech);
				if (strcmp(xmpp_stanza_get_name(mech), "password") == 0) {
					text = xmpp_stanza_get_text(mech);
					dmuci_init();
					password = dmuci_get_value("cwmp", "cpe", "passwd");
					dmuci_fini();
					if(strcmp(text, password) == 0)
						auth_status = true;
					else
						auth_status = false;
				}
			}
		}
	}
	else
	{
		service_available = false;
		goto xmpp_end; //send error response
	}
xmpp_end:
	if (!valid_ns)
	{
		xmpp_log(SINFO,"XMPP Invalid Name space");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_SERVICE_UNAVAILABLE);
		return 1;
	}
	else if (!permitted)
	{
		xmpp_log(SINFO,"XMPP Invalid Name space");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_SERVICE_UNAVAILABLE);
		return 1;
	}
	else if (!service_available) {
		xmpp_log(SINFO,"XMPP Service Unavailable");
		return 1;
	} else if (!auth_status) {
		xmpp_log(SINFO,"XMPP Not Authorized");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_NOT_AUTHORIZED);
		return 1;
	} else {
		xmpp_log(SINFO,"XMPP Authorized");
		send_stanza_cr_response(conn, stanza1, userdata);
		XMPP_CMD(7, "ubus", "-t", "3", "call", "tr069", "inform", "{\"event\" : \"connection request\"}");
		return 1;
	}
	return 1;
}

void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
		const int error, xmpp_stream_error_t * const stream_error,
		void * const userdata)
{
	xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
	xmpp_stanza_t* pres;
	static int attempt = 0;

	if (status == XMPP_CONN_CONNECT) {
		attempt = 0;
		xmpp_log(SINFO,"XMPP Connection Established");
		xmpp_handler_add(conn, cr_handler, NULL, "iq", NULL, ctx);
		pres = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(pres, "presence");
		xmpp_send(conn, pres);
		xmpp_stanza_release(pres);
		xmpp_conn_set_keepalive(conn, 30, cur_xmpp_con.keepalive_interval);
	}
	else
	{
		xmpp_log(SINFO,"XMPP Connection Lost");
		xmpp_exit(ctx, conn);
		xmpp_log(SINFO,"XMPP Connection Retry");
		srand(time(NULL));
		if (attempt == 0 && cur_xmpp_con.connect_attempt != 0 )
		{
			if (cur_xmpp_con.retry_initial_interval != 0)
			sleep(rand()%cur_xmpp_con.retry_initial_interval);
		}
		else if(attempt > cur_xmpp_con.connect_attempt)
		{
			xmpp_log(SINFO,"XMPP Connection Aborted");
			xmpp_exit(ctx, conn);
			xmpp_con_exit();
			exit(EXIT_FAILURE);
		}
		else if( attempt >= 1 && cur_xmpp_con.connect_attempt != 0 )
		{
			int delay = cur_xmpp_con.retry_initial_interval * (cur_xmpp_con.retry_interval_multiplier/1000) * (attempt -1);
			if (delay > cur_xmpp_con.retry_max_interval)
				sleep(cur_xmpp_con.retry_max_interval);
			else
				sleep(delay);
		}
		else
			sleep(DEFAULT_XMPP_RECONNECTION_RETRY);
		attempt += 1;
		xmpp_connecting();
	}
}

void xmpp_connecting(void)
{
	xmpp_ctx_t *ctx;
	xmpp_conn_t *conn;
	xmpp_log_t *log;
	char *jid, *pass;
	static int attempt = 0;
	int connected = 0, delay = 0;

	xmpp_initialize();
	log = xmpp_get_default_logger(XMPP_LEVEL_ERROR);
	ctx = xmpp_ctx_new(NULL, log);
	conn = xmpp_conn_new(ctx);
	asprintf(&jid, "%s@%s/%s", cur_xmpp_con.username, cur_xmpp_con.domain, cur_xmpp_con.resource);
	xmpp_conn_set_jid(conn, jid);
	xmpp_conn_set_pass(conn, cur_xmpp_con.password);
	free(jid);
	/* initiate connection */
	if( strcmp(cur_xmpp_con.serveralgorithm,"DNS-SRV") == 0)
		connected = xmpp_connect_client(conn, NULL, 0, conn_handler, ctx);
	else
		connected = xmpp_connect_client(conn, cur_xmpp_con.serveraddress[0] ? cur_xmpp_con.serveraddress : NULL,
										cur_xmpp_con.port, conn_handler, ctx);

	if (connected == -1 )
	{
		xmpp_exit(ctx, conn);
		xmpp_log(SINFO,"XMPP Connection Retry");
		srand(time(NULL));
		if (attempt == 0 && cur_xmpp_con.connect_attempt != 0 )
		{
			if (cur_xmpp_con.retry_initial_interval != 0)
				sleep(rand()%cur_xmpp_con.retry_initial_interval);
		}
		else if(attempt > cur_xmpp_con.connect_attempt)
		{
			xmpp_log(SINFO,"XMPP Connection Aborted");
			xmpp_exit(ctx, conn);
			xmpp_con_exit();
			exit(EXIT_FAILURE);
		}
		else if( attempt >= 1 && cur_xmpp_con.connect_attempt != 0 )
		{
			delay = cur_xmpp_con.retry_initial_interval * (cur_xmpp_con.retry_interval_multiplier/1000) * (attempt -1);
			if (delay > cur_xmpp_con.retry_max_interval)
				sleep(cur_xmpp_con.retry_max_interval);
			else
				sleep(delay);
		}
		else
			sleep(DEFAULT_XMPP_RECONNECTION_RETRY);
		attempt += 1;
		xmpp_connecting();
	}
	else
	{
		attempt = 0;
		xmpp_log(SDEBUG,"XMPP Handle Connection");
		xmpp_run(ctx);
	}
}

void xmpp_exit(xmpp_ctx_t *ctx, xmpp_conn_t *conn)
{
	xmpp_stop(ctx);
	xmpp_conn_release(conn);
	xmpp_ctx_free(ctx);
	xmpp_shutdown();
}

void xmpp_global_conf(void)
{
	int a = 0, b;
	char *value = NULL;

	value = dmuci_get_value("cwmp", "cpe", "amd_version");
	if( atoi(value) >= 5)
	{
		value = dmuci_get_value("cwmp_xmpp", "xmpp", "loglevel");
		if(value != NULL && *value != '\0')
		{
			b = atoi(value);
			free(value);
			value = NULL;
			cur_xmpp_conf.xmpp_loglevel = b;
		}
		else
			cur_xmpp_conf.xmpp_loglevel = DEFAULT_LOGLEVEL;
		xmpp_log(SDEBUG,"Log Level of XMPP connection is :%d", cur_xmpp_conf.xmpp_loglevel);

		value = dmuci_get_value("cwmp_xmpp", "xmpp", "enable");
		if(value != NULL && *value != '\0')
		{
			if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0))
			{
				cur_xmpp_conf.xmpp_enable = true;
				xmpp_log(SDEBUG,"XMPP connection is Enabled :%d", cur_xmpp_conf.xmpp_enable);
			}
			free(value);
			value = NULL;
		}

		value = dmuci_get_value("cwmp_xmpp", "xmpp", "id");
		if(value != NULL && *value != '\0')
		{
			a = atoi(value);
			free(value);
			value = NULL;
		}
		if(a==0)
		{
			xmpp_log(SDEBUG,"XMPP connection id :Empty");
			cur_xmpp_conf.xmpp_connection_id = 0;
		}
		else
		{
			xmpp_log(SDEBUG,"XMPP connection id :%d", a);
			cur_xmpp_conf.xmpp_connection_id = a;
		}

		value = dmuci_get_value("cwmp_xmpp", "xmpp", "allowed_jid");
		if(value != NULL && *value != '\0')
		{
			if (cur_xmpp_conf.xmpp_allowed_jid != NULL)
			{
				free(cur_xmpp_conf.xmpp_allowed_jid);
			}
			cur_xmpp_conf.xmpp_allowed_jid = strdup(value);
			xmpp_log(SDEBUG,"XMPP connection allowed jaber id :%s", cur_xmpp_conf.xmpp_allowed_jid);
			free(value);
			value = NULL;
		}
		else
		{
			cur_xmpp_conf.xmpp_allowed_jid = strdup("");
			xmpp_log(SDEBUG,"XMPP connection allowed jaber id is empty");
		}
	}
}

int xmpp_con_init(void)
{
	char *instance, *connection_enable, *connection, *connectionserver_enable, *connectionserver;
	char *enable_conn, *enable_srv;
	dmuci_init();
	instance = dmuci_get_value("cwmp_xmpp", "xmpp", "id");
	asprintf(&connection, "%s", get_xmppconnection_enable(instance));
	asprintf(&connectionserver, "%s", get_xmppconnection_server_enable(instance));
	connection_enable = strdup(connection);
	connectionserver_enable = strdup(connectionserver);
	free(connection);
	free(connectionserver);
	cur_xmpp_con.serveralgorithm = strdup((const char *)get_xmpp_serveralgorithm(instance));
    if( strcmp(cur_xmpp_con.serveralgorithm,"DNS-SRV") == 0)
    {
         if(connection_enable[0] == '\0' || connection_enable[0] == '0')
        {
        	 cur_xmpp_conf.xmpp_enable = false;//disable xmpp_enable
            goto end;
        }
    }
    else
    {
        if(connection_enable[0] == '\0' || connection_enable[0] == '0' || connectionserver_enable[0] == '\0' || connectionserver_enable[0] == '0')
        {
        	cur_xmpp_conf.xmpp_enable = false;//disable xmpp_enable
            goto end;
        }
    }
	cur_xmpp_con.username = strdup((const char *)get_xmpp_username(instance));
	cur_xmpp_con.password = strdup((const char *)get_xmpp_password(instance));
	cur_xmpp_con.domain = strdup((const char *)get_xmpp_domain(instance));
	cur_xmpp_con.resource = strdup((const char *)get_xmpp_resource(instance));
	cur_xmpp_con.serveraddress = strdup((const char *)get_xmpp_server_address(instance));
	cur_xmpp_con.port = atoi((const char *)get_xmpp_port(instance));
	cur_xmpp_con.keepalive_interval = atoi((const char *)get_xmpp_keepalive_interval(instance));
	cur_xmpp_con.connect_attempt = atoi((const char *)get_xmpp_connect_attempts(instance));
	if(cur_xmpp_con.connect_attempt)
	{
		cur_xmpp_con.retry_initial_interval = atoi((const char *)get_xmpp_connect_initial_retry_interval(instance));
		cur_xmpp_con.retry_initial_interval = (cur_xmpp_con.retry_initial_interval) ? cur_xmpp_con.retry_initial_interval : DEFAULT_RETRY_INITIAL_INTERVAL;
		cur_xmpp_con.retry_interval_multiplier = atoi((const char *)get_xmpp_connect_retry_interval_multiplier(instance));
		cur_xmpp_con.retry_interval_multiplier = (cur_xmpp_con.retry_interval_multiplier) ? cur_xmpp_con.retry_interval_multiplier : DEFAULT_RETRY_INTERVAL_MULTIPLIER;
		cur_xmpp_con.retry_max_interval = atoi((const char *)get_xmpp_connect_retry_max_interval(instance));
		cur_xmpp_con.retry_max_interval = (cur_xmpp_con.retry_max_interval) ? cur_xmpp_con.retry_max_interval : DEFAULT_RETRY_MAX_INTERVAL;
	}

	xmpp_log(SDEBUG,"XMPP username: %s", cur_xmpp_con.username);
	xmpp_log(SDEBUG,"XMPP password: %s", cur_xmpp_con.password);
	xmpp_log(SDEBUG,"XMPP domain: %s", cur_xmpp_con.domain);
	xmpp_log(SDEBUG,"XMPP resource: %s", cur_xmpp_con.resource);
	xmpp_log(SDEBUG,"XMPP serveralgorithm: %s", cur_xmpp_con.serveralgorithm);
	xmpp_log(SDEBUG,"XMPP server_address: %s", cur_xmpp_con.serveraddress);
	xmpp_log(SDEBUG,"XMPP port: %d", cur_xmpp_con.port);
	xmpp_log(SDEBUG,"XMPP keepalive_interval: %d", cur_xmpp_con.keepalive_interval);
	xmpp_log(SDEBUG,"XMPP connect_attempt: %d", cur_xmpp_con.connect_attempt);
	xmpp_log(SDEBUG,"XMPP retry_initial_interval: %d", cur_xmpp_con.retry_initial_interval);
	xmpp_log(SDEBUG,"XMPP retry_interval_multiplier: %d", cur_xmpp_con.retry_interval_multiplier);
	xmpp_log(SDEBUG,"XMPP retry_max_interval: %d", cur_xmpp_con.retry_max_interval);
end:
	dmuci_fini();
	return 0;
}

void xmpp_con_exit(void)
{
	free(cur_xmpp_con.username);
	free(cur_xmpp_con.password);
	free(cur_xmpp_con.domain);
	free(cur_xmpp_con.resource);
	free(cur_xmpp_con.serveraddress);
	free(cur_xmpp_con.serveralgorithm);
	free(cur_xmpp_conf.xmpp_allowed_jid);
}

int main(void)
{
	dmuci_init();
	xmpp_global_conf();
	dmuci_fini();
	xmpp_log(SINFO,"START XMPP");

	if (cur_xmpp_conf.xmpp_enable && cur_xmpp_conf.xmpp_connection_id > 0)
		xmpp_con_init();
    else
       xmpp_log(SINFO,"XMPP is Disabled");

	if (cur_xmpp_conf.xmpp_enable)
		xmpp_connecting();

	xmpp_con_exit();
	xmpp_log(SINFO,"EXIT XMPP");
	return 0;
}
