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
#ifndef __PPP_H
#define __PPP_H

extern DMLEAF tpppInterfaceParam[];
extern DMLEAF tStatsParam[];
extern DMOBJ tpppInterfaceObj[];
extern DMOBJ tpppObj[];

int browseInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_ppp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_eth_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_eth_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_eth_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ppp_eth_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_ppp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_linker_ppp_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int add_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
#endif
