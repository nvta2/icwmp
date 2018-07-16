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

#ifndef __XMPP_H
#define __XMPP_H

extern DMOBJ tXMPPObj[];
extern DMLEAF tXMPPParams[];
extern DMLEAF tXMPPConnectionParams[];
extern DMOBJ tXMPPConnectionObj[];
extern DMLEAF tXMPPConnectionServerParams[];

int browsexmpp_connectionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsexmpp_connection_serverInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

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

int get_xmpp_connection_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_supported_server_connect_algorithms(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_connection_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_resource(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_connect_algorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_keepalive_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_attempts(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_retry_initial_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_retry_max_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_usetls(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_jabber_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_xmpp_connection_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_connection_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_resource(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_connect_algorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_keepalive_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_attempts(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_retry_initial_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_retry_max_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_usetls(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_xmpp_connection_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_xmpp_connection_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
