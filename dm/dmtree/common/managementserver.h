/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */
#ifndef __MANAGEMENT_SERVER_H
#define __MANAGEMENT_SERVER_H
#include "dmcwmp.h"
extern DMLEAF tManagementServerParams[];

int get_management_server_url(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_empty(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_periodic_inform_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_periodic_inform_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_periodic_inform_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_connection_request_url(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_connection_request_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_http_compression_supportted(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_http_compression(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lwn_protocol_supported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lwn_protocol_used(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lwn_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lwn_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_retry_min_wait_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_alias_based_addressing(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_instance_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_conn_rep_allowed_jabber_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_conn_req_jabber_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_conn_req_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_management_server_supported_conn_req_methods(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_upd_cr_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_upd_cr_address_notification_limit(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_maximum_keepalive_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_stun_minimum_keepalive_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_detected(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_management_server_url(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_passwd(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_periodic_inform_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_periodic_inform_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_periodic_inform_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_connection_request_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_connection_request_passwd(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_http_compression(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lwn_protocol_used(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lwn_protocol_used(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lwn_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_lwn_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_retry_min_wait_interval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_retry_interval_multiplier(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_instance_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_conn_rep_allowed_jabber_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_management_server_conn_req_xmpp_connection(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int set_upd_cr_address_notification_limit(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_server_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_maximum_keepalive_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_stun_minimum_keepalive_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
