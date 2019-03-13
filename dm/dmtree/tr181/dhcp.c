/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "dhcp.h"
#include "dmjson.h"
#define DELIMITOR ","

/*** DHCPv4. ***/
DMOBJ tDhcpv4Obj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Client", &DMWRITE, addObjDHCPv4Client, delObjDHCPv4Client, NULL, browseDHCPv4ClientInst, NULL, NULL, tDHCPv4ClientObj, tDHCPv4ClientParams, NULL},
{"Server", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDhcpv4ServerObj, tDHCPv4ServerParams, NULL},
{"Relay", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDHCPv4RelayObj, tDHCPv4RelayParams, NULL},
{0}
};

DMLEAF tDHCPv4Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ClientNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv4_ClientNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv4.Client.{i}. *** */
DMOBJ tDHCPv4ClientObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"SentOption", &DMWRITE, addObjDHCPv4ClientSentOption, delObjDHCPv4ClientSentOption, NULL, browseDHCPv4ClientSentOptionInst, NULL, NULL, NULL, tDHCPv4ClientSentOptionParams, NULL},
{"ReqOption", &DMWRITE, addObjDHCPv4ClientReqOption, delObjDHCPv4ClientReqOption, NULL, browseDHCPv4ClientReqOptionInst, NULL, NULL, NULL, tDHCPv4ClientReqOptionParams, NULL},
{0}
};

DMLEAF tDHCPv4ClientParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4Client_Enable, set_DHCPv4Client_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv4Client_Alias, set_DHCPv4Client_Alias, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_DHCPv4Client_Interface, set_DHCPv4Client_Interface, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DHCPv4Client_Status, NULL, NULL, NULL},
{"DHCPStatus", &DMREAD, DMT_STRING, get_DHCPv4Client_DHCPStatus, NULL, NULL, NULL},
{"Renew", &DMWRITE, DMT_BOOL, get_DHCPv4Client_Renew, set_DHCPv4Client_Renew, NULL, NULL},
{"IPAddress", &DMREAD, DMT_STRING, get_DHCPv4Client_IPAddress, NULL, NULL, NULL},
{"SubnetMask", &DMREAD, DMT_STRING, get_DHCPv4Client_SubnetMask, NULL, NULL, NULL},
{"IPRouters", &DMREAD, DMT_STRING, get_DHCPv4Client_IPRouters, NULL, NULL, NULL},
{"DNSServers", &DMREAD, DMT_STRING, get_DHCPv4Client_DNSServers, NULL, NULL, NULL},
{"LeaseTimeRemaining", &DMREAD, DMT_INT, get_DHCPv4Client_LeaseTimeRemaining, NULL, NULL, NULL},
{"DHCPServer", &DMREAD, DMT_STRING, get_DHCPv4Client_DHCPServer, NULL, NULL, NULL},
{"PassthroughEnable", &DMWRITE, DMT_BOOL, get_DHCPv4Client_PassthroughEnable, set_DHCPv4Client_PassthroughEnable, NULL, NULL},
{"PassthroughDHCPPool", &DMWRITE, DMT_STRING, get_DHCPv4Client_PassthroughDHCPPool, set_DHCPv4Client_PassthroughDHCPPool, NULL, NULL},
{"SentOptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv4Client_SentOptionNumberOfEntries, NULL, NULL, NULL},
{"ReqOptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv4Client_ReqOptionNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv4.Client.{i}.SentOption.{i}. *** */
DMLEAF tDHCPv4ClientSentOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4ClientSentOption_Enable, set_DHCPv4ClientSentOption_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv4ClientSentOption_Alias, set_DHCPv4ClientSentOption_Alias, NULL, NULL},
{"Tag", &DMWRITE, DMT_UNINT, get_DHCPv4ClientSentOption_Tag, set_DHCPv4ClientSentOption_Tag, NULL, NULL},
{"Value", &DMWRITE, DMT_HEXBIN, get_DHCPv4ClientSentOption_Value, set_DHCPv4ClientSentOption_Value, NULL, NULL},
{0}
};

/* *** Device.DHCPv4.Client.{i}.ReqOption.{i}. *** */
DMLEAF tDHCPv4ClientReqOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4ClientReqOption_Enable, set_DHCPv4ClientReqOption_Enable, NULL, NULL},
{"Order", &DMWRITE, DMT_UNINT, get_DHCPv4ClientReqOption_Order, set_DHCPv4ClientReqOption_Order, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv4ClientReqOption_Alias, set_DHCPv4ClientReqOption_Alias, NULL, NULL},
{"Tag", &DMWRITE, DMT_UNINT, get_DHCPv4ClientReqOption_Tag, set_DHCPv4ClientReqOption_Tag, NULL, NULL},
{"Value", &DMREAD, DMT_HEXBIN, get_DHCPv4ClientReqOption_Value, NULL, NULL, NULL},
{0}
};

DMLEAF tDHCPv4ServerParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4Server_Enable, set_DHCPv4Server_Enable, NULL, NULL},
{"PoolNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv4Server_PoolNumberOfEntries, NULL, NULL, NULL},
{0}
};


/*** DHCPv4.Server. ***/
DMOBJ tDhcpv4ServerObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Pool", &DMWRITE, add_dhcp_server, delete_dhcp_server, NULL, browseDhcpInst, NULL, NULL, tDhcpServerPoolObj, tDhcpServerPoolParams, NULL},
{0}
};


/*** DHCPv4.Server.Pool.{i}. ***/
DMOBJ tDhcpServerPoolObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"StaticAddress", &DMWRITE, add_dhcp_staticaddress, delete_dhcp_staticaddress, NULL, browseDhcpStaticInst, NULL, NULL, NULL, tDhcpServerPoolAddressParams, NULL},
{"Option", &DMWRITE, addObjDHCPv4ServerPoolOption, delObjDHCPv4ServerPoolOption, NULL, browseDHCPv4ServerPoolOptionInst, NULL, NULL, NULL, tDHCPv4ServerPoolOptionParams, NULL},
{"Client", &DMREAD, NULL, NULL, NULL, browseDhcpClientInst, NULL, NULL, tDhcpServerPoolClientObj, tDhcpServerPoolClientParams, get_dhcp_client_linker},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}. ***/
DMOBJ tDhcpServerPoolClientObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"IPv4Address", &DMREAD, NULL, NULL, NULL, browseDhcpClientIPv4Inst, NULL, NULL, NULL, tDhcpServerPoolClientIPv4AddressParams, NULL},
{"Option", &DMREAD, NULL, NULL, NULL, browseDHCPv4ServerPoolClientOptionInst, NULL, NULL, NULL, tDHCPv4ServerPoolClientOptionParams, NULL},
{0}
};

DMLEAF tDhcpServerPoolParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING,  get_server_pool_alias, set_server_pool_alias, NULL, NULL},
{"DNSServers", &DMWRITE, DMT_STRING,  get_dns_server, set_dns_server, NULL, NULL},
{"Status", &DMREAD, DMT_STRING,  get_dhcp_status, NULL, NULL, NULL},
{"Order", &DMWRITE, DMT_UNINT, get_dhcp_sever_pool_order, set_dhcp_sever_pool_order, NULL, NULL},
{CUSTOM_PREFIX"DHCPServerConfigurable", &DMWRITE, DMT_BOOL, get_dhcp_configurable, set_dhcp_configurable, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL,  get_dhcp_enable, set_dhcp_enable, NULL, NULL},
{"MinAddress", &DMWRITE, DMT_STRING, get_dhcp_interval_address_min, set_dhcp_address_min, NULL, NULL},
{"MaxAddress", &DMWRITE, DMT_STRING,get_dhcp_interval_address_max, set_dhcp_address_max, NULL, NULL},
{"ReservedAddresses", &DMWRITE, DMT_STRING, get_dhcp_reserved_addresses, set_dhcp_reserved_addresses, NULL, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING,get_dhcp_subnetmask, set_dhcp_subnetmask, NULL, NULL},
{"IPRouters", &DMWRITE, DMT_STRING, get_dhcp_iprouters, set_dhcp_iprouters, NULL, NULL},
{"LeaseTime", &DMWRITE, DMT_INT, get_dhcp_leasetime, set_dhcp_leasetime, NULL, NULL},
{"DomainName", &DMWRITE, DMT_STRING, get_dhcp_domainname, set_dhcp_domainname, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_dhcp_interface, set_dhcp_interface_linker_parameter, NULL, NULL},
{"StaticAddressNumberOfEntries", &DMWRITE, DMT_UNINT, get_static_address_number_of_entries, NULL, NULL, NULL},
{"OptionNumberOfEntries", &DMWRITE, DMT_UNINT, get_option_number_of_entries, NULL, NULL, NULL},
{"ClientNumberOfEntries", &DMWRITE, DMT_UNINT, get_clients_number_of_entries, NULL, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.StaticAddress.{i}. ***/
DMLEAF tDhcpServerPoolAddressParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_dhcp_static_alias, set_dhcp_static_alias, NULL, NULL},
{"Chaddr", &DMWRITE, DMT_STRING,  get_dhcp_staticaddress_chaddr, set_dhcp_staticaddress_chaddr, NULL, NULL},
{"Yiaddr", &DMWRITE, DMT_STRING,  get_dhcp_staticaddress_yiaddr, set_dhcp_staticaddress_yiaddr, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}. ***/
DMLEAF tDhcpServerPoolClientParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Chaddr", &DMREAD, DMT_STRING,  get_dhcp_client_chaddr, NULL, NULL, NULL},
{"Active", &DMREAD, DMT_STRING,  get_dhcp_client_active, NULL, NULL, NULL},
{0}
};

/*** DHCPv4.Server.Pool.{i}.Client.{i}.IPv4Address.{i}. ***/
DMLEAF tDhcpServerPoolClientIPv4AddressParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"LeaseTimeRemaining", &DMREAD, DMT_TIME,  get_dhcp_client_ipv4address_leasetime, NULL, NULL, NULL},
{"IPAddress", &DMREAD, DMT_TIME,  get_dhcp_client_ipv4address_ip_address, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv4.Server.Pool.{i}.Option.{i}. *** */
DMLEAF tDHCPv4ServerPoolOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4ServerPoolOption_Enable, set_DHCPv4ServerPoolOption_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv4ServerPoolOption_Alias, set_DHCPv4ServerPoolOption_Alias, NULL, NULL},
{"Tag", &DMWRITE, DMT_UNINT, get_DHCPv4ServerPoolOption_Tag, set_DHCPv4ServerPoolOption_Tag, NULL, NULL},
{"Value", &DMWRITE, DMT_HEXBIN, get_DHCPv4ServerPoolOption_Value, set_DHCPv4ServerPoolOption_Value, NULL, NULL},
{0}
};

/* *** Device.DHCPv4.Server.Pool.{i}.Client.{i}.Option.{i}. *** */
DMLEAF tDHCPv4ServerPoolClientOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Tag", &DMREAD, DMT_UNINT, get_DHCPv4ServerPoolClientOption_Tag, NULL, NULL, NULL},
{"Value", &DMREAD, DMT_HEXBIN, get_DHCPv4ServerPoolClientOption_Value, NULL, NULL, NULL},
{0}
};


/* *** Device.DHCPv4.Relay. *** */
DMOBJ tDHCPv4RelayObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Forwarding", &DMWRITE, addObjDHCPv4RelayForwarding, delObjDHCPv4RelayForwarding, NULL, browseDHCPv4RelayForwardingInst, NULL, NULL, NULL, tDHCPv4RelayForwardingParams, NULL},
{0}
};

DMLEAF tDHCPv4RelayParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4Relay_Enable, set_DHCPv4Relay_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DHCPv4Relay_Status, NULL, NULL, NULL},
{"ForwardingNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv4Relay_ForwardingNumberOfEntries, NULL, NULL, NULL},
{0}
};
/* *** Device.DHCPv4.Relay.Forwarding.{i}. *** */
DMLEAF tDHCPv4RelayForwardingParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_Enable, set_DHCPv4RelayForwarding_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DHCPv4RelayForwarding_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_Alias, set_DHCPv4RelayForwarding_Alias, NULL, NULL},
{"Order", &DMWRITE, DMT_UNINT, get_DHCPv4RelayForwarding_Order, set_DHCPv4RelayForwarding_Order, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_Interface, set_DHCPv4RelayForwarding_Interface, NULL, NULL},
{"VendorClassID", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_VendorClassID, set_DHCPv4RelayForwarding_VendorClassID, NULL, NULL},
{"VendorClassIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_VendorClassIDExclude, set_DHCPv4RelayForwarding_VendorClassIDExclude, NULL, NULL},
{"VendorClassIDMode", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_VendorClassIDMode, set_DHCPv4RelayForwarding_VendorClassIDMode, NULL, NULL},
{"ClientID", &DMWRITE, DMT_HEXBIN, get_DHCPv4RelayForwarding_ClientID, set_DHCPv4RelayForwarding_ClientID, NULL, NULL},
{"ClientIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_ClientIDExclude, set_DHCPv4RelayForwarding_ClientIDExclude, NULL, NULL},
{"UserClassID", &DMWRITE, DMT_HEXBIN, get_DHCPv4RelayForwarding_UserClassID, set_DHCPv4RelayForwarding_UserClassID, NULL, NULL},
{"UserClassIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_UserClassIDExclude, set_DHCPv4RelayForwarding_UserClassIDExclude, NULL, NULL},
{"Chaddr", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_Chaddr, set_DHCPv4RelayForwarding_Chaddr, NULL, NULL},
{"ChaddrMask", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_ChaddrMask, set_DHCPv4RelayForwarding_ChaddrMask, NULL, NULL},
{"ChaddrExclude", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_ChaddrExclude, set_DHCPv4RelayForwarding_ChaddrExclude, NULL, NULL},
{"LocallyServed", &DMWRITE, DMT_BOOL, get_DHCPv4RelayForwarding_LocallyServed, set_DHCPv4RelayForwarding_LocallyServed, NULL, NULL},
{"DHCPServerIPAddress", &DMWRITE, DMT_STRING, get_DHCPv4RelayForwarding_DHCPServerIPAddress, set_DHCPv4RelayForwarding_DHCPServerIPAddress, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_dhcp_client_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (data && ((struct client_args *)data)->key) {
		*linker = ((struct client_args *)data)->key;
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/*************************************************************
 * INIT
/*************************************************************/
inline int init_dhcp_args(struct dhcp_args *args, struct uci_section *s, char *interface)
{
	args->interface = interface;
	args->dhcp_sec = s;
	return 0;
}
inline int init_args_dhcp_host(struct dhcp_static_args *args, struct uci_section *s)
{
	args->dhcpsection = s;
	return 0;
}

inline int init_dhcp_client_args(struct client_args *args, json_object *client, char *key)
{
	args->client = client;
	args->key = key;
	return 0;
}
/*************************************************************
 * Other functions
/*************************************************************/
/*int assign_dhcp_server_pool_order(struct uci_section *dmmap_sect, char* instance) {
	struct uci_section *s;
	char *v= NULL;
	bool is_exists= false;
	int i;

	dmuci_get_value_by_section_string(dmmap_sect, "order", &v);
	if(v != NULL && strlen(v)>0)
		return 0;

	i= atoi(instance);
	while(!test){
		if(test)
	}
	uci_path_foreach_option_eq(icwmpd, "dmmap_dhcp", "dhcp", "order", instance, s){
		test=true;
	}
}*/
/*******************ADD-DEL OBJECT*********************/
int add_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL, *dmmap_dhcp= NULL;
	
	check_create_dmmap_package("dmmap_dhcp");
	instance = get_last_instance_icwmpd("dmmap_dhcp", "dhcp", "dhcp_instance");
	dmuci_add_section("dhcp", "dhcp", &s, &value);
	dmuci_set_value_by_section(s, "start", "100");
	dmuci_set_value_by_section(s, "leasetime", "12h");
	dmuci_set_value_by_section(s, "limit", "150");

	dmuci_add_section_icwmpd("dmmap_dhcp", "dhcp", &dmmap_dhcp, &v);
	dmuci_set_value_by_section(dmmap_dhcp, "section_name", section_name(s));
	*instancepara = update_instance_icwmpd(dmmap_dhcp, instance, "dhcp_instance");
	return 0;
}

int delete_dhcp_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_section= NULL;

	switch (del_action) {
	case DEL_INST:
		if(is_section_unnamed(section_name(((struct dhcp_args *)data)->dhcp_sec))){
			LIST_HEAD(dup_list);
			delete_sections_save_next_sections("dmmap_dhcp", "dhcp", "dhcp_instance", section_name(((struct dhcp_args *)data)->dhcp_sec), atoi(instance), &dup_list);
			update_dmmap_sections(&dup_list, "dhcp_instance", "dmmap_dhcp", "dhcp");
			dmuci_delete_by_section_unnamed(((struct dhcp_args *)data)->dhcp_sec, NULL, NULL);
		} else {
			get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(((struct dhcp_args *)data)->dhcp_sec), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(((struct dhcp_args *)data)->dhcp_sec, NULL, NULL);
		}

		break;
	case DEL_ALL:
		uci_foreach_sections("dhcp", "dhcp", s) {
			if (found != 0){
				get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(s), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			ss = s;
			found++;
		}
		if (ss != NULL){
			get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(ss), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(ss, NULL, NULL);
		}
		break;
	}
	return 0;
}

int add_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL, *dmmap_dhcp_host= NULL;
	
	check_create_dmmap_package("dmmap_dhcp");
	instance = get_last_instance_lev2_icwmpd("dhcp", "host", "dmmap_dhcp", "ldhcpinstance", "dhcp", ((struct dhcp_args *)data)->interface);
	dmuci_add_section("dhcp", "host", &s, &value);
	dmuci_set_value_by_section(s, "dhcp", ((struct dhcp_args *)data)->interface);


	dmuci_add_section_icwmpd("dmmap_dhcp", "host", &dmmap_dhcp_host, &v);
	dmuci_set_value_by_section(dmmap_dhcp_host, "section_name", section_name(s));
	*instancepara = update_instance_icwmpd(dmmap_dhcp_host, instance, "ldhcpinstance");
	return 0;
}

int delete_dhcp_staticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL, *dmmap_section = NULL;
	struct uci_section *ss = NULL;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name(dhcpargs->dhcpsection))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_dhcp", "host", "ldhcpinstance", section_name(dhcpargs->dhcpsection), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "ldhcpinstance", "dmmap_dhcp", "host");
				dmuci_delete_by_section_unnamed(dhcpargs->dhcpsection, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(dhcpargs->dhcpsection), &dmmap_section);
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(dhcpargs->dhcpsection, NULL, NULL);
			}

			break;
		case DEL_ALL:
			uci_foreach_option_eq("dhcp", "host", "dhcp", ((struct dhcp_args *)data)->interface, s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(ss), &dmmap_section);
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(ss), &dmmap_section);
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjDHCPv4Client(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_sect;
	char *wan_eth, *value, *wanname, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcp_client");
	instancepara = get_last_instance_icwmpd("dmmap_dhcp_client", "interface", "cwmp_dhcpv4client_instance");
	dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_eth);
	dmasprintf(&wanname, "%s.1", wan_eth);
	dmuci_add_section("network", "interface", &s, &value);
	dmuci_set_value_by_section(s, "proto", "dhcp");
	dmuci_set_value_by_section(s, "ifname", wanname);
	dmuci_set_value_by_section(s, "type", "anywan");
	dmuci_add_section_icwmpd("dmmap_dhcp_client", "interface", &dmmap_sect, &v);
	dmuci_set_value_by_section(dmmap_sect, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv4client_instance");
	return 0;
}

int delObjDHCPv4Client(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *s, *dmmap_section, *stmp;
	json_object *res, *jobj;

	char *v;
	char *type, *ipv4addr = "", *ipv6addr = "", *proto, *inst, *mask4;

	switch (del_action) {
		case DEL_INST:
			if(dhcp_client_args->dhcp_client_conf != NULL){
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "proto", "static");
				if(strlen(dhcp_client_args->ip) == 0){
					dmasprintf(&ipv4addr, "%s.%s.%s.%s", instance, instance, instance, instance);
					dmasprintf(&mask4, "%s", "255.255.255.0");
				} else {
					dmasprintf(&ipv4addr, "%s", dhcp_client_args->ip);
					dmasprintf(&mask4, "%s", dhcp_client_args->mask);
				}
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "ipaddr", ipv4addr);
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "netmask", mask4);
			}
			dmuci_delete_by_section_unnamed_icwmpd(dhcp_client_args->dhcp_client_dm, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("network", "interface", s) {
				dmuci_get_value_by_section_string(s, "type", &type);
				if (strcmp(type, "alias") == 0 || strcmp(section_name(s), "loopback")==0)
					continue;
				dmuci_get_value_by_section_string(s, "ipaddr", &ipv4addr);
				dmuci_get_value_by_section_string(s, "netmask", &mask4);
				if (ipv4addr[0] == '\0') {
					dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &res);
					if (res)
					{
						jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
						ipv4addr = dmjson_get_value(jobj, 1, "address");
						mask4= dmjson_get_value(jobj, 1, "mask");
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
				if (ipv4addr[0] == '\0' && ipv6addr[0] == '\0' && strcmp(proto, "dhcp") != 0 && strcmp(proto, "dhcpv6") != 0 && strcmp(type, "bridge") != 0)
					continue;

				dmuci_set_value_by_section(s, "proto", "static");

				get_dmmap_section_of_config_section("dmmap_dhcp_client", "interface", section_name(s), &dmmap_section);
				if(strlen(ipv4addr) == 0){
					if(dmmap_section != NULL)
						dmuci_get_value_by_section_string(dmmap_section, "cwmp_dhcpv4client_instance", &v);
					else
						dmasprintf(&v, "%d", 0);

					dmasprintf(&ipv4addr, "%s.%s.%s.%s", v, v, v, v);
					dmasprintf(&mask4, "%s", "255.255.255.0");
				}
				dmuci_set_value_by_section(s, "ipaddr", ipv4addr);
				dmuci_set_value_by_section(s, "netmask", mask4);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
			}
			uci_path_foreach_sections_safe(icwmpd, "dmmap_dhcp_client", "interface", stmp, s) {
				dmuci_delete_by_section_unnamed_icwmpd(s, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjDHCPv4ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*) data;
	struct uci_section *s, *dmmap_sect;
	char *value, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcp_client");
	instancepara= get_last_instance_lev2_icwmpd_dmmap_opt("dmmap_dhcp_client", "send_option", "cwmp_dhcpv4_sentopt_instance", "section_name", section_name(dhcp_client_args->dhcp_client_conf));
	dmuci_add_section_icwmpd("dmmap_dhcp_client", "send_option", &dmmap_sect, &value);
	if(dhcp_client_args->dhcp_client_conf != NULL)
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_client_args->dhcp_client_conf));
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", "0");
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv4_sentopt_instance");
	return 0;
}

int delObjDHCPv4ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s, *stmp;
	char *list= NULL, *opt_value= NULL;

	switch (del_action) {
		case DEL_INST:
			if(strcmp(((struct dhcp_client_option_args*) data)->option_tag, "0") != 0)
			{
				dmasprintf(&opt_value, "%s:%s", ((struct dhcp_client_option_args*) data)->option_tag, ((struct dhcp_client_option_args*) data)->value);
				dmuci_get_value_by_section_string(((struct dhcp_client_option_args*) data)->client_sect, "sendopts", &list);
				if(list != NULL){
					remove_elt_from_str_list(&list, opt_value);
					dmuci_set_value_by_section(((struct dhcp_client_option_args*) data)->client_sect, "sendopts", list);
				}
			}
			dmuci_delete_by_section_unnamed_icwmpd(((struct dhcp_client_option_args*) data)->opt_sect, NULL, NULL);
			break;
		case DEL_ALL:
			dmuci_set_value_by_section(((struct dhcp_client_args*) data)->dhcp_client_conf, "sendopts", "");
			uci_path_foreach_sections_safe(icwmpd, "dmmap_dhcp_client", "send_option", stmp, s) {
				dmuci_delete_by_section_unnamed_icwmpd(s, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjDHCPv4ClientReqOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*) data;
	struct uci_section *s, *dmmap_sect;
	char *value, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcp_client");
	instancepara= get_last_instance_lev2_icwmpd_dmmap_opt("dmmap_dhcp_client", "req_option", "cwmp_dhcpv4_sentopt_instance", "section_name", section_name(dhcp_client_args->dhcp_client_conf));
	dmuci_add_section_icwmpd("dmmap_dhcp_client", "req_option", &dmmap_sect, &value);
	if(dhcp_client_args->dhcp_client_conf != NULL)
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_client_args->dhcp_client_conf));
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", "0");
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv4_sentopt_instance");
	return 0;
}

int delObjDHCPv4ClientReqOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s, *stmp;
	char *list= NULL, *opt_value= NULL;

	switch (del_action) {
		case DEL_INST:
			if(strcmp(((struct dhcp_client_option_args*) data)->option_tag, "0") != 0)
			{
				dmuci_get_value_by_section_string(((struct dhcp_client_option_args*) data)->client_sect, "reqopts", &list);
				if(list != NULL){
					remove_elt_from_str_list(&list, ((struct dhcp_client_option_args*) data)->option_tag);
					dmuci_set_value_by_section(((struct dhcp_client_option_args*) data)->client_sect, "reqopts", list);
				}
			}
			dmuci_delete_by_section_unnamed_icwmpd(((struct dhcp_client_option_args*) data)->opt_sect, NULL, NULL);
			break;
		case DEL_ALL:
			dmuci_set_value_by_section(((struct dhcp_client_args*) data)->dhcp_client_conf, "reqopts", "");
			uci_path_foreach_sections_safe(icwmpd, "dmmap_dhcp_client", "req_option", stmp, s) {
				dmuci_delete_by_section_unnamed_icwmpd(s, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjDHCPv4ServerPool(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjDHCPv4ServerPool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	switch (del_action) {
		case DEL_INST:
			//TODO
			break;
		case DEL_ALL:
			//TODO
			break;
	}
	return 0;
}

int addObjDHCPv4ServerPoolStaticAddress(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjDHCPv4ServerPoolStaticAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	switch (del_action) {
		case DEL_INST:
			//TODO
			break;
		case DEL_ALL:
			//TODO
			break;
	}
	return 0;
}

int addObjDHCPv4ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct dhcp_args *dhcp_arg = (struct dhcp_args*)data;
	struct uci_section *s, *dmmap_sect;
	char *value, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcp");
	instancepara= get_last_instance_lev2_icwmpd_dmmap_opt("dmmap_dhcp", "servpool_option", "cwmp_dhcpv4_servpool_option_instance", "section_name", section_name(dhcp_arg->dhcp_sec));
	dmuci_add_section_icwmpd("dmmap_dhcp", "servpool_option", &dmmap_sect, &value);
	if(dhcp_arg->dhcp_sec != NULL)
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_arg->dhcp_sec));
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", "0");
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv4_servpool_option_instance");
	return 0;
}

int delObjDHCPv4ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s, *stmp;
	char *list= NULL, *opt_value= NULL;
	struct uci_list *dhcp_options_list = NULL;

	switch (del_action) {
		case DEL_INST:
			if(strcmp(((struct dhcp_client_option_args*) data)->option_tag, "0") != 0)
			{
				dmasprintf(&opt_value, "%s,%s", ((struct dhcp_client_option_args*) data)->option_tag, ((struct dhcp_client_option_args*) data)->value);
				dmuci_get_value_by_section_list(((struct dhcp_client_option_args*) data)->client_sect, "dhcp_option", &dhcp_options_list);
				if(dhcp_options_list != NULL){
					dmuci_del_list_value_by_section(((struct dhcp_client_option_args*) data)->client_sect, "dhcp_option", opt_value);
				}
			}
			dmuci_delete_by_section_unnamed_icwmpd(((struct dhcp_client_option_args*) data)->opt_sect, NULL, NULL);
			break;
		case DEL_ALL:
			dmuci_set_value_by_section(((struct dhcp_args*) data)->dhcp_sec, "dhcp_option", "");
			uci_path_foreach_sections_safe(icwmpd, "dmmap_dhcp", "servpool_option", stmp, s) {
				dmuci_delete_by_section_unnamed_icwmpd(s, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjDHCPv4RelayForwarding(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_sect;
	char *wan_eth, *value, *bridgerelay, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcp_relay");
	instancepara = get_last_instance_icwmpd("dmmap_dhcp_relay", "interface", "cwmp_dhcpv4relay_instance");
	dmuci_add_section("network", "interface", &s, &value);
	dmuci_set_value_by_section(s, "proto", "relay");
	dmuci_add_section_icwmpd("dmmap_dhcp_relay", "interface", &dmmap_sect, &v);
	dmuci_set_value_by_section(dmmap_sect, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv4relay_instance");
	return 0;
}

int delObjDHCPv4RelayForwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dhcp_client_args *dhcp_relay_args = (struct dhcp_client_args*)data;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_section;
	int found= 0;

	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name((struct uci_section *)data))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_dhcp_relay", "interface", "cwmp_dhcpv4relay_instance", section_name(dhcp_relay_args->dhcp_client_conf), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "cwmp_dhcpv4relay_instance", "dmmap_dhcp_relay", "interface");
				dmuci_delete_by_section_unnamed(dhcp_relay_args->dhcp_client_conf, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_dhcp_relay", "interface", section_name(dhcp_relay_args->dhcp_client_conf), &dmmap_section);
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(dhcp_relay_args->dhcp_client_conf, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("network", "interface", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_dhcp_relay", "interface", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_dhcp_relay", "interface", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}

/*************************************************************
 * GET & SET PARAM
/*************************************************************/
int get_server_pool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct dhcp_args *dhcp_arg= (struct dhcp_args*)data;
	struct uci_section *dmmap_sect;

	get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(dhcp_arg->dhcp_sec), &dmmap_sect);
	if (dmmap_sect)
		dmuci_get_value_by_section_string(dmmap_sect, "dhcp_alias", value);
	return 0;
}

int set_server_pool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct dhcp_args *dhcp_arg= (struct dhcp_args*)data;
	struct uci_section *dmmap_sect;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_dhcp", "dhcp", section_name(dhcp_arg->dhcp_sec), &dmmap_sect);
			if (dmmap_sect)
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "dhcp_alias", value);
	}
	return 0;
}

int get_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	int len;
	struct uci_section *s = NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
	if(res)
	{
		*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	}
	else
		*value = "";
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "dns", value);
		*value = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
		char *p = *value;
		while (*p) {
			if (*p == ' ' && p != *value && *(p-1) != ',')
				*p++ = ',';
			else
				p++;
		}
	}
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", value);
	}
	return 0;
}

int set_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *dup, *p;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dup = dmstrdup(value);
			p = dup;
			while (*p) {
				if (*p == ',')
					*p++ = ' ';
				else
					p++;
			}
			dmuci_set_value("network", ((struct dhcp_args *)data)->interface, "dns", dup);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		*value = "1";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_dhcp_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				if (!b) {
					dmuci_delete_by_section(s, NULL, NULL);
				}
				break;
			}
			if (s == NULL && b) {
				dmuci_set_value("dhcp",((struct dhcp_args *)data)->interface, NULL, "dhcp");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "interface", ((struct dhcp_args *)data)->interface);
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "start", "100");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "limit", "150");
				dmuci_set_value("dhcp", ((struct dhcp_args *)data)->interface, "leasetime", "12h");
			}
			return 0;
	}
	return 0;
}

int get_dhcp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	char *v= NULL;
	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", &v);
		*value = (v && *v == '1') ? "Disabled" : "Enabled";
		return 0;
	}
	*value="Error_Misconfigured";
	return 0;
}

int get_dhcp_sever_pool_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int set_dhcp_sever_pool_order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_static_address_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct uci_section *s;
	int i= 0;

	uci_foreach_sections("dhcp", "host", s){
		i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_option_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct dhcp_args *curr_dhcp_args = (struct dhcp_args*)data;
	struct uci_list *dhcp_options_list = NULL;
	struct uci_element *e;

	int i= 0;

	dmuci_get_value_by_section_list(curr_dhcp_args->dhcp_sec, "dhcp_option", &dhcp_options_list);
	if (dhcp_options_list != NULL) {
		uci_foreach_element(dhcp_options_list, e) {
			i++;
		}
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_clients_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res = NULL, *client_obj = NULL;
	int i= 0;
	char *dhcp;

	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
			json_object_object_foreach(res, key, client_obj) {
				dhcp = dmjson_get_value(client_obj, 1, "dhcp");
				if(strcmp(dhcp, "true") == 0)
					i++;
			}
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", value);
		if ((*value)[0] == '\0')
			*value = "1";
		else
			*value = "0";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_dhcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				if (b)
					dmuci_set_value_by_section(s, "ignore", "");
				else
					dmuci_set_value_by_section(s, "ignore", "1");
				break;
			}
			return 0;
	}
	return 0;
}

enum enum_lanip_interval_address {
	LANIP_INTERVAL_START,
	LANIP_INTERVAL_END
};

int get_dhcp_interval_address(struct dmctx *ctx, void *data, char *instance, char **value, int option)
{
	json_object *res, *jobj;
	char *ipaddr = "" , *mask = "", *start , *limit;
	struct uci_section *s = NULL;
	char bufipstart[16], bufipend[16];

	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "start", &start);
		if (option == LANIP_INTERVAL_END)
			dmuci_get_value_by_section_string(s, "limit", &limit);
		break;
	}
	if (s == NULL) {
		return 0;
	}
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
	if (ipaddr[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		if (res)
		{
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			ipaddr = dmjson_get_value(jobj, 1, "address");			
		}
	}
	if (ipaddr[0] == '\0') {
		return 0;
	}
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		if (res) {
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			mask = dmjson_get_value(jobj, 1, "mask");
			if (mask[0] == '\0') {
				return 0;
			}
			mask = cidr2netmask(atoi(mask));
		}
	}
	if (mask[0] == '\0') {
		mask = "255.255.255.0";
	}
	if (option == LANIP_INTERVAL_START) {
		ipcalc(ipaddr, mask, start, NULL, bufipstart, NULL);
		*value = dmstrdup(bufipstart); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	else {
		ipcalc(ipaddr, mask, start, limit, bufipstart, bufipend);
		*value = dmstrdup(bufipend); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}
int get_dhcp_interval_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	get_dhcp_interval_address(ctx, data, instance, value, LANIP_INTERVAL_START);
	return 0;
}

int get_dhcp_interval_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	get_dhcp_interval_address(ctx, data, instance, value, LANIP_INTERVAL_END);
	return 0;
}

int set_dhcp_address_min(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start , *limit, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");					
				}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					mask = dmjson_get_value(jobj, 1, "mask");
					if (mask[0] == '\0')
						return 0;
					mask = cidr2netmask(atoi(mask));
				}
			}
			if (mask[0] == '\0')
				mask = "255.255.255.0";

			ipcalc_rev_start(ipaddr, mask, value, buf);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_set_value_by_section(s, "start", buf);
				break;
			}

			return 0;
	}
	return 0;
}

int set_dhcp_address_max(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int i_val;
	json_object *res, *jobj;
	char *ipaddr = "", *mask = "", *start, buf[16];
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_get_value_by_section_string(s, "start", &start);
				break;
			}
			if (!s) return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", &ipaddr);
			if (ipaddr[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					ipaddr = dmjson_get_value(jobj, 1, "address");									}
			}
			if (ipaddr[0] == '\0')
				return 0;

			dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", &mask);
			if (mask[0] == '\0') {
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
				if (res) {
					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
					mask = dmjson_get_value(jobj, 1, "mask");
					if (mask[0] == '\0')
						return 0;
					mask = cidr2netmask(atoi(mask));
				}
			}
			if (mask[0] == '\0')
				mask = "255.255.255.0";

			ipcalc_rev_end(ipaddr, mask, start, value, buf);
			dmuci_set_value_by_section(s, "limit", buf);
			return 0;
	}
	return 0;
}


int get_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char val[512] = {0}, *p;
	struct uci_section *s = NULL;
	char *min, *max, *ip, *s_n_ip;
	unsigned int n_min, n_max, n_ip;
	*value = "";

	get_dhcp_interval_address(ctx, data, instance, &min, LANIP_INTERVAL_START);
	get_dhcp_interval_address(ctx, data, instance, &max, LANIP_INTERVAL_END);
	if (min[0] == '\0' || max[0] == '\0')
		return 0;
	n_min = inet_network(min);
	n_max = inet_network(max);
	p = val;
	uci_foreach_sections("dhcp", "host", s) {
		dmuci_get_value_by_section_string(s, "ip", &ip);
		if (ip[0] == '\0')
			continue;
		n_ip = inet_network(ip);
		if (n_ip >= n_min && n_ip <= n_max) {
			if (val[0] != '\0')
				dmstrappendchr(p, ',');
			dmstrappendstr(p, ip);
		}
	}
	dmstrappendend(p);
	*value = dmstrdup(val); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	struct uci_section *dhcp_section = NULL;
	char *min, *max, *ip, *val, *local_value;
	char *pch, *spch;
	unsigned int n_min, n_max, n_ip;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dhcp_interval_address(ctx, data, instance, &min, LANIP_INTERVAL_START);
			get_dhcp_interval_address(ctx, data, instance, &max, LANIP_INTERVAL_END);
			n_min = inet_network(min);
			n_max = inet_network(max);
			local_value = dmstrdup(value);

			for (pch = strtok_r(local_value, ",", &spch);
				pch != NULL;
				pch = strtok_r(NULL, ",", &spch)) {
				uci_foreach_option_eq("dhcp", "host", "ip", pch, s) {
					continue;
				}
				n_ip = inet_network(pch);
				if (n_ip < n_min && n_ip > n_max)
					continue;
				else {
					dmuci_add_section_and_rename("dhcp", "host", &dhcp_section, &val);
					dmuci_set_value_by_section(dhcp_section, "dhcp", ((struct dhcp_args *)data)->interface);
					dmuci_set_value_by_section(dhcp_section, "ip", pch);
				}
			}
			dmfree(local_value);
			return 0;
	}
	return 0;
}

int get_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mask;
	json_object *res, *jobj;
	struct uci_section *s = NULL;
	char *val;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "netmask", value);
		break;
	}
	if (s == NULL || (*value)[0] == '\0')
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "netmask", value);
	if ((*value)[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ((struct dhcp_args *)data)->interface, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		mask = dmjson_get_value(jobj, 1, "mask");
		int i_mask = atoi(mask);
		val = cidr2netmask(i_mask);
		*value = dmstrdup(val);// MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_set_value_by_section(s, "netmask", value);
				return 0;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "gateway", value);
	if ((*value)[0] == '\0') {
		dmuci_get_option_value_string("network", ((struct dhcp_args *)data)->interface, "ipaddr", value);
	}
	return 0;
}

int set_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("network", ((struct dhcp_args *)data)->interface, "gateway", value);
			return 0;
	}
	return 0;
}

int get_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int len, mtime = 0;
	char *ltime = "", *pch, *spch, *ltime_ini, *tmp, *tmp_ini;
	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "leasetime", &ltime);
		break;
	}
	if (ltime[0] == '\0') {
		*value = "-1";
		return 0;
	}
	ltime = dmstrdup(ltime);
	ltime_ini = dmstrdup(ltime);
	tmp = ltime;
	tmp_ini = ltime_ini;
	pch = strtok_r(ltime, "h", &spch);
	if (strcmp(pch, ltime_ini) != 0) {
		mtime = 3600 * atoi(pch);
		if(spch[0] != '\0') {
			ltime += strlen(pch)+1;
			ltime_ini += strlen(pch)+1;
			pch = strtok_r(ltime, "m", &spch);
			if (strcmp(pch, ltime_ini) != 0) {
				mtime += 60 * atoi(pch);
				if(spch[0] !='\0') {
					ltime += strlen(pch)+1;
					ltime_ini += strlen(pch)+1;
					pch = strtok_r(ltime, "s", &spch);
					if (strcmp(pch, ltime_ini) != 0) {
						mtime += atoi(pch);
					}
				}
			} else {
				pch = strtok_r(ltime, "s", &spch);
	if (strcmp(pch, ltime_ini) != 0)
				mtime +=  atoi(pch);
			}
		}
	}
	else {
		pch = strtok_r(ltime, "m", &spch);
		if (strcmp(pch, ltime_ini) != 0) {
		mtime += 60 * atoi(pch);
			if(spch[0] !='\0') {
				ltime += strlen(pch)+1;
				ltime_ini += strlen(pch)+1;
				pch = strtok_r(ltime, "s", &spch);
				if (strcmp(pch, ltime_ini) != 0) {
					mtime += atoi(pch);
				}
			}
		} else {
			pch = strtok_r(ltime, "s", &spch);
			if (strcmp(pch, ltime_ini) != 0)
				mtime +=  atoi(pch);
		}
	}
	dmfree(tmp);
	dmfree(tmp_ini);

	dmasprintf(value, "%d", mtime); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	char buf[32];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				int val = atoi(value);
				sprintf(buf, "%ds", val);
				dmuci_set_value_by_section(s, "leasetime",  buf);
				break;
			}
			return 0;
	}
	return 0;
}

int get_dhcp_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	linker = dmstrdup(((struct dhcp_args *)data)->interface);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	dmfree(linker);
	return 0;
}

int set_dhcp_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(((struct dhcp_args *)data)->dhcp_sec, "interface", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *result, *str;
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *s = NULL;
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
		dmuci_get_value_by_section_list(s, "dhcp_option", &val);
		if (val) {
			uci_foreach_element(val, e)
			{
				if ((str = strstr(e->name, "15,"))) {
					*value = dmstrdup(str + sizeof("15,") - 1); //MEM WILL BE FREED IN DMMEMCLEAN
					return 0;
				}
			}
		}
	}
	return 0;
}

int set_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *result, *dn, *pch;
	struct uci_list *val;
	struct uci_section *s = NULL;
	struct uci_element *e = NULL, *tmp;
	char *option = "dhcp_option", buf[64];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcp_args *)data)->interface, s) {
				dmuci_get_value_by_section_list(s, option, &val);
				if (val) {
					uci_foreach_element_safe(val, e, tmp)
					{
						if (strstr(tmp->name, "15,")) {
							dmuci_del_list_value_by_section(s, "dhcp_option", tmp->name); //TODO test it
						}
					}
				}
				break;
			}
			goto end;
	}
end:
	sprintf(buf, "15,%s", value);
	dmuci_add_list_value_by_section(((struct dhcp_args *)data)->dhcp_sec, "dhcp_option", buf);
	return 0;
}

int get_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(((struct dhcp_static_args *)data)->dhcpsection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ldhcpalias", value);
	return 0;
}

int set_dhcp_static_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_dhcp", "host", section_name(((struct dhcp_static_args *)data)->dhcpsection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "ldhcpalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac", &chaddr);
	if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac_orig", value);
	else 
		*value = chaddr;
	return 0;
}

int set_dhcp_staticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	char *chaddr;
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
		
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "mac", &chaddr);
			if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
				dmuci_set_value_by_section(dhcpargs->dhcpsection, "mac_orig", value);
			else
				dmuci_set_value_by_section(dhcpargs->dhcpsection, "mac", value);
			return 0;
	}
	return 0;
}

int get_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	dmuci_get_value_by_section_string(dhcpargs->dhcpsection, "ip", value);
	return 0;
}

int set_dhcp_staticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_static_args *dhcpargs = (struct dhcp_static_args *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dhcpargs->dhcpsection, "ip", value);
			return 0;
	}
	return 0;
}

int get_dhcp_client_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmjson_get_value(((struct client_args *)data)->client, 1, "macaddr");
	return 0;
}

int get_dhcp_client_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmjson_get_value(((struct client_args *)data)->client, 1, "connected");
	return 0;
}

int get_dhcp_client_ipv4address_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char time_buf[26] = {0};
	struct tm *t_tm;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = *((struct dhcp_client_ipv4address_args*)data);

	*value = "0001-01-01T00:00:00Z";
	time_t t_time = current_dhcp_client_ipv4address_args.leasetime;
	t_tm = localtime(&t_time);
	if (t_tm == NULL)
		return 0;
	if(strftime(time_buf, sizeof(time_buf), "%FT%T%z", t_tm) == 0)
		return 0;

	time_buf[25] = time_buf[24];
	time_buf[24] = time_buf[23];
	time_buf[22] = ':';
	time_buf[26] = '\0';

	*value = dmstrdup(time_buf);
	return 0;
}

int get_dhcp_client_ipv4address_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = *((struct dhcp_client_ipv4address_args*)data);

	if(current_dhcp_client_ipv4address_args.ip != NULL && strlen(current_dhcp_client_ipv4address_args.ip)>0)
		*value= dmstrdup(current_dhcp_client_ipv4address_args.ip);
	else
		*value= "";
	return 0;
}

int get_DHCPv4_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s, *dmmap_sect;
	int nbre_confs= 0, nbre_dmmaps= 0;

	uci_foreach_option_eq("network", "interface", "proto", "dhcp", s) {
		nbre_confs++;
	}
	uci_path_foreach_sections(icwmpd, "dmmap_dhcp_client", "interface", dmmap_sect) {
		nbre_dmmaps++;
	}
	if(nbre_dmmaps ==0 || nbre_dmmaps < nbre_confs)
		dmasprintf(value, "%d", nbre_confs);
	else
		dmasprintf(value, "%d", nbre_dmmaps);
	return 0;
}

int get_DHCPv4Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *v= NULL;


	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "0";
		return 0;
	}

	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "disabled", &v);

	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "1";
	else
		*value= "0";

	return 0;
}

int set_DHCPv4Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "disabled", "0");
			else
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "disabled", "1");
			break;
	}
	return 0;
}

int get_DHCPv4Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *dmmap_section;
	char *v;

	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_dm, "cwmp_dhcpv4client_alias", value);

	return 0;
}

int set_DHCPv4Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *dmmap_section;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(dhcp_client_args->dhcp_client_dm, "cwmp_dhcpv4client_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv4Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}
	char *linker= dmstrdup(section_name(dhcp_client_args->dhcp_client_conf));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim, dm_delim), linker, value);

	return 0;
}

int set_DHCPv4Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *s;
	char *linker= NULL, *newvalue= NULL, *v;

	switch (action)	{
		case VALUECHECK:
			if(strlen(value) == 0 || strcmp(value, "") == 0)
				return FAULT_9007;

			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			uci_path_foreach_sections(icwmpd, "dmmap_dhcp_client", "interface", s) {
				dmuci_get_value_by_section_string(s, "section_name", &v);
				if(strcmp(v, linker) == 0)
					return FAULT_9007;
			}
			uci_foreach_sections("network", "interface", s) {
				if(strcmp(section_name(s), linker) == 0){
					dmuci_get_value_by_section_string(s, "proto", &v);
					if(strcmp(v, "dhcp") != 0)
						return FAULT_9007;
				}
			}
			break;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_args->dhcp_client_dm, "section_name", linker);
			break;
	}
	return 0;
}

int get_DHCPv4Client_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *v= NULL;
	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "Error_Misconfigured";

		return 0;
	}
	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "disabled", &v);
	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "Enabled";
	else
		*value= "Disabled";

	return 0;
}

int get_DHCPv4Client_DHCPStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *ipaddr;
	json_object *res, *jobj;

	if (dhcp_client_args->dhcp_client_conf == NULL)
		return 0;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcp_client_args->dhcp_client_conf), String}}, 1, &res);
	if (res) {
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		ipaddr = dmjson_get_value(jobj, 1, "address");
	}

	if (ipaddr[0] == '\0')
		*value = "Requesting";
	else
		*value = "Bound";

	return 0;
}

int get_DHCPv4Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	*value = "0";

	return 0;
}

int set_DHCPv4Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4Client_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;

	*value= dmstrdup(dhcp_client_args->ip);

	return 0;
}

int get_DHCPv4Client_SubnetMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;

	*value= dmstrdup(dhcp_client_args->mask);

	return 0;
}

int get_DHCPv4Client_IPRouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *v, buf[256] = "";
	json_object *jobj= NULL, *res;
	int i = 0;

	if (dhcp_client_args->dhcp_client_conf == NULL)
		return 0;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcp_client_args->dhcp_client_conf), String}}, 1, &res);

	while (1) {
		jobj = dmjson_select_obj_in_array_idx(res, i, 1, "route");
		i++;

		if (jobj == NULL)
			break;

		v = dmjson_get_value(jobj, 1, "target");
		if (*v == '\0')
			continue;
		if (strcmp(v, "0.0.0.0") == 0)
			continue;
		if (buf[0] != '\0') {
			strcat(buf, ",");
		} else
			strcat(buf, v);

	}

	*value = dmstrdup(buf);

	return 0;
}

int get_DHCPv4Client_DNSServers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	json_object *res;

	if (dhcp_client_args->dhcp_client_conf == NULL)
		return 0;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcp_client_args->dhcp_client_conf), String}}, 1, &res);

	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");

	return 0;
}

int get_DHCPv4Client_LeaseTimeRemaining(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv4Client_DHCPServer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv4Client_PassthroughEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	*value = "0";

	return 0;
}

int set_DHCPv4Client_PassthroughEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4Client_PassthroughDHCPPool(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	return 0;
}

int set_DHCPv4Client_PassthroughDHCPPool(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4Client_SentOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args* dhcp_client_args= (struct dhcp_client_args*)data;
	char *v= NULL, **sendopts;
	int length;

	if(dhcp_client_args->dhcp_client_conf != NULL)
		dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "sendopts", &v);

	if(v==NULL){
		*value= "0";
		return 0;
	}

	sendopts= strsplit(v, " ", &length);
	dmasprintf(value, "%d", length);

	return 0;
}

int get_DHCPv4Client_ReqOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args* dhcp_client_args= (struct dhcp_client_args*)data;
	char *v, **sendopts;
	int length;

	if(dhcp_client_args->dhcp_client_conf != NULL)
		dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "reqopts", &v);

	if(v==NULL){
		*value= "0";
		return 0;
	}

	sendopts= strsplit(v, " ", &length);

	dmasprintf(value, "%d", length);

	return 0;
}

int get_DHCPv4ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *v;
	char *opttagvalue= NULL;

	if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0){
		*value= "0";
		return 0;
	}

	dmasprintf(&opttagvalue, "%s:%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);
	dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "sendopts", &v);

	if(is_elt_exit_in_str_list(v, opttagvalue))
		*value = "1";
	else
		*value= "0";


	return 0;
}

int set_DHCPv4ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	bool b;
	char *v;
	char *opttagvalue= NULL;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "sendopts", &v);

			if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0)
				return 0;

			dmasprintf(&opttagvalue, "%s:%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);

			if (b) {
				if(!is_elt_exit_in_str_list(v, opttagvalue)){
					add_elt_to_str_list(&v, opttagvalue);
					dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "sendopts", v);
				}
			} else {
				remove_elt_from_str_list(&v, opttagvalue);
				dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "sendopts", v);
			}
			break;
	}
	return 0;
}

int get_DHCPv4ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	dmuci_get_value_by_section_string(dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_sentopt_alias", value);

	return 0;
}

int set_DHCPv4ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_sentopt_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv4ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->option_tag);

	return 0;
}

int set_DHCPv4ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *pch, *spch, *list, *v, *opttagvalue, **sendopts, *oldopttagvalue;
	int length;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "sendopts", &v);
			if(v==NULL)
				return 0;

			list= dmstrdup(v);
			for (pch = strtok_r(list, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
				sendopts= strsplit(pch, ":", &length);
				if(strcmp(sendopts[0], value) == 0)
					return FAULT_9007;
			}
			break;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "sendopts", &v);
			dmasprintf(&oldopttagvalue, "%s:%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);
			remove_elt_from_str_list(&v, oldopttagvalue);
			dmasprintf(&opttagvalue, "%s:%s", value, dhcp_client_opt_args->value);
			add_elt_to_str_list(&v, opttagvalue);
			dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "sendopts", v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "option_tag", value);
			break;
	}
	return 0;
}

int get_DHCPv4ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	*value= dmstrdup(dhcp_client_opt_args->value);

	return 0;
}

int set_DHCPv4ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *v, *opttagvalue, *oldopttagvalue;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "sendopts", &v);
			dmasprintf(&oldopttagvalue, "%s:%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);
			remove_elt_from_str_list(&v, oldopttagvalue);
			dmasprintf(&opttagvalue, "%s:%s", dhcp_client_opt_args->option_tag, value);
			add_elt_to_str_list(&v, opttagvalue);
			dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "sendopts", v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "option_value", value);
			break;
	}
	return 0;
}

int get_DHCPv4ClientReqOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *v;
	char *opttagvalue= NULL;

	if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0){
		*value= "0";
		return 0;
	}

	dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "reqopts", &v);

	if(is_elt_exit_in_str_list(v, dhcp_client_opt_args->option_tag))
		*value = "1";
	else
		*value= "0";

	return 0;
}

int set_DHCPv4ClientReqOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	bool b;
	char *v;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "reqopts", &v);

			if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0)
				return 0;

			if (b) {
				if(!is_elt_exit_in_str_list(v,  dhcp_client_opt_args->option_tag)){
					add_elt_to_str_list(&v,  dhcp_client_opt_args->option_tag);
					dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "reqopts", v);
				}
			} else {
				remove_elt_from_str_list(&v,  dhcp_client_opt_args->option_tag);
				dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "reqopts", v);
			}
			break;
	}
	return 0;
}

int get_DHCPv4ClientReqOption_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4ClientReqOption_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4ClientReqOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	dmuci_get_value_by_section_string(dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_reqtopt_alias", value);

	return 0;
}

int set_DHCPv4ClientReqOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_reqtopt_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv4ClientReqOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->option_tag);
	return 0;
}

int set_DHCPv4ClientReqOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	char *pch, *spch, *list, *v, *opttagvalue, **sendopts, *oldopttagvalue;
	int length;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "reqopts", &v);
			if(v==NULL)
				return 0;
			list= dmstrdup(v);
			for (pch = strtok_r(list, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
				if(strcmp(pch, value) == 0)
					return FAULT_9007;
			}
			break;
		case VALUESET:
			dmuci_get_value_by_section_string(dhcp_client_opt_args->client_sect, "reqopts", &v);
			remove_elt_from_str_list(&v, dhcp_client_opt_args->option_tag);
			add_elt_to_str_list(&v, value);
			dmuci_set_value_by_section(dhcp_client_opt_args->client_sect, "reqopts", v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "option_tag", value);
			break;
	}
	return 0;
}

int get_DHCPv4ClientReqOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv4Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_DHCPServerIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_DHCPServerIPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *v;
	char *opttagvalue= NULL;
	struct uci_list *dhcp_option_list;
	struct uci_element *e;
	char **buf;
	int length;

	if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0){
		*value= "0";
		return 0;
	}

	dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);
	if (dhcp_option_list != NULL) {
			uci_foreach_element(dhcp_option_list, e) {
				buf= strsplit(e->name, ",", &length);
				if(strcmp(buf[0], dhcp_client_opt_args->option_tag) == 0) {
					*value= "1";
					return 0;
				}
			}
	}

	*value= "0";
	return 0;
}

int set_DHCPv4ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	struct uci_list *dhcp_option_list;
	struct uci_element *e;
	char **buf, *opt_value;
	int length;
	bool test= false, b;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);

			if(strcmp(dhcp_client_opt_args->option_tag, "0") == 0)
				return 0;


			dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);
			dmasprintf(&opt_value, "%s,%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);

			if (dhcp_option_list != NULL) {
					uci_foreach_element(dhcp_option_list, e) {
						buf= strsplit(e->name, ",", &length);
						if(strcmp(buf[0], dhcp_client_opt_args->option_tag) == 0) {
							test= true;
							if(!b)
								dmuci_del_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opt_value);
							break;
						}
					}
			}
			if(!test && b)
				dmuci_add_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opt_value);

	}
	return 0;
}

int get_DHCPv4Server_PoolNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int i= 0;

	uci_foreach_sections("dhcp", "dhcp", s) {
		i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_DHCPv4ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;

	dmuci_get_value_by_section_string(dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_servpool_option_instance", value);
	return 0;
}

int set_DHCPv4ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "cwmp_dhcpv4_servpool_option_instance", value);
			break;
	}
	return 0;
}

int get_DHCPv4ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->option_tag);
	return 0;
}

int set_DHCPv4ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *opttagvalue, **option, *oldopttagvalue;
	int length;
	struct uci_list *dhcp_option_list= NULL;
	struct uci_element *e;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);

			if(dhcp_option_list==NULL)
				return 0;

			uci_foreach_element(dhcp_option_list, e) {
				option= strsplit(e->name, ",", &length);
				if (strcmp(option[0], value)==0)
					return FAULT_9007;
			}
			break;
		case VALUESET:
			dmasprintf(&oldopttagvalue, "%s,%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);
			dmasprintf(&opttagvalue, "%s,%s", value, dhcp_client_opt_args->value);
			dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);
			dmuci_del_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", oldopttagvalue);
			dmuci_add_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opttagvalue);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "option_tag", value);
			break;
	}
	return 0;
}

int get_DHCPv4ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->value);
	return 0;
}

int set_DHCPv4ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_option_args* dhcp_client_opt_args= (struct dhcp_client_option_args*)data;
	char *opttagvalue, **option, *oldopttagvalue;
	int length;
	struct uci_list *dhcp_option_list= NULL;
	struct uci_element *e;

	switch (action)	{
		case VALUECHECK:
			dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);

			if(dhcp_option_list==NULL)
				return 0;

			uci_foreach_element(dhcp_option_list, e) {
				option= strsplit(e->name, ",", &length);
				if (strcmp(option[0], value)==0)
					return FAULT_9007;
			}
			break;
		case VALUESET:
			dmasprintf(&oldopttagvalue, "%s,%s", dhcp_client_opt_args->option_tag, dhcp_client_opt_args->value);
			dmasprintf(&opttagvalue, "%s,%s", dhcp_client_opt_args->option_tag, value);
			dmuci_get_value_by_section_list(dhcp_client_opt_args->client_sect, "dhcp_option", &dhcp_option_list);
			dmuci_del_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", oldopttagvalue);
			dmuci_add_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opttagvalue);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "option_value", value);
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *v= NULL;


	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "0";
		return 0;
	}

	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "disabled", &v);

	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "1";
	else
		*value= "0";

	return 0;
}

int set_DHCPv4RelayForwarding_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "disabled", "0");
			else
				dmuci_set_value_by_section(dhcp_client_args->dhcp_client_conf, "disabled", "1");
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *dmmap_section;
	char *v;

	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_dm, "cwmp_dhcpv4relay_alias", value);

	return 0;
}

int set_DHCPv4RelayForwarding_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *dmmap_section;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(dhcp_client_args->dhcp_client_dm, "cwmp_dhcpv4relay_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}
	char *linker= dmstrdup(section_name(dhcp_client_args->dhcp_client_conf));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim, dm_delim), linker, value);

	return 0;
}

int set_DHCPv4RelayForwarding_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	struct uci_section *s;
	char *linker= NULL, *newvalue= NULL, *v;

	switch (action)	{
		case VALUECHECK:
			if(strlen(value) == 0 || strcmp(value, "") == 0)
				return FAULT_9007;

			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			uci_path_foreach_sections(icwmpd, "dmmap_dhcp_relay", "interface", s) {
				dmuci_get_value_by_section_string(s, "section_name", &v);
				if(strcmp(v, linker) == 0)
					return FAULT_9007;
			}
			uci_foreach_sections("network", "interface", s) {
				if(strcmp(section_name(s), linker) == 0){
					dmuci_get_value_by_section_string(s, "proto", &v);
					if(strcmp(v, "relay") != 0)
						return FAULT_9007;
				}
			}
			break;
		case VALUESET:
			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_args->dhcp_client_dm, "section_name", linker);
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "";
	return 0;
}

int set_DHCPv4RelayForwarding_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_VendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_VendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_Chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_Chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_ChaddrMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_ChaddrMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_ChaddrExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_ChaddrExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_LocallyServed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_LocallyServed(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4ServerPoolClientOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv4RelayForwarding_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)data;
	char *v= NULL;
	if(dhcp_client_args->dhcp_client_conf == NULL)
	{
		*value= "Error_Misconfigured";

		return 0;
	}
	dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "disabled", &v);
	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "Enabled";
	else
		*value= "Disabled";

	return 0;
}

int get_DHCPv4RelayForwarding_ClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_ClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_ClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_ClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4RelayForwarding_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv4RelayForwarding_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			//TODO
			break;
	}
	return 0;
}

int get_DHCPv4ServerPoolClientOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv4Relay_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*relayd";
	if (check_file(path))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_DHCPv4Relay_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmcmd("/etc/init.d/relayd", 1, "enable");
			else
				dmcmd("/etc/init.d/relayd", 1, "disable");
			break;
	}
	return 0;
}

int get_DHCPv4Relay_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*relayd";
	if (check_file(path))
		*value = "Enabled";
	else
		*value = "Disabled";
	return 0;
}

int get_DHCPv4Relay_ForwardingNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s, *dmmap_sect;
	int nbre_confs= 0, nbre_dmmaps= 0;

	uci_foreach_option_eq("network", "interface", "proto", "relay", s) {
		nbre_confs++;
	}
	uci_path_foreach_sections(icwmpd, "dmmap_dhcp_relay", "interface", dmmap_sect) {
		nbre_dmmaps++;
	}
	if(nbre_dmmaps ==0 || nbre_dmmaps < nbre_confs)
		dmasprintf(value, "%d", nbre_confs);
	else
		dmasprintf(value, "%d", nbre_dmmaps);
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseDhcpInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	char *interface, *idhcp = NULL, *idhcp_last = NULL;
	struct dhcp_args curr_dhcp_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("dhcp", "dhcp", "dmmap_dhcp", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "interface", &interface);
		init_dhcp_args(&curr_dhcp_args, p->config_section, interface);
		idhcp = handle_update_instance(1, dmctx, &idhcp_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "dhcp_instance", "dhcp_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_args, idhcp) == DM_STOP)
			break;
	}

	free_dmmap_config_dup_list(&dup_list);

	return 0;
}

int browseDhcpStaticInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *sss = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;
	struct dhcp_static_args curr_dhcp_staticargs = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_cont("dhcp", "host", "dmmap_dhcp", "dhcp", ((struct dhcp_args *)prev_data)->interface, &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		idhcp = handle_update_instance(2, dmctx, &idhcp_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "ldhcpinstance", "ldhcpalias");
		init_args_dhcp_host(&curr_dhcp_staticargs, p->config_section);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_staticargs, idhcp) == DM_STOP)
			break;
	}

	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseDhcpClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *sss = NULL;
	char *idx = NULL, *idx_last = NULL;
	json_object *res = NULL, *client_obj = NULL;
	char *dhcp, *network;
	int id = 0;
	struct client_args curr_dhcp_client_args = {0};

	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			dhcp = dmjson_get_value(client_obj, 1, "dhcp");
			if(strcmp(dhcp, "true") == 0)
			{
				network = dmjson_get_value(client_obj, 1, "network");
				if(strcmp(network, ((struct dhcp_args *)prev_data)->interface) == 0)
				{
					init_dhcp_client_args(&curr_dhcp_client_args, client_obj, key);
					idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_client_args, idx) == DM_STOP)
						break;
				}
			}
		}
	}
	return 0;
}

int browseDhcpClientIPv4Inst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	unsigned int leasetime;
	char *macaddr;
	char mac[32], ip[32], buf[512];
	json_object *passed_args;
	FILE *fp;
	struct dhcp_client_ipv4address_args current_dhcp_client_ipv4address_args = {0};
	int id = 0;
	char *idx = NULL, *idx_last = NULL;

	fp = fopen(DHCP_LEASES_FILE, "r");
	if (fp == NULL)
		return 0;
	while (fgets (buf , 256 , fp) != NULL) {
		sscanf(buf, "%u %s %s", &leasetime, mac, ip);
		passed_args= ((struct client_args*)prev_data)->client;
		macaddr=dmjson_get_value(passed_args, 1, "macaddr");
		if(!strcmp(mac, macaddr)){
			current_dhcp_client_ipv4address_args.ip= dmstrdup(ip);
			current_dhcp_client_ipv4address_args.mac= dmstrdup(macaddr);
			current_dhcp_client_ipv4address_args.leasetime= leasetime;

			idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&current_dhcp_client_ipv4address_args, idx) == DM_STOP)
				break;
		}
	}

	return 0;
}

int browseDHCPv4ClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *instnbr = NULL, *v;
	struct dmmap_dup *p;
	char *type, *ipv4addr = "", *ipv6addr = "", *proto, *inst, *mask4= NULL;
	json_object *res, *jobj;
	struct dhcp_client_args dhcp_client_arg = {0};
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq_no_delete("network", "interface", "dmmap_dhcp_client", "proto", "dhcp", &dup_list);
	list_for_each_entry(p, &dup_list, list) {

		if(p->config_section != NULL){

			dmuci_get_value_by_section_string(p->config_section, "type", &type);

			if (strcmp(type, "alias") == 0 || strcmp(section_name(p->config_section), "loopback")==0)
				continue;

			dmuci_get_value_by_section_string(p->config_section, "ipaddr", &ipv4addr);

			dmuci_get_value_by_section_string(p->config_section, "netmask", &mask4);

			if (ipv4addr[0] == '\0') {

				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(p->config_section), String}}, 1, &res);

				if (res)
				{

					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");

					ipv4addr = dmjson_get_value(jobj, 1, "address");

					mask4= dmjson_get_value(jobj, 1, "mask");

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

			if (ipv4addr[0] == '\0' && ipv6addr[0] == '\0' && strcmp(proto, "dhcp") != 0 && strcmp(proto, "dhcpv6") != 0 && strcmp(inst, "") == 0 && strcmp(type, "bridge") != 0) {
				p->config_section=NULL;
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, p->dmmap_section, "section_name", "");
			}

		}

		if (ipv4addr == NULL || strlen(ipv4addr)==0)
			dhcp_client_arg.ip = dmstrdup("");
		else
			dhcp_client_arg.ip = dmstrdup(ipv4addr);
		if (mask4 == NULL || strlen(mask4)==0)
			dhcp_client_arg.mask = dmstrdup("");
		else
			dhcp_client_arg.mask = dmstrdup(mask4);

		dhcp_client_arg.dhcp_client_conf = p->config_section;

		dhcp_client_arg.dhcp_client_dm= p->dmmap_section;

		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, (void *)p->dmmap_section, "cwmp_dhcpv4client_instance", "cwmp_dhcpv4client_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_client_arg, instance) == DM_STOP)
			break;


	}
	return 0;
}

int browseDHCPv4ClientSentOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)prev_data;
	struct uci_section *dmmap_sect;
	struct dhcp_client_option_args dhcp_client_opt_args = {0};
	char *instance, *instnbr = NULL, *v1, *v2, **sentopts, **buf= NULL, *tmp, *optionvalue, *v= NULL;
	int length= 0, lgh2, i, j;

	if(dhcp_client_args->dhcp_client_conf != NULL)
		dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "sendopts", &v);

	if(v != NULL)
		sentopts= strsplit(v, " ", &length);

	check_create_dmmap_package("dmmap_dhcp_client");

	for (i= 0; i<length; i++){

		buf= strsplit(sentopts[i], ":", &lgh2);

		if ((dmmap_sect = get_dup_section_in_dmmap_eq("dmmap_dhcp_client", "send_option", section_name(dhcp_client_args->dhcp_client_conf), "option_tag", buf[0])) == NULL) {

			dmuci_add_section_icwmpd("dmmap_dhcp_client", "send_option", &dmmap_sect, &v);

			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", buf[0]);

			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_client_args->dhcp_client_conf));

		}

		optionvalue=dmstrdup(buf[1]);

		if(lgh2>2){

			for(j=2; j<lgh2; j++){

				tmp=dmstrdup(optionvalue);

				free(optionvalue);

				optionvalue= NULL;

				dmasprintf(&optionvalue, "%s:%s", tmp, buf[j]);

				free(tmp);

				tmp= NULL;

			}

		}

		DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_value", optionvalue);

	}

	uci_path_foreach_option_eq(icwmpd, "dmmap_dhcp_client", "send_option", "section_name", dhcp_client_args->dhcp_client_conf?section_name(dhcp_client_args->dhcp_client_conf):"", dmmap_sect) {

		dmuci_get_value_by_section_string(dmmap_sect, "option_tag", &v1);
		dmuci_get_value_by_section_string(dmmap_sect, "option_value", &v2);
		dhcp_client_opt_args.client_sect= dhcp_client_args->dhcp_client_conf;
		dhcp_client_opt_args.option_tag= dmstrdup(v1);
		dhcp_client_opt_args.value= dmstrdup(v2);
		dhcp_client_opt_args.opt_sect= dmmap_sect;

		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, dmmap_sect, "cwmp_dhcpv4_sentopt_instance", "cwmp_dhcpv4_sentopt_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_client_opt_args, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseDHCPv4ClientReqOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct dhcp_client_args *dhcp_client_args = (struct dhcp_client_args*)prev_data;
	struct uci_section *dmmap_sect;
	struct dhcp_client_option_args dhcp_client_opt_args = {0};
	char *instance, *instnbr = NULL, *v1, **reqtopts, *v= NULL;
	int length= 0, lgh2, i, j;

	if (dhcp_client_args->dhcp_client_conf != NULL)
		dmuci_get_value_by_section_string(dhcp_client_args->dhcp_client_conf, "reqopts", &v);
	if (v!= NULL)
		reqtopts= strsplit(v, " ", &length);

	check_create_dmmap_package("dmmap_dhcp_client");
	for (i= 0; i<length; i++){
		if ((dmmap_sect = get_dup_section_in_dmmap_eq("dmmap_dhcp_client", "req_option", section_name(dhcp_client_args->dhcp_client_conf), "option_tag", reqtopts[i])) == NULL) {
			dmuci_add_section_icwmpd("dmmap_dhcp_client", "req_option", &dmmap_sect, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", reqtopts[i]);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_client_args->dhcp_client_conf));
		}
	}

	uci_path_foreach_option_eq(icwmpd, "dmmap_dhcp_client", "req_option", "section_name", dhcp_client_args->dhcp_client_conf?section_name(dhcp_client_args->dhcp_client_conf):"", dmmap_sect) {

		dmuci_get_value_by_section_string(dmmap_sect, "option_tag", &v1);
		dhcp_client_opt_args.client_sect= dhcp_client_args->dhcp_client_conf;
		dhcp_client_opt_args.option_tag= dmstrdup(v1);
		dhcp_client_opt_args.value= dmstrdup("");
		dhcp_client_opt_args.opt_sect= dmmap_sect;

		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, dmmap_sect, "cwmp_dhcpv4_reqtopt_instance", "cwmp_dhcpv4_reqtopt_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_client_opt_args, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseDHCPv4ServerPoolOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_list *dhcp_options_list = NULL;
	struct uci_element *e;
	struct dhcp_args *curr_dhcp_args = (struct dhcp_args*)prev_data;
	struct uci_section *dmmap_sect;
	char **tagvalue= NULL;
	char *instance, *instnbr = NULL, *optionvalue= NULL, *tmp, *v1, *v2, *v;
	int length, j;
	struct dhcp_client_option_args dhcp_client_opt_args = {0};

	dmuci_get_value_by_section_list(curr_dhcp_args->dhcp_sec, "dhcp_option", &dhcp_options_list);
	if (dhcp_options_list != NULL) {
		uci_foreach_element(dhcp_options_list, e) {
			tagvalue= strsplit(e->name, ",", &length);
			if ((dmmap_sect = get_dup_section_in_dmmap_eq("dmmap_dhcp", "servpool_option", section_name(curr_dhcp_args->dhcp_sec), "option_tag", tagvalue[0])) == NULL) {
				dmuci_add_section_icwmpd("dmmap_dhcp", "servpool_option", &dmmap_sect, &v);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", tagvalue[0]);
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(curr_dhcp_args->dhcp_sec));
			}
			optionvalue=dmstrdup(tagvalue[1]);
			if(length>2){
				for(j=2; j<length; j++){
					tmp=dmstrdup(optionvalue);
					free(optionvalue);
					optionvalue= NULL;
					dmasprintf(&optionvalue, "%s,%s", tmp, tagvalue[j]);
					free(tmp);
					tmp= NULL;
				}
			}
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_value", optionvalue);
		}
	}
	uci_path_foreach_option_eq(icwmpd, "dmmap_dhcp", "servpool_option", "section_name", section_name(curr_dhcp_args->dhcp_sec), dmmap_sect) {
		dmuci_get_value_by_section_string(dmmap_sect, "option_tag", &v1);
		dmuci_get_value_by_section_string(dmmap_sect, "option_value", &v2);
		dhcp_client_opt_args.client_sect= curr_dhcp_args->dhcp_sec;
		dhcp_client_opt_args.option_tag= strdup(v1);
		dhcp_client_opt_args.value= strdup(v2);
		dhcp_client_opt_args.opt_sect= dmmap_sect;
		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, dmmap_sect, "cwmp_dhcpv4_servpool_option_instance", "cwmp_dhcpv4_servpool_option_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_client_opt_args, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseDHCPv4ServerPoolClientOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv4RelayForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *instnbr = NULL, *v;
	struct dmmap_dup *p;
	char *type, *ipv4addr = "", *ipv6addr = "", *proto, *inst, *mask4= NULL;
	json_object *res, *jobj;
	struct dhcp_client_args dhcp_relay_arg = {0};
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq_no_delete("network", "interface", "dmmap_dhcp_relay", "proto", "relay", &dup_list);
	list_for_each_entry(p, &dup_list, list) {

		if(p->config_section != NULL){

			dmuci_get_value_by_section_string(p->config_section, "type", &type);

			if (strcmp(type, "alias") == 0 || strcmp(section_name(p->config_section), "loopback")==0)
				continue;

			dmuci_get_value_by_section_string(p->config_section, "ipaddr", &ipv4addr);

			dmuci_get_value_by_section_string(p->config_section, "netmask", &mask4);

			if (ipv4addr[0] == '\0') {

				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(p->config_section), String}}, 1, &res);

				if (res)
				{

					jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");

					ipv4addr = dmjson_get_value(jobj, 1, "address");

					mask4= dmjson_get_value(jobj, 1, "mask");

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

			if (ipv4addr[0] == '\0' && ipv6addr[0] == '\0' && strcmp(inst, "") == 0 && strcmp(type, "bridge") != 0 && strcmp(proto, "relay") != 0) {
				p->config_section=NULL;
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, p->dmmap_section, "section_name", "");
			}

		}

		if (ipv4addr == NULL || strlen(ipv4addr)==0)
			dhcp_relay_arg.ip = dmstrdup("");
		else
			dhcp_relay_arg.ip = dmstrdup(ipv4addr);
		if (mask4 == NULL || strlen(mask4)==0)
			dhcp_relay_arg.mask = dmstrdup("");
		else
			dhcp_relay_arg.mask = dmstrdup(mask4);

		dhcp_relay_arg.dhcp_client_conf = p->config_section;

		dhcp_relay_arg.dhcp_client_dm= p->dmmap_section;

		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, (void *)p->dmmap_section, "cwmp_dhcpv4relay_instance", "cwmp_dhcpv4relay_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_relay_arg, instance) == DM_STOP)
			break;


	}
	return 0;
}
