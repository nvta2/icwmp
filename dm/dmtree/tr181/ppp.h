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
extern DMLEAF tPPPParams[];
DMLEAF tPPPInterfacePPPoEParams[];

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
int get_PPPInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_ppp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ppp_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_linker_ppp_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_PPP_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_PPPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int add_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_ppp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_PPPInterfacePPPoE_SessionID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_PPPInterfacePPPoE_ACName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_PPPInterfacePPPoE_ACName(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_PPPInterfacePPPoE_ServiceName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_PPPInterfacePPPoE_ServiceName(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
