/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "dhcpv6.h"
#include "dmjson.h"

/* *** Device.DHCPv6. *** */
DMOBJ tDHCPv6Obj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Client", &DMWRITE, addObjDHCPv6Client, delObjDHCPv6Client, NULL, browseDHCPv6ClientInst, NULL, NULL, tDHCPv6ClientObj, tDHCPv6ClientParams, NULL},
{"Server", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDHCPv6ServerObj, tDHCPv6ServerParams, NULL},
{0}
};

DMLEAF tDHCPv6Params[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ClientNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6_ClientNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Client.{i}. *** */
DMOBJ tDHCPv6ClientObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Server", &DMREAD, NULL, NULL, NULL, browseDHCPv6ClientServerInst, NULL, NULL, NULL, tDHCPv6ClientServerParams, NULL},
{"SentOption", &DMWRITE, addObjDHCPv6ClientSentOption, delObjDHCPv6ClientSentOption, NULL, browseDHCPv6ClientSentOptionInst, NULL, NULL, NULL, tDHCPv6ClientSentOptionParams, NULL},
{"ReceivedOption", &DMREAD, NULL, NULL, NULL, browseDHCPv6ClientReceivedOptionInst, NULL, NULL, NULL, tDHCPv6ClientReceivedOptionParams, NULL},
{0}
};

DMLEAF tDHCPv6ClientParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv6Client_Enable, set_DHCPv6Client_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv6Client_Alias, set_DHCPv6Client_Alias, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_DHCPv6Client_Interface, set_DHCPv6Client_Interface, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DHCPv6Client_Status, NULL, NULL, NULL},
{"DUID", &DMREAD, DMT_HEXBIN, get_DHCPv6Client_DUID, NULL, NULL, NULL},
{"RequestAddresses", &DMWRITE, DMT_BOOL, get_DHCPv6Client_RequestAddresses, set_DHCPv6Client_RequestAddresses, NULL, NULL},
{"RequestPrefixes", &DMWRITE, DMT_BOOL, get_DHCPv6Client_RequestPrefixes, set_DHCPv6Client_RequestPrefixes, NULL, NULL},
{"RapidCommit", &DMWRITE, DMT_BOOL, get_DHCPv6Client_RapidCommit, set_DHCPv6Client_RapidCommit, NULL, NULL},
{"Renew", &DMWRITE, DMT_BOOL, get_DHCPv6Client_Renew, set_DHCPv6Client_Renew, NULL, NULL},
{"SuggestedT1", &DMWRITE, DMT_INT, get_DHCPv6Client_SuggestedT1, set_DHCPv6Client_SuggestedT1, NULL, NULL},
{"SuggestedT2", &DMWRITE, DMT_INT, get_DHCPv6Client_SuggestedT2, set_DHCPv6Client_SuggestedT2, NULL, NULL},
{"SupportedOptions", &DMREAD, DMT_STRING, get_DHCPv6Client_SupportedOptions, NULL, NULL, NULL},
{"RequestedOptions", &DMWRITE, DMT_STRING, get_DHCPv6Client_RequestedOptions, set_DHCPv6Client_RequestedOptions, NULL, NULL},
{"ServerNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6Client_ServerNumberOfEntries, NULL, NULL, NULL},
{"SentOptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6Client_SentOptionNumberOfEntries, NULL, NULL, NULL},
{"ReceivedOptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6Client_ReceivedOptionNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Client.{i}.Server.{i}. *** */
DMLEAF tDHCPv6ClientServerParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"SourceAddress", &DMREAD, DMT_STRING, get_DHCPv6ClientServer_SourceAddress, NULL, NULL, NULL},
{"DUID", &DMREAD, DMT_HEXBIN, get_DHCPv6ClientServer_DUID, NULL, NULL, NULL},
{"InformationRefreshTime", &DMREAD, DMT_TIME, get_DHCPv6ClientServer_InformationRefreshTime, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Client.{i}.SentOption.{i}. *** */
DMLEAF tDHCPv6ClientSentOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv6ClientSentOption_Enable, set_DHCPv6ClientSentOption_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv6ClientSentOption_Alias, set_DHCPv6ClientSentOption_Alias, NULL, NULL},
{"Tag", &DMWRITE, DMT_UNINT, get_DHCPv6ClientSentOption_Tag, set_DHCPv6ClientSentOption_Tag, NULL, NULL},
{"Value", &DMWRITE, DMT_HEXBIN, get_DHCPv6ClientSentOption_Value, set_DHCPv6ClientSentOption_Value, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Client.{i}.ReceivedOption.{i}. *** */
DMLEAF tDHCPv6ClientReceivedOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Tag", &DMREAD, DMT_UNINT, get_DHCPv6ClientReceivedOption_Tag, NULL, NULL, NULL},
{"Value", &DMREAD, DMT_HEXBIN, get_DHCPv6ClientReceivedOption_Value, NULL, NULL, NULL},
{"Server", &DMREAD, DMT_STRING, get_DHCPv6ClientReceivedOption_Server, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server. *** */
DMOBJ tDHCPv6ServerObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Pool", &DMWRITE, addObjDHCPv6ServerPool, delObjDHCPv6ServerPool, NULL, browseDHCPv6ServerPoolInst, NULL, NULL, tDHCPv6ServerPoolObj, tDHCPv6ServerPoolParams, NULL},
{0}
};

DMLEAF tDHCPv6ServerParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv6Server_Enable, set_DHCPv6Server_Enable, NULL, NULL},
{"PoolNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6Server_PoolNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}. *** */
DMOBJ tDHCPv6ServerPoolObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Client", &DMREAD, NULL, NULL, NULL, browseDHCPv6ServerPoolClientInst, NULL, NULL, tDHCPv6ServerPoolClientObj, tDHCPv6ServerPoolClientParams, NULL},
{"Option", &DMWRITE, addObjDHCPv6ServerPoolOption, delObjDHCPv6ServerPoolOption, NULL, browseDHCPv6ServerPoolOptionInst, NULL, NULL, NULL, tDHCPv6ServerPoolOptionParams, NULL},
{0}
};

DMLEAF tDHCPv6ServerPoolParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_Enable, set_DHCPv6ServerPool_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DHCPv6ServerPool_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_Alias, set_DHCPv6ServerPool_Alias, NULL, NULL},
{"Order", &DMWRITE, DMT_UNINT, get_DHCPv6ServerPool_Order, set_DHCPv6ServerPool_Order, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_Interface, set_DHCPv6ServerPool_Interface, NULL, NULL},
{"DUID", &DMWRITE, DMT_HEXBIN, get_DHCPv6ServerPool_DUID, set_DHCPv6ServerPool_DUID, NULL, NULL},
{"DUIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_DUIDExclude, set_DHCPv6ServerPool_DUIDExclude, NULL, NULL},
{"VendorClassID", &DMWRITE, DMT_HEXBIN, get_DHCPv6ServerPool_VendorClassID, set_DHCPv6ServerPool_VendorClassID, NULL, NULL},
{"VendorClassIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_VendorClassIDExclude, set_DHCPv6ServerPool_VendorClassIDExclude, NULL, NULL},
{"UserClassID", &DMWRITE, DMT_HEXBIN, get_DHCPv6ServerPool_UserClassID, set_DHCPv6ServerPool_UserClassID, NULL, NULL},
{"UserClassIDExclude", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_UserClassIDExclude, set_DHCPv6ServerPool_UserClassIDExclude, NULL, NULL},
{"SourceAddress", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_SourceAddress, set_DHCPv6ServerPool_SourceAddress, NULL, NULL},
{"SourceAddressMask", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_SourceAddressMask, set_DHCPv6ServerPool_SourceAddressMask, NULL, NULL},
{"SourceAddressExclude", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_SourceAddressExclude, set_DHCPv6ServerPool_SourceAddressExclude, NULL, NULL},
{"IANAEnable", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_IANAEnable, set_DHCPv6ServerPool_IANAEnable, NULL, NULL},
{"IANAManualPrefixes", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_IANAManualPrefixes, set_DHCPv6ServerPool_IANAManualPrefixes, NULL, NULL},
{"IANAPrefixes", &DMREAD, DMT_STRING, get_DHCPv6ServerPool_IANAPrefixes, NULL, NULL, NULL},
{"IAPDEnable", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPool_IAPDEnable, set_DHCPv6ServerPool_IAPDEnable, NULL, NULL},
{"IAPDManualPrefixes", &DMWRITE, DMT_STRING, get_DHCPv6ServerPool_IAPDManualPrefixes, set_DHCPv6ServerPool_IAPDManualPrefixes, NULL, NULL},
{"IAPDPrefixes", &DMREAD, DMT_STRING, get_DHCPv6ServerPool_IAPDPrefixes, NULL, NULL, NULL},
{"IAPDAddLength", &DMWRITE, DMT_UNINT, get_DHCPv6ServerPool_IAPDAddLength, set_DHCPv6ServerPool_IAPDAddLength, NULL, NULL},
{"ClientNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6ServerPool_ClientNumberOfEntries, NULL, NULL, NULL},
{"OptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6ServerPool_OptionNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}.Client.{i}. *** */
DMOBJ tDHCPv6ServerPoolClientObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"IPv6Address", &DMREAD, NULL, NULL, NULL, browseDHCPv6ServerPoolClientIPv6AddressInst, NULL, NULL, NULL, tDHCPv6ServerPoolClientIPv6AddressParams, NULL},
{"IPv6Prefix", &DMREAD, NULL, NULL, NULL, browseDHCPv6ServerPoolClientIPv6PrefixInst, NULL, NULL, NULL, tDHCPv6ServerPoolClientIPv6PrefixParams, NULL},
{"Option", &DMREAD, NULL, NULL, NULL, browseDHCPv6ServerPoolClientOptionInst, NULL, NULL, NULL, tDHCPv6ServerPoolClientOptionParams, NULL},
{0}
};

DMLEAF tDHCPv6ServerPoolClientParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv6ServerPoolClient_Alias, set_DHCPv6ServerPoolClient_Alias, NULL, NULL},
{"SourceAddress", &DMREAD, DMT_STRING, get_DHCPv6ServerPoolClient_SourceAddress, NULL, NULL, NULL},
{"Active", &DMREAD, DMT_BOOL, get_DHCPv6ServerPoolClient_Active, NULL, NULL, NULL},
{"IPv6AddressNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6ServerPoolClient_IPv6AddressNumberOfEntries, NULL, NULL, NULL},
{"IPv6PrefixNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6ServerPoolClient_IPv6PrefixNumberOfEntries, NULL, NULL, NULL},
{"OptionNumberOfEntries", &DMREAD, DMT_UNINT, get_DHCPv6ServerPoolClient_OptionNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}.Client.{i}.IPv6Address.{i}. *** */
DMLEAF tDHCPv6ServerPoolClientIPv6AddressParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"IPAddress", &DMREAD, DMT_STRING, get_DHCPv6ServerPoolClientIPv6Address_IPAddress, NULL, NULL, NULL},
{"PreferredLifetime", &DMREAD, DMT_TIME, get_DHCPv6ServerPoolClientIPv6Address_PreferredLifetime, NULL, NULL, NULL},
{"ValidLifetime", &DMREAD, DMT_TIME, get_DHCPv6ServerPoolClientIPv6Address_ValidLifetime, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}.Client.{i}.IPv6Prefix.{i}. *** */
DMLEAF tDHCPv6ServerPoolClientIPv6PrefixParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Prefix", &DMREAD, DMT_STRING, get_DHCPv6ServerPoolClientIPv6Prefix_Prefix, NULL, NULL, NULL},
{"PreferredLifetime", &DMREAD, DMT_TIME, get_DHCPv6ServerPoolClientIPv6Prefix_PreferredLifetime, NULL, NULL, NULL},
{"ValidLifetime", &DMREAD, DMT_TIME, get_DHCPv6ServerPoolClientIPv6Prefix_ValidLifetime, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}.Client.{i}.Option.{i}. *** */
DMLEAF tDHCPv6ServerPoolClientOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Tag", &DMREAD, DMT_UNINT, get_DHCPv6ServerPoolClientOption_Tag, NULL, NULL, NULL},
{"Value", &DMREAD, DMT_HEXBIN, get_DHCPv6ServerPoolClientOption_Value, NULL, NULL, NULL},
{0}
};

/* *** Device.DHCPv6.Server.Pool.{i}.Option.{i}. *** */
DMLEAF tDHCPv6ServerPoolOptionParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DHCPv6ServerPoolOption_Enable, set_DHCPv6ServerPoolOption_Enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DHCPv6ServerPoolOption_Alias, set_DHCPv6ServerPoolOption_Alias, NULL, NULL},
{"Tag", &DMWRITE, DMT_UNINT, get_DHCPv6ServerPoolOption_Tag, set_DHCPv6ServerPoolOption_Tag, NULL, NULL},
{"Value", &DMWRITE, DMT_HEXBIN, get_DHCPv6ServerPoolOption_Value, set_DHCPv6ServerPoolOption_Value, NULL, NULL},
{"PassthroughClient", &DMWRITE, DMT_STRING, get_DHCPv6ServerPoolOption_PassthroughClient, set_DHCPv6ServerPoolOption_PassthroughClient, NULL, NULL},
{0}
};

struct uci_section* get_dhcpv6_classifier(char *classifier_name, char *network) {
	struct uci_section* s= NULL;
	char *v;

	uci_foreach_sections("dhcp", classifier_name, s) {
		dmuci_get_value_by_section_string(s, "networkid", &v);
		if(strcmp(v, network) == 0)
			return s;
	}
	return NULL;
}

inline int init_dhcpv6_client_args(struct clientv6_args *args, json_object *client, json_object *client_param, int i)
{
	args->client = client;
	args->clientparam= client_param;
	args->idx = i;
	return 0;
}

struct uci_section* exist_other_section_dhcp6_same_order(struct uci_section *dmmap_sect, char * package, char* sect_type, char *order) {
	struct uci_section *s;
	uci_path_foreach_option_eq(icwmpd, package, sect_type, "order", order, s) {
		if(strcmp(section_name(s), section_name(dmmap_sect)) != 0){
			return s;
		}
	}
	return NULL;
}

int set_section_dhcp6_order(char *package, char *dmpackage, char* sect_type, struct uci_section *dmmap_sect, struct uci_section *conf, int set_force, char* order) {
	char *v= NULL, *sect_name, *incrorder;
	struct uci_section *s, *dm;
	dmuci_get_value_by_section_string(dmmap_sect, "order", &v);
	if((v!=NULL || strlen(v)>0) && strcmp(v, order) == 0)
		return 0;
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "order", order);
	if(conf==NULL){
		dmuci_get_value_by_section_string(dmmap_sect, "section_name", &sect_name);
		get_config_section_of_dmmap_section(package, sect_type, sect_name, &s);
	} else
		s= conf;

	if(strcmp(order, "1") != 0 && s!=NULL){
		dmuci_set_value_by_section(s, "force", "");
	}

	if(set_force==1 && strcmp(order, "1") == 0 && s!=NULL) {
		dmuci_set_value_by_section(s, "force", "1");
	}

	if ((dm = exist_other_section_dhcp6_same_order(dmmap_sect, dmpackage, sect_type, order)) != NULL) {
		dmuci_get_value_by_section_string(dm, "section_name", &sect_name);
		get_config_section_of_dmmap_section(package, sect_type, sect_name, &s);
		dmasprintf(&incrorder, "%d", atoi(order)+1);
		if(s!=NULL && strcmp(order, "1") == 0){
			dmuci_set_value_by_section(s, "force", "");
		}
		set_section_dhcp6_order(package, dmpackage, sect_type, dm, s, set_force, incrorder);
	}
	return 0;

}

inline int init_dhcpv6_args(struct dhcpv6_args *args, struct uci_section *s, char *interface)
{
	args->interface = interface;
	args->dhcp_sec = s;
	return 0;
}

int browseDHCPv6ClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	struct dhcpv6_client_args dhcpv6_client_arg = {0};
	json_object *res, *jobj;
	char *instance, *instnbr = NULL, *proto= NULL, *ipv6addr;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq_no_delete("network", "interface", "dmmap_dhcpv6", "proto", "dhcpv6", &dup_list);
	list_for_each_entry(p, &dup_list, list) {

		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(p->config_section), String}}, 1, &res);

		if (res)
		{

			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv6-address");

			ipv6addr = dmjson_get_value(jobj, 1, "address");

		}
		dhcpv6_client_arg.dhcp_client_conf = p->config_section;

		dhcpv6_client_arg.dhcp_client_dm= p->dmmap_section;
		dhcpv6_client_arg.ip= strdup(ipv6addr?ipv6addr:"");

		instance = handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, p->dmmap_section, "cwmp_dhcpv6client_instance", "cwmp_dhcpv6client_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcpv6_client_arg, instance) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseDHCPv6ClientServerInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ClientSentOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ClientReceivedOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	char *interface, *idhcp = NULL, *idhcp_last = NULL, *v;
	struct dhcpv6_args curr_dhcp6_args = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq("dhcp", "dhcp", "dmmap_dhcpv6", "dhcpv6", "server", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "interface", &interface);
		init_dhcpv6_args(&curr_dhcp6_args, p->config_section, interface);
		idhcp = handle_update_instance(1, dmctx, &idhcp_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "dhcpv6_serv_pool_instance", "dhcpv6_serv_pool_alias");
		dmuci_get_value_by_section_string(p->dmmap_section, "order", &v);
		if(v==NULL || strlen(v) == 0)
			set_section_dhcp6_order("dhcp", "dmmap_dhcpv6", "dhcp", p->dmmap_section, p->config_section, 0, idhcp);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp6_args, idhcp) == DM_STOP)
			break;
	}

	free_dmmap_config_dup_list(&dup_list);

	return 0;
}

int browseDHCPv6ServerPoolClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct dhcpv6_args *dhcp_arg= (struct dhcpv6_args*)prev_data;
	json_object *res, *res1, *jobj, *dev_obj= NULL, *net_obj= NULL;
	struct clientv6_args curr_dhcp_client_args = {0};
	char *device;
	int i= 0;
	char *idx = NULL, *idx_last = NULL;
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcp_arg->dhcp_sec), String}}, 1, &res1);
	device = dmjson_get_value(res1, 1, "device");
	dmubus_call("dhcp", "ipv6leases", UBUS_ARGS{}, 0, &res);
	if(!res)
		return 0;
	dev_obj = dmjson_get_obj(res, 1, "device");
	if (!dev_obj)
		return 0;

	net_obj = dmjson_get_obj(dev_obj, 1, device);
	if (!net_obj)
		return 0;

	while (1) {
		jobj = dmjson_select_obj_in_array_idx(net_obj, i, 1, "leases");
		if (jobj == NULL)
			break;
		init_dhcpv6_client_args(&curr_dhcp_client_args, jobj, NULL, i);
		i++;
		idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, i);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcp_client_args, idx) == DM_STOP)
			break;
	}
	return 0;
}

int browseDHCPv6ServerPoolOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_list *dhcp_options_list = NULL;
	struct uci_element *e;
	struct dhcpv6_args *curr_dhcp_args = (struct dhcpv6_args*)prev_data;
	struct uci_section *dmmap_sect;
	char **tagvalue= NULL;
	char *instance, *instnbr = NULL, *optionvalue= NULL, *tmp, *v1, *v2, *v;
	int length, j;
	struct dhcpv6_client_option_args dhcp_client_opt_args = {0};

	dmuci_get_value_by_section_list(curr_dhcp_args->dhcp_sec, "dhcp_option", &dhcp_options_list);
	if (dhcp_options_list != NULL) {
		uci_foreach_element(dhcp_options_list, e) {
			tagvalue= strsplit(e->name, ",", &length);
			if ((dmmap_sect = get_dup_section_in_dmmap_eq("dmmap_dhcpv6", "servpool_option", section_name(curr_dhcp_args->dhcp_sec), "option_tag", tagvalue[0])) == NULL) {
				dmuci_add_section_icwmpd("dmmap_dhcpv6", "servpool_option", &dmmap_sect, &v);
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
	uci_path_foreach_option_eq(icwmpd, "dmmap_dhcpv6", "servpool_option", "section_name", section_name(curr_dhcp_args->dhcp_sec), dmmap_sect) {
		dmuci_get_value_by_section_string(dmmap_sect, "option_tag", &v1);
		dmuci_get_value_by_section_string(dmmap_sect, "option_value", &v2);
		dhcp_client_opt_args.client_sect= curr_dhcp_args->dhcp_sec;
		dhcp_client_opt_args.option_tag= strdup(v1);
		dhcp_client_opt_args.value= strdup(v2);
		dhcp_client_opt_args.opt_sect= dmmap_sect;
		instance= handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, dmmap_sect, "cwmp_dhcpv6_servpool_option_instance", "cwmp_dhcpv6_servpool_option_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &dhcp_client_opt_args, instance) == DM_STOP)
			break;
	}
	return 0;
}

int browseDHCPv6ServerPoolClientIPv6AddressInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct clientv6_args *dhcpv6_serv_pool_client= (struct clientv6_args *)prev_data;
	json_object *address_obj= NULL, *prefix_obj= NULL;
	struct clientv6_args curr_dhcv6_address_args = {0};
	char *idx = NULL, *idx_last = NULL;
	int i= 0;

	while (1) {
		address_obj = dmjson_select_obj_in_array_idx(dhcpv6_serv_pool_client->client, i, 1, "ipv6-addr");
		if (address_obj == NULL)
			break;
		init_dhcpv6_client_args(&curr_dhcv6_address_args, dhcpv6_serv_pool_client->client, address_obj, i);
		i++;
		idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, i);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcv6_address_args, idx) == DM_STOP)
			break;
	}

	return 0;
}

int browseDHCPv6ServerPoolClientIPv6PrefixInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct clientv6_args *dhcpv6_serv_pool_client= (struct clientv6_args *)prev_data;
	json_object *address_obj= NULL, *prefix_obj= NULL;
	struct clientv6_args curr_dhcv6_address_args = {0};
	char *idx = NULL, *idx_last = NULL;
	int i= 0;

	while (1) {
		address_obj = dmjson_select_obj_in_array_idx(dhcpv6_serv_pool_client->client, i, 1, "ipv6-prefix");
		if (address_obj == NULL)
			break;
		init_dhcpv6_client_args(&curr_dhcv6_address_args, dhcpv6_serv_pool_client->client, address_obj, i);
		i++;
		idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, i);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dhcv6_address_args, idx) == DM_STOP)
			break;
	}

	return 0;
}

int browseDHCPv6ServerPoolClientOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}


int addObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_sect;
	char *wan_eth, *value, *wanname, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcpv6");
	instancepara = get_last_instance_icwmpd("dmmap_dhcpv6", "interface", "cwmp_dhcpv6client_instance");
	dmuci_add_section("network", "interface", &s, &value);
	dmuci_set_value_by_section(s, "proto", "dhcpv6");
	dmuci_set_value_by_section(s, "ifname", "@wan");
	dmuci_set_value_by_section(s, "type", "anywan");
	dmuci_add_section_icwmpd("dmmap_dhcpv6", "interface", &dmmap_sect, &v);
	dmuci_set_value_by_section(dmmap_sect, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv6client_instance");
	return 0;
}

int delObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	struct uci_section *s, *dmmap_section, *ss;
	int found= 0;

	char *proto;

	switch (del_action) {
		case DEL_INST:
			if(dhcpv6_client_args->dhcp_client_conf != NULL && is_section_unnamed(section_name(dhcpv6_client_args->dhcp_client_conf))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_dhcpv6", "interface", "cwmp_dhcpv6client_instance", section_name(dhcpv6_client_args->dhcp_client_conf), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "cwmp_dhcpv6client_instance", "dmmap_dhcpv6", "interface");
				dmuci_delete_by_section_unnamed(dhcpv6_client_args->dhcp_client_conf, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name(dhcpv6_client_args->dhcp_client_conf), &dmmap_section);
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(dhcpv6_client_args->dhcp_client_conf, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("network", "interface", s) {
				if (found != 0){
					dmuci_get_value_by_section_string(ss, "proto", &proto);
					if(strcmp(proto, "dhcpv6") == 0) {
						get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name(ss), &dmmap_section);
						if(dmmap_section != NULL)
							dmuci_delete_by_section(dmmap_section, NULL, NULL);
						dmuci_delete_by_section(ss, NULL, NULL);
					}
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				dmuci_get_value_by_section_string(ss, "proto", &proto);
				if(strcmp(proto, "dhcpv6") == 0) {
					get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
			}
			break;
	}
	return 0;
}

int addObjDHCPv6ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjDHCPv6ClientSentOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjDHCPv6ServerPool(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value, *v;
	char *instancepara;
	struct uci_section *s = NULL, *dmmap_dhcp= NULL;

	check_create_dmmap_package("dmmap_dhcpv6");
	instancepara = get_last_instance_icwmpd("dmmap_dhcpv6", "dhcp", "dhcpv6_serv_pool_instance");
	dmuci_add_section("dhcp", "dhcp", &s, &value);
	dmuci_set_value_by_section(s, "dhcpv6", "server");
	dmuci_set_value_by_section(s, "start", "100");
	dmuci_set_value_by_section(s, "leasetime", "12h");
	dmuci_set_value_by_section(s, "limit", "150");

	dmuci_add_section_icwmpd("dmmap_dhcpv6", "dhcp", &dmmap_dhcp, &v);
	dmuci_set_value_by_section(dmmap_dhcp, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_dhcp, instancepara, "dhcpv6_serv_pool_instance");
	return 0;
}

int delObjDHCPv6ServerPool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_section= NULL;
	char *dhcpv6= NULL;

	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name(((struct dhcpv6_args *)data)->dhcp_sec))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_dhcpv6", "dhcp", "dhcpv6_serv_pool_instance", section_name(((struct dhcpv6_args *)data)->dhcp_sec), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "dhcpv6_serv_pool_instance", "dmmap_dhcpv6", "dhcp");
				dmuci_delete_by_section_unnamed(((struct dhcpv6_args *)data)->dhcp_sec, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(((struct dhcpv6_args *)data)->dhcp_sec), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(((struct dhcpv6_args *)data)->dhcp_sec, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("dhcp", "dhcp", s) {
				if (found != 0){
					dmuci_get_value_by_section_string(ss, "dhcpv6", &dhcpv6);
					if(strcmp(dhcpv6, "server") == 0){
						get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(s), &dmmap_section);
						if(dmmap_section != NULL)
							dmuci_delete_by_section(dmmap_section, NULL, NULL);
						dmuci_delete_by_section(ss, NULL, NULL);
					}
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				dmuci_get_value_by_section_string(ss, "dhcpv6", &dhcpv6);
				if(strcmp(dhcpv6, "server") == 0){
					get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
			}
			break;
	}
	return 0;
}

int addObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct dhcpv6_args *dhcp_arg = (struct dhcpv6_args*)data;
	struct uci_section *s, *dmmap_sect;
	char *value, *instancepara, *v;

	check_create_dmmap_package("dmmap_dhcpv6");
	instancepara= get_last_instance_lev2_icwmpd_dmmap_opt("dmmap_dhcpv6", "servpool_option", "cwmp_dhcpv6_servpool_option_instance", "section_name", section_name(dhcp_arg->dhcp_sec));
	dmuci_add_section_icwmpd("dmmap_dhcpv6", "servpool_option", &dmmap_sect, &value);
	if(dhcp_arg->dhcp_sec != NULL)
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "section_name", section_name(dhcp_arg->dhcp_sec));
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "option_tag", "0");
	*instance = update_instance_icwmpd(dmmap_sect, instancepara, "cwmp_dhcpv6_servpool_option_instance");
	return 0;
}

int delObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s, *stmp;
	char *list= NULL, *opt_value= NULL;
	struct uci_list *dhcp_options_list = NULL;

	switch (del_action) {
		case DEL_INST:
			if(strcmp(((struct dhcpv6_client_option_args*) data)->option_tag, "0") != 0)
			{
				dmasprintf(&opt_value, "%s,%s", ((struct dhcpv6_client_option_args*) data)->option_tag, ((struct dhcpv6_client_option_args*) data)->value);
				dmuci_get_value_by_section_list(((struct dhcpv6_client_option_args*) data)->client_sect, "dhcp_option", &dhcp_options_list);
				if(dhcp_options_list != NULL){
					dmuci_del_list_value_by_section(((struct dhcpv6_client_option_args*) data)->client_sect, "dhcp_option", opt_value);
				}
			}
			dmuci_delete_by_section_unnamed_icwmpd(((struct dhcpv6_client_option_args*) data)->opt_sect, NULL, NULL);
			break;
		case DEL_ALL:
			dmuci_set_value_by_section(((struct dhcpv6_args*) data)->dhcp_sec, "dhcp_option", "");
			uci_path_foreach_sections_safe(icwmpd, "dmmap_dhcpv6", "servpool_option", stmp, s) {
				dmuci_delete_by_section_unnamed_icwmpd(s, NULL, NULL);
			}
			break;
	}
	return 0;
}


int get_DHCPv6_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;
	char *proto;

	uci_path_foreach_sections(icwmpd, "dmmap_dhcpv6", "interface", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_DHCPv6Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	char *v= NULL;


	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "0";
		return 0;
	}

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_conf, "disabled", &v);
	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "1";
	else
		*value= "0";
	return 0;
}

int set_DHCPv6Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	switch (action)	{
	case VALUECHECK:
		if (string_to_bool(value, &b))
			return FAULT_9007;
		return 0;
	case VALUESET:
		string_to_bool(value, &b);
		if (b)
			dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "disabled", "0");
		else
			dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "disabled", "1");
		break;
	}
	return 0;
}

int get_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_dm, "cwmp_dhcpv6client_alias", value);
	return 0;
}

int set_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_dm, "cwmp_dhcpv6client_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}

	char *linker= dmstrdup(section_name(dhcpv6_client_args->dhcp_client_conf));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim, dm_delim), linker, value);
	return 0;
}

int set_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	struct uci_section *s;
	char *linker= NULL, *newvalue= NULL, *v;
	struct uci_section *dm;

	switch (action)	{
		case VALUECHECK:
			if(strlen(value) == 0 || strcmp(value, "") == 0){
				return FAULT_9007;
			}

			if (value[strlen(value)-1]!='.') {
				dmasprintf(&newvalue, "%s.", value);
				adm_entry_get_linker_value(ctx, newvalue, &linker);
			} else
				adm_entry_get_linker_value(ctx, value, &linker);
			uci_path_foreach_sections(icwmpd, "dmmap_dhcpv6", "interface", s) {
				dmuci_get_value_by_section_string(s, "section_name", &v);
				if(strcmp(v, linker) == 0)
					return FAULT_9007;
			}
			uci_foreach_sections("network", "interface", s) {
				if(strcmp(section_name(s), linker) == 0){
					dmuci_get_value_by_section_string(s, "proto", &v);
					if(strcmp(v, "dhcpv6") != 0)
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
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcpv6_client_args->dhcp_client_dm, "section_name", linker);
			break;
	}
	return 0;
}

int get_DHCPv6Client_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	char *v= "";
	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "Error_Misconfigured";

		return 0;
	}

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_conf, "disabled", &v);

	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "Enabled";
	else
		*value= "Disabled";

	return 0;
}

int get_DHCPv6Client_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	json_object *res;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcpv6_client_args->dhcp_client_conf), String}}, 1, &res);
	if (res)
	{
		*value = dmjson_get_value(res, 2, "data", "passthru");
	}
	return 0;
}

int get_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	char *v= "";
	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_conf, "reqaddress", &v);
	if(strcmp(v, "none") == 0)
		*value= "0";
	else
		*value= "1";
	return 0;
}

int set_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "reqaddress", "force");
			else
				dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "reqaddress", "none");
			break;
	}
	return 0;
}

int get_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;
	char *v= "";
	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_conf, "reqprefix", &v);
	if(strcmp(v, "no") == 0)
		*value= "0";
	else
		*value= "1";
	return 0;
}

int set_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "reqprefix", "auto");
			else
				dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "reqprefix", "no");
			break;
	}
	return 0;
}

int get_DHCPv6Client_RapidCommit(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6Client_RapidCommit(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "false";
	return 0;
}

int set_DHCPv6Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_args *dhcp_client_args = (struct dhcpv6_client_args*)data;
	json_object *res;
	bool b;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if (dhcp_client_args->dhcp_client_conf == NULL && !b)
				return 0;

			dmubus_call("network.interface", "renew", UBUS_ARGS{{"interface", section_name(dhcp_client_args->dhcp_client_conf), String}}, 1, &res);
			break;
	}
	return 0;
}

int get_DHCPv6Client_SuggestedT1(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6Client_SuggestedT1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6Client_SuggestedT2(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6Client_SuggestedT2(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6Client_SupportedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Client_RequestedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	if(dhcpv6_client_args->dhcp_client_conf == NULL)
	{
		*value= "";

		return 0;
	}

	dmuci_get_value_by_section_string(dhcpv6_client_args->dhcp_client_conf, "reqopts", value);
	return 0;
}

int set_DHCPv6Client_RequestedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_args *dhcpv6_client_args = (struct dhcpv6_client_args*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(dhcpv6_client_args->dhcp_client_conf, "reqopts", *value);
			break;
	}
	return 0;
}

int get_DHCPv6Client_ServerNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Client_SentOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Client_ReceivedOptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientServer_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientServer_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientServer_InformationRefreshTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ClientSentOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ClientSentOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ClientSentOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ClientSentOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ClientReceivedOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientReceivedOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ClientReceivedOption_Server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*odhcpd";
	if (check_file(path))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_DHCPv6Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	unsigned char b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmcmd("/etc/init.d/odhcpd", 1, "enable");
			else
				dmcmd("/etc/init.d/odhcpd", 1, "disable");
			break;
	}
    return 0;
}

int get_DHCPv6Server_PoolNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int i= 0;
	char *v= NULL;

	uci_foreach_sections("dhcp", "dhcp", s) {
		dmuci_get_value_by_section_string(s, "dhcpv6", &v);
		if (v!=NULL && strcmp(v, "server") == 0)
			i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{	struct uci_section *s = NULL;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcpv6_args *)data)->interface, s) {
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

int set_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
			uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcpv6_args *)data)->interface, s) {
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

int get_DHCPv6ServerPool_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	char *v= NULL;
	uci_foreach_option_eq("dhcp", "dhcp", "interface", ((struct dhcpv6_args *)data)->interface, s) {
		dmuci_get_value_by_section_string(s, "ignore", &v);
		*value = (v && *v == '1') ? "Disabled" : "Enabled";
		return 0;
	}
	*value="Error_Misconfigured";
	return 0;
}

int get_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcpv6_arg= (struct dhcpv6_args*)data;
	struct uci_section *dmmap_sect;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(dhcpv6_arg->dhcp_sec), &dmmap_sect);
	if (dmmap_sect)
		dmuci_get_value_by_section_string(dmmap_sect, "dhcpv6_serv_pool_alias", value);
	return 0;
}

int set_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_args *dhcpv6_arg= (struct dhcpv6_args*)data;
	struct uci_section *dmmap_sect;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(dhcpv6_arg->dhcp_sec), &dmmap_sect);
			if (dmmap_sect)
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "dhcpv6_serv_pool_alias", value);
	}
	return 0;
}

int get_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcp_arg= (struct dhcpv6_args*)data;
	struct uci_section *dmmap_sect;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(dhcp_arg->dhcp_sec), &dmmap_sect);
	if (dmmap_sect)
		dmuci_get_value_by_section_string(dmmap_sect, "order", value);
	return 0;
}

int set_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_args *dhcp_arg= (struct dhcpv6_args*)data;
	struct uci_section *dmmap_sect;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "dhcp", section_name(dhcp_arg->dhcp_sec), &dmmap_sect);
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			set_section_order("dhcp", "dmmap_dhcpv6", "dhcp", dmmap_sect, dhcp_arg->dhcp_sec, 1, value);
			break;
	}
	return 0;
}

int get_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;
	linker = dmstrdup(((struct dhcpv6_args *)data)->interface);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		*value = "";
	dmfree(linker);
	return 0;
}

int set_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(((struct dhcpv6_args *)data)->dhcp_sec, "interface", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_DHCPv6ServerPool_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_DUIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_DUIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcpv6_client_args = (struct dhcpv6_args*)data;

	struct uci_section *vendorclassidclassifier= get_dhcpv6_classifier("vendorclass", dhcpv6_client_args->interface);
	if(vendorclassidclassifier == NULL)
		return 0;
	dmuci_get_value_by_section_string(vendorclassidclassifier, "vendorclass", value);
	return 0;
}

int set_DHCPv6ServerPool_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_args *dhcpv6_client_args = (struct dhcpv6_args*)data;
	struct uci_section *vendorclassidclassifier= NULL;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			vendorclassidclassifier= get_dhcpv6_classifier("vendorclass", dhcpv6_client_args->interface);
			if(vendorclassidclassifier == NULL)
				return 0;
			dmuci_set_value_by_section(vendorclassidclassifier, "vendorclass", value);
			break;
	}
	return 0;
}

int get_DHCPv6ServerPool_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value="false";
	return 0;
}

int set_DHCPv6ServerPool_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcpv6_client_args = (struct dhcpv6_args*)data;

	struct uci_section *userclassidclassifier= get_dhcpv6_classifier("userclass", dhcpv6_client_args->interface);
	if(userclassidclassifier == NULL)
		return 0;
	dmuci_get_value_by_section_string(userclassidclassifier, "userclass", value);
	return 0;
}

int set_DHCPv6ServerPool_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_args *dhcpv6_client_args = (struct dhcpv6_args*)data;

	struct uci_section *userclassidclassifier;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			userclassidclassifier= get_dhcpv6_classifier("userclass", dhcpv6_client_args->interface);
			if(userclassidclassifier == NULL)
				return 0;
			dmuci_set_value_by_section(userclassidclassifier, "userclass", value);
			break;
	}
	return 0;
}

int get_DHCPv6ServerPool_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= "false";
	return 0;
}

int set_DHCPv6ServerPool_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcpv6_client_args = (struct dhcpv6_args*)data;
	struct uci_section *macaddrclassifier;
	char *mac, *mac1, *mac2, *mac3, *mac4, *mac5, *mac6, **macarray, *res= NULL, *tmp= "";
	int length, i;

	macaddrclassifier= get_dhcpv6_classifier("mac", dhcpv6_client_args->interface);
	if(macaddrclassifier == NULL) {
		*value= "";
		return 0;
	}
	dmuci_get_value_by_section_string(macaddrclassifier, "mac", &mac);
	macarray= strsplit(mac, ":", &length);
	res= (char*)dmcalloc(18, sizeof(char));
	tmp=res;
	for(i= 0; i<6; i++){
		if(strcmp(macarray[i], "*") == 0) {
			sprintf(tmp, "%s", "00");
		} else{
			sprintf(tmp, "%s", macarray[i]);
		}
		tmp+=2;

		if(i<5){
			sprintf(tmp, "%s", ":");
			tmp++;
		}
	}
	dmasprintf(value, "%s", res);
	return 0;
}

int set_DHCPv6ServerPool_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_SourceAddressMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcp_relay_args = (struct dhcpv6_args*)data;
	struct uci_section *macaddrclassifier;
	char *mac, *mac1, *mac2, *mac3, *mac4, *mac5, *mac6, **macarray, *res= NULL, *tmp= "";
	int length, i;

	if(macaddrclassifier == NULL) {
		*value= "";
		return 0;
	}
	dmuci_get_value_by_section_string(macaddrclassifier, "mac", &mac);
	macarray= strsplit(mac, ":", &length);
	res= (char*)dmcalloc(18, sizeof(char));
	tmp=res;
	for(i= 0; i<6; i++){
		if(strcmp(macarray[i], "*") == 0) {
			sprintf(tmp, "%s", "00");
		} else{
			sprintf(tmp, "%s", "FF");
		}
		tmp+=2;

		if(i<5){
			sprintf(tmp, "%s", ":");
			tmp++;
		}
	}
	dmasprintf(value, "%s", res);
	return 0;
}

int set_DHCPv6ServerPool_SourceAddressMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_SourceAddressExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_SourceAddressExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_IANAEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_IANAEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_IANAManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_IANAManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_IANAPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPool_IAPDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_IAPDEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_IAPDManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_IAPDManualPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_IAPDPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPool_IAPDAddLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_IAPDAddLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_ClientNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *dhcp_arg= (struct dhcpv6_args*)data;
	json_object *res, *res1, *jobj, *dev_obj= NULL, *net_obj= NULL;
	struct clientv6_args curr_dhcp_client_args = {0};
	char *device;
	int i= 0;
	char *idx = NULL, *idx_last = NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(dhcp_arg->dhcp_sec), String}}, 1, &res1);
	device = dmjson_get_value(res1, 1, "device");
	dmubus_call("dhcp", "ipv6leases", UBUS_ARGS{}, 0, &res);
	if(!res)
		return 0;
	dev_obj = dmjson_get_obj(res, 1, "device");
	if (!dev_obj)
		return 0;

	net_obj = dmjson_get_obj(dev_obj, 1, device);
	if (!net_obj)
		return 0;

	while (1) {

		jobj = dmjson_select_obj_in_array_idx(net_obj, i, 1, "leases");
		if (jobj == NULL)
			break;
		i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_DHCPv6ServerPool_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_args *curr_dhcp_args = (struct dhcpv6_args*)data;
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

int get_DHCPv6ServerPoolClient_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolClient_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPoolClient_SourceAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClient_Active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClient_IPv6AddressNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client= (struct clientv6_args *)data;
	json_object *address_obj= NULL, *prefix_obj= NULL;
	struct clientv6_args curr_dhcv6_address_args = {0};
	char *idx = NULL, *idx_last = NULL;
	int i= 0;

	while (1) {
		address_obj = dmjson_select_obj_in_array_idx(dhcpv6_serv_pool_client->client, i, 1, "ipv6-addr");
		if (address_obj == NULL)
			break;
		i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_DHCPv6ServerPoolClient_IPv6PrefixNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client= (struct clientv6_args *)data;
	json_object *address_obj= NULL, *prefix_obj= NULL;
	struct clientv6_args curr_dhcv6_address_args = {0};
	char *idx = NULL, *idx_last = NULL;
	int i= 0;

	while (1) {
		address_obj = dmjson_select_obj_in_array_idx(dhcpv6_serv_pool_client->client, i, 1, "ipv6-prefix");
		if (address_obj == NULL)
			break;
		i++;
	}
	dmasprintf(value, "%d", i);
	return 0;
}

int get_DHCPv6ServerPoolClient_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "address");
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "preferred-lifetime");
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "valid-lifetime");
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "address");
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "preferred-lifetime");
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct clientv6_args *dhcpv6_serv_pool_client_ip6address= (struct clientv6_args *)data;
	*value = dmjson_get_value(dhcpv6_serv_pool_client_ip6address->clientparam, 1, "valid-lifetime");
	return 0;
}

int get_DHCPv6ServerPoolClientOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
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

int set_DHCPv6ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
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
							if(!b){
								dmuci_del_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opt_value);
							}
							break;
						}
					}
			}
			if(!test && b){
				dmuci_add_list_value_by_section(dhcp_client_opt_args->client_sect, "dhcp_option", opt_value);
			}

	}
	return 0;
}

int get_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;

	dmuci_get_value_by_section_string(dhcp_client_opt_args->opt_sect, "cwmp_dhcpv6_servpool_option_alias", value);
	return 0;

}

int set_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dhcp_client_opt_args->opt_sect, "cwmp_dhcpv6_servpool_option_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->option_tag);
	return 0;
}

int set_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
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

int get_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
	*value= dmstrdup(dhcp_client_opt_args->value);
	return 0;
}

int set_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcpv6_client_option_args* dhcp_client_opt_args= (struct dhcpv6_client_option_args*)data;
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

int get_DHCPv6ServerPoolOption_PassthroughClient(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolOption_PassthroughClient(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

