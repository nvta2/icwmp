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

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include "cwmp.h"
#include "log.h"
#include "http.h"
#include "xmpp_cr.h"
#include <unistd.h>
#ifdef XMPP_ENABLE
#include <strophe.h>
#endif

static int send_stanza_cr_response(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
	xmpp_stanza_t *reply;
	xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
	
    reply = xmpp_stanza_new(ctx);
	if (!reply) {
		CWMP_LOG(ERROR,"XMPP CR response Error");
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
		CWMP_LOG(ERROR,"XMPP CR response Error");
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
	struct cwmp *cwmp = &cwmp_main;
	
	if (cwmp->conf.xmpp_allowed_jid == NULL || cwmp->conf.xmpp_allowed_jid[0] == '\0')
		return 1;
	else
	{
		str = strdup(cwmp->conf.xmpp_allowed_jid);
		pch = strtok_r(str, ",", &spch);
		int len = strlen(pch);
		while (pch != NULL) {
			if(strncmp(pch, from, len) == 0 && (from[len] == '\0' || from[len] == '/') ){
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
	char *intext = NULL, *name_space, *text, *from, *username, *password;
	xmpp_stanza_t *child, *mech;
	bool valid_ns = true, auth_status = false, service_available = false, permitted = true;

    if(xmpp_stanza_get_child_by_name(stanza1, "connectionRequest")) {
		intext = xmpp_stanza_get_text(xmpp_stanza_get_child_by_name(stanza1, "connectionRequest"));
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
				CWMP_LOG(INFO,"Permitted CR Request Exceeded");
				goto xmpp_end;
            }
        }
	}
	else {
		return 1;
	}
	if (!check_xmpp_authorized(from))
	{
		service_available = false;
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
			uci_get_value(UCI_CPE_USERID_PATH,&username);
			if(strcmp(text, username) == 0) {
				mech = xmpp_stanza_get_next(mech);
				if (strcmp(xmpp_stanza_get_name(mech), "password") == 0) {
					text = xmpp_stanza_get_text(mech);
					uci_get_value(UCI_CPE_PASSWD_PATH,&password);
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
		CWMP_LOG(INFO,"XMPP Invalid Name space");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_SERVICE_UNAVAILABLE);
		return 1;
	}
	else if (!permitted)
	{
		CWMP_LOG(INFO,"XMPP Invalid Name space");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_SERVICE_UNAVAILABLE);
		return 1;		
	}
	else if (!service_available) {
		CWMP_LOG(INFO,"XMPP Service Unavailable");
		return 1;
	} else if (!auth_status) {
		CWMP_LOG(INFO,"XMPP Not Authorized");
		send_stanza_cr_error(conn, stanza1, userdata, XMPP_NOT_AUTHORIZED);
		return 1;
	} else {
		CWMP_LOG(INFO,"XMPP Authorized");
		send_stanza_cr_response(conn, stanza1, userdata);
		http_success_cr();
		return 1;
	}
	return 1;
}

void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
const int error, xmpp_stream_error_t * const stream_error,
void * const userdata)
{
	xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
	struct cwmp *cwmp = &cwmp_main;
	long int keepalive;
	static int attempt = 0;

	if (status == XMPP_CONN_CONNECT) {
		attempt = 0;
		xmpp_stanza_t* pres;
		CWMP_LOG(INFO,"XMPP Connection Established");
		xmpp_handler_add(conn, cr_handler, NULL, "iq", NULL, ctx);
		pres = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(pres, "presence");
		xmpp_send(conn, pres);
		xmpp_stanza_release(pres);
		xmpp_conn_set_keepalive(conn, 30, cwmp->xmpp_param.keepalive_interval);
	}
	else 
	{
		CWMP_LOG(INFO,"XMPP Connection Lost");
		xmpp_stop(cwmp->xmpp_ctx);
		cwmp_xmpp_exit();
		CWMP_LOG(INFO,"XMPP Connection Retry");
		srand(time(NULL));		
		if (attempt == 0 && cwmp->xmpp_param.connect_attempt != 0 )
		{
			if (cwmp->xmpp_param.retry_initial_interval != 0)
			sleep(rand()%cwmp->xmpp_param.retry_initial_interval);
		}
		else if(attempt > cwmp->xmpp_param.connect_attempt)
		{
			CWMP_LOG(INFO,"XMPP Connection Aborted");
			pthread_exit(0);			
			//xmpp_stop(cwmp->xmpp_ctx);
		}			
		else if( attempt >= 1 && cwmp->xmpp_param.connect_attempt != 0 )
		{
			int delay = cwmp->xmpp_param.retry_initial_interval * (cwmp->xmpp_param.retry_interval_multiplier/1000) * (attempt -1);
			if (delay > cwmp->xmpp_param.retry_max_interval)
				sleep(cwmp->xmpp_param.retry_max_interval);
			else
				sleep(delay);
		}
		else
			sleep(DEFAULT_XMPP_RECONNECTION_RETRY);
		attempt += 1;		
		cwmp_xmpp_connect_client();		
	}
}

void cwmp_xmpp_connect_client()
{
	xmpp_log_t 	*log;
	char 		*jid, *pass;
	static int attempt = 0;
	int connected = 0, delay = 0;
	const xmpp_conn_event_t status;
	struct cwmp *cwmp = &cwmp_main;

	xmpp_initialize();
	log = xmpp_get_default_logger(XMPP_LEVEL_ERROR);	
	cwmp->xmpp_ctx = xmpp_ctx_new(NULL, log);
	cwmp->xmpp_conn = xmpp_conn_new(cwmp->xmpp_ctx);
	asprintf(&jid, "%s@%s/%s", cwmp->xmpp_param.username, cwmp->xmpp_param.domain, cwmp->xmpp_param.ressource);
	xmpp_conn_set_jid(cwmp->xmpp_conn, jid);
	xmpp_conn_set_pass(cwmp->xmpp_conn, cwmp->xmpp_param.password);
	free(jid);
	/* initiate connection */
	if( strcmp(cwmp->xmpp_param.serveralgorithm,"DNS-SRV") == 0)
		connected = xmpp_connect_client(cwmp->xmpp_conn, NULL, 0, conn_handler, cwmp->xmpp_ctx);
	else
		connected = xmpp_connect_client(cwmp->xmpp_conn, cwmp->xmpp_param.serveraddress[0] ? cwmp->xmpp_param.serveraddress : NULL,
										cwmp->xmpp_param.port, conn_handler, cwmp->xmpp_ctx);
	if (connected == -1 )
	{
		xmpp_stop(cwmp->xmpp_ctx);
		cwmp_xmpp_exit();
		CWMP_LOG(INFO,"XMPP Connection Retry");
		srand(time(NULL));
		if (attempt == 0 && cwmp->xmpp_param.connect_attempt != 0 )
		{
			if (cwmp->xmpp_param.retry_initial_interval != 0)
				sleep(rand()%cwmp->xmpp_param.retry_initial_interval);
		}
		else if(attempt > cwmp->xmpp_param.connect_attempt)
		{
			CWMP_LOG(INFO,"XMPP Connection Aborted");
			pthread_exit(0);
		}
		else if( attempt >= 1 && cwmp->xmpp_param.connect_attempt != 0 )
		{
			delay = cwmp->xmpp_param.retry_initial_interval * (cwmp->xmpp_param.retry_interval_multiplier/1000) * (attempt -1);
			if (delay > cwmp->xmpp_param.retry_max_interval)
				sleep(cwmp->xmpp_param.retry_max_interval);
			else
				sleep(delay);
		}
		else
			sleep(DEFAULT_XMPP_RECONNECTION_RETRY);
		attempt += 1;
		cwmp_xmpp_connect_client();
	}
	else
	{
		attempt = 0;
		CWMP_LOG(INFO,"XMPP Handle Connection");
		xmpp_run(cwmp->xmpp_ctx);
	}
}

void cwmp_xmpp_exit()
{
	struct cwmp *cwmp = &cwmp_main;	
		
	xmpp_conn_release(cwmp->xmpp_conn);
	xmpp_ctx_free(cwmp->xmpp_ctx);
	xmpp_shutdown();
}
