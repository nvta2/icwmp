/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#ifndef __DHCPV6_H
#define __DHCPV6_H

extern DMOBJ tDHCPv6Obj[];
extern DMLEAF tDHCPv6Params[];
extern DMOBJ tDHCPv6ClientObj[];
extern DMLEAF tDHCPv6ClientParams[];
extern DMLEAF tDHCPv6ClientServerParams[];
extern DMLEAF tDHCPv6ClientSentOptionParams[];
extern DMLEAF tDHCPv6ClientReceivedOptionParams[];
extern DMOBJ tDHCPv6ServerObj[];
extern DMLEAF tDHCPv6ServerParams[];
extern DMOBJ tDHCPv6ServerPoolObj[];
extern DMLEAF tDHCPv6ServerPoolParams[];
extern DMOBJ tDHCPv6ServerPoolClientObj[];
extern DMLEAF tDHCPv6ServerPoolClientParams[];
extern DMLEAF tDHCPv6ServerPoolClientIPv6AddressParams[];
extern DMLEAF tDHCPv6ServerPoolClientIPv6PrefixParams[];
extern DMLEAF tDHCPv6ServerPoolClientOptionParams[];
extern DMLEAF tDHCPv6ServerPoolOptionParams[];

struct dhcpv6_client_args {
	struct uci_section *dhcp_client_conf;
	struct uci_section *dhcp_client_dm;
	struct uci_section *macclassifier;
	struct uci_section *vendorclassidclassifier;
	struct uci_section *userclassclassifier;
	char *ip;
};

int browseDHCPv6ClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ClientServerInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ClientSentOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ClientReceivedOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolClientIPv6AddressInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolClientIPv6PrefixInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv6ServerPoolClientOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int addObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv6ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv6ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv6ServerPool(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv6ServerPool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_DHCPv6_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_RapidCommit(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_RapidCommit(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_SuggestedT1(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_SuggestedT1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_SuggestedT2(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_SuggestedT2(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_SupportedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_RequestedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Client_RequestedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Client_ServerNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_SentOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Client_ReceivedOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientServer_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientServer_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientServer_InformationRefreshTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ClientReceivedOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientReceivedOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ClientReceivedOption_Server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6Server_PoolNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_DUIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_DUIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_SourceAddressMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_SourceAddressMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_SourceAddressExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_SourceAddressExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_IANAEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_IANAEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_IANAManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_IANAManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_IANAPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPool_IAPDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_IAPDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_IAPDManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_IAPDManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_IAPDPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPool_IAPDAddLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPool_IAPDAddLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPool_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPool_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClient_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolClient_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPoolClient_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClient_Active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClient_IPv6AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClient_IPv6PrefixNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClient_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolClientOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv6ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv6ServerPoolOption_PassthroughClient(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv6ServerPoolOption_PassthroughClient(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif //__DHCPV6_H

