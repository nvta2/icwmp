/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include "dmcwmp.h"
#include "dmcommon.h"
#include "dhcpv6.h"

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

int browseDHCPv6ClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	char *instance, *instnbr = NULL, *proto= NULL;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq_no_delete("network", "interface", "dmmap_dhcpv6", "proto", "dhcpv6", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		instance = handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, p->dmmap_section, "cwmp_dhcpv6client_instance", "cwmp_dhcpv6client_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, p->config_section, instance) == DM_STOP)
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
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolClientInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolClientIPv6AddressInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolClientIPv6PrefixInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseDHCPv6ServerPoolClientOptionInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}


int addObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjDHCPv6Client(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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
	//TODO
	return 0;
}

int delObjDHCPv6ServerPool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjDHCPv6ServerPoolOption(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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
	char *v;

	dmuci_get_value_by_section_string((struct uci_section *)data, "disabled", &v);
	if (v==NULL || strlen(v)==0 || strcmp(v, "1")!=0)
		*value= "1";
	else
		*value= "0";
	return 0;
}

int set_DHCPv6Client_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action)	{
	case VALUECHECK:
		if (string_to_bool(value, &b))
			return FAULT_9007;
		return 0;
	case VALUESET:
		string_to_bool(value, &b);
		if (b)
			dmuci_set_value_by_section((struct uci_section *)data, "disabled", "0");
		else
			dmuci_set_value_by_section((struct uci_section *)data, "disabled", "1");
		break;
	}
	return 0;
}

int get_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name((struct uci_section*)data), &s);
	dmuci_get_value_by_section_string(s, "cwmp_dhcpv6client_alias", value);
	return 0;
}

int set_DHCPv6Client_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name((struct uci_section*)data), &s);
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(s, "cwmp_dhcpv6client_alias", value);
			break;
	}
	return 0;
}

int get_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker= dmstrdup(section_name((struct uci_section*)data));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim, dm_delim), linker, value);
	return 0;
}

int set_DHCPv6Client_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s;
	char *linker= NULL, *newvalue= NULL, *v;
	struct uci_section *dm;

	get_dmmap_section_of_config_section("dmmap_dhcpv6", "interface", section_name((struct uci_section*)data), &dm);

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
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dm, "section_name", linker);
			break;
	}
	return 0;
}

int get_DHCPv6Client_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Client_DUID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6Client_RequestAddresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6Client_RequestPrefixes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
	//TODO
	return 0;
}

int set_DHCPv6Client_Renew(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
	//TODO
	return 0;
}

int set_DHCPv6Client_RequestedOptions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
	//TODO
	return 0;
}

int set_DHCPv6Server_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6Server_PoolNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_VendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_VendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
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
	//TODO
	return 0;
}

int set_DHCPv6ServerPool_UserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPool_UserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
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
	//TODO
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
	//TODO
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
	//TODO
	return 0;
}

int get_DHCPv6ServerPool_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
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
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClient_IPv6PrefixNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClient_OptionNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Address_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_Prefix(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_PreferredLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_DHCPv6ServerPoolClientIPv6Prefix_ValidLifetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
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
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolOption_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolOption_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolOption_Tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_DHCPv6ServerPoolOption_Value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

