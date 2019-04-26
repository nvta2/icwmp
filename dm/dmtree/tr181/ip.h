/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#ifndef __IP_H
#define __IP_H

struct ip_args
{
	struct uci_section *ip_sec;
	char *ip_4address;
};

struct ipv6_args
{
	struct uci_section *ip_sec;
	char *ip_6address;
	char *ip_6mask;
	char *ip_6preferred;
	char *ip_6valid;
};

struct ipv6prefix_args
{
	struct uci_section *ip_sec;
	char *ip_6prefixaddress;
	char *ip_6prefixmask;
	char *ip_6prefixpreferred;
	char *ip_6prefixvalid;
};

extern DMOBJ tIPObj[];
extern DMLEAF tIPParams[];
extern DMOBJ tInterfaceObj[];
extern DMLEAF tIPv4Params[];
extern DMLEAF tIPv6Params[];
extern DMLEAF tIPv6PrefixParams[];
extern DMLEAF tIPInterfaceStatsParams[];
extern DMLEAF tIPInterfaceTWAMPReflectorParams[];
extern DMLEAF tIPintParams[];
extern DMOBJ tDiagnosticObj[];
extern DMLEAF tIPDiagnosticsParams[];
extern DMLEAF tIpPingDiagParams[];
extern DMOBJ tIPDiagnosticsTraceRouteObj[];
extern DMLEAF tIPDiagnosticsTraceRouteParams[];
extern DMLEAF tIPDiagnosticsTraceRouteRouteHopsParams[];
extern DMOBJ tIPDiagnosticsDownloadDiagnosticsObj[];
extern DMLEAF tIPDiagnosticsDownloadDiagnosticsParams[];
extern DMLEAF tIPDiagnosticsDownloadDiagnosticsPerConnectionResultParams[];
extern DMOBJ tIPDiagnosticsUploadDiagnosticsObj[];
extern DMLEAF tIPDiagnosticsUploadDiagnosticsParams[];
extern DMLEAF tIPDiagnosticsUploadDiagnosticsPerConnectionResultParams[];
extern DMLEAF tIPDiagnosticsUDPEchoConfigParams[];
extern DMLEAF tIPDiagnosticsUDPEchoDiagnosticsParams[];
extern DMLEAF tIPDiagnosticsServerSelectionDiagnosticsParams[];

unsigned char get_ipv4_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance);
unsigned char get_ipv6_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance);

int get_linker_ip_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_linker_ipv6_prefix(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int browseIPIfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIfaceIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIfaceIPv6Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIfaceIPv6PrefixInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPInterfaceTWAMPReflectorInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPDiagnosticsTraceRouteRouteHopsInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPDiagnosticsDownloadDiagnosticsPerConnectionResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPDiagnosticsUploadDiagnosticsPerConnectionResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_ip_interface(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ip_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv4(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ipv4(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv6(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_ipv6(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_ipv6_prefix(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_ipv6_prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjIPInterfaceTWAMPReflector(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjIPInterfaceTWAMPReflector(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_diag_enable_true(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IP_IPv4Capable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IP_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IP_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IP_IPv4Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IP_IPv6Capable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IP_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IP_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IP_IPv6Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IP_ULAPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IP_ULAPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IP_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ipv4_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ipv4_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_IPInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_Router(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_Router(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_MaxMTUSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_MaxMTUSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_Loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterface_Loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterface_IPv4AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_IPv6AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_IPv6PrefixNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterface_TWAMPReflectorNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPInterfaceIPv6Address_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Address_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Address_IPAddressStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Address_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Address_Origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Address_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_IPInterfaceIPv6Prefix_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Prefix_PrefixStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Prefix_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_Origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceIPv6Prefix_StaticType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_StaticType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_ParentPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_ParentPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_ChildPrefixBits(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_ChildPrefixBits(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_IPInterfaceTWAMPReflector_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceTWAMPReflector_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPInterfaceTWAMPReflector_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceTWAMPReflector_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceTWAMPReflector_MaximumTTL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_MaximumTTL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceTWAMPReflector_IPAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_IPAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPInterfaceTWAMPReflector_PortAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPInterfaceTWAMPReflector_PortAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_ip_interface_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_interface_statistics_rx_multicastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_protocolversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_protocolversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_ip_ping_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_ip_ping_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_failure_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_average_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_min_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_max_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_AverageResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_MinimumResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ip_ping_MaximumResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsTraceRoute_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_NumberOfTries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_NumberOfTries(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_MaxHopCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsTraceRoute_MaxHopCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsTraceRoute_ResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsTraceRoute_RouteHopsNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsTraceRouteRouteHops_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsTraceRouteRouteHops_HostAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsTraceRouteRouteHops_ErrorCode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsTraceRouteRouteHops_RTTimes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsDownloadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_DownloadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_DownloadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_DownloadTransports(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_DownloadDiagnosticMaxConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnostics_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TestBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TestBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TotalBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_PeriodOfFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_PerConnectionResultNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TestBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsUploadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_UploadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_UploadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_UploadTransports(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_TestFileLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_TestFileLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnostics_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TestBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TestBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TotalBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TotalBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_PeriodOfFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_PerConnectionResultNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TestBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsUDPEchoConfig_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoConfig_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoConfig_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoConfig_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoConfig_SourceIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoConfig_SourceIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoConfig_UDPPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoConfig_UDPPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoConfig_EchoPlusSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_PacketsResponded(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_BytesResponded(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_TimeFirstPacketReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoConfig_TimeLastPacketReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsUDPEchoDiagnostics_SuccessCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoDiagnostics_FailureCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoDiagnostics_AverageResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoDiagnostics_MinimumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsUDPEchoDiagnostics_MaximumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_HostList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_HostList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_IPDiagnosticsServerSelectionDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_IPDiagnosticsServerSelectionDiagnostics_FastestHost(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsServerSelectionDiagnostics_MinimumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsServerSelectionDiagnostics_AverageResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_IPDiagnosticsServerSelectionDiagnostics_MaximumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif
