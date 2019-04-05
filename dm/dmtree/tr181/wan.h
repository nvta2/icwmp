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
#ifndef __WAN_H
#define __WAN_H

struct atm_args
{
	struct uci_section *atm_sec;
	char *ifname;
};

struct ptm_args
{
	struct uci_section *ptm_sec;
	char *ifname;
};

extern DMOBJ tAtmObj[];
extern DMOBJ tPtmObj[];
extern DMOBJ tAtmLinkStatsObj[];
extern DMOBJ tPtmLinkStatsObj[];
extern DMLEAF tAtmLineParams[];
extern DMLEAF tAtmLinkStatsParams[] ;
extern DMLEAF tPtmLinkStatsParams[];
extern DMLEAF tPtmLineParams[];

int browseAtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsePtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_atm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_atm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ptm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ptm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_atm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_link_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_link_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_stats_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_stats_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_stats_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_atm_stats_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_link_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_stats_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_stats_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_stats_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ptm_stats_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_atm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_link_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ptm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_atm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_ptm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
