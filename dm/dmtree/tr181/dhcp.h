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
#ifndef __DHCP_H
#define __DHCP_H

#include <json-c/json.h>

struct dhcp_args
{
	struct uci_section *dhcp_sec;
	char *interface;
};

struct dhcp_static_args
{
	struct uci_section *dhcpsection;
};

struct client_args
{
	json_object *client;
	char *key;
};

struct dhcp_client_ipv4address_args {
	char *mac;
	char *ip;
	unsigned int leasetime;
};

struct dhcp_client_args {
	struct uci_section *dhcp_client_conf;
	struct uci_section *dhcp_client_dm;
	struct uci_section *macclassifier;
	struct uci_section *vendorclassidclassifier;
	struct uci_section *userclassclassifier;
	char *ip;
	char *mask;
};

struct dhcp_client_option_args {
	struct uci_section *opt_sect;
	struct uci_section *client_sect;
	char *option_tag;
	char *value;
};

extern DMOBJ tDhcpv4Obj[];
extern DMOBJ tDhcpv4ServerObj[];
extern DMOBJ tDhcpServerPoolObj[];
extern DMOBJ tDhcpServerPoolClientObj[];
extern DMLEAF tDhcpServerPoolParams[];
extern DMLEAF tDhcpServerPoolAddressParams[];
extern DMLEAF tDhcpServerPoolClientParams[];
extern DMLEAF tDhcpServerPoolClientIPv4AddressParams[];

extern DMLEAF tDHCPv4Params[];
extern DMOBJ tDHCPv4ClientObj[];
extern DMLEAF tDHCPv4ClientParams[];
extern DMLEAF tDHCPv4ClientSentOptionParams[];
extern DMLEAF tDHCPv4ClientReqOptionParams[];
extern DMOBJ tDHCPv4ServerObj[];
extern DMLEAF tDHCPv4ServerParams[];
extern DMLEAF tDHCPv4ServerPoolOptionParams[];
extern DMLEAF tDHCPv4ServerPoolClientIPv4AddressParams[];
extern DMLEAF tDHCPv4ServerPoolClientOptionParams[];
extern DMOBJ tDHCPv4RelayObj[];
extern DMLEAF tDHCPv4RelayParams[];
extern DMLEAF tDHCPv4RelayForwardingParams[];

int browseDhcpInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcpStaticInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcpClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDhcpClientIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4ClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4ClientSentOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4ClientReqOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4ServerPoolOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4ServerPoolClientOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDHCPv4RelayForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv4Client(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv4Client(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv4ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv4ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv4ClientReqOption(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv4ClientReqOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv4ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv4ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int addObjDHCPv4RelayForwarding(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delObjDHCPv4RelayForwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_server_pool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interval_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interval_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_client_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_client_ipv4address_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_client_ipv4address_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_sever_pool_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_static_address_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_option_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_clients_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_dhcp_client_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_server_pool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_dhcp_sever_pool_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int get_dhcp_client_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int set_section_order(char *package, char *dmpackage, char* sect_type, struct uci_section *dmmap_sect, struct uci_section *conf, int set_force, char* order);

int get_DHCPv4_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_DHCPStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_SubnetMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_IPRouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_DNSServers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_LeaseTimeRemaining(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_DHCPServer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_PassthroughEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_PassthroughEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_PassthroughDHCPPool(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Client_PassthroughDHCPPool(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Client_SentOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Client_ReqOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientReqOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientReqOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientReqOption_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientReqOption_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientReqOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientReqOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientReqOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ClientReqOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ClientReqOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Server_PoolNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ServerPoolClientOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4RelayForwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4RelayForwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_VendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_VendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_ClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_ClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_ClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_ClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_Chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_Chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_ChaddrMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_ChaddrMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_ChaddrExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_ChaddrExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_LocallyServed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_LocallyServed(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4RelayForwarding_DHCPServerIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4RelayForwarding_DHCPServerIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4ServerPoolClientOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Relay_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DHCPv4Relay_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DHCPv4Relay_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DHCPv4Relay_ForwardingNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
#endif
