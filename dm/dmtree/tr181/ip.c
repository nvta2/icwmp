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

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "ip.h"
#include "diagnostic.h"
#include "dmjson.h"

struct dm_forced_inform_s IPv4INFRM = {0, get_ipv4_finform};
struct dm_forced_inform_s IPv6INFRM = {0, get_ipv6_finform};

/* *** Device.IP. *** */
DMOBJ tIPObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Interface", &DMWRITE, add_ip_interface, delete_ip_interface, NULL, browseIPIfaceInst, NULL, NULL, tInterfaceObj, tIPintParams, get_linker_ip_interface},
{"Diagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDiagnosticObj, tIPDiagnosticsParams, NULL},
{0}
};

DMLEAF tIPParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"IPv4Capable", &DMREAD, DMT_BOOL, get_IP_IPv4Capable, NULL, NULL, NULL},
{"IPv4Enable", &DMWRITE, DMT_BOOL, get_IP_IPv4Enable, set_IP_IPv4Enable, NULL, NULL},
{"IPv4Status", &DMREAD, DMT_STRING, get_IP_IPv4Status, NULL, NULL, NULL},
{"IPv6Capable", &DMREAD, DMT_BOOL, get_IP_IPv6Capable, NULL, NULL, NULL},
{"IPv6Enable", &DMWRITE, DMT_BOOL, get_IP_IPv6Enable, set_IP_IPv6Enable, NULL, NULL},
{"IPv6Status", &DMREAD, DMT_STRING, get_IP_IPv6Status, NULL, NULL, NULL},
{"ULAPrefix", &DMWRITE, DMT_STRING, get_IP_ULAPrefix, set_IP_ULAPrefix, NULL, NULL},
{"InterfaceNumberOfEntries", &DMREAD, DMT_UNINT, get_IP_InterfaceNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Interface. *** */
DMOBJ tInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"IPv4Address", &DMWRITE, add_ipv4, delete_ipv4, NULL, browseIfaceIPv4Inst, NULL, NULL, NULL, tIPv4Params, NULL},
{"IPv6Address", &DMWRITE, add_ipv6, delete_ipv6, NULL, browseIfaceIPv6Inst, NULL, NULL, NULL, tIPv6Params, NULL},
{"IPv6Prefix", &DMWRITE, add_ipv6_prefix, delete_ipv6_prefix, NULL, browseIfaceIPv6PrefixInst, NULL, NULL, NULL, tIPv6PrefixParams, get_linker_ipv6_prefix},
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIPInterfaceStatsParams, NULL},
{"TWAMPReflector", &DMWRITE, addObjIPInterfaceTWAMPReflector, delObjIPInterfaceTWAMPReflector, NULL, browseIPInterfaceTWAMPReflectorInst, NULL, NULL, NULL, tIPInterfaceTWAMPReflectorParams, NULL},
{0}
};

DMLEAF tIPintParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPInterface_Enable, set_IPInterface_Enable, NULL, NULL},
{"IPv4Enable", &DMWRITE, DMT_BOOL, get_IPInterface_IPv4Enable, set_IPInterface_IPv4Enable, NULL, NULL},
{"IPv6Enable", &DMWRITE, DMT_BOOL, get_IPInterface_IPv6Enable, set_IPInterface_IPv6Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_IPInterface_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_IPInterface_Alias, set_IPInterface_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_IPInterface_Name, NULL, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_IPInterface_LastChange, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_IPInterface_LowerLayers, set_IPInterface_LowerLayers, NULL, NULL},
{"Router", &DMWRITE, DMT_STRING, get_IPInterface_Router, set_IPInterface_Router, NULL, NULL},
{"Reset", &DMWRITE, DMT_BOOL, get_IPInterface_Reset, set_IPInterface_Reset, NULL, NULL},
{"MaxMTUSize", &DMWRITE, DMT_UNINT, get_IPInterface_MaxMTUSize, set_IPInterface_MaxMTUSize, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_IPInterface_Type, NULL, NULL, NULL},
{"Loopback", &DMWRITE, DMT_BOOL, get_IPInterface_Loopback, set_IPInterface_Loopback, NULL, NULL},
{"IPv4AddressNumberOfEntries", &DMREAD, DMT_UNINT, get_IPInterface_IPv4AddressNumberOfEntries, NULL, NULL, NULL},
{"IPv6AddressNumberOfEntries", &DMREAD, DMT_UNINT, get_IPInterface_IPv6AddressNumberOfEntries, NULL, NULL, NULL},
{"IPv6PrefixNumberOfEntries", &DMREAD, DMT_UNINT, get_IPInterface_IPv6PrefixNumberOfEntries, NULL, NULL, NULL},
{"TWAMPReflectorNumberOfEntries", &DMREAD, DMT_UNINT, get_IPInterface_TWAMPReflectorNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.IPv4Address.{i}. *** */
DMLEAF tIPv4Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPInterface_Enable, set_IPInterface_Enable, &IPv4INFRM, NULL},
{"Status", &DMREAD, DMT_STRING, get_IPInterface_Status, NULL, &IPv4INFRM, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_ipv4_alias, set_ipv4_alias, &IPv4INFRM, NULL},
{CUSTOM_PREFIX"FirewallEnabled", &DMWRITE, DMT_BOOL, get_firewall_enabled, set_firewall_enabled, &IPv4INFRM, NULL},
{"IPAddress", &DMWRITE, DMT_STRING, get_ipv4_address, set_ipv4_address, &IPv4INFRM, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING, get_ipv4_netmask, set_ipv4_netmask, &IPv4INFRM, NULL},
{"AddressingType", &DMWRITE, DMT_STRING, get_ipv4_addressing_type, set_ipv4_addressing_type, &IPv4INFRM, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.IPv6Address.{i}. *** */
DMLEAF tIPv6Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPInterfaceIPv6Address_Enable, set_IPInterfaceIPv6Address_Enable, &IPv6INFRM, NULL},
{"Status", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Address_Status, NULL, &IPv6INFRM, NULL},
{"IPAddressStatus", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Address_IPAddressStatus, NULL, &IPv6INFRM, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Address_Alias, set_IPInterfaceIPv6Address_Alias, &IPv6INFRM, NULL},
{"IPAddress", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Address_IPAddress, set_IPInterfaceIPv6Address_IPAddress, &IPv6INFRM, NULL},
{"Origin", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Address_Origin, NULL, &IPv6INFRM, NULL},
{"Prefix", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Address_Prefix, set_IPInterfaceIPv6Address_Prefix, &IPv6INFRM, NULL},
{"PreferredLifetime", &DMWRITE, DMT_TIME, get_IPInterfaceIPv6Address_PreferredLifetime, set_IPInterfaceIPv6Address_PreferredLifetime, &IPv6INFRM, NULL},
{"ValidLifetime", &DMWRITE, DMT_TIME, get_IPInterfaceIPv6Address_ValidLifetime, set_IPInterfaceIPv6Address_ValidLifetime, &IPv6INFRM, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.IPv6Prefix.{i}. *** */
DMLEAF tIPv6PrefixParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPInterfaceIPv6Prefix_Enable, set_IPInterfaceIPv6Prefix_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Prefix_Status, NULL, NULL, NULL},
{"PrefixStatus", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Prefix_PrefixStatus, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Prefix_Alias, set_IPInterfaceIPv6Prefix_Alias, NULL, NULL},
{"Prefix", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Prefix_Prefix, set_IPInterfaceIPv6Prefix_Prefix, NULL, NULL},
{"Origin", &DMREAD, DMT_STRING, get_IPInterfaceIPv6Prefix_Origin, NULL, NULL, NULL},
{"StaticType", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Prefix_StaticType, set_IPInterfaceIPv6Prefix_StaticType, NULL, NULL},
{"ParentPrefix", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Prefix_ParentPrefix, set_IPInterfaceIPv6Prefix_ParentPrefix, NULL, NULL},
{"ChildPrefixBits", &DMWRITE, DMT_STRING, get_IPInterfaceIPv6Prefix_ChildPrefixBits, set_IPInterfaceIPv6Prefix_ChildPrefixBits, NULL, NULL},
{"PreferredLifetime", &DMWRITE, DMT_TIME, get_IPInterfaceIPv6Prefix_PreferredLifetime, set_IPInterfaceIPv6Prefix_PreferredLifetime, NULL, NULL},
{"ValidLifetime", &DMWRITE, DMT_TIME, get_IPInterfaceIPv6Prefix_ValidLifetime, set_IPInterfaceIPv6Prefix_ValidLifetime, NULL, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.Stats. *** */
DMLEAF tIPInterfaceStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"BytesSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_packets, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_errors, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_errors, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_discardpackets, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_discardpackets, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_unicastpackets, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_unicastpackets, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_multicastpackets, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_multicastpackets, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_ip_interface_statistics_tx_broadcastpackets, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_broadcastpackets, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_ip_interface_statistics_rx_unknownprotopackets, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.TWAMPReflector.{i}. *** */
DMLEAF tIPInterfaceTWAMPReflectorParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPInterfaceTWAMPReflector_Enable, set_IPInterfaceTWAMPReflector_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_IPInterfaceTWAMPReflector_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_IPInterfaceTWAMPReflector_Alias, set_IPInterfaceTWAMPReflector_Alias, NULL, NULL},
{"Port", &DMWRITE, DMT_UNINT, get_IPInterfaceTWAMPReflector_Port, set_IPInterfaceTWAMPReflector_Port, NULL, NULL},
{"MaximumTTL", &DMWRITE, DMT_UNINT, get_IPInterfaceTWAMPReflector_MaximumTTL, set_IPInterfaceTWAMPReflector_MaximumTTL, NULL, NULL},
{"IPAllowedList", &DMWRITE, DMT_STRING, get_IPInterfaceTWAMPReflector_IPAllowedList, set_IPInterfaceTWAMPReflector_IPAllowedList, NULL, NULL},
{"PortAllowedList", &DMWRITE, DMT_STRING, get_IPInterfaceTWAMPReflector_PortAllowedList, set_IPInterfaceTWAMPReflector_PortAllowedList, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics. *** */
DMOBJ tDiagnosticObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"IPPing", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIpPingDiagParams, NULL},
{"TraceRoute", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsTraceRouteObj, tIPDiagnosticsTraceRouteParams, NULL},
{"DownloadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsDownloadDiagnosticsObj, tIPDiagnosticsDownloadDiagnosticsParams, NULL},
{"UploadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsUploadDiagnosticsObj, tIPDiagnosticsUploadDiagnosticsParams, NULL},
{"UDPEchoConfig", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsUDPEchoConfigParams, NULL},
{"UDPEchoDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsUDPEchoDiagnosticsParams, NULL},
{"ServerSelectionDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIPDiagnosticsServerSelectionDiagnosticsParams, NULL},
{0}
};

DMLEAF tIPDiagnosticsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"IPv4PingSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6PingSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv4TraceRouteSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6TraceRouteSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv4DownloadDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6DownloadDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv4UploadDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6UploadDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv4UDPEchoDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6UDPEchoDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv4ServerSelectionDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{"IPv6ServerSelectionDiagnosticsSupported", &DMREAD, DMT_BOOL, get_diag_enable_true, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.IPPing. *** */
DMLEAF tIpPingDiagParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_ip_ping_diagnostics_state, set_ip_ping_diagnostics_state, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_ip_ping_interface, set_ip_ping_interface, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_ip_ping_protocolversion, set_ip_ping_protocolversion, NULL, NULL},
{"Host", &DMWRITE, DMT_STRING, get_ip_ping_host, set_ip_ping_host, NULL, NULL},
{"NumberOfRepetitions", &DMWRITE, DMT_UNINT, get_ip_ping_repetition_number, set_ip_ping_repetition_number, NULL, NULL},
{"Timeout", &DMWRITE, DMT_UNINT, get_ip_ping_timeout, set_ip_ping_timeout, NULL, NULL},
{"DataBlockSize", &DMWRITE, DMT_UNINT, get_ip_ping_block_size, set_ip_ping_block_size, NULL, NULL},
{"DSCP", &DMWRITE, DMT_UNINT, get_ip_ping_DSCP, set_ip_ping_DSCP, NULL, NULL},
{"SuccessCount", &DMREAD, DMT_UNINT, get_ip_ping_success_count, NULL, NULL},
{"FailureCount", &DMREAD, DMT_UNINT, get_ip_ping_failure_count, NULL, NULL, NULL},
{"AverageResponseTime", &DMREAD, DMT_UNINT, get_ip_ping_average_response_time, NULL, NULL, NULL},
{"MinimumResponseTime", &DMREAD, DMT_UNINT, get_ip_ping_min_response_time, NULL, NULL, NULL},
{"MaximumResponseTime", &DMREAD, DMT_UNINT, get_ip_ping_max_response_time, NULL, NULL, NULL},
{"AverageResponseTimeDetailed", &DMREAD, DMT_UNINT, get_ip_ping_AverageResponseTimeDetailed, NULL, NULL, NULL},
{"MinimumResponseTimeDetailed", &DMREAD, DMT_UNINT, get_ip_ping_MinimumResponseTimeDetailed, NULL, NULL, NULL},
{"MaximumResponseTimeDetailed", &DMREAD, DMT_UNINT, get_ip_ping_MaximumResponseTimeDetailed, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.TraceRoute. *** */
DMOBJ tIPDiagnosticsTraceRouteObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"RouteHops", &DMREAD, NULL, NULL, NULL, browseIPDiagnosticsTraceRouteRouteHopsInst, NULL, NULL, NULL, tIPDiagnosticsTraceRouteRouteHopsParams, NULL},
{0}
};

DMLEAF tIPDiagnosticsTraceRouteParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_IPDiagnosticsTraceRoute_DiagnosticsState, set_IPDiagnosticsTraceRoute_DiagnosticsState, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsTraceRoute_Interface, set_IPDiagnosticsTraceRoute_Interface, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_IPDiagnosticsTraceRoute_ProtocolVersion, set_IPDiagnosticsTraceRoute_ProtocolVersion, NULL, NULL},
{"Host", &DMWRITE, DMT_STRING, get_IPDiagnosticsTraceRoute_Host, set_IPDiagnosticsTraceRoute_Host, NULL, NULL},
{"NumberOfTries", &DMWRITE, DMT_UNINT, get_IPDiagnosticsTraceRoute_NumberOfTries, set_IPDiagnosticsTraceRoute_NumberOfTries, NULL, NULL},
{"Timeout", &DMWRITE, DMT_UNINT, get_IPDiagnosticsTraceRoute_Timeout, set_IPDiagnosticsTraceRoute_Timeout, NULL, NULL},
{"DataBlockSize", &DMWRITE, DMT_UNINT, get_IPDiagnosticsTraceRoute_DataBlockSize, set_IPDiagnosticsTraceRoute_DataBlockSize, NULL, NULL},
{"DSCP", &DMWRITE, DMT_UNINT, get_IPDiagnosticsTraceRoute_DSCP, set_IPDiagnosticsTraceRoute_DSCP, NULL, NULL},
{"MaxHopCount", &DMWRITE, DMT_UNINT, get_IPDiagnosticsTraceRoute_MaxHopCount, set_IPDiagnosticsTraceRoute_MaxHopCount, NULL, NULL},
{"ResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsTraceRoute_ResponseTime, NULL, NULL, NULL},
{"RouteHopsNumberOfEntries", &DMREAD, DMT_UNINT, get_IPDiagnosticsTraceRoute_RouteHopsNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.TraceRoute.RouteHops.{i}. *** */
DMLEAF tIPDiagnosticsTraceRouteRouteHopsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Host", &DMREAD, DMT_STRING, get_IPDiagnosticsTraceRouteRouteHops_Host, NULL, NULL, NULL},
{"HostAddress", &DMREAD, DMT_STRING, get_IPDiagnosticsTraceRouteRouteHops_HostAddress, NULL, NULL, NULL},
{"ErrorCode", &DMREAD, DMT_UNINT, get_IPDiagnosticsTraceRouteRouteHops_ErrorCode, NULL, NULL, NULL},
{"RTTimes", &DMREAD, DMT_STRING, get_IPDiagnosticsTraceRouteRouteHops_RTTimes, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.DownloadDiagnostics. *** */
DMOBJ tIPDiagnosticsDownloadDiagnosticsObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"PerConnectionResult", &DMREAD, NULL, NULL, NULL, browseIPDiagnosticsDownloadDiagnosticsPerConnectionResultInst, NULL, NULL, NULL, tIPDiagnosticsDownloadDiagnosticsPerConnectionResultParams, NULL},
{0}
};

DMLEAF tIPDiagnosticsDownloadDiagnosticsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_IPDiagnosticsDownloadDiagnostics_DiagnosticsState, set_IPDiagnosticsDownloadDiagnostics_DiagnosticsState, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsDownloadDiagnostics_Interface, set_IPDiagnosticsDownloadDiagnostics_Interface, NULL, NULL},
{"DownloadURL", &DMWRITE, DMT_STRING, get_IPDiagnosticsDownloadDiagnostics_DownloadURL, set_IPDiagnosticsDownloadDiagnostics_DownloadURL, NULL, NULL},
{"DownloadTransports", &DMREAD, DMT_STRING, get_IPDiagnosticsDownloadDiagnostics_DownloadTransports, NULL, NULL, NULL},
{"DownloadDiagnosticMaxConnections", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_DownloadDiagnosticMaxConnections, NULL, NULL, NULL},
{"DSCP", &DMWRITE, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_DSCP, set_IPDiagnosticsDownloadDiagnostics_DSCP, NULL, NULL},
{"EthernetPriority", &DMWRITE, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_EthernetPriority, set_IPDiagnosticsDownloadDiagnostics_EthernetPriority, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_IPDiagnosticsDownloadDiagnostics_ProtocolVersion, set_IPDiagnosticsDownloadDiagnostics_ProtocolVersion, NULL, NULL},
{"NumberOfConnections", &DMWRITE, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_NumberOfConnections, set_IPDiagnosticsDownloadDiagnostics_NumberOfConnections, NULL, NULL},
{"ROMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnostics_ROMTime, NULL, NULL, NULL},
{"BOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnostics_BOMTime, NULL, NULL, NULL},
{"EOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnostics_EOMTime, NULL, NULL, NULL},
{"TestBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TestBytesReceived, NULL, NULL, NULL},
{"TotalBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceived, NULL, NULL, NULL},
{"TotalBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TotalBytesSent, NULL, NULL, NULL},
{"TestBytesReceivedUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TestBytesReceivedUnderFullLoading, NULL, NULL, NULL},
{"TotalBytesReceivedUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceivedUnderFullLoading, NULL, NULL, NULL},
{"TotalBytesSentUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_TotalBytesSentUnderFullLoading, NULL, NULL, NULL},
{"PeriodOfFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_PeriodOfFullLoading, NULL, NULL, NULL},
{"TCPOpenRequestTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnostics_TCPOpenRequestTime, NULL, NULL, NULL},
{"TCPOpenResponseTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnostics_TCPOpenResponseTime, NULL, NULL, NULL},
{"PerConnectionResultNumberOfEntries", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnostics_PerConnectionResultNumberOfEntries, NULL, NULL, NULL},
{"EnablePerConnectionResults", &DMWRITE, DMT_BOOL, get_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults, set_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.DownloadDiagnostics.PerConnectionResult.{i}. *** */
DMLEAF tIPDiagnosticsDownloadDiagnosticsPerConnectionResultParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ROMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_ROMTime, NULL, NULL, NULL},
{"BOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_BOMTime, NULL, NULL, NULL},
{"EOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_EOMTime, NULL, NULL, NULL},
{"TestBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TestBytesReceived, NULL, NULL, NULL},
{"TotalBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesReceived, NULL, NULL, NULL},
{"TotalBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesSent, NULL, NULL, NULL},
{"TCPOpenRequestTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenRequestTime, NULL, NULL, NULL},
{"TCPOpenResponseTime", &DMREAD, DMT_TIME, get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenResponseTime, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.UploadDiagnostics. *** */
DMOBJ tIPDiagnosticsUploadDiagnosticsObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"PerConnectionResult", &DMREAD, NULL, NULL, NULL, browseIPDiagnosticsUploadDiagnosticsPerConnectionResultInst, NULL, NULL, NULL, tIPDiagnosticsUploadDiagnosticsPerConnectionResultParams, NULL},
{0}
};

DMLEAF tIPDiagnosticsUploadDiagnosticsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_IPDiagnosticsUploadDiagnostics_DiagnosticsState, set_IPDiagnosticsUploadDiagnostics_DiagnosticsState, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsUploadDiagnostics_Interface, set_IPDiagnosticsUploadDiagnostics_Interface, NULL, NULL},
{"UploadURL", &DMWRITE, DMT_STRING, get_IPDiagnosticsUploadDiagnostics_UploadURL, set_IPDiagnosticsUploadDiagnostics_UploadURL, NULL, NULL},
{"UploadTransports", &DMREAD, DMT_STRING, get_IPDiagnosticsUploadDiagnostics_UploadTransports, NULL, NULL, NULL},
{"DSCP", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_DSCP, set_IPDiagnosticsUploadDiagnostics_DSCP, NULL, NULL},
{"EthernetPriority", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_EthernetPriority, set_IPDiagnosticsUploadDiagnostics_EthernetPriority, NULL, NULL},
{"TestFileLength", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TestFileLength, set_IPDiagnosticsUploadDiagnostics_TestFileLength, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_IPDiagnosticsUploadDiagnostics_ProtocolVersion, set_IPDiagnosticsUploadDiagnostics_ProtocolVersion, NULL, NULL},
{"NumberOfConnections", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_NumberOfConnections, set_IPDiagnosticsUploadDiagnostics_NumberOfConnections, NULL, NULL},
{"ROMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnostics_ROMTime, NULL, NULL, NULL},
{"BOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnostics_BOMTime, NULL, NULL, NULL},
{"EOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnostics_EOMTime, NULL, NULL, NULL},
{"TestBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TestBytesSent, NULL, NULL, NULL},
{"TotalBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TotalBytesReceived, NULL, NULL, NULL},
{"TotalBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TotalBytesSent, NULL, NULL, NULL},
{"TestBytesSentUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TestBytesSentUnderFullLoading, NULL, NULL, NULL},
{"TotalBytesReceivedUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TotalBytesReceivedUnderFullLoading, NULL, NULL, NULL},
{"TotalBytesSentUnderFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_TotalBytesSentUnderFullLoading, NULL, NULL, NULL},
{"PeriodOfFullLoading", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_PeriodOfFullLoading, NULL, NULL, NULL},
{"TCPOpenRequestTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnostics_TCPOpenRequestTime, NULL, NULL, NULL},
{"TCPOpenResponseTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnostics_TCPOpenResponseTime, NULL, NULL, NULL},
{"PerConnectionResultNumberOfEntries", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnostics_PerConnectionResultNumberOfEntries, NULL, NULL, NULL},
{"EnablePerConnectionResults", &DMWRITE, DMT_BOOL, get_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults, set_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.UploadDiagnostics.PerConnectionResult.{i}. *** */
DMLEAF tIPDiagnosticsUploadDiagnosticsPerConnectionResultParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ROMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_ROMTime, NULL, NULL, NULL},
{"BOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_BOMTime, NULL, NULL, NULL},
{"EOMTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_EOMTime, NULL, NULL, NULL},
{"TestBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TestBytesSent, NULL, NULL, NULL},
{"TotalBytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesReceived, NULL, NULL, NULL},
{"TotalBytesSent", &DMREAD, DMT_UNINT, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesSent, NULL, NULL, NULL},
{"TCPOpenRequestTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenRequestTime, NULL, NULL, NULL},
{"TCPOpenResponseTime", &DMREAD, DMT_TIME, get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenResponseTime, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.UDPEchoConfig. *** */
DMLEAF tIPDiagnosticsUDPEchoConfigParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_IPDiagnosticsUDPEchoConfig_Enable, set_IPDiagnosticsUDPEchoConfig_Enable, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoConfig_Interface, set_IPDiagnosticsUDPEchoConfig_Interface, NULL, NULL},
{"SourceIPAddress", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoConfig_SourceIPAddress, set_IPDiagnosticsUDPEchoConfig_SourceIPAddress, NULL, NULL},
{"UDPPort", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoConfig_UDPPort, set_IPDiagnosticsUDPEchoConfig_UDPPort, NULL, NULL},
{"EchoPlusEnabled", &DMWRITE, DMT_BOOL, get_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled, set_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled, NULL, NULL},
{"EchoPlusSupported", &DMREAD, DMT_BOOL, get_IPDiagnosticsUDPEchoConfig_EchoPlusSupported, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoConfig_PacketsReceived, NULL, NULL, NULL},
{"PacketsResponded", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoConfig_PacketsResponded, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoConfig_BytesReceived, NULL, NULL, NULL},
{"BytesResponded", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoConfig_BytesResponded, NULL, NULL, NULL},
{"TimeFirstPacketReceived", &DMREAD, DMT_TIME, get_IPDiagnosticsUDPEchoConfig_TimeFirstPacketReceived, NULL, NULL, NULL},
{"TimeLastPacketReceived", &DMREAD, DMT_TIME, get_IPDiagnosticsUDPEchoConfig_TimeLastPacketReceived, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.UDPEchoDiagnostics. *** */
DMLEAF tIPDiagnosticsUDPEchoDiagnosticsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState, set_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoDiagnostics_Interface, set_IPDiagnosticsUDPEchoDiagnostics_Interface, NULL, NULL},
{"Host", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoDiagnostics_Host, set_IPDiagnosticsUDPEchoDiagnostics_Host, NULL, NULL},
{"Port", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_Port, set_IPDiagnosticsUDPEchoDiagnostics_Port, NULL, NULL},
{"NumberOfRepetitions", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions, set_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions, NULL, NULL},
{"Timeout", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_Timeout, set_IPDiagnosticsUDPEchoDiagnostics_Timeout, NULL, NULL},
{"DataBlockSize", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize, set_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize, NULL, NULL},
{"DSCP", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_DSCP, set_IPDiagnosticsUDPEchoDiagnostics_DSCP, NULL, NULL},
{"InterTransmissionTime", &DMWRITE, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime, set_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion, set_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion, NULL, NULL},
{"SuccessCount", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_SuccessCount, NULL, NULL, NULL},
{"FailureCount", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_FailureCount, NULL, NULL, NULL},
{"AverageResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_AverageResponseTime, NULL, NULL, NULL},
{"MinimumResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_MinimumResponseTime, NULL, NULL, NULL},
{"MaximumResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsUDPEchoDiagnostics_MaximumResponseTime, NULL, NULL, NULL},
{0}
};

/* *** Device.IP.Diagnostics.ServerSelectionDiagnostics. *** */
DMLEAF tIPDiagnosticsServerSelectionDiagnosticsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState, set_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_Interface, set_IPDiagnosticsServerSelectionDiagnostics_Interface, NULL, NULL},
{"ProtocolVersion", &DMWRITE, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion, set_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion, NULL, NULL},
{"Protocol", &DMWRITE, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_Protocol, set_IPDiagnosticsServerSelectionDiagnostics_Protocol, NULL, NULL},
{"Port", &DMWRITE, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_Port, set_IPDiagnosticsServerSelectionDiagnostics_Port, NULL, NULL},
{"HostList", &DMWRITE, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_HostList, set_IPDiagnosticsServerSelectionDiagnostics_HostList, NULL, NULL},
{"NumberOfRepetitions", &DMWRITE, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions, set_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions, NULL, NULL},
{"Timeout", &DMWRITE, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_Timeout, set_IPDiagnosticsServerSelectionDiagnostics_Timeout, NULL, NULL},
{"FastestHost", &DMREAD, DMT_STRING, get_IPDiagnosticsServerSelectionDiagnostics_FastestHost, NULL, NULL, NULL},
{"MinimumResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_MinimumResponseTime, NULL, NULL, NULL},
{"AverageResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_AverageResponseTime, NULL, NULL, NULL},
{"MaximumResponseTime", &DMREAD, DMT_UNINT, get_IPDiagnosticsServerSelectionDiagnostics_MaximumResponseTime, NULL, NULL, NULL},
{0}
};

unsigned char get_ipv4_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	return 1;
}

unsigned char get_ipv6_finform(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	return 1;
}

/*************************************************************
 * INIT
/*************************************************************/
inline int init_ip_args(struct ip_args *args, struct uci_section *s, char *ip_4address)
{
	args->ip_sec = s;
	args->ip_4address = ip_4address;
	return 0;
}

inline int init_ipv6_args(struct ipv6_args *args, struct uci_section *s, char *ip_6address, char *ip_6mask, char *ip_6preferred, char *ip_6valid)
{
	args->ip_sec = s;
	args->ip_6address = ip_6address;
	args->ip_6mask = ip_6mask;
	args->ip_6preferred = ip_6preferred;
	args->ip_6valid = ip_6valid;
	return 0;
}

inline int init_ipv6prefix_args(struct ipv6prefix_args *args, struct uci_section *s, char *ip_6prefixaddress, char *ip_6prefixmask, char *ip_6prefixpreferred, char *ip_6prefixvalid)
{
	args->ip_sec = s;
	args->ip_6prefixaddress = ip_6prefixaddress;
	args->ip_6prefixmask = ip_6prefixmask;
	args->ip_6prefixpreferred = ip_6prefixpreferred;
	args->ip_6prefixvalid = ip_6prefixvalid;
	return 0;
}

/*************************************************************
 * COMMON Functions
/*************************************************************/
static char *ubus_call_get_value_with_two_objects(char *interface, char *obj1, char *obj2, char *key)
{
	json_object *res, *jobj1, *jobj2;
	char *value = "";

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
	if (res)
	{
		jobj1 = dmjson_select_obj_in_array_idx(res, 0, 1, obj1);
		if(jobj1)
			jobj2 = dmjson_get_obj(jobj1, 1, obj2);
		if(jobj2)
			value = dmjson_get_value(jobj2, 1, key);
	}
	return value;
}

static char *ubus_call_get_value(char *interface, char *obj, char *key)
{
	json_object *res, *jobj;
	char *value = "";

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
	if (res)
	{
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, obj);
		value = dmjson_get_value(jobj, 1, key);
	}
	return value;
}

static char *get_child_prefix_linker(char *interface)
{
	char *address = NULL, *mask = NULL, *value;
	json_object *res, *jobj, *jobj1, *jobj2;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
	if(res) {
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-prefix");
		if(jobj) {
			jobj1 = dmjson_get_obj(jobj, 1, "assigned");
			if(jobj1) {
				jobj2 = dmjson_get_obj(jobj1, 1, "lan");
				if(jobj2) {
					address = dmjson_get_value(jobj2, 1, "address");
					mask = dmjson_get_value(jobj2, 1, "mask");
					dmasprintf(&value, "%s/%s", address,mask);
					return value;
				}
			}
		}
	}
	return "";
}

/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_diag_enable_true(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

/*
 * *** Device.IP.Diagnostics.IPPing. ***
 */

static inline char *ipping_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@ippingdiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("DiagnosticState", "None");
	return 0;
}

int set_ip_ping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				IPPING_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_IPPING_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@ippingdiagnostic[0]", "interface", value);
	return 0;
}

int set_ip_ping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			IPPING_STOP
			curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "interface", value);
			return 0;
	}
	return 0;
}

int get_ip_ping_protocolversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("ProtocolVersion", "Any");
	return 0;
}

int set_ip_ping_protocolversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				IPPING_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@ippingdiagnostic[0]", "Host", value);
	return 0;
}

int set_ip_ping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			IPPING_STOP
			curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "Host", value);
			return 0;
	}
	return 0;
}

int get_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("NumberOfRepetitions", "3");
	return 0;
}

int set_ip_ping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1) {
				IPPING_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "NumberOfRepetitions", value);
			}
			return 0;
	}
	return 0;
}

int get_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("Timeout", "1000");
	return 0;
}

int set_ip_ping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1) {
				IPPING_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "Timeout", value);
			}
			return 0;
	}
	return 0;
}

int get_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("DataBlockSize", "64");

	return 0;
}

int set_ip_ping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 1) && (atoi(value) <= 65535)) {
				IPPING_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "DataBlockSize", value);
			}
	}
	return 0;
}

int get_ip_ping_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("DSCP", "0");
	return 0;
}

int set_ip_ping_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 0) && (atoi(value) <= 63)) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "ippingdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "ippingdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@ippingdiagnostic[0]", "DSCP", value);
				return 0;
			}
	}
	return 0;
}

int get_ip_ping_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("SuccessCount", "0");
	return 0;
}

int get_ip_ping_failure_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("FailureCount", "0");
	return 0;
}

int get_ip_ping_average_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("AverageResponseTime", "0");
	return 0;
}

int get_ip_ping_min_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("MinimumResponseTime", "0");
	return 0;
}

int get_ip_ping_max_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("MaximumResponseTime", "0");
	return 0;
}

int get_ip_ping_AverageResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("AverageResponseTimeDetailed", "0");
	return 0;
}

int get_ip_ping_MinimumResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("MinimumResponseTimeDetailed", "0");
	return 0;
}

int get_ip_ping_MaximumResponseTimeDetailed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ipping_get("MaximumResponseTimeDetailed", "0");
	return 0;
}

/*
 * *** Device.IP.Diagnostics.TraceRoute. ***
 */

static inline char *traceroute_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@traceroutediagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsTraceRoute_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("DiagnosticState", "None");
	return 0;
}

int set_IPDiagnosticsTraceRoute_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_TRACEROUTE_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@traceroutediagnostic[0]", "interface", value);
	return 0;
}

int set_IPDiagnosticsTraceRoute_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			TRACEROUTE_STOP
			curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "interface", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("ProtocolVersion", "Any");
	return 0;
}

int set_IPDiagnosticsTraceRoute_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@traceroutediagnostic[0]", "Host", value);
	return 0;
}

int set_IPDiagnosticsTraceRoute_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			TRACEROUTE_STOP
			curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "Host", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_NumberOfTries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("NumberOfTries", "3");
	return 0;
}

int set_IPDiagnosticsTraceRoute_NumberOfTries(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 1) && (atoi(value) <= 3)) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "NumberOfTries", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("Timeout", "5000");
	return 0;
}

int set_IPDiagnosticsTraceRoute_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "Timeout", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("DataBlockSize", "38");
	return 0;
}

int set_IPDiagnosticsTraceRoute_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 1) && (atoi(value) <= 65535)) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "DataBlockSize", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("DSCP", "0");
	return 0;
}

int set_IPDiagnosticsTraceRoute_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 0) && (atoi(value) <= 63)) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "DSCP", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_MaxHopCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("MaxHops", "30");
	return 0;
}

int set_IPDiagnosticsTraceRoute_MaxHopCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if ((atoi(value) >= 1) && (atoi(value) <= 64)) {
				TRACEROUTE_STOP
				curr_section = dmuci_walk_state_section("cwmp", "traceroutediagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "traceroutediagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@traceroutediagnostic[0]", "MaxHops", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsTraceRoute_ResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("ResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsTraceRoute_RouteHopsNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = traceroute_get("NumberOfHops", "0");
	return 0;
}

int get_IPDiagnosticsTraceRouteRouteHops_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "host", value);
	return 0;
}

int get_IPDiagnosticsTraceRouteRouteHops_HostAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "ip", value);
	return 0;
}

int get_IPDiagnosticsTraceRouteRouteHops_ErrorCode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	return 0;
}

int get_IPDiagnosticsTraceRouteRouteHops_RTTimes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "time", value);
	return 0;
}

/*
 * *** Device.IP.Diagnostics.DownloadDiagnostics. ***
 */

static inline char *download_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@downloaddiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsDownloadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("DiagnosticState", "None");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				DOWNLOAD_DIAGNOSTIC_STOP
				curr_section = dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_DOWNLOAD_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	dmuci_get_varstate_string("cwmp", "@downloaddiagnostic[0]", "interface", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker = NULL, *tmp, *device = NULL;
	struct uci_section *curr_section = NULL;
	json_object *res;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", linker, String}}, 1, &res);
				device = dmjson_get_value(res, 1, "device");
				if (device) {
					DOWNLOAD_DIAGNOSTIC_STOP
					curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
					if(!curr_section)
					{
						dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
					}
					dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "interface", linker);
					dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "device", device);
				}
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_DownloadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@downloaddiagnostic[0]", "url", value);
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_DownloadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			DOWNLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "url", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_DownloadTransports(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "HTTP,FTP";
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_DownloadDiagnosticMaxConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("DSCP", "0");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			DOWNLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "DSCP", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("ethernetpriority", "");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			DOWNLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "ethernetpriority", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("ProtocolVersion", "Any");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				DOWNLOAD_DIAGNOSTIC_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("NumberOfConnections", "1");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "1") == 0) {
				DOWNLOAD_DIAGNOSTIC_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "NumberOfConnections", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("ROMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("BOMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("EOMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TestBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TestBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TotalBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TotalBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TestBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TestBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TotalBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TotalBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TotalBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TotalBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_PeriodOfFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("PeriodOfFullLoading", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TCPOpenRequestTime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TCPOpenResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_PerConnectionResultNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp;
	bool b;
	dmuci_get_varstate_string("cwmp", "@downloaddiagnostic[0]", "EnablePerConnection", &tmp);
	string_to_bool(tmp, &b);
	*value = (b) ? "1" : "0";
	return 0;
}

int get_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("EnablePerConnection", "0");
	return 0;
}

int set_IPDiagnosticsDownloadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			DOWNLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "downloaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "downloaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@downloaddiagnostic[0]", "EnablePerConnection", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("ROMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("BOMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("EOMtime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TestBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TestBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "TotalBytesReceived", value);
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "TotalBytesSent", value);
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TCPOpenRequestTime", "0");
	return 0;
}

int get_IPDiagnosticsDownloadDiagnosticsPerConnectionResult_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = download_get("TCPOpenResponseTime", "0");
	return 0;
}

/*
 * *** Device.IP.Diagnostics.UploadDiagnostics. ***
 */

static inline char *upload_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@uploaddiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsUploadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("DiagnosticState", "None");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				UPLOAD_DIAGNOSTIC_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_UPLOAD_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	dmuci_get_varstate_string("cwmp", "@uploaddiagnostic[0]", "interface", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker = NULL, *tmp, *device= NULL;
	struct uci_section *curr_section = NULL;
	json_object *res;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", linker, String}}, 1, &res);
				device = dmjson_get_value(res, 1, "device");
				if (device) {
					UPLOAD_DIAGNOSTIC_STOP
					curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
					if(!curr_section)
					{
						dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
					}
					dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "interface", linker);
					dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "device", device);
				}
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_UploadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@uploaddiagnostic[0]", "url", value);
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_UploadURL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UPLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "url", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_UploadTransports(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "HTTP,FTP";
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("DSCP", "0");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UPLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "DSCP", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("ethernetpriority", "0");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_EthernetPriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UPLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "ethernetpriority", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TestFileLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TestFileLength", "0");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_TestFileLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UPLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "TestFileLength", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("ProtocolVersion", "Any");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				UPLOAD_DIAGNOSTIC_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("NumberOfConnections", "1");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_NumberOfConnections(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "1") == 0) {
				UPLOAD_DIAGNOSTIC_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "NumberOfConnections", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("ROMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("BOMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("EOMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TestBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TestBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TotalBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TotalBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TestBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TestBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TotalBytesReceivedUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TotalBytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TotalBytesSentUnderFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TotalBytesSent", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_PeriodOfFullLoading(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("PeriodOfFullLoading", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TCPOpenRequestTime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TCPOpenResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_PerConnectionResultNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp;
	bool b;
	dmuci_get_varstate_string("cwmp", "@uploaddiagnostic[0]", "EnablePerConnection", &tmp);
	string_to_bool(tmp, &b);
	*value = (b) ? "1" : "0";
	return 0;
}

int get_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("EnablePerConnection", "0");
	return 0;
}

int set_IPDiagnosticsUploadDiagnostics_EnablePerConnectionResults(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			UPLOAD_DIAGNOSTIC_STOP
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "uploaddiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "uploaddiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@uploaddiagnostic[0]", "EnablePerConnection", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_ROMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("ROMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_BOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("BOMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_EOMTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("EOMtime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TestBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "TestBytesSent", value);
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "TotalBytesReceived", value);
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TotalBytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "TotalBytesSent", value);
	return 0;

}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenRequestTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TCPOpenRequestTime", "0");
	return 0;
}

int get_IPDiagnosticsUploadDiagnosticsPerConnectionResult_TCPOpenResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = upload_get("TCPOpenResponseTime", "0");
	return 0;
}

/*
 * *** Device.IP.Diagnostics.UDPEchoConfig. ***
 */

int get_IPDiagnosticsUDPEchoConfig_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp_udpechoserver", "udpechoserver", "enable", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoConfig_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char file[32] = "/var/state/cwmp_udpechoserver";

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				if( access( file, F_OK ) != -1 )
					dmcmd("/bin/rm", 1, file);
				dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "enable", "1");
			}
			else
				dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "enable", "0");
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp_udpechoserver", "udpechoserver", "interface", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoConfig_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "interface", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_SourceIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp_udpechoserver", "udpechoserver", "address", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoConfig_SourceIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "address", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_UDPPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp_udpechoserver", "udpechoserver", "server_port", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoConfig_UDPPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "server_port", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp_udpechoserver", "udpechoserver", "plus", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoConfig_EchoPlusEnabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "plus", "1");
			else
				dmuci_set_value("cwmp_udpechoserver", "udpechoserver", "plus", "0");
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_EchoPlusSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

static inline char *udpechoconfig_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp_udpechoserver", "udpechoserver", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsUDPEchoConfig_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("PacketsReceived", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_PacketsResponded(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("PacketsResponded", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("BytesReceived", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_BytesResponded(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("BytesResponded", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_TimeFirstPacketReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("TimeFirstPacketReceived", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoConfig_TimeLastPacketReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechoconfig_get("TimeLastPacketReceived", "0");
	return 0;
}

/*
 * *** Device.IP.Diagnostics.UDPEchoDiagnostics. ***
 */

static inline char *udpechodiagnostics_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@udpechodiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("DiagnosticState", "None");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_UDPECHO_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@udpechodiagnostic[0]", "Interface", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UDPECHO_STOP;
			curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "Interface", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@udpechodiagnostic[0]", "Host", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_Host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UDPECHO_STOP;
			curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "Host", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@udpechodiagnostic[0]", "port", value);
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1 && atoi(value) <= 65535) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "port", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("NumberOfRepetitions", "1");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UDPECHO_STOP;
			curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "NumberOfRepetitions", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("Timeout", "5000");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			UDPECHO_STOP;
			curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "Timeout", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("DataBlockSize", "24");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_DataBlockSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1 && atoi(value) <= 65535) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "DataBlockSize", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("DSCP", "0");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_DSCP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 0 && atoi(value) <= 63) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "DSCP", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("InterTransmissionTime", "1000");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_InterTransmissionTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1 && atoi(value) <= 65535) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "InterTransmissionTime", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("ProtocolVersion", "Any");
	return 0;
}

int set_IPDiagnosticsUDPEchoDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				UDPECHO_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "udpechodiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "udpechodiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@udpechodiagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_SuccessCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("SuccessCount", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_FailureCount(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("FailureCount", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_AverageResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("AverageResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_MinimumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("MinimumResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsUDPEchoDiagnostics_MaximumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = udpechodiagnostics_get("MaximumResponseTime", "0");
	return 0;
}

/*
 * *** Device.IP.Diagnostics.ServerSelectionDiagnostics. ***
 */

static inline char *serverselection_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@serverselectiondiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

int get_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("DiagnosticState", "None");
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_DiagnosticsState(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				SERVERSELECTION_STOP
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_SERVERSELECTION_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@serverselectiondiagnostic[0]", "interface", value);
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			SERVERSELECTION_STOP
			curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "interface", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("ProtocolVersion", "Any");
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_ProtocolVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "IPv4") == 0 || strcmp(value, "IPv6") == 0) {
				SERVERSELECTION_STOP
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "ProtocolVersion", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("Protocol", "ICMP");
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "ICMP") == 0 || strcmp(value, "UDP Echo") == 0) {
				SERVERSELECTION_STOP
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "Protocol", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@serverselectiondiagnostic[0]", "port", value);
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1 && atoi(value) <= 65535) {
				SERVERSELECTION_STOP;
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "port", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_HostList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@serverselectiondiagnostic[0]", "HostList", value);
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_HostList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			SERVERSELECTION_STOP
			curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "HostList", value);
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("NumberOfRepetitions", "3");
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_NumberOfRepetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1) {
				SERVERSELECTION_STOP
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "NumberOfRepetitions", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("Timeout", "1000");
	return 0;
}

int set_IPDiagnosticsServerSelectionDiagnostics_Timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (atoi(value) >= 1) {
				SERVERSELECTION_STOP
				curr_section = dmuci_walk_state_section("cwmp", "serverselectiondiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "serverselectiondiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@serverselectiondiagnostic[0]", "Timeout", value);
			}
			return 0;
	}
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_FastestHost(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("FastestHost", "");
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_MinimumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("MinimumResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_AverageResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("AverageResponseTime", "0");
	return 0;
}

int get_IPDiagnosticsServerSelectionDiagnostics_MaximumResponseTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = serverselection_get("MaximumResponseTime", "0");
	return 0;
}

/*
 * *** Device.IP. ***
 */
int get_IP_IPv4Capable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_IP_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int set_IP_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IP_IPv4Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Enabled";
	return 0;
}

int get_IP_IPv6Capable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_IP_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int set_IP_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IP_IPv6Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Enabled";
	return 0;
}

int get_IP_ULAPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("network", "globals", "ula_prefix", value);
	return 0;
}

int set_IP_ULAPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value("network", "globals", "ula_prefix", value);
			break;
	}
	return 0;
}

int get_IP_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	uci_foreach_sections("network", "interface", s) {
		if (strcmp(section_name(s), "loopback") == 0)
			continue;
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

/*
 * *** Device.IP.Interface. ***
 */
int get_IPInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "disabled", &v);
	*value = (*v != '1') ? "1" : "0";
	return 0;
}

int set_IPInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "disabled", (b) ? "0" : "1");
			break;
	}
	return 0;
}

int get_IPInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *val= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	val = dmjson_get_value(res, 1, "up");
	*value = !strcmp(val, "true") ? "Up" : "Down";
	return 0;
}

int get_IPInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct ip_args *)data)->ip_sec));
	return 0;
}

int get_IPInterface_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int set_IPInterface_IPv4Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterface_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ipv6", &v);
	*value = (*v != '0') ? "1" : "0";
	return 0;
}

int set_IPInterface_IPv6Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;
	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipv6", (b) ? "" : "0");
			break;
	}
	return 0;
}

int get_IPInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)data)->ip_sec), String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 1, "uptime");
	return 0;
}

int get_IPInterface_Router(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Device.Routing.Router.1.";
	return 0;
}

int set_IPInterface_Router(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	return 0;
}

int set_IPInterface_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				set_interface_enable_ubus(section_name(((struct ip_args *)data)->ip_sec), refparam, ctx, action, "0");
				set_interface_enable_ubus(section_name(((struct ip_args *)data)->ip_sec), refparam, ctx, action, "1");
			}
			break;
	}
	return 0;
}

int get_IPInterface_MaxMTUSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *device= NULL;

	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "mtu", value);
	if(*value[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)data)->ip_sec), String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		device = dmjson_get_value(res, 1, "device");
		if(device) {
			dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
			DM_ASSERT(diag, *value = "");
			*value = dmjson_get_value(diag, 1, "mtu");
		}
		return 0;
	}
	return 0;
}

int set_IPInterface_MaxMTUSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "mtu", value);
			break;
	}
	return 0;
}

int get_IPInterface_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (strcmp(section_name(((struct ip_args *)data)->ip_sec), "loopback") == 0)
		*value = "Loopback";
	else
		*value = "Normal";
	return 0;
}

int get_IPInterface_Loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if (strcmp(section_name(((struct ip_args *)data)->ip_sec), "loopback") == 0)
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_IPInterface_Loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterface_IPv4AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	char *inst;

	*value = "0";
	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "interface", "section_name", section_name(((struct ip_args *)data)->ip_sec), s) {
		dmuci_get_value_by_section_string(s, "ipv4_instance", &inst);
		if(inst[0] != '\0')
			*value = "1";
	}
	return 0;
}

int get_IPInterface_IPv6AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int cnt = 0;

	*value = "0";
	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6", "section_name", section_name(((struct ip_args *)data)->ip_sec), s) {
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_IPInterface_IPv6PrefixNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int cnt = 0;

	*value = "0";
	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6prefix", "section_name", section_name(((struct ip_args *)data)->ip_sec), s) {
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_IPInterface_TWAMPReflectorNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;
	uci_foreach_option_eq("cwmp_twamp", "twamp_refector", "interface", section_name(((struct ip_args *)data)->ip_sec), s) {
			cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

/*
 * *** Device.IP.Interface.{i}.IPv4Address.{i}. ***
 */
int get_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	get_interface_firewall_enabled(section_name(((struct ip_args *)data)->ip_sec), refparam, ctx, value);
	return 0;
}

int set_firewall_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	set_interface_firewall_enabled(section_name(((struct ip_args *)data)->ip_sec), refparam, ctx, action, value);
	return 0;
}


int get_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct ip_args *)data)->ip_4address;
	return 0;
}

int set_ipv4_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", value);
			return 0;
	}
	return 0;
}

int get_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *jobj;
	char *mask;

	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)data)->ip_sec), String}}, 1, &res);
		if (res) {
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			mask = dmjson_get_value(jobj, 1, "mask");
			if (mask[0] == '\0')
				return 0;
			mask = cidr2netmask(atoi(mask));
		}
	}
	*value = mask;
	return 0;
}

int set_ipv4_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "netmask", value);
			return 0;
	}
	return 0;
}

int get_ipv4_addressing_type (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", value);
	if (strcmp(*value, "static") == 0)
		*value = "Static";
	else if (strcmp(*value, "dhcp") == 0)
		*value = "DHCP";
	else
		*value = "";
	return 0;
}

int set_ipv4_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(strcasecmp(value, "static") == 0) {
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "static");
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "0.0.0.0");
			}
			if(strcasecmp(value, "dhcp") == 0) {
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "dhcp");
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "");
			}
			return 0;
	}
	return 0;
}

int get_IPInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	char *wifname, *wtype, *br_inst, *mg, *device, *proto;
	struct uci_section *port;
	json_object *res;
	char buf[8];
	char linker[64] = "";

	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "type", &wtype);
	if (strcmp(wtype, "bridge") == 0) {
		char *mac;
		mac = get_macaddr(section_name(((struct ip_args *)data)->ip_sec));
		if (mac != NULL) {
			/* Expect the Ethernet.Link to be the lowerlayer*/
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), mac, value);
			return 0;
		}
		get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
		dmuci_get_value_by_section_string(dmmap_section, "bridge_instance", &br_inst);
		uci_path_foreach_option_eq(icwmpd, "dmmap_bridge_port", "bridge_port", "bridge_key", br_inst, port) {
			dmuci_get_value_by_section_string(port, "mg_port", &mg);
			if (strcmp(mg, "true") == 0)
				sprintf(linker, "%s+", section_name(port));
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cBridging%cBridge%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
			if (*value == NULL)
				*value = "";
			return 0;
		}
	} else if (wtype[0] == '\0' || strcmp(wtype, "anywan") == 0) {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)data)->ip_sec), String}}, 1, &res);
		dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ifname", &wifname);
		strcpy (linker, wifname);
		if (res) {
			device = dmjson_get_value(res, 1, "device");
			strcpy(linker, device);
			if(device[0] == '\0') {
				strncpy(buf, wifname, 6);
				buf[6]='\0';
				strcpy(linker, buf);
			}
		}
		dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
		if (strstr(proto, "ppp")) {
			sprintf(linker, "%s", section_name(((struct ip_args *)data)->ip_sec));
		}
	}
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cATM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cPTM%cLink%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cVLANTermination%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cSSID%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cPPP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);

	if (*value == NULL)
		*value = "";
	return 0;
}

int set_IPInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker= NULL, *pch, *spch, *dup, *b_key, *proto, *ipaddr, *ip_inst, *ipv4_inst, *p, *type;
	char *newvalue= NULL;
	char sec[16];
	struct uci_section *s;
	char pat[32] = "";

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			sprintf(pat, "%cPort%c1%c", dm_delim, dm_delim, dm_delim);
			if (linker && strstr(value, pat))
			{
				strncpy(sec, linker, strlen(linker) - 1);
				sec[strlen(linker) - 1] = '\0';
				DMUCI_GET_OPTION_VALUE_STRING(icwmpd, "dmmap_bridge_port", sec, "bridge_key", &b_key);
				dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
				dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ipaddr", &ipaddr);
				dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ip_int_instance", &ip_inst);
				dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ipv4_instance", &ipv4_inst);
				uci_foreach_option_eq("network", "interface", "bridge_instance", b_key, s) {
					dmuci_set_value_by_section(s, "proto", proto);
					dmuci_set_value_by_section(s, "ipaddr", ipaddr);
					dmuci_set_value_by_section(s, "ip_int_instance", ip_inst);
					dmuci_set_value_by_section(s, "ipv4_instance", ipv4_inst);
					dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "type", &type);
					if (strcmp (type, "bridge"))
						dmuci_delete_by_section(((struct ip_args *)data)->ip_sec, NULL, NULL);
				}
				return 0;
			}

			//lowerlayer is expected to be Device.Ethernet.VLANTerminaiton.{i}.
			if (linker)
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ifname", linker);
			else
				return FAULT_9005;

			return 0;
	}
	return 0;
}

/*
 * *** Device.IP.Interface.{i}.IPv6Address.{i}. ***
 */
int get_IPInterfaceIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct ipv6_args *)data)->ip_6address;
	if(((struct ipv6_args *)data)->ip_6mask[0] != '\0')
		dmasprintf(value, "%s/%s", ((struct ipv6_args *)data)->ip_6address, ((struct ipv6_args *)data)->ip_6mask);
	return 0;
}

int set_IPInterfaceIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ipv6_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", value);
			return 0;
	}
	return 0;
}

int get_IPInterfaceIPv6Address_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int set_IPInterfaceIPv6Address_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Address_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Enabled";
	return 0;
}

int get_IPInterfaceIPv6Address_IPAddressStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if(((struct ipv6_args *)data)->ip_6valid[0] != '\0')
		*value = "Preferred";
	else
		*value = "Unknown";
	return 0;
}

int get_IPInterfaceIPv6Address_Origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ubus_call_get_value_with_two_objects(section_name(((struct ipv6_args *)data)->ip_sec), "ipv6-prefix-assignment", "local-address", "address");
	if(*value[0] != '\0')
		*value = "AutoConfigured";
	else {
		dmuci_get_value_by_section_string(((struct ipv6_args *)data)->ip_sec, "proto", value);
		if (strcmp(*value, "static") == 0)
			*value = "Static";
		else if (strcmp(*value, "dhcpv6") == 0)
			*value = "DHCPv6";
		else
			*value = "WellKnown";
	}
	return 0;
}

int get_IPInterfaceIPv6Address_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	char *inst;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ipv6_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ip_int_instance", &inst);

	*value = "";
	if(((struct ipv6prefix_args *)data)->ip_6prefixaddress[0] != '\0')
		dmasprintf(value, "Device.IP.Interface.%s.IPv6Prefix.1.", inst);
	return 0;
}

int set_IPInterfaceIPv6Address_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char local_time[32] = {0};
	char *preferred = ((struct ipv6_args *)data)->ip_6preferred;
	*value = "0001-01-01T00:00:00Z";
	if (get_shift_time_time(atoi(preferred), local_time, sizeof(local_time)) == -1)
		return 0;
	*value = dmstrdup(local_time);
	return 0;
}

int set_IPInterfaceIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char buf[32] = "", *proto;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ipv6_args *)data)->ip_sec, "proto", &proto);
			if(strcasecmp(proto, "static") == 0) {
				get_shift_time_shift(value, buf);
				if (!(*buf))
					return 0;
				dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "adv_preferred_lifetime", buf);
			}
			return 0;
	}
	return 0;
}

int get_IPInterfaceIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char local_time[32] = {0};
	char *preferred = ((struct ipv6_args *)data)->ip_6valid;
	*value = "0001-01-01T00:00:00Z";
	if (get_shift_time_time(atoi(preferred), local_time, sizeof(local_time)) == -1)
		return 0;
	*value = dmstrdup(local_time);
	return 0;
}

int set_IPInterfaceIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char buf[32] = "", *proto;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ipv6_args *)data)->ip_sec, "proto", &proto);
			if(strcasecmp(proto, "static") == 0) {
				get_shift_time_shift(value, buf);
				if (!(*buf))
					return 0;
				dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "adv_valid_lifetime", buf);
			}
			return 0;
	}
	return 0;
}

/*
 * *** Device.IP.Interface.{i}.IPv6Prefix.{i}. ***
 */
int get_IPInterfaceIPv6Prefix_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int set_IPInterfaceIPv6Prefix_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Enabled";
	return 0;
}

int get_IPInterfaceIPv6Prefix_PrefixStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	if(((struct ipv6prefix_args *)data)->ip_6prefixvalid[0] != '\0')
		*value = "Preferred";
	else
		*value = "Unknown";
	return 0;
}

int get_IPInterfaceIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct ipv6prefix_args *)data)->ip_6prefixaddress;
	if(((struct ipv6prefix_args *)data)->ip_6prefixmask[0] != '\0')
		dmasprintf(value, "%s/%s", ((struct ipv6prefix_args *)data)->ip_6prefixaddress, ((struct ipv6prefix_args *)data)->ip_6prefixmask);
	return 0;
}

int set_IPInterfaceIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ipv6prefix_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(((struct ipv6prefix_args *)data)->ip_sec, "ip6prefix", value);
			return 0;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_Origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ubus_call_get_value(section_name(((struct ipv6prefix_args *)data)->ip_sec), "ipv6-prefix-assignment", "address");
	if(*value[0] != '\0')
		*value = "AutoConfigured";
	else {
		dmuci_get_value_by_section_string(((struct ipv6prefix_args *)data)->ip_sec, "proto", value);
		if (strcmp(*value, "static") == 0)
			*value = "Static";
		else if (strcmp(*value, "dhcpv6") == 0)
			*value = "DHCPv6";
		else
			*value = "WellKnown";
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_StaticType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Static";
	return 0;
}

int set_IPInterfaceIPv6Prefix_StaticType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_ParentPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	dmasprintf(&linker, "%s/%s", ((struct ipv6prefix_args *)data)->ip_6prefixaddress, ((struct ipv6prefix_args *)data)->ip_6prefixmask);
	if(linker[0] != '\0')
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%Interface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_IPInterfaceIPv6Prefix_ParentPrefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_ChildPrefixBits(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_child_prefix_linker(section_name(((struct ipv6prefix_args *)data)->ip_sec));
	return 0;
}

int set_IPInterfaceIPv6Prefix_ChildPrefixBits(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char local_time[32] = {0};
	char *preferred = ((struct ipv6prefix_args *)data)->ip_6prefixpreferred;
	*value = "0001-01-01T00:00:00Z";
	if (get_shift_time_time(atoi(preferred), local_time, sizeof(local_time)) == -1)
		return 0;
	*value = dmstrdup(local_time);
	return 0;
}

int set_IPInterfaceIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char local_time[32] = {0};
	char *preferred = ((struct ipv6prefix_args *)data)->ip_6prefixvalid;
	*value = "0001-01-01T00:00:00Z";
	if (get_shift_time_time(atoi(preferred), local_time, sizeof(local_time)) == -1)
		return 0;
	*value = dmstrdup(local_time);
	return 0;
}

int set_IPInterfaceIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

/*
 * *** Device.IP.Interface.{i}.Stats. ***
 */
static char *get_ip_interface_statistics(char *interface, char *key)
{
	json_object *res, *diag;
	char *device, *value = "0";

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device[0] != '\0') {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		value = dmjson_get_value(diag, 2, "statistics", key);
	}
	return value;
}

int get_ip_interface_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "tx_bytes");
	return 0;
}

int get_ip_interface_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "rx_bytes");
	return 0;
}

int get_ip_interface_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "tx_packets");
	return 0;
}

int get_ip_interface_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "rx_packets");
	return 0;
}

int get_ip_interface_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "tx_errors");
	return 0;
}

int get_ip_interface_statistics_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "rx_errors");
	return 0;
}

int get_ip_interface_statistics_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "tx_dropped");
	return 0;
}

int get_ip_interface_statistics_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "rx_dropped");
	return 0;
}

int get_ip_interface_statistics_tx_unicastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "unicast"));
	return 0;
}

int get_ip_interface_statistics_rx_unicastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "unicast"));
	return 0;
}

int get_ip_interface_statistics_tx_multicastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "multicast"));
	return 0;
}

int get_ip_interface_statistics_rx_multicastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "multicast"));
	return 0;
}

int get_ip_interface_statistics_tx_broadcastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "TX", "broadcast"));
	return 0;
}

int get_ip_interface_statistics_rx_broadcastpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	char *device = get_device(section_name(((struct ip_args *)data)->ip_sec));
	if(device[0] != '\0')
		dmasprintf(value, "%d", get_stats_from_ifconfig_command(device, "RX", "broadcast"));
	return 0;
}

int get_ip_interface_statistics_rx_unknownprotopackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_ip_interface_statistics(section_name(((struct ip_args *)data)->ip_sec), "rx_over_errors");
	return 0;
}

/*
 * *** Device.IP.Interface.{i}.TWAMPReflector.{i}. ***
 */
int get_IPInterfaceTWAMPReflector_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "enable", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;
	struct uci_section *s;
	char *type, *interface, *device, *id, *ipv4addr = "";
	json_object *res, *jobj;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
				dmuci_get_value_by_section_string((struct uci_section *)data, "id", &id);
				dmuci_set_value_by_section((struct uci_section *)data, "enable", "1");
				dmuci_set_value("cwmp_twamp", "twamp", "id", id);
				uci_foreach_sections("network", "interface", s) {
					if(strcmp(section_name(s), interface) != 0)
						continue;
					dmuci_get_value_by_section_string(s, "ipaddr", &ipv4addr);
					break;
				}
				if (ipv4addr[0] == '\0') {
					dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
					if (res)
					{
						jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
						ipv4addr = dmjson_get_value(jobj, 1, "address");
						if (ipv4addr[0] == '\0')
							dmuci_set_value_by_section((struct uci_section *)data, "ip_version", "6");
						else
							dmuci_set_value_by_section((struct uci_section *)data, "ip_version", "4");
					}
				}
				else
					dmuci_set_value_by_section((struct uci_section *)data, "ip_version", "4");
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interface, String}}, 1, &res);
				if (res)
				{
					device = dmjson_get_value(res, 1, "device");
					dmuci_set_value_by_section((struct uci_section *)data, "device", device);
				}
			} else {
				dmuci_set_value_by_section((struct uci_section *)data, "enable", "0");
			}
			break;
	}
	return 0;
}

int get_IPInterfaceTWAMPReflector_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *enable;
	dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &enable);
	if(strcmp(enable, "1")==0)
		*value = "Active";
	else
		*value = "Disabled";
	return 0;
}

int get_IPInterfaceTWAMPReflector_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "twamp_alias", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "twamp_alias", value);
			break;
	}
	return 0;
}

int get_IPInterfaceTWAMPReflector_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "port", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "port", value);
			break;
	}
	return 0;
}

int get_IPInterfaceTWAMPReflector_MaximumTTL(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "max_ttl", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_MaximumTTL(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "max_ttl", value);
			break;
	}
	return 0;
}

int get_IPInterfaceTWAMPReflector_IPAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "ip_list", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_IPAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "ip_list", value);
			break;
	}
	return 0;
}

int get_IPInterfaceTWAMPReflector_PortAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "port_list", value);
	return 0;
}

int set_IPInterfaceTWAMPReflector_PortAllowedList(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "port_list", value);
			break;
	}
	return 0;
}

/*************************************************************
 * GET & SET ALIAS
/*************************************************************/
int get_IPInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ip_int_alias", value);
	return 0;
}

int set_IPInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section =NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_set_value_by_section(dmmap_section, "ip_int_alias", value);
			return 0;
	}
	return 0;
}

int get_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ipv4_alias", value);
	return 0;
}

int set_ipv4_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section =NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_set_value_by_section(dmmap_section, "ipv4_alias", value);
			return 0;
	}
	return 0;
}

int get_IPInterfaceIPv6Address_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	char *name;

	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6", "ipv6_instance", instance, dmmap_section) {
		dmuci_get_value_by_section_string(dmmap_section, "section_name", &name);
		if(strcmp(name, section_name(((struct ipv6_args *)data)->ip_sec)) == 0)
			dmuci_get_value_by_section_string(dmmap_section, "ipv6_alias", value);
	}
	return 0;
}

int set_IPInterfaceIPv6Address_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;
	char *name;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6", "ipv6_instance", instance, dmmap_section) {
				dmuci_get_value_by_section_string(dmmap_section, "section_name", &name);
				if(strcmp(name, section_name(((struct ipv6_args *)data)->ip_sec)) == 0)
					break;
			}
			dmuci_set_value_by_section(dmmap_section, "ipv6_alias", value);
			return 0;
	}
	return 0;
}

int get_IPInterfaceIPv6Prefix_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	char *name;

	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6prefix", "ipv6prefix_instance", instance, dmmap_section) {
		dmuci_get_value_by_section_string(dmmap_section, "section_name", &name);
		if(strcmp(name, section_name(((struct ipv6prefix_args *)data)->ip_sec)) == 0)
			dmuci_get_value_by_section_string(dmmap_section, "ipv6prefix_alias", value);
	}
	return 0;
}

int set_IPInterfaceIPv6Prefix_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;
	char *name;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6prefix", "ipv6prefix_instance", instance, dmmap_section) {
				dmuci_get_value_by_section_string(dmmap_section, "section_name", &name);
				if(strcmp(name, section_name(((struct ipv6prefix_args *)data)->ip_sec)) == 0)
					break;
			}
			dmuci_set_value_by_section(dmmap_section, "ipv6prefix_alias", value);
			return 0;
	}
	return 0;
}

/*************************************************************
 * ADD & DEL OBJ
/*************************************************************/
char *get_last_instance_cond(char* dmmap_package, char *package, char *section, char *opt_inst, char *opt_cond, char *cond_val, char *opt_filter, char *filter_val, char *refused_interface)
{
	struct uci_section *s, *dmmap_section;
	char *inst = NULL, *val, *val_f;
	char *type, *ipv4addr = "", *ipv6addr = "", *proto;
	json_object *res, *jobj;

	uci_foreach_sections(package, section, s) {
		if (opt_cond) dmuci_get_value_by_section_string(s, opt_cond, &val);
		if (opt_filter) dmuci_get_value_by_section_string(s, opt_filter, &val_f);
		if(opt_cond && opt_filter && (strcmp(val, cond_val) == 0 || strcmp(val_f, filter_val) == 0))
			continue;
		if (strcmp(section_name(s), refused_interface)==0)
			continue;

		dmuci_get_value_by_section_string(s, "ipaddr", &ipv4addr);
		if (ipv4addr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &res);
			if (res)
			{
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipv4addr = dmjson_get_value(jobj, 1, "address");
			}
		}
		dmuci_get_value_by_section_string(s, "ip6addr", &ipv6addr);
		if (ipv6addr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &res);
			if (res)
			{
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-address");
				ipv6addr = dmjson_get_value(jobj, 1, "address");
			}
		}
		dmuci_get_value_by_section_string(s, "proto", &proto);
		if (ipv4addr[0] == '\0' && ipv6addr[0] == '\0' && strcmp(proto, "dhcp") != 0 && strcmp(proto, "dhcpv6") != 0 && strcmp(val, "bridge") != 0) {
			continue;
		}
		get_dmmap_section_of_config_section(dmmap_package, section, section_name(s), &dmmap_section);
		inst = update_instance_icwmpd(dmmap_section, inst, opt_inst);
	}
	return inst;
}

int add_ip_interface(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *last_inst, *v;
	char ip_name[32], ib[8];
	char *p = ip_name;
	struct uci_section *dmmap_ip_interface, *dmmap_section;

	last_inst = get_last_instance_cond("dmmap_network", "network", "interface", "ip_int_instance", "type", "alias", "proto", "", "loopback");
	sprintf(ib, "%d", last_inst ? atoi(last_inst)+1 : 1);
	dmstrappendstr(p, "ip_interface_");
	dmstrappendstr(p, ib);
	dmstrappendend(p);
	dmuci_set_value("network", ip_name, "", "interface");
	dmuci_set_value("network", ip_name, "proto", "dhcp");

	dmuci_add_section_icwmpd("dmmap_network", "interface", &dmmap_ip_interface, &v);
	dmuci_set_value_by_section(dmmap_ip_interface, "section_name", ip_name);
	*instance = update_instance_icwmpd(dmmap_ip_interface, last_inst, "ip_int_instance");
	return 0;
}

int delete_ip_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *dmmap_section = NULL;

	switch (del_action) {
	case DEL_INST:
		dmuci_delete_by_section(((struct ip_args *)data)->ip_sec, NULL, NULL);
		get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
		if(dmmap_section != NULL)
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int add_ipv4(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *instance;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ipv4_instance", &instance);
	*instancepara = update_instance_icwmpd(dmmap_section, instance, "ipv4_instance");
	if(instance[0] == '\0') {
		dmuci_set_value_by_section(dmmap_section, "ipv4_instance", *instancepara);
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "0.0.0.0");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "static");
	}
	return 0;
}

int delete_ipv4(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *dmmap_section;

	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "");
		get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
		if(dmmap_section != NULL)
			dmuci_set_value_by_section(dmmap_section, "ipv4_instance", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int add_ipv6(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	struct uci_section *s, *ss;
	char *ip, *name, *inst, *curr_inst;

	uci_foreach_sections("network", "interface", s) {
		if(strcmp(section_name(s), section_name(((struct ipv6_args *)data)->ip_sec)) != 0)
			continue;
		dmuci_get_value_by_section_string(s, "ip6addr", &ip);
		break;
	}
	if(ip[0] == '\0') {
		uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6", "section_name", section_name(((struct ipv6_args *)data)->ip_sec), s) {
			dmuci_get_value_by_section_string(s, "ipv6_instance", &inst);
		}
		dmasprintf(&curr_inst, "%d", atoi(inst)+1);
		dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "ip6addr", "::");
		dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "proto", "static");
		DMUCI_ADD_SECTION(icwmpd, "dmmap_network", "ipv6", &ss, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "section_name", section_name(((struct ipv6_args *)data)->ip_sec));
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "ipv6_instance", curr_inst);
	}
	return 0;
}

int delete_ipv6(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "ipv6", section_name(((struct ipv6_args *)data)->ip_sec), &dmmap_section);
	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "ip6addr", "");
		dmuci_set_value_by_section(dmmap_section, "ipv6_instance", "");
		dmuci_set_value_by_section(((struct ipv6_args *)data)->ip_sec, "proto", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int add_ipv6_prefix(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	struct uci_section *s, *ss;
	char *ip, *name, *inst, *curr_inst;

	uci_foreach_sections("network", "interface", s) {
		if(strcmp(section_name(s), section_name(((struct ipv6prefix_args *)data)->ip_sec)) != 0)
			continue;
		dmuci_get_value_by_section_string(s, "ip6prefix", &ip);
		break;
	}
	if(ip[0] == '\0') {
		uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6prefix", "section_name", section_name(((struct ipv6prefix_args *)data)->ip_sec), s) {
			dmuci_get_value_by_section_string(s, "ipv6prefix_instance", &inst);
		}
		dmasprintf(&curr_inst, "%d", atoi(inst)+1);
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6prefix", "::");
		dmuci_set_value_by_section(((struct ipv6prefix_args *)data)->ip_sec, "proto", "static");
		DMUCI_ADD_SECTION(icwmpd, "dmmap_network", "ipv6prefix", &ss, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "section_name", section_name(((struct ipv6prefix_args *)data)->ip_sec));
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "ipv6prefix_instance", curr_inst);
	}
	return 0;
}

int delete_ipv6_prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "ipv6prefix", section_name(((struct ipv6prefix_args *)data)->ip_sec), &dmmap_section);
	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ipv6prefix_args *)data)->ip_sec, "ip6prefix", "");
		dmuci_set_value_by_section(dmmap_section, "ipv6prefix_instance", "");
		dmuci_set_value_by_section(((struct ipv6prefix_args *)data)->ip_sec, "proto", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

static char *get_last_instance_with_option(char *package, char *section, char *option, char *val, char *opt_inst)
{
	struct uci_section *s;
	char *inst = NULL;

	uci_foreach_option_eq(package, section, option, val, s) {
		inst = update_instance(s, inst, opt_inst);
	}
	return inst;
}

static char *get_last_id(char *package, char *section)
{
	struct uci_section *s;
	char *id;
	int cnt = 0;

	uci_foreach_sections(package, section, s) {
		cnt++;
	}
	dmasprintf(&id, "%d", cnt+1);
	return id;
}

int addObjIPInterfaceTWAMPReflector(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *connection;
	char *value1, *last_inst, *id;

	last_inst = get_last_instance_with_option("cwmp_twamp", "twamp_refector", "interface", section_name(((struct ip_args *)data)->ip_sec), "twamp_inst");
	id = get_last_id("cwmp_twamp", "twamp_refector");
	dmuci_add_section("cwmp_twamp", "twamp_refector", &connection, &value1);
	dmasprintf(instance, "%d", last_inst?atoi(last_inst)+1:1);
	dmuci_set_value_by_section(connection, "twamp_inst", *instance);
	dmuci_set_value_by_section(connection, "id", id);
	dmuci_set_value_by_section(connection, "enable", "0");
	dmuci_set_value_by_section(connection, "interface", section_name(((struct ip_args *)data)->ip_sec));
	dmuci_set_value_by_section(connection, "port", "862");
	dmuci_set_value_by_section(connection, "max_ttl", "1");
	return 0;
}

int delObjIPInterfaceTWAMPReflector(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	struct uci_section *s, *ss = NULL;
	char *interface;
	struct uci_section *section = (struct uci_section *)data;

	switch (del_action) {
		case DEL_INST:
			dmuci_delete_by_section(section, NULL, NULL);
			return 0;
		case DEL_ALL:
			uci_foreach_sections("cwmp_twamp", "twamp_refector", s) {
				dmuci_get_value_by_section_string(s, "interface", &interface);
				if(strcmp(interface, section_name(((struct ip_args *)data)->ip_sec)) != 0)
					continue;
				if (found != 0) {
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL) {
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_ip_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct ip_args *)data)->ip_sec) {
		dmasprintf(linker,"%s", section_name(((struct ip_args *)data)->ip_sec));
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

int get_linker_ipv6_prefix(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(((struct ipv6prefix_args *)data)->ip_sec) {
		dmasprintf(linker,"%s", get_child_prefix_linker(section_name(((struct ipv6prefix_args *)data)->ip_sec)));
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseIPIfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *ip_int = NULL, *ip_int_last = NULL;
	char *type, *ipv4addr = "";
	struct ip_args curr_ip_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "type", &type);
		if (strcmp(type, "alias") == 0 || strcmp(section_name(p->config_section), "loopback")==0)
			continue;

		/* IPv4 address */
		dmuci_get_value_by_section_string(p->config_section, "ipaddr", &ipv4addr);
		if (ipv4addr[0] == '\0')
			ipv4addr = ubus_call_get_value(section_name(p->config_section), "ipv4-address", "address");

		init_ip_args(&curr_ip_args, p->config_section, ipv4addr);
		ip_int = handle_update_instance(1, dmctx, &ip_int_last, update_instance_alias, 3, p->dmmap_section, "ip_int_instance", "ip_int_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ip_args, ip_int) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseIfaceIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *ipv4_inst = NULL, *ipv4_inst_last = NULL;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)prev_data)->ip_sec), &dmmap_section);
	if(((struct ip_args *)prev_data)->ip_4address[0] != '\0') {
		ipv4_inst = handle_update_instance(2, dmctx, &ipv4_inst_last, update_instance_alias, 3, dmmap_section, "ipv4_instance", "ipv4_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, prev_data, ipv4_inst) == DM_STOP)
			goto end;
	}
end:
	return 0;
}

static struct uci_section *update_dmmap_network_ipv6(char *curr_inst, char *section_name)
{
	struct uci_section *s = NULL;
	char *inst, *name;

	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6", "section_name", section_name, s) {
		dmuci_get_value_by_section_string(s, "ipv6_instance", &inst);
		if(strcmp(curr_inst, inst) == 0)
			return s;
	}
	if (!s) {
		DMUCI_ADD_SECTION(icwmpd, "dmmap_network", "ipv6", &s, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "section_name", section_name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "ipv6_instance", curr_inst);
	}
	return s;
}

int browseIfaceIPv6Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	char *ipv6_int = NULL, *ipv6_int_last = NULL, *ipv6addr = "", *ipv6mask = "", *ipv6_preferred = "", *ipv6_valid = "", buf[4]="";
	struct ipv6_args curr_ipv6_args = {0};
	json_object *res, *jobj, *jobj1;
	int entries = 0;

	if(prev_data && ((struct ip_args *)prev_data)->ip_sec) {
		dmuci_get_value_by_section_string(((struct ip_args *)prev_data)->ip_sec, "ip6addr", &ipv6addr);
		dmuci_get_value_by_section_string(((struct ip_args *)prev_data)->ip_sec, "adv_preferred_lifetime", &ipv6_preferred);
		dmuci_get_value_by_section_string(((struct ip_args *)prev_data)->ip_sec, "adv_valid_lifetime", &ipv6_valid);
		if (ipv6addr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)prev_data)->ip_sec), String}}, 1, &res);
			while (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-prefix-assignment");
				if(jobj) {
					jobj1 = dmjson_get_obj(jobj, 1, "local-address");
					if(jobj1) {
						ipv6addr = dmjson_get_value(jobj1, 1, "address");
						ipv6mask = dmjson_get_value(jobj1, 1, "mask");
						goto browse;
					}
				}
				jobj = dmjson_select_obj_in_array_idx(res, entries, 1, "ipv6-address");
				if(jobj) {
					ipv6addr = dmjson_get_value(jobj, 1, "address");
					ipv6mask = dmjson_get_value(jobj, 1, "mask");
					if (ipv6_preferred[0] == '\0')
						ipv6_preferred = dmjson_get_value(jobj, 1, "preferred");
					if (ipv6_valid[0] == '\0')
						ipv6_valid = dmjson_get_value(jobj, 1, "valid");
					entries++;
					sprintf(buf, "%d", entries);
					s = update_dmmap_network_ipv6(buf, section_name(((struct ip_args *)prev_data)->ip_sec));
					init_ipv6_args(&curr_ipv6_args, ((struct ip_args *)prev_data)->ip_sec, ipv6addr, ipv6mask, ipv6_preferred, ipv6_valid);
					ipv6_int = handle_update_instance(1, dmctx, &ipv6_int_last, update_instance_alias, 3, s, "ipv6_instance", "ipv6_alias");
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ipv6_args, ipv6_int) == DM_STOP)
						goto end;
				} else
					goto end;
			}
		}
browse:
		s = update_dmmap_network_ipv6("1", section_name(((struct ip_args *)prev_data)->ip_sec));
		init_ipv6_args(&curr_ipv6_args, ((struct ip_args *)prev_data)->ip_sec, ipv6addr, ipv6mask, ipv6_preferred, ipv6_valid);
		ipv6_int = handle_update_instance(1, dmctx, &ipv6_int_last, update_instance_alias, 3, s, "ipv6_instance", "ipv6_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ipv6_args, ipv6_int) == DM_STOP)
			goto end;
	}
end:
	return 0;
}


static struct uci_section *update_dmmap_network_ipv6prefix(char *curr_inst, char *section_name)
{
	struct uci_section *s = NULL;
	char *inst, *name;

	uci_path_foreach_option_eq(icwmpd, "dmmap_network", "ipv6prefix", "section_name", section_name, s) {
		dmuci_get_value_by_section_string(s, "ipv6prefix_instance", &inst);
		if(strcmp(curr_inst, inst) == 0)
			return s;
	}
	if (!s) {
		DMUCI_ADD_SECTION(icwmpd, "dmmap_network", "ipv6prefix", &s, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "section_name", section_name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "ipv6prefix_instance", curr_inst);
	}
	return s;
}

int browseIfaceIPv6PrefixInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	char *ipv6prefix_int = NULL, *ipv6prefix_int_last = NULL, *ipv6prefixaddr = "", *ipv6prefixmask = "", *ipv6prefix_preferred = "", *ipv6prefix_valid = "", buf[4] = "";
	struct ipv6prefix_args curr_ipv6prefix_args = {0};
	json_object *res, *jobj;
	int entries = 0;

	if(prev_data && ((struct ip_args *)prev_data)->ip_sec) {
		dmuci_get_value_by_section_string(((struct ip_args *)prev_data)->ip_sec, "ip6prefix", &ipv6prefixaddr);
		if (ipv6prefixaddr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(((struct ip_args *)prev_data)->ip_sec), String}}, 1, &res);
			while (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-prefix-assignment");
				if(jobj) {
					ipv6prefixaddr = dmjson_get_value(jobj, 1, "address");
					ipv6prefixmask = dmjson_get_value(jobj, 1, "mask");
					ipv6prefix_preferred = dmjson_get_value(jobj, 1, "preferred");
					ipv6prefix_valid = dmjson_get_value(jobj, 1, "valid");
					goto browse;
				}
				jobj = dmjson_select_obj_in_array_idx(res, entries, 1, "ipv6-prefix");
				if(jobj) {
					ipv6prefixaddr = dmjson_get_value(jobj, 1, "address");
					ipv6prefixmask = dmjson_get_value(jobj, 1, "mask");
					ipv6prefix_preferred = dmjson_get_value(jobj, 1, "preferred");
					ipv6prefix_valid = dmjson_get_value(jobj, 1, "valid");
					entries++;
					sprintf(buf, "%d", entries);
					s = update_dmmap_network_ipv6prefix(buf, section_name(((struct ip_args *)prev_data)->ip_sec));
					init_ipv6prefix_args(&curr_ipv6prefix_args, ((struct ip_args *)prev_data)->ip_sec, ipv6prefixaddr, ipv6prefixmask, ipv6prefix_preferred, ipv6prefix_valid);
					ipv6prefix_int = handle_update_instance(1, dmctx, &ipv6prefix_int_last, update_instance_alias, 3, s, "ipv6prefix_instance", "ipv6prefix_alias");
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ipv6prefix_args, ipv6prefix_int) == DM_STOP)
						goto end;
				} else
					goto end;
			}
		}
browse:
		s = update_dmmap_network_ipv6prefix("1", section_name(((struct ip_args *)prev_data)->ip_sec));
		init_ipv6prefix_args(&curr_ipv6prefix_args, ((struct ip_args *)prev_data)->ip_sec, ipv6prefixaddr, ipv6prefixmask, ipv6prefix_preferred, ipv6prefix_valid);
		ipv6prefix_int = handle_update_instance(1, dmctx, &ipv6prefix_int_last, update_instance_alias, 3, s, "ipv6prefix_instance", "ipv6prefix_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ipv6prefix_args, ipv6prefix_int) == DM_STOP)
			goto end;
	}
end:
	return 0;
}

int browseIPInterfaceTWAMPReflectorInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *twamp_inst = NULL, *twamp_inst_last = NULL;

	uci_foreach_option_eq("cwmp_twamp", "twamp_refector", "interface", section_name(((struct ip_args *)prev_data)->ip_sec), s)
	{
		twamp_inst = handle_update_instance(2, dmctx, &twamp_inst_last, update_instance_alias, 3, (void *)s, "twamp_inst", "twamp_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, twamp_inst) == DM_STOP)
			break;
	}
	return 0;
}

int browseIPDiagnosticsTraceRouteRouteHopsInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *idx_last = NULL;

	uci_foreach_sections_state("cwmp", "RouteHops", s)
	{
		instance = handle_update_instance(2, dmctx, &idx_last, update_instance_alias, 3, (void *)s, "routehop_instance", "routehop_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseIPDiagnosticsDownloadDiagnosticsPerConnectionResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *idx_last = NULL;

	uci_foreach_sections_state("cwmp", "DownloadPerConnection", s)
	{
		instance = handle_update_instance(2, dmctx, &idx_last, update_instance_alias, 3, (void *)s, "perconnection_instance", "perconnection_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseIPDiagnosticsUploadDiagnosticsPerConnectionResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *idx_last = NULL;

	uci_foreach_sections_state("cwmp", "UploadPerConnection", s)
	{
		instance = handle_update_instance(2, dmctx, &idx_last, update_instance_alias, 3, (void *)s, "perconnection_instance", "perconnection_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;
	}
	return 0;
}
