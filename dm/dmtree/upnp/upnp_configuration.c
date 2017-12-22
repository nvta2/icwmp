#include "dmcwmp.h"
#include "upnp_configuration.h"
#include "upnp_common.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmjson.h"
#include <stdio.h>
#include <stdlib.h>

#define DELIMITOR ","
/**************************************************************************
*
* /UPnP/DM/Configuration/ datamodel tree
*
***************************************************************************/

/*** /UPnP/DM/Configuration/ objects ***/
DMOBJ upnpConfigurationObj[] = {
{"Network",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, upnpConfigurationNetworkObj, upnpConfigurationNetworkParams, NULL},
{0}
};

/*** /UPnP/DM/Configuration/Network objects ***/
DMOBJ upnpConfigurationNetworkObj[] = {
{"IPInterface",&DMWRITE, upnp_configuration_ipinterface_createinstance, upnp_configuration_ipinterface_deleteinstance, NULL, upnp_browseIPInterfaceInst, &DMFINFRM, &DMNONE, upnpConfigurationNetworkIPInterfaceObj, upnpConfigurationNetworkIPInterfaceParams, NULL},
{0}
};

/*** /UPnP/DM/Configuration/Network parameters ***/
DMLEAF upnpConfigurationNetworkParams[]= {
{"HostName", &DMWRITE, DMT_STRING, upnp_configuration_get_hostname, upnp_configuration_set_hostname, &DMFINFRM, NULL},
{"IPInterfaceNumberOfEntries", &DMREAD, DMT_UNINT, upnp_configuration_get_IPInterfaceNumberOfEntries, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/Configuration/Network/IPInterface/#/ objects ***/
DMOBJ upnpConfigurationNetworkIPInterfaceObj[]= {
{"IPv4",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, NULL, upnpConfigurationNetworkIPInterfaceIpv4Params, NULL},
{0}
};

/*** /UPnP/DM/Configuration/Network/IPInterface/#/ parameters ***/
DMLEAF upnpConfigurationNetworkIPInterfaceParams[]= {
{"SystemName", &DMREAD, DMT_STRING, upnp_configuration_get_SystemName, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/Configuration/Network/IPInterface/#/IPv4/ parameters ***/
DMLEAF upnpConfigurationNetworkIPInterfaceIpv4Params[]= {
{"IPAddress", &DMWRITE, DMT_STRING, upnp_configuration_get_ipv4_IPAddress, upnp_configuration_set_ipv4_IPAddress, &DMFINFRM, NULL},
{"AddressingType", &DMWRITE, DMT_STRING, upnp_configuration_get_ipv4_addressingtype, upnp_configuration_set_ipv4_addressingtype, &DMFINFRM, NULL},
{"DNSServers", &DMWRITE, DMT_STRING, upnp_configuration_get_ipv4_dnssevers, upnp_configuration_set_ipv4_dnssevers, &DMFINFRM, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING, upnp_configuration_get_ipv4_subnetmask, upnp_configuration_set_ipv4_subnetmask, &DMFINFRM, NULL},
{"DefaultGateway", &DMWRITE, DMT_STRING, upnp_configuration_get_ipv4_defaultgateway, upnp_configuration_set_ipv4_defaultgateway, &DMFINFRM, NULL},
{0}
};

/**************************************************************************
*
* /UPnP/DM/Configuration/* parameters functions
*
***************************************************************************/

int upnp_configuration_get_hostname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
		return 0;
}

int upnp_configuration_get_IPInterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	int n = upnp_get_IPInterfaceNumberOfEntries();
	dmasprintf(value, "%d", n);
	return 0;
}

int upnp_configuration_get_SystemName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	if(upnp_configargs->systemName !=NULL && strlen(upnp_configargs->systemName)>0) dmasprintf(value, "%s", upnp_configargs->systemName);
	return 0;
}

int upnp_configuration_get_ipv4_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *ipv4addr=NULL;
	dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "ipaddr", &ipv4addr);
	if(ipv4addr!=NULL && strlen(ipv4addr)> 0) dmasprintf(value, "%s", ipv4addr);
	return 0;
}

int upnp_configuration_get_ipv4_addressingtype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	char *proto=NULL;
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "proto", &proto);

	if(proto!=NULL && strlen(proto)>0 && strstr(proto,"dhcp")) dmasprintf(value, "%s", "DHCP");
	else dmasprintf(value, "%s", "Static");
	return 0;
}

int upnp_configuration_get_ipv4_dnssevers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res= NULL;
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *iface_name = section_name(upnp_configargs->upnpConfiguration_sec);
	if(iface_name==NULL || strlen(iface_name)<=0) {
		*value = NULL;
		return 0;
	}
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", iface_name}}, 1, &res);
	if(res == NULL) {
		*value = NULL;
		return 0;
	}
	DM_ASSERT(res, *value = "");

	*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	if ((*value)[0] == '\0') {
		dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "dns", value);
		*value = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
		char *p = *value;
		while (*p) {
			if (*p == ' ' && p != *value && *(p-1) != ',')
				*p++ = ',';
			else
				p++;
		}
	}
	return 0;
}

int upnp_configuration_get_ipv4_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){

	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *netmask = NULL;
	dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "netmask", &netmask);

	if(netmask!=NULL && strlen(netmask)>0) dmasprintf(value, "%s", netmask);
	else *value =NULL;
	return 0;
}

int upnp_configuration_get_ipv4_defaultgateway(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	FILE* fp = NULL;
	struct proc_routing proute = {0};
	char line[MAX_PROC_ROUTING];
	fp = fopen(ROUTING_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTING, fp);
		while (fgets(line, MAX_PROC_ROUTING, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			parse_proc_route_line(line, &proute);
			if(!strcmp(proute.iface, upnp_configargs->systemName)) {
				if(proute.gateway!=NULL && strlen(proute.gateway)>0) dmasprintf(value, "%s", proute.gateway);
				return 0;
			}
			else continue;
		}
		fclose(fp) ;
	}
	return 0;
}

int upnp_configuration_set_hostname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	return 0;
}

int upnp_configuration_set_ipv4_IPAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *proto= NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "proto", &proto);
			if(proto == NULL || strlen(proto)<=0) return 0;
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "ipaddr", value);
			return 0;
	}
	return 0;
}

int upnp_configuration_set_ipv4_addressingtype(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *proto= NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(strcasecmp(value, "static") == 0) {
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "proto", "static");
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "ipaddr", "0.0.0.0");
			}
			if(strcasecmp(value, "dhcp") == 0) {
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "proto", "dhcp");
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "ipaddr", "");
			}
			return 0;
	}
	return 0;
}

int upnp_configuration_set_ipv4_dnssevers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *dup= NULL, *p= NULL;

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
			dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "dns", dup);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int upnp_configuration_set_ipv4_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	char *proto = NULL;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(upnp_configargs->upnpConfiguration_sec, "proto", &proto);
			if(proto == NULL || strlen(proto)<=0) return 0;
			if(strcmp(proto, "static") == 0)
				dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "netmask", value);
			return 0;
	}
	return 0;
}

int upnp_configuration_set_ipv4_defaultgateway(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(upnp_configargs->upnpConfiguration_sec, "gateway", value);
			return 0;
	}
	return 0;
}


int upnp_configuration_ipinterface_createinstance(char *refparam, struct dmctx *ctx, void *data, char **instance){
	char *value=NULL;
	char *iface_instance=NULL, ib[8], ip_name[32];
	char *p = ip_name;
	struct uci_section *iface_sec = NULL;
	iface_instance = get_last_instance("network","interface","upnp_ip_iface_instance");
	sprintf(ib, "%d", iface_instance ? atoi(iface_instance)+1 : 1);
	dmstrappendstr(p, "ip_interface_");
	dmstrappendstr(p, ib);
	dmstrappendend(p);
	dmuci_add_section("network", "interface", &iface_sec, &value);
	dmuci_set_value("network", ip_name, "", "interface");
	dmuci_set_value("network", ip_name, "proto", "dhcp");
	*instance = update_instance(iface_sec, iface_instance, "upnp_ip_iface_instance");
	return 0;
}

int upnp_configuration_ipinterface_deleteinstance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action){
	struct upnp_configuration_args *upnp_configargs = (struct upnp_configuration_args *)data;
	switch (del_action) {
	case DEL_INST:
		if(upnp_configargs->upnpConfiguration_sec == NULL) return FAULT_9005;
		dmuci_delete_by_section(upnp_configargs->upnpConfiguration_sec, NULL, NULL);
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int upnp_browseIPInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance){
	char *value1, *value2;
	struct uci_section *net_sec = NULL;
	char *iface_int = NULL, *iface_int_last = NULL;
	struct upnp_configuration_args currUpnpConfigurationArgs = {0};
	char *type=NULL, *ipv4addr = "", *ipv6addr = "", *proto, *inst;
	char *value= NULL, *netmask = NULL;
	json_object *res = NULL, *res1 = NULL;

	uci_foreach_sections("network", "interface", net_sec) {
		currUpnpConfigurationArgs.upnpConfiguration_sec = net_sec;
		currUpnpConfigurationArgs.systemName = section_name(net_sec);
		dmuci_get_value_by_section_string(net_sec, "netmask", &netmask);

		iface_int = handle_update_instance(1, dmctx, &iface_int_last, update_instance_alias, 3, net_sec, "upnp_ip_iface_instance", "upnp_ip_iface_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&currUpnpConfigurationArgs, iface_int) == DM_STOP) break;

	}
	return 0;
}
