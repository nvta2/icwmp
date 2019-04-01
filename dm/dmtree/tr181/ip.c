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

DMLEAF tIPintParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_ip_interface_enable, set_ip_interface_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_ip_interface_status, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_ip_interface_name, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_ip_int_lower_layer, set_ip_int_lower_layer, NULL, NULL},
{0}
};

/* *** Device.IP.Interface. *** */
DMOBJ tInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"IPv4Address", &DMWRITE, add_ipv4, delete_ipv4, NULL, browseIfaceIPv4Inst, NULL, NULL, NULL, tIPv4Params, NULL},
{"IPv6Address", &DMWRITE, add_ipv6, delete_ipv6, NULL, browseIfaceIPv6Inst, NULL, NULL, NULL, tIPv6Params, NULL},
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tIPInterfaceStatsParams, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.IPv4Address.{i}. *** */
DMLEAF tIPv4Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_ipv4_alias, set_ipv4_alias, &IPv4INFRM, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_ip_interface_enable, set_ip_interface_enable, &IPv4INFRM, NULL},
{CUSTOM_PREFIX"FirewallEnabled", &DMWRITE, DMT_BOOL, get_firewall_enabled, set_firewall_enabled, &IPv4INFRM, NULL},
{"IPAddress", &DMWRITE, DMT_STRING, get_ipv4_address, set_ipv4_address, &IPv4INFRM, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING, get_ipv4_netmask, set_ipv4_netmask, &IPv4INFRM, NULL},
{"AddressingType", &DMWRITE, DMT_STRING, get_ipv4_addressing_type, set_ipv4_addressing_type, &IPv4INFRM, NULL},
{0}
};

/* *** Device.IP.Interface.{i}.IPv6Address.{i}. *** */
DMLEAF tIPv6Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_ipv6_alias, set_ipv6_alias, &IPv6INFRM, NULL},
{"Enable", &DMREAD, DMT_BOOL, get_ip_enable, NULL, &IPv6INFRM, NULL},
{"IPAddress", &DMWRITE, DMT_STRING, get_ipv6_address, set_ipv6_address, &IPv6INFRM, NULL},
{"Origin", &DMWRITE, DMT_STRING, get_ipv6_addressing_type, set_ipv6_addressing_type, &IPv6INFRM, NULL},
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
inline int init_ip_args(struct ip_args *args, struct uci_section *s, char *ip_4address, char *ip_6address)
{
	args->ip_sec = s;
	args->ip_4address = ip_4address;
	args->ip_6address = ip_6address;
	return 0;
}

/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_diag_enable_true(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_diag_enable_false(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
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

int get_ip_interface_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec);
	get_interface_enable_ubus(lan_name, refparam, ctx, value);
	return 0;
}

int set_ip_interface_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *lan_name;
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			lan_name = section_name(((struct ip_args *)data)->ip_sec);
			set_interface_enable_ubus(lan_name, refparam, ctx, action, value);
			break;
	}
	return 0;
}

int get_ip_interface_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *val= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	val = dmjson_get_value(res, 1, "up");
	*value = !strcmp(val, "true") ? "Up" : "Down";
	return 0;
}

int get_ip_interface_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(section_name(((struct ip_args *)data)->ip_sec));
	return 0;
}

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
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "netmask", value);
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

int get_ipv6_addressing_type (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", value);
	if (strcmp(*value, "static") == 0)
		*value = "Static";
	else if (strcmp(*value, "dhcpv6") == 0)
		*value = "DHCPv6";
	else
		*value = "";
	return 0;
}

int set_ipv6_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(strcasecmp(value, "static") == 0) {
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "static");
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", "0.0.0.0");
			}
			if(strcasecmp(value, "dhcpv6") == 0) {
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "dhcpv6");
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", "");
			}
			return 0;
	}
	return 0;
}

int get_ip_int_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_ip_int_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_ipv6_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct ip_args *)data)->ip_6address;
	return 0;
}

int set_ipv6_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", value);
			return 0;
	}
	return 0;
}

int get_ip_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

int get_ip_interface_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "tx_bytes");
	}
	return 0;
}

int get_ip_interface_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "rx_bytes");
	}
	return 0;
}

int get_ip_interface_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "tx_packets");
	}
	return 0;
}

int get_ip_interface_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "rx_packets");
	}
	return 0;
}

int get_ip_interface_statistics_tx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "tx_errors");
	}
	return 0;
}

int get_ip_interface_statistics_rx_errors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "rx_errors");
	}
	return 0;
}

int get_ip_interface_statistics_tx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "tx_dropped");
	}
	return 0;
}

int get_ip_interface_statistics_rx_discardpackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *diag;
	char *lan_name = section_name(((struct ip_args *)data)->ip_sec), *device= NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	device = dmjson_get_value(res, 1, "device");
	if(device) {
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &diag);
		DM_ASSERT(diag, *value = "");
		*value = dmjson_get_value(diag, 2, "statistics", "rx_dropped");
	}
	return 0;
}
/*************************************************************
 * GET & SET ALIAS
/*************************************************************/
int get_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ip_int_alias", value);
	return 0;
}

int set_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip_int_alias", value);
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
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	char *proto;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(dmmap_section, "ipv4_alias", value);
			return 0;
	}
	return 0;
}

int get_ipv6_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ipv6_alias", value);
	return 0;
}

int set_ipv6_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "proto", &proto);
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(dmmap_section, "ipv6_alias", value);
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
	struct uci_section *dmmap_section;


	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "type", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ifname", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", "");

		get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
		dmuci_set_value_by_section(dmmap_section, "ip_int_instance", "");
		dmuci_set_value_by_section(dmmap_section, "ipv4_instance", "");
		dmuci_set_value_by_section(dmmap_section, "ipv6_instance", "");
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

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipaddr", "");
		dmuci_set_value_by_section(dmmap_section, "ipv4_instance", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int add_ipv6(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *instance;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	dmuci_get_value_by_section_string(((struct ip_args *)data)->ip_sec, "ipv6_instance", &instance);
	*instancepara = update_instance(((struct ip_args *)data)->ip_sec, instance, "ipv6_instance");
	if(instance[0] == '\0') {
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ipv6_instance", *instancepara);
		dmuci_set_value_by_section(dmmap_section, "ip6addr", "::");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "static");
	}
	return 0;
}

int delete_ipv6(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)data)->ip_sec), &dmmap_section);
	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "ip6addr", "");
		dmuci_set_value_by_section(dmmap_section, "ipv6_instance", "");
		dmuci_set_value_by_section(((struct ip_args *)data)->ip_sec, "proto", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_ip_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(((struct ip_args *)data)->ip_sec) {
		dmasprintf(linker,"%s", section_name(((struct ip_args *)data)->ip_sec));
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
	struct uci_section *net_sec = NULL;
	char *ip_int = NULL, *ip_int_last = NULL;
	char *type, *ipv4addr = "", *ipv6addr = "", *proto, *inst;
	json_object *res, *jobj;
	struct ip_args curr_ip_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "type", &type);
		if (strcmp(type, "alias") == 0 || strcmp(section_name(p->config_section), "loopback")==0)
			continue;
		dmuci_get_value_by_section_string(p->config_section, "ipaddr", &ipv4addr);
		if (ipv4addr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(p->config_section), String}}, 1, &res);
			if (res)
			{
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipv4addr = dmjson_get_value(jobj, 1, "address");
			}
		}
		dmuci_get_value_by_section_string(p->config_section, "ip6addr", &ipv6addr);
		if (ipv6addr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(p->config_section), String}}, 1, &res);
			if (res)
			{
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-address");
				ipv6addr = dmjson_get_value(jobj, 1, "address");
			}
		}
		dmuci_get_value_by_section_string(p->config_section, "proto", &proto);
		dmuci_get_value_by_section_string(p->config_section, "ip_int_instance", &inst);
		if (ipv4addr[0] == '\0' && ipv6addr[0] == '\0' && strcmp(proto, "dhcp") != 0 && strcmp(proto, "dhcpv6") != 0 && strcmp(proto, "relay") != 0 && strcmp(inst, "") == 0 && strcmp(type, "bridge") != 0) {
			continue;
		}
		init_ip_args(&curr_ip_args, p->config_section, ipv4addr, ipv6addr);
		ip_int = handle_update_instance(1, dmctx, &ip_int_last, update_instance_alias, 3, p->dmmap_section, "ip_int_instance", "ip_int_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ip_args, ip_int) == DM_STOP)
			break;
	}
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

int browseIfaceIPv6Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *ipv6_inst = NULL, *ipv6_inst_last = NULL;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ip_args *)prev_data)->ip_sec), &dmmap_section);
	if (((struct ip_args *)prev_data)->ip_6address[0] != '\0') {
		ipv6_inst = handle_update_instance(2, dmctx, &ipv6_inst_last, update_instance_alias, 3, dmmap_section, "ipv6_instance", "ipv6_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, prev_data, ipv6_inst) == DM_STOP)
			goto end;
	}
end:
	return 0;
}

int browseIPDiagnosticsTraceRouteRouteHopsInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *idx_last = NULL;
	int id = 0;

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
	int id = 0;

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
	int id = 0;

	uci_foreach_sections_state("cwmp", "UploadPerConnection", s)
	{
		instance = handle_update_instance(2, dmctx, &idx_last, update_instance_alias, 3, (void *)s, "perconnection_instance", "perconnection_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;
	}
	return 0;
}
