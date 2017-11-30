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


struct dsl_line_args
{
	struct uci_section *line_sec;
	char *type;
};

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

extern DMOBJ tDslObj[];
extern DMOBJ tAtmObj[];
extern DMOBJ tPtmObj[];
extern DMOBJ tAtmLinkStatsObj[];
extern DMOBJ tPtmLinkStatsObj[];
extern DMLEAF tDslLineParams[];
extern DMLEAF tDslChanelParams[] ;
extern DMLEAF tAtmLineParams[];
extern DMLEAF tAtmLinkStatsParams[] ;
extern DMLEAF tPtmLinkStatsParams[];
extern DMLEAF tPtmLineParams[];

int browseDslLineInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDslChannelInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseAtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browsePtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_atm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_atm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ptm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ptm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_dsl_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_standard_inuse(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vdsl_link_supported_profile(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vdsl_link_profile_inuse(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_downstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_downstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_downstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_upstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_upstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_link_upstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_channel_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_channel_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_channel_downstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dsl_channel_upstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_channel_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_channel_supported_encap(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
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

int set_dsl_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_channel_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_channel_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_link_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ptm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_dsl_line_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_dsl_channel_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_atm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_ptm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif
