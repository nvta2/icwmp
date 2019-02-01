/*
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      Copyright (C) 2019 iopsys Software Solutions AB
 *
 *      Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef _FIREWALL_H
#define _FIREWALL_H

#include "dmcwmp.h"


extern DMOBJ tFirewallObj[];
extern DMLEAF tFirewallParams[];
extern DMLEAF tLevelParams[];
extern DMLEAF tChainParams[];
extern DMOBJ tChainObj[];
extern DMLEAF tRuleParams[];
extern DMOBJ tRuleObj[];
extern DMLEAF tTimeSpanParams[];

int browseLevelInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseChainInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseRuleInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int get_firewall_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_firewall_config(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_firewall_advanced_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_firewall_level_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_firewall_chain_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_level_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_level_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_level_chain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_level_port_mapping_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_level_default_log_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_chain_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_chain_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_chain_creator(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_chain_rule_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_target(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_target_chain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_dest_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_i_p_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_dest_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_dest_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_dest_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_dest_port_range_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_port_range_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_icmp_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_rule_source_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_span_supported_days(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_span_days(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_span_start_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_span_stop_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_firewall_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_firewall_config(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_firewall_advanced_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_level_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_level_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_level_port_mapping_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_level_default_log_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_chain_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_chain_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_target(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_target_chain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_dest_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_i_p_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_dest_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_dest_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_dest_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_dest_port_range_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_port_range_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_icmp_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_rule_source_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_span_supported_days(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_span_days(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_span_start_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_span_stop_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
