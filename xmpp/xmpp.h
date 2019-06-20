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

#ifndef _XMPP_H__
#define _XMPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define XMPP_CR_NS							"urn:broadband-forum-org:cwmp:xmppConnReq-1-0"
#define XMPP_ERROR_NS						"urn:ietf:params:xml:ns:xmpp-stanzas"
#define DEFAULT_RETRY_INITIAL_INTERVAL		60
#define DEFAULT_RETRY_INTERVAL_MULTIPLIER	2000
#define DEFAULT_RETRY_MAX_INTERVAL			60
#define DEFAULT_XMPP_RECONNECTION_RETRY		5

enum xmpp_cr_error {
	XMPP_CR_NO_ERROR = 0,
	XMPP_SERVICE_UNAVAILABLE,
	XMPP_NOT_AUTHORIZED
};

struct xmpp_config
{
	bool xmpp_enable;
	char *xmpp_allowed_jid;
	int	xmpp_connection_id;
	int xmpp_loglevel;
};

struct xmpp_connection
{
	char *username;
	char *password;
	char *domain;
	char *resource;
	char *serveralgorithm;
	char *serveraddress;
	int keepalive_interval;
	int connect_attempt;
	int retry_initial_interval;
	int retry_interval_multiplier;
	int retry_max_interval;
	int port;
	bool usetls;
};

extern struct xmpp_config cur_xmpp_conf;

void cwmp_xmpp_connect_client();
void cwmp_xmpp_exit();

#endif /* _XMPP_H__ */
