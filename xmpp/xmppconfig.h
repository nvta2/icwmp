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

#ifndef _XMPPCONFIG_H__
#define _XMPPCONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xmpp.h"

char *get_xmppconnection_enable(char *instance);
char *get_xmppconnection_server_enable(char *instance);
char *get_xmpp_username(char *instance);
char *get_xmpp_password(char *instance);
char *get_xmpp_domain(char *instance);
char *get_xmpp_resource(char *instance);
char *get_xmpp_keepalive_interval(char *instance);
char *get_xmpp_connect_attempts(char *instance);
char *get_xmpp_connect_initial_retry_interval(char *instance);
char *get_xmpp_connect_retry_interval_multiplier(char *instance);
char *get_xmpp_connect_retry_max_interval(char *instance);
char *get_xmpp_serveralgorithm(char *instance);
char *get_xmpp_server_address(char *instance);
char *get_xmpp_port(char *instance);

#endif /* _XMPPCONFIG_H__ */
