/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *      Copyright (C) 2019 iopsys Software Solutions AB
 *      Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#ifndef __ETHERNET_H
#define __ETHERNET_H

struct eth_port_args
{
	struct uci_section *eth_port_sec;
	char *ifname;
};

extern DMOBJ tEthernetObj[];
extern DMLEAF tEthernetParams[];
extern DMOBJ tEthernetInterfaceObj[];
extern DMLEAF tEthernetInterfaceParams[];
extern DMLEAF tEthernetInterfaceStatsParams[];
extern DMOBJ tEthernetLinkObj[];
extern DMLEAF tEthernetLinkParams[];
extern DMLEAF tEthernetLinkStatsParams[];
extern DMOBJ tEthernetVLANTerminationObj[];
extern DMLEAF tEthernetVLANTerminationParams[];
extern DMLEAF tEthernetVLANTerminationStatsParams[];

int browseEthernetInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseEthernetLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseEthernetVLANTerminationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int addObjEthernetLink(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjEthernetLink(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjEthernetVLANTermination(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjEthernetVLANTermination(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_linker_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_linker_link(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_linker_vlan_term(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int get_Ethernet_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Ethernet_LinkNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_Ethernet_VLANTerminationNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_EthernetInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetInterface_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetInterface_CurrentBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterface_DuplexMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetInterface_DuplexMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetInterface_EEECapability(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_EthernetLink_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetLink_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetLink_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLink_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetLink_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetLink_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLink_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLink_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetLink_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetLink_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetLinkStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_EthernetVLANTermination_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetVLANTermination_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetVLANTermination_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTermination_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetVLANTermination_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetVLANTermination_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTermination_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTermination_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetVLANTermination_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetVLANTermination_VLANID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetVLANTermination_VLANID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetVLANTermination_TPID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_EthernetVLANTermination_TPID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_EthernetVLANTerminationStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_EthernetVLANTerminationStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif //__ETHERNET_H
