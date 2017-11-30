/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *
 */
#ifndef __IP_H
#define __IP_H

extern struct ip_ping_diagnostic ipping_diagnostic; 
struct ip_args
{
	struct uci_section *ip_sec;
	char *ip_4address;
	char *ip_6address;
};

extern DMOBJ tIPObj[];
extern DMOBJ tInterfaceObj[];
extern DMLEAF tIPv4Params[];
extern DMLEAF tIPv6Params[];
extern DMLEAF tIPintParams[];
extern DMLEAF tIpPingDiagParams[];
extern DMOBJ tDiagnosticObj[];
unsigned char get_ipv4_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance);
unsigned char get_ipv6_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance);
int browseIPIfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIfaceIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIfaceIPv6Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_ip_interface(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ip_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv4(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ipv4(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv6(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ipv6(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_ip_interface_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_int_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv4_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv6_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv6_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipv6_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_failure_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_average_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_min_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_max_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_ip_int_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_int_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv4_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv6_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv6_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv6_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipv6_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ip_interface_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_linker_ip_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
