/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *		Author: Imen BHIRI <imen.bhiri@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */
#ifndef __NAT_H
#define __NAT_H

extern DMOBJ tnatObj[];
extern DMLEAF tnatParam[];
extern DMLEAF tInterfaceSettingParam[];
extern DMLEAF tPortMappingParam[];

int browseInterfaceSettingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsePortMappingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_NAT_InterfaceSetting(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_NAT_InterfaceSetting(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_NAT_PortMapping(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_NAT_PortMapping(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_nat_interface_setting_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_port_mapping_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_nat_interface_setting_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_interface_setting_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_interface_setting_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_interface_setting_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_interface_setting_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_interface_setting_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_interface_setting_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_nat_port_mapping_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_port_mapping_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_all_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_all_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_lease_duration(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_lease_duration(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_remote_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_remote_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_external_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_external_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_external_port_end_range(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_external_port_end_range(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_internal_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_internal_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_internal_client(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_internal_client(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_nat_port_mapping_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_nat_port_mapping_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
