/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2019 iopsys Software Solutions AB
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#ifndef __ROUTING_H
#define __ROUTING_H

#define PROC_ROUTE6 "/proc/net/ipv6_route"
extern struct dm_permession_s DMRouting;

extern DMOBJ tRoutingObj[];
extern DMLEAF tRoutingParam[];
extern DMOBJ tRouterObj[];
extern DMLEAF tRouterInstParam[];
extern DMLEAF tIPv4ForwardingParam[];
extern DMLEAF tIPv6ForwardingParams[];
extern DMOBJ tRoutingRouteInformationObj[];
extern DMLEAF tRoutingRouteInformationParams[];
extern DMLEAF tRoutingRouteInformationInterfaceSettingParams[];

int browseRouterInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPv4ForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPv6ForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseRoutingRouteInformationInterfaceSettingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_ipv4forwarding(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ipv4forwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv6Forwarding(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_ipv6Forwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_router_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouter_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouter_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouter_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouter_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouter_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouter_IPv4ForwardingNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouter_IPv6ForwardingNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_static_route(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_forwarding_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_RoutingRouterIPv6Forwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouterIPv6Forwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_DestIPPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_DestIPPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_NextHop(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_NextHop(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_Origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouterIPv6Forwarding_ForwardingMetric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouterIPv6Forwarding_ForwardingMetric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouterIPv6Forwarding_ExpirationTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_RoutingRouteInformation_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_RoutingRouteInformation_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_RoutingRouteInformation_InterfaceSettingNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouteInformationInterfaceSetting_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouteInformationInterfaceSetting_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouteInformationInterfaceSetting_SourceRouter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_RoutingRouteInformationInterfaceSetting_RouteLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif
