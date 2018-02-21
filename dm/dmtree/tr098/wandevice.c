/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */
 
#include <ctype.h>
#include <uci.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "wandevice.h"
#include "landevice.h"
#include "dmjson.h"

#define WAN_DEVICE 3
#define WAN_INST_ETH 1
#define WAN_INST_ATM 2
#define WAN_INST_PTM 3


/*** WANDevice.{i}. ***/
DMOBJ tWANDeviceObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
//dynamic
{"WANConnectionDevice", &DMWANConnectionDevice, add_wan_wanconnectiondevice, delete_wan_wanconnectiondevice, NULL, browsewanconnectiondeviceInst, &DMFINFRM, &DMWANConnectionDevicenotif, tWANConnectionDeviceObj, tWANConnectionDeviceParam, NULL},
{"WANCommonInterfaceConfig", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWANCommonInterfaceConfigParam, NULL},
#ifndef EX400
{"WANDSLInterfaceConfig", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWANDSLInterfaceConfigParam, NULL},
#endif
{"WANEthernetInterfaceConfig", &DMREAD, NULL, NULL, check_wan_is_ethernet, NULL, NULL, NULL, tWANEthernetInterfaceConfigObj, tWANEthernetInterfaceConfigParam, NULL},
{0}
};

DMLEAF tWANDeviceParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wan_dev_alias, set_wan_dev_alias, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANConnectionDevice.{i}. ***/
DMOBJ tWANConnectionDeviceObj[] = {
{"WANIPConnection", &DMWRITE, add_wan_wanipconnection, delete_wan_wanipconnectiondevice, NULL, browsewanprotocolconnectionipInst, &DMFINFRM, &DMWANConnectionDevicenotif, tWANConnectionObj, tWANIPConnectionParam, get_protocol_connection_linker},
{"WANPPPConnection", &DMWRITE, add_wan_wanpppconnection, delete_wan_wanpppconnectiondevice, NULL, browsewanprotocolconnectionpppInst, &DMFINFRM, &DMWANConnectionDevicenotif, tWANConnectionObj, tWANPPPConnectionParam, get_protocol_connection_linker},
//Depend on wandev type
#ifndef EX400
{"WANDSLLinkConfig", &DMREAD, NULL, NULL, check_wan_is_atm, NULL, NULL, NULL, NULL, tWANDSLLinkConfigParam, NULL},
#endif
{0}
};

DMLEAF tWANConnectionDeviceParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wan_con_dev_alias, set_wan_con_dev_alias, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}. ***/
/*** WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}. ***/
DMOBJ tWANConnectionObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
//dynamic
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWANConnectionStatsParam, NULL},
{"X_INTENO_COM_VLAN", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWANConnection_VLANParam, NULL},
{0}
};

DMLEAF tWANIPConnectionParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wan_ip_con_alias, set_wan_ip_con_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_interface_enable_wanproto, set_interface_enable_wanproto, NULL, NULL},
{"ConnectionStatus", &DMREAD, DMT_STRING, get_wan_device_mng_status, NULL, NULL, NULL},
{"ExternalIPAddress", &DMREAD, DMT_STRING, get_wan_device_mng_interface_ip, NULL, &DMWANConnectionProtocolinform, &DMWANConnectionDevicenotif}, //TO CHECK
{"MACAddress", &DMREAD, DMT_STRING, get_wan_device_mng_interface_mac, NULL, NULL, NULL},
{"ConnectionType", &DMWRITE, DMT_STRING, get_wan_ip_link_connection_connection_type, set_wan_ip_link_connection_connection_type, NULL, NULL},
{"AddressingType", &DMWRITE, DMT_STRING, get_wan_ip_link_connection_addressing_type, set_wan_ip_link_connection_addressing_type, NULL, NULL},
{"NATEnabled", &DMWRITE, DMT_BOOL, get_wan_ip_link_connection_nat_enabled, set_wan_ip_link_connection_nat_enabled, NULL, NULL},
{"X_BROADCOM_COM_FirewallEnabled", &DMWRITE, DMT_BOOL, get_interface_firewall_enabled_wanproto, set_interface_firewall_enabled_wanproto, NULL, NULL},
{"X_BROADCOM_COM_IGMPEnabled", &DMWRITE, DMT_BOOL, get_wan_ip_link_connection_igmp_enabled, set_wan_ip_link_connection_igmp_enabled, NULL, NULL},
{"DNSEnabled", &DMWRITE, DMT_BOOL, get_wan_ip_link_connection_dns_enabled, set_wan_ip_link_connection_dns_enabled, NULL, NULL},
{"DNSOverrideAllowed", &DMREAD, DMT_STRING, get_empty, NULL, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_wan_ip_link_connection_name, set_wan_ip_link_connection_connection_name, NULL, NULL},
{0}
};

DMLEAF tWANPPPConnectionParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wan_ppp_con_alias, set_wan_ppp_con_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_interface_enable_wanproto, set_interface_enable_wanproto, NULL, NULL},
{"ConnectionStatus", &DMREAD, DMT_STRING, get_wan_device_ppp_status, NULL, NULL, NULL},
//{"ExternalIPAddress", &DMREAD, DMT_STRING, get_wan_device_ppp_interface_ip, NULL, &DMWANConnectionProtocolinform, NULL}, //TO CHECK
{"MACAddress", &DMREAD, DMT_STRING, get_wan_device_mng_interface_mac, NULL, NULL, NULL},
{"Username", &DMWRITE, DMT_STRING, get_wan_device_ppp_username, set_wan_device_username, NULL, NULL},
{"Password", &DMWRITE, DMT_STRING, get_empty, set_wan_device_password, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_wan_ip_link_connection_name, set_wan_ip_link_connection_connection_name, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.Stats. ***/
/*** WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.Stats. ***/
DMLEAF tWANConnectionStatsParam[] = {
{"EthernetBytesReceived", &DMREAD, DMT_UNINT, get_wan_link_connection_eth_bytes_received, NULL, NULL, NULL},
{"EthernetBytesSent", &DMREAD, DMT_UNINT, get_wan_link_connection_eth_bytes_sent, NULL, NULL, NULL},
{"EthernetPacketsReceived", &DMREAD, DMT_UNINT, get_wan_link_connection_eth_pack_received, NULL, NULL, NULL},
{"EthernetPacketsSent", &DMREAD, DMT_UNINT, get_wan_link_connection_eth_pack_sent, NULL, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.X_INTENO_COM_VLAN. ***/
/*** WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.X_INTENO_COM_VLAN. ***/
DMLEAF tWANConnection_VLANParam[] = {
{"VLANID", &DMWRITE, DMT_UNINT, get_wan_ip_link_connection_vid, set_wan_ip_link_connection_vid, NULL, NULL},
{"VLANPriority", &DMWRITE, DMT_UNINT, get_wan_ip_link_connection_vpriority, set_wan_ip_link_connection_vpriority, NULL, NULL},
{"Layer2Interface", &DMWRITE, DMT_STRING, get_wan_ip_link_connection_layer2_interface, set_wan_ip_link_connection_layer2_interface, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig. ***/
DMLEAF tWANDSLLinkConfigParam[] = {
#ifndef EX400
{"Enable", &DMREAD, DMT_BOOL, get_wan_dsl_link_config_enable, NULL, NULL, NULL},
{"DestinationAddress", &DMWRITE, DMT_STRING, get_wan_dsl_link_config_destination_address, set_wan_dsl_link_config_destination_address, NULL, NULL},
{"ATMEncapsulation", &DMWRITE, DMT_STRING, get_wan_dsl_link_config_atm_encapsulation, set_wan_dsl_link_config_atm_encapsulation, NULL, NULL},
#endif
{0}
};

/*** WANDevice.{i}.WANCommonInterfaceConfig. ***/
DMLEAF tWANCommonInterfaceConfigParam[] = {
{"WANAccessType", &DMREAD, DMT_STRING, get_wan_device_wan_access_type, NULL, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANDSLInterfaceConfig. ***/
DMLEAF tWANDSLInterfaceConfigParam[] = {
#ifndef EX400
{"Status", &DMREAD, DMT_STRING, get_wan_device_wan_dsl_interface_config_status, NULL, NULL, NULL},
{"ModulationType", &DMREAD, DMT_STRING, get_wan_device_wan_dsl_interface_config_modulation_type, NULL, NULL, NULL},
{"DataPath", &DMREAD, DMT_STRING, get_wan_device_dsl_datapath, NULL, NULL, NULL},
{"DownstreamCurrRate", &DMREAD, DMT_UNINT, get_wan_device_dsl_downstreamcurrrate, NULL, NULL, NULL},
{"DownstreamMaxRate", &DMREAD, DMT_UNINT, get_wan_device_dsl_downstreammaxrate, NULL, NULL, NULL},
{"DownstreamAttenuation", &DMREAD, DMT_INT, get_wan_device_dsl_downstreamattenuation, NULL, NULL, NULL},
{"DownstreamNoiseMargin", &DMREAD, DMT_INT, get_wan_device_dsl_downstreamnoisemargin, NULL, NULL, NULL},
{"UpstreamCurrRate", &DMREAD, DMT_UNINT, get_wan_device_dsl_upstreamcurrrate, NULL, NULL, NULL},
{"UpstreamMaxRate", &DMREAD, DMT_UNINT, get_wan_device_dsl_upstreammaxrate, NULL, NULL, NULL},
{"UpstreamAttenuation", &DMREAD, DMT_INT, get_wan_device_dsl_upstreamattenuation, NULL, NULL, NULL},
{"UpstreamNoiseMargin", &DMREAD, DMT_INT, get_wan_device_dsl_upstreamnoisemargin, NULL, NULL, NULL},
{"X_INTENO_SE_AnnexMEnable", &DMWRITE, DMT_BOOL, get_annexm_status, set_annexm_enable, NULL, NULL},
#endif
{0}
};

/*** WANDevice.{i}.WANEthernetInterfaceConfig. ***/
DMOBJ tWANEthernetInterfaceConfigObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
//dynamic
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWANEthernetInterfaceConfigStatsParam, NULL},
{0}
};

DMLEAF tWANEthernetInterfaceConfigParam[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_wan_eth_intf_enable, set_wan_eth_intf_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_wan_eth_intf_status, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_wan_eth_intf_mac, NULL, NULL, NULL},
{0}
};

/*** WANDevice.{i}.WANEthernetInterfaceConfig.Stats. ***/
DMLEAF tWANEthernetInterfaceConfigStatsParam[] = {
{"BytesSent", &DMREAD, DMT_UNINT, get_wan_eth_intf_stats_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_wan_eth_intf_stats_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_wan_eth_intf_stats_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_wan_eth_intf_stats_rx_packets, NULL, NULL, NULL},
{0}
};


enum WAN_TYPE_CONNECTION {
	WAN_IP_CONNECTION,
	WANPPPConnection
};

enum enum_wan_dsl {
	WAN_DSL_NODSL,
	WAN_DSL_ADSL,
	WAN_DSL_VDSL
};

enum enum_wandevice_idx {
	WAN_IDX_ETH,
	WAN_IDX_ATM,
	WAN_IDX_PTM
};

enum enum_wan_proto {
	WAN_PROTO_NIL,
	WAN_PROTO_PPP,
	WAN_PROTO_IP
};

#ifndef EX400
struct wan_device 
{
	char *instance;
	char *fdev;
	char *stype;
	char *cdev;
};

struct wan_device wan_devices[WAN_DEVICE] = {
	[WAN_IDX_ETH] = {"1", "eth0", "ethport", "ports"},
	[WAN_IDX_ATM] = {"2", "atm", "atm-device", "dsl"},
	[WAN_IDX_PTM] = {"3", "ptm", "ptm-device", "dsl"}
};
#endif

static char *default_wan;
static char *default_wan_ifname;
static int default_wan_proto;
static char *eth_wan = NULL;

/**************************************************************************
* LINKER
***************************************************************************/

int get_protocol_connection_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {

	if (data && ((struct wanargs *)data)->wancprotosection)
	{
		dmasprintf(linker, "linker_interface:%s", section_name(((struct wanargs *)data)->wancprotosection));
		return 0;
	}
	*linker = "";
	return 0;
}

inline int init_wanargs(struct wanargs *args, int wan_instance, char *fdev, struct uci_section *s)
{
	args->instance = wan_instance;
	args->fdev = fdev;
	args->wandevsection = s;
	return 0;
}

inline int init_wancprotoargs(struct wanargs *args, struct uci_section *s)
{
	args->wancprotosection = s;
	return 0;
}

inline int init_wancdevargs(struct wanargs *args, struct uci_section *s, char *fwan, char *iwan, char *wan_ifname)
{
	args->wancdsection = s;
	args->fwan = fwan;
	args->iwan = iwan;
	args->wan_ifname = wan_ifname;
	return 0;
}

inline int add_wvlan(char *baseifname, char *ifname, char *vid, char *prioprity, char *wan_name) {
	struct uci_section *ss = NULL, *vlan_interface_s;
	char *add_value;

	uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "wan_name", wan_name, ss) {
		dmuci_set_value_by_section(ss, "wan_name", wan_name);
		dmuci_set_value_by_section(ss, "baseifname", baseifname);
		dmuci_set_value_by_section(ss, "ifname", ifname);
		dmuci_set_value_by_section(ss, "vlan8021q", vid);
		dmuci_set_value_by_section(ss, "vlan8021p", prioprity);
		return 0;
	}
	dmuci_add_section("layer2_interface_vlan", "vlan_interface", &vlan_interface_s, &add_value);
	dmuci_set_value_by_section(vlan_interface_s, "wan_name", wan_name);
	dmuci_set_value_by_section(vlan_interface_s, "baseifname", baseifname);
	dmuci_set_value_by_section(vlan_interface_s, "ifname", ifname);
	dmuci_set_value_by_section(vlan_interface_s, "vlan8021q", vid);
	dmuci_set_value_by_section(vlan_interface_s, "vlan8021p", prioprity);
	return 0;
}

void set_bridge_layer2(struct dmctx *ctx, char *bridge, struct wanargs *wandcprotoargs)
{
	char *wifname, *dup, *pch, *spch;
	struct uci_section *s;

	dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "ifname", &wifname);
	dup = dmstrdup(wifname); // MEM will be freed in the DMMCLEAN
	for (pch = strtok_r(dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch))
	{
		if (atoi(pch + 5) > 1) {
			uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", pch, s)
			{
				dmuci_set_value_by_section(s, "bridge", bridge);
			}
		}
#ifndef EX400
		else if (strstr(pch, "atm")) {
			uci_foreach_option_eq("network", "device", "name", pch, s)
			{
				dmuci_set_value_by_section(s, "bridge", bridge);
			}
		}
		else if (strstr(pch, "ptm")) {
			uci_foreach_option_eq("network", "device", "name", pch, s)
			{
				dmuci_set_value_by_section(s, "bridge", bridge);
			}
		}
#endif
		else if (strstr(pch, eth_wan)) {
#ifdef EX400

			uci_foreach_option_eq("network", "interface", "ifname", pch, s)
			{
				dmuci_set_value_by_section(s, "bridge", bridge);
			}

#else
			uci_foreach_option_eq("network", "device", "name", pch, s)
			{
				dmuci_set_value_by_section(s, "bridge", bridge);
			}
#endif
		}
	}
}

int check_multiwan_interface(struct uci_section *interface_section, char *fwan)
{
	char *ifname, *type, *device = NULL, *dup, *pch, *spch;
	json_object *res;
	int cn = 0;
	
	dmuci_get_value_by_section_string(interface_section, "type", &type);
	dmuci_get_value_by_section_string(interface_section, "ifname", &ifname);
	dup = dmstrdup(ifname);
	pch = strtok_r(dup, " ", &spch);
	while (pch != NULL) {
		if (strstr(pch, "atm"))
			cn++;
		if (strstr(pch, "ptm"))
			cn++;
		if (strstr(pch, eth_wan))
			cn++;
		pch = strtok_r(NULL, " ", &spch);
	}
	dmfree(dup);
	if (type[0] == '\0' || cn < 2)
		return 0;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(interface_section), String}}, 1, &res);
	if (res) {
		device = dmjson_get_value(res, 1, "device");
	}
	if (!res || device[0] == '\0') {
		cn = 0;
		dup = dmstrdup(ifname);
		pch = strtok_r(dup, " ", &spch);
		while (pch != NULL) {
			if (strstr(pch, "atm")) {
				cn++;
				break;
			}
			if (strstr(pch, "ptm")) {
				cn++;
				break;
			}
			if (strstr(pch, eth_wan)) {
				cn++;
				break;
			}
			pch = strtok_r(NULL, " ", &spch);
		}
		dmfree(dup);
		if (cn && strstr(pch, fwan))
			return 0;
		return -1;
	}
	else if (strstr(device, fwan)) {
		return 0;
	}
	else if(strcmp(type, "bridge") == 0 && strcmp(device, "br-wan") == 0)
	{
		cn = 0;
		dup = dmstrdup(ifname);
		for (pch = strtok_r(dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
			if (strstr(pch, "atm")) {
				cn++;
				break;
			}
			if (strstr(pch, "ptm")) {
				cn++;
				break;
			}
			if (strstr(pch, eth_wan)) {
				cn++;
				break;
			}
			pch = strtok_r(NULL, " ", &spch);
		}
		dmfree(dup);
		if (cn && strstr(pch, fwan)) {
			return 0;
		}
		return -1;
	}
	return -1;
}

/****** ADD-DEL OBJECT *******************/
char *get_last_instance_proto(char *package, char *section, char *opt_inst, char *opt_check, char *value_check, char *opt_check1, int value_check1)
{
	struct uci_section *s;
	char *instance = NULL;
	char *value = NULL;
	int proto = -1;
	
	uci_foreach_option_cont(package, section, opt_check, value_check, s) {
		dmuci_get_value_by_section_string(s, opt_check1, &value);
		if (strstr(value, "ppp"))
			proto = WAN_PROTO_PPP;
		else if (strcmp(value, "dhcp") == 0 || strcmp(value, "static") == 0)
			proto = WAN_PROTO_IP;
		else
			proto = WAN_PROTO_NIL;
		if (proto == value_check1) {
			instance = update_instance(s, instance, opt_inst);
		}		
	}
	return instance;
}

int add_wan_wanconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
#ifndef EX400
	char *instance;
	char *device = NULL;
	char *instance_update = NULL;
	struct wanargs *wandargs = (struct wanargs *)data;

	if (wandargs->instance == WAN_INST_ATM) {
		instance = get_last_instance_lev2("dsl", "atm-device", "waninstance", "device", "atm");
		dmasprintf(&device, "atm%d", instance ? atoi(instance) : 0);
		dmasprintf(&instance_update, "%d", instance ? atoi(instance)+ 1 : 1);
		dmuci_set_value("dsl", device, "", "atm-device");
		dmuci_set_value("dsl", device, "name", "ATM");
		dmuci_set_value("dsl", device, "vpi", "8");
		dmuci_set_value("dsl", device, "vci", "35");
		dmuci_set_value("dsl", device, "device", device);
		dmuci_set_value("dsl", device, "link_type", "eoa");
		dmuci_set_value("dsl", device, "encapsulation", "llc");
		dmuci_set_value("dsl", device, "qos_class", "ubr");
		*instancepara = dmuci_set_value("dsl", device, "waninstance", instance_update);
		return 0;
	}
	else if (wandargs->instance == WAN_INST_PTM) {
		instance = get_last_instance_lev2("dsl", "ptm-device", "waninstance", "device", "ptm");
		dmasprintf(&device, "ptm%d", instance ? atoi(instance) : 0);
		dmasprintf(&instance_update, "%d", instance ? atoi(instance)+ 1 : 1);
		dmuci_set_value("dsl", device, "", "ptm-device");
		dmuci_set_value("dsl", device, "name", "PTM");
		dmuci_set_value("dsl", device, "device", device);
		dmuci_set_value("dsl", device, "priority", "1");
		dmuci_set_value("dsl", device, "portid", "1");
		*instancepara = dmuci_set_value("dsl", device, "waninstance", instance_update);
		return 0;
	}
	return FAULT_9005;
#else
	return FAULT_9008;
#endif
}

int delete_wan_wanconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
#ifndef EX400
	struct uci_section *s = NULL; 
	struct uci_section *ss = NULL;
	struct wanargs *wandargs;
	struct wanargs *wandcdevargs;

	switch (del_action) {
		case DEL_INST:
			wandcdevargs = (struct wanargs *)data;
			dmuci_delete_by_section(wandcdevargs->wancdsection, NULL, NULL);
			uci_foreach_option_cont("network", "interface", "ifname", wandcdevargs->fwan, s) {
				if (ss)
					wan_remove_dev_interface(ss, wandcdevargs->fwan);
				ss = s;
			}
			if (ss != NULL)
				wan_remove_dev_interface(ss, wandcdevargs->fwan);
			return 0;
		case DEL_ALL:
			wandargs = (struct wanargs *)data;
			uci_foreach_option_cont(wan_devices[wandargs->instance - 1].cdev, wan_devices[wandargs->instance - 1].stype, "device", wandargs->fdev, s) {
				if (ss)
					dmuci_delete_by_section(ss, NULL, NULL);
				ss = s;
			}
			if (ss != NULL)
				dmuci_delete_by_section(ss, NULL, NULL);

			ss = NULL;
			uci_foreach_option_cont("network", "interface", "ifname", wandargs->fdev, s) {
				if (ss)
					wan_remove_dev_interface(ss, wandargs->fdev);
				ss = s;
			}
			if (ss != NULL)
				wan_remove_dev_interface(ss, wandargs->fdev);
			return 0;
	}
	return 0;
#else
	return FAULT_9008;
#endif
}

int add_wan_wanipconnection(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	struct wanargs *wandcdevargs = (struct wanargs *)data;
	char sname[16] = {0};
	char ifname[8] = {0};
	char *instance;
	
	instance = get_last_instance_proto("network", "interface", "conipinstance", "ifname", wandcdevargs->fwan, "proto", WAN_PROTO_IP);
	dmasprintf(instancepara, "%d", instance ? atoi(instance) + 1 : 1); //MEM WILL BE FREED IN DMMEMCLEAN
#ifdef EX400
	sprintf(sname,"wan_1_%s_%d_%s", wandcdevargs->iwan, WAN_IP_CONNECTION, *instancepara);

#else
	sprintf(sname,"wan_%s_%s_%d_%s", wan_devices[wandcdevargs->instance].instance, wandcdevargs->iwan, WAN_IP_CONNECTION, *instancepara);
#endif
	sprintf(ifname, "%s.1", wandcdevargs->fwan);
	dmuci_set_value("network", sname, NULL, "interface");
	dmuci_set_value("network", sname, "ifname", ifname);
	dmuci_set_value("network", sname, "proto", "dhcp");
	dmuci_set_value("network", sname, "conipinstance", *instancepara);
	return 0;
}

int delete_wan_wanipconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	char *ifname, *iproto;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct wanargs *wandcdevargs = (struct wanargs *)data;

	switch (del_action) {
		case DEL_INST:
			dmuci_delete_by_section(wandcdevargs->wancprotosection, NULL, NULL);
			return 0;
		case DEL_ALL:
			dmuci_get_value_by_section_string(wandcdevargs->wancdsection, "ifname", &ifname);
			uci_foreach_option_eq("network", "interface", "ifname", ifname, s) {
				dmuci_get_value_by_section_string(s, "proto", &iproto);
				if (strcmp(iproto, "dhcp") == 0 || strcmp(iproto, "static") == 0) {
					if (ss)
						dmuci_delete_by_section(ss, NULL, NULL);
					ss = s;
				}
			}
			if (ss != NULL)
				dmuci_delete_by_section(ss, NULL, NULL);
			return 0;
	}
	return 0;
}

int add_wan_wanpppconnection(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	struct wanargs *wandcdevargs = (struct wanargs *)data;
	char sname[16] = {0};
	char ifname[8] = {0};
	char *instance;

	instance = get_last_instance_proto("network", "interface", "conpppinstance", "ifname", wandcdevargs->fwan, "proto", WAN_PROTO_PPP);
	dmasprintf(instancepara, "%d", instance ? atoi(instance) + 1 : 1);
#ifdef EX400
	sprintf(sname,"wan_1_%s_%d_%s", wandcdevargs->iwan, WANPPPConnection, *instancepara);

#else
	sprintf(sname,"wan_%s_%s_%d_%s", wan_devices[wandcdevargs->instance].instance, wandcdevargs->iwan, WANPPPConnection, *instancepara);
#endif
	sprintf(ifname, "%s.1", wandcdevargs->fwan);
	dmuci_set_value("network", sname, NULL, "interface");
	dmuci_set_value("network", sname, "ifname", ifname);
	dmuci_set_value("network", sname, "proto", "pppoe");
	dmuci_set_value("network", sname, "conpppinstance", *instancepara); //MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int delete_wan_wanpppconnectiondevice(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *ifname, *iproto;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct wanargs *wandcdevargs = (struct wanargs *)data;

	switch (del_action) {
			case DEL_INST:
				dmuci_delete_by_section(wandcdevargs->wancprotosection, NULL, NULL);
				return 0;
			case DEL_ALL:
				dmuci_get_value_by_section_string(wandcdevargs->wancdsection, "ifname", &ifname);
				uci_foreach_option_eq("network", "interface", "ifname", ifname, s) {
					dmuci_get_value_by_section_string(s, "proto", &iproto);
					if (strstr(iproto, "ppp")) { //CHECK IF WE CAN OPTIMISE AND IF iproto can be pppoa
						if (ss)
							dmuci_delete_by_section(ss, NULL, NULL);
						ss = s;
					}
				}
				if (ss != NULL)
					dmuci_delete_by_section(ss, NULL, NULL);
				return 0;
		}
	return 0;
}
/********************/

/************************************************************************** 
**** ****  function related to get_wandevice_wandevice_parameters  **** ****
***************************************************************************/

int get_wan_device_wan_dsl_traffic()
{
	json_object *res;
	int dsl = WAN_DSL_NODSL;
	char *str;

	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	if (!res) 
		return dsl;
	str = dmjson_get_value(res, 2, "dslstats", "traffic");
	if (str) {
		if (strstr(str, "ATM")) {
			dsl = WAN_DSL_ADSL;
		}
		else if (strstr(str, "PTM")) {
			dsl = WAN_DSL_VDSL;
		}
	}
	return dsl;
}

int get_wan_device_wan_access_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	switch(wandargs->instance) {
		case WAN_INST_ETH:
			*value = "Ethernet";
			break;
		case WAN_INST_ATM:
		case WAN_INST_PTM:
			*value = "DSL";
			break;
		default:
			*value = "";
			break;
	}
	return 0;
}

int get_wan_device_wan_dsl_interface_config_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *status;
	int dsl;
	json_object *res;

	*value = "";
	if (wandargs->instance == WAN_INST_ETH)
		*value = "NoSignal Not a dsl interface";
	else {
		json_object *res = NULL;
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			*value = "";
			return 0;
		}

		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "");
		status = dmjson_get_value(res, 2, "dslstats", "status");
		if (strcmp(status, "Showtime") == 0)
			*value = "Up";
		else if (strcmp(status, "Training") == 0)
			*value = "Initializing";
		else if (strcmp(status, "Channel Analysis") == 0)
			*value = "EstablishingLink";
		else if (strcmp(status, "Disabled") == 0)
			*value = "Disabled";
		else 
			*value = "NoSignal";
	}	
end:
	return 0;
}

int get_wan_device_wan_dsl_interface_config_modulation_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *mode;
	int dsl;
	json_object *res = NULL;
	*value = "";

	if (wandargs->instance == WAN_INST_ETH)
		*value = "Not a dsl interface";
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			*value = "";
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "");
		mode = dmjson_get_value(res, 2, "dslstats", "mode");
		if (strcmp(mode, "G.Dmt") == 0)
			*value = "ADSL_G.dmt";
		else if (strcmp(mode, "G.lite") == 0)
			*value = "ADSL_G.lite";
		else if (strcmp(mode, "T1.413") == 0)
			*value = "ADSL_ANSI_T1.413";
		else if (strcmp(mode, "ADSL2") == 0)
			*value = "ADSL_G.dmt.bis";
		else if (strcmp(mode, "AnnexL") == 0)
			*value = "ADSL_re-adsl";
		else if (strcmp(mode, "ADSL2+") == 0)
			*value = "ADSL_2plus";
		else
			*value = mode;
	}
	return 0;
}

int get_wan_device_dsl_datapath(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char buf[512], *val = "", *pch, *spch, *pch2, *spch2, *dup;
	int dsl, pp, r;
	*value = "None";

	if (wandargs->instance != WAN_INST_ETH) {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}

		buf[0] = '\0';
		pp = dmcmd("adsl", 2, "info", "--state"); //TODO wait ubus command
		if (pp) {
			r = dmcmd_read(pp, buf, 512);
			close(pp);
		}
		for (pch = strtok_r(buf, "\n\r", &spch); pch; pch = strtok_r(NULL, "\n\r", &spch)) {
			if (!strstr(pch, "Upstream rate"))
				continue;
			if (strstr(pch, "Max:"))
				continue;
			dup = dmstrdup(pch);
			pch2 = strtok_r(dup, " \t", &spch2);
			pch2 = strtok_r(NULL, " \t", &spch2);
			if (pch2 != NULL) {
				if (strcasecmp(pch2, "FAST") == 0) {
					*value = "Fast";
				}
				else if (strcasecmp(pch2, "INTR") == 0) {
					*value = "Interleaved";
				}
				else {
					*value = "None";
				}
			}
			dmfree(dup);
		}
	}
	return 0;
}

int get_wan_device_dsl_downstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *rate_down;
	int dsl;
	json_object *res = NULL;
	json_object *sub_obj= NULL;
	json_object *sub_obj_2= NULL;
	*value = "0";

	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			rate_down = dmjson_get_value(sub_obj_2, 1, "rate_down");
		}
		else 
			return 0;
		if (rate_down && rate_down[0] != '\0') {
			*value = rate_down;
		}
	}
	return 0;
}

int get_wan_device_dsl_downstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *max_down;
	int dsl;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			max_down = dmjson_get_value(sub_obj_2, 1, "max_rate_down");
		}
		else 
			return 0;
		if (max_down && max_down[0] != '\0') {
			*value = max_down;
		}
	}
	return 0; 	
}

int get_wan_device_dsl_downstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *attn_down_x100;
	int dsl;
	json_object *res = NULL;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		attn_down_x100 = dmjson_get_value(res, 2, "dslstats", "attn_down_x100");
		if (attn_down_x100) {
			dmasprintf(&attn_down_x100, "%d", (atoi(attn_down_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = attn_down_x100;
		}
	}
	return 0;
}

int get_wan_device_dsl_downstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *snr_down_x100;
	int dsl;
	json_object *res;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		snr_down_x100 = dmjson_get_value(res, 2, "dslstats", "snr_down_x100");
		if (snr_down_x100) {
			dmasprintf(&snr_down_x100, "%d", (atoi(snr_down_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = snr_down_x100;
		}
	}
	return 0;
}

int get_wan_device_dsl_upstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *rate_up;
	int dsl;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			rate_up = dmjson_get_value(sub_obj_2, 1, "rate_up");
		}
		else 
			return 0;
		*value = rate_up;
	}
	return 0;
}

int get_wan_device_dsl_upstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *max_up;
	int dsl;
	json_object *res = NULL; 
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			max_up = dmjson_get_value(sub_obj_2, 1, "max_rate_up");
		}
		else 
			return 0;
		*value = max_up;
	}
	return 0;
}

int get_wan_device_dsl_upstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *attn_up_x100;
	int dsl;
	json_object *res = NULL;
	*value = "0";
	if (wandargs->instance == WAN_INST_ETH)
		return 0;
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		attn_up_x100 = dmjson_get_value(res, 2, "dslstats", "attn_up_x100");
		if (attn_up_x100) {
			dmasprintf(&attn_up_x100, "%d", (atoi(attn_up_x100) / 10)); // MEM WILL BE FREED IN DMMEMCLEAN
			*value = attn_up_x100;
		}
	}
	return 0;
}

int get_wan_device_dsl_upstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	char *snr_up_x100;
	int dsl;
	json_object *res;
	if (wandargs->instance == WAN_INST_ETH) {
		*value = "0";
		return 0;
	}
	else {
		dsl = get_wan_device_wan_dsl_traffic();
		if (!(wandargs->instance == WAN_INST_ATM && dsl == WAN_DSL_ADSL) &&
			!(wandargs->instance == WAN_INST_PTM && dsl == WAN_DSL_VDSL) ) {
			*value = "0";
			return 0;
		}
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		snr_up_x100 = dmjson_get_value(res, 2, "dslstats", "snr_up_x100");
		if (snr_up_x100) {
			dmasprintf(&snr_up_x100, "%d", (atoi(snr_up_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = snr_up_x100;
		}
		else {
			*value = "0";
		}
	}
	return 0;
}

int get_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	struct uci_list *v;
	struct uci_element *e;
	*value = "0";
	
	if (wandargs->instance == WAN_INST_ATM) {
		dmuci_get_option_value_list("dsl","line","mode", &v);
		if (v) {
			uci_foreach_element(v, e) {
				if (strcasecmp(e->name, "AnnexM") == 0) {
					*value = "1";
					return 0;
				}
			}
		}
	}
	return 0;
}

int set_annexm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_list *v;
	struct uci_element *e;
	struct wanargs *wandargs = (struct wanargs *)data;
	bool b;
		
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			if (wandargs->instance != WAN_INST_ATM) {
				return 0;
			}
			string_to_bool(value, &b);

			dmuci_get_option_value_list("dsl","line","mode", &v);
			if (v) {
				uci_foreach_element(v, e) {
					if (strcasecmp(e->name, "AnnexM") == 0 && b==false) {
						dmuci_del_list_value("dsl", "line", "mode", "annexm");
						return 0;
					}else if(strcasecmp(e->name, "AnnexM") != 0 && b==true){
						dmuci_add_list_value("dsl", "line", "mode", "annexm");
						return 0;
					}
				}
			}
	}
	return 0;
}

//TO CHECK IF NO VALUE RETURNE BY UBUS CMD
int get_wan_eth_intf_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	bool b;
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	val = dmjson_get_value(res, 1, "up");
	if (val) {
		string_to_bool(val, &b);
		if (b)
			*value = "1";
		else
			*value = "0";
	}
	return 0;
}

int set_wan_eth_intf_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{		
	struct uci_section *s;
	json_object *res;
	char *enable, *type, *device, json_name[32];
	bool b;
	bool enable_b;
	struct wanargs *wandargs = (struct wanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET: //ENHANCEMENT look for function to start and stop the ethernet driver
			string_to_bool(value, &b);
			dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
			if (res) {
				enable = dmjson_get_value(res, 1, "up");
				string_to_bool(enable, &enable_b);
				if (b == enable_b)
					return 0;
			}
			if(b) {
				uci_foreach_option_cont("network", "interface", "ifname", wandargs->fdev, s) {
					sprintf(json_name, "network.interface.%s", section_name(s));
					dmubus_call_set(json_name, "up", UBUS_ARGS{}, 0);
				}
			}
			else {
				uci_foreach_option_cont("network", "interface", "ifname", wandargs->fdev, s) {
					dmuci_get_value_by_section_string(s, "type", &type);
					if (strcmp(type, "anywan") != 0 && strcmp(type, "multiwan") != 0) {
						sprintf(json_name, "network.interface.%s", section_name(s));
						dmubus_call_set(json_name, "down", UBUS_ARGS{}, 0);
						goto end;
					}
					else {
						dmubus_call("network.interface", "status", UBUS_ARGS{{"name", section_name(s), String}}, 1, &res);
						if (res) {
							device = dmjson_get_value(res, 1, "device");
							if (strstr(device, wandargs->fdev)) {
								sprintf(json_name, "network.interface.%s", section_name(s));
								dmubus_call_set(json_name, "down", UBUS_ARGS{}, 0);
								goto end;
							}
						}
					}
				}
			}
			return 0;
	}
end:
	return 0;
}

int get_wan_eth_intf_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	bool b;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "Disabled");
	*value = dmjson_get_value(res, 1, "up");
	if (*value) {
		string_to_bool(*value, &b);
		if (!b)
			*value = "Disabled";
		else
			*value = "Up";
	}	
	return 0;
}

int get_wan_eth_intf_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "00:00:00:00:00:00");
	*value = dmjson_get_value(res, 1, "macaddr");
	if (!(*value) || (*value)[0] == '\0') {
		*value = "00:00:00:00:00:00";
	}
	return 0;
}

int get_wan_eth_intf_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", "tx_bytes");
	if (!(*value) || (*value)[0] == '\0') {
		*value = "0";
	}
	return 0;
}

int get_wan_eth_intf_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", "rx_bytes");
	if (!(*value) || (*value)[0] == '\0') {
		*value = "0";
	}
	return 0;
}

int get_wan_eth_intf_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", "tx_packets");
	if (!(*value) || (*value)[0] == '\0') {
		*value = "0";
	}
	return 0;
}

int get_wan_eth_intf_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandargs = (struct wanargs *)data;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wandargs->fdev, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	*value = dmjson_get_value(res, 2, "statistics", "rx_packets");
	if (!(*value) || (*value)[0] == '\0') {
		*value = "0";
	}
	return 0;
}

/************************************************************************** 
**** ****  function related to get_wandevice_wanconnectiondevice_parameters **** ****
***************************************************************************/

int get_wan_dsl_link_config_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_wan_dsl_link_config_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcdevargs = (struct wanargs *)data;
	char *vpi, *vci;
	struct uci_section *s;

	uci_foreach_option_cont("dsl", "atm-device", "device", wandcdevargs->fwan, s) {
		dmuci_get_value_by_section_string(s, "vpi", &vpi);
		dmuci_get_value_by_section_string(s, "vci", &vci);
		dmasprintf(value, "PVC: %s/%s", vpi, vci); // MEM WILL BE FREED IN DMMEMCLEAN
		break;
	}
	return 0;
}

int set_wan_dsl_link_config_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *vpi = NULL, *vci = NULL, *spch, *val;
	struct uci_section *s;
	struct wanargs *wandcdevargs = (struct wanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_cont("dsl", "atm-device", "device", wandcdevargs->fwan, s) {
				if (strstr(value, "PVC: "))
					value += 5;
				else
					return 0;
				val = dmstrdup(value);
				vpi = strtok_r(val, "/", &spch);
				if (vpi) {
					vci = strtok_r(NULL, "/", &spch);
				}
				if (vpi && vci) {
					dmuci_set_value_by_section(s, "vpi", vpi);
					dmuci_set_value_by_section(s, "vci", vci);
				}
				dmfree(val);
				break;
			}
			return 0;
	}	
	return 0;
} 

int get_wan_dsl_link_config_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcdevargs = (struct wanargs *)data;
	struct uci_section *s;
	char *encapsulation;
	*value = "";
	
	uci_foreach_option_cont("dsl", "atm-device", "device", wandcdevargs->fwan, s) {
		dmuci_get_value_by_section_string(s, "encapsulation", &encapsulation);
		if (strstr(encapsulation, "vcmux")) {
			*value = "VCMUX";
		}
		else if (strstr(encapsulation, "llc")) {
			*value = "LLC";
		}
		else {
			*value = "";
		}
		break;
	}
	return 0;
}

int set_wan_dsl_link_config_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s;
	char *type, *encapsulation;
	struct wanargs *wandcdevargs = (struct wanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_cont("dsl", "atm-device", "device", wandcdevargs->fwan, s) {
				if (strstr(value, "VCMUX")) {
					encapsulation = "vcmux";
				}
				else if (strstr(value, "LLC")) {
					encapsulation = "llc";
				}
				else
					return 0;
				break;
			}
			dmuci_set_value_by_section(s, "encapsulation", encapsulation);
			return 0;
	}
	return 0;
}

/************************************************************************** 
**** ****  function related to get_wandevice_wanprotoclconnection_parameters **** ****
***************************************************************************/

int get_interface_enable_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *intf = section_name(wandcprotoargs->wancprotosection);
	return get_interface_enable_ubus(intf, refparam, ctx, value);
}

int set_interface_enable_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *intf = section_name(wandcprotoargs->wancprotosection);
	return set_interface_enable_ubus(intf, refparam, ctx, action, value);
}

int get_interface_firewall_enabled_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *intf = section_name(wandcprotoargs->wancprotosection);
	return get_interface_firewall_enabled(intf, refparam, ctx, value);
}

int set_interface_firewall_enabled_wanproto(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *intf = section_name(wandcprotoargs->wancprotosection);
	return set_interface_firewall_enabled(intf, refparam, ctx, action, value);
}
//THE same as get_wan_device_ppp_status WHY DO WE CREATE A SEPARATED FUNCTION
int get_wan_device_mng_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res = NULL, *jobj = NULL;
	char *pending = NULL;
	char *intf;
	char *status = NULL;
	char *uptime = NULL;
	bool b = false;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;

	intf = section_name(wandcprotoargs->wancprotosection);
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", intf, String}}, 1, &res);
	DM_ASSERT(res, *value = "Disconnected");
	jobj = dmjson_get_obj(res, 1, "up");
	if (jobj)
	{
		status = dmjson_get_value(res, 1, "up");	
		if (strcasecmp(status, "true") == 0) {
			uptime = dmjson_get_value(res, 1, "uptime");
			pending = dmjson_get_value(res, 1, "pending");			
			string_to_bool(pending, &b);
		}
	}
	if (uptime && atoi(uptime) > 0)
		*value = "Connected";
	else if (pending && b) {
		*value = "Pending Disconnect";
	}
	else
		*value = "Disconnected";
	return 0;
}

int get_wan_device_mng_interface_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	network_get_ipaddr(value, intf);
	return 0;
}

int get_wan_ip_link_connection_connection_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *type;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "type", &type);
	if (strcmp(type, "bridge") == 0 || strcmp(type, "alias") == 0)
		*value = "IP_Bridged";
	else 
		*value = "IP_Routed";
	return 0;
}

int set_wan_ip_link_connection_connection_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *type = "";
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "type", &type);
			if (strcmp(value, "IP_Bridged") == 0) {
				if (strcmp(type, "bridge") == 0 || strcmp(type, "alias") == 0)
					return 0;
				else {
					type = "bridge";
					set_bridge_layer2(ctx, "1", wandcprotoargs);
				}
			}
			else if (strcmp(value, "IP_Routed") == 0) {
				if (strcmp(type, "bridge") != 0 && strcmp(type, "alias") != 0)
					return 0;
				else {
					if (strcmp(type, "bridge") == 0)
						set_bridge_layer2(ctx, "", wandcprotoargs);
					type = "anywan";
				}
			}
			else 
				return 0;
			dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "type", type);
			return 0;
	}
	return 0;
} 

int get_wan_ip_link_connection_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *proto;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "proto", &proto);
	if (strcmp(proto, "dhcp") == 0)
		*value = "DHCP";
	else if (strcmp(proto, "static") == 0)
		*value = "Static";
	else 
		*value = proto;
	return 0;
}

int set_wan_ip_link_connection_addressing_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *proto;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(strcmp(value, "DHCP") == 0)
				proto = "dhcp";
			else if(strcmp(value, "Static") == 0)
				proto = "static";
			else 
				return 0;
			dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "proto", proto);
			return 0;
	}
	return 0;
}

int get_wan_ip_link_connection_nat_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf, *masq, *network;
	struct uci_section *s = NULL;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	uci_foreach_sections("firewall", "zone", s) {
		dmuci_get_value_by_section_string(s, "masq", &masq);
		if (masq[0] == '1' && masq[1] == '\0') {
			dmuci_get_value_by_section_string(s, "network", &network);
			if (strstr(network, intf)) {
				*value = "1";
				return 0;
			}
		}
	}
	*value = "0";
	return 0;
}

int set_wan_ip_link_connection_nat_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *intf;
	int found = 0;
	struct uci_section *s = NULL;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(!b)
				value = "";
			else
				value = "1";
			uci_foreach_option_cont("firewall", "zone", "network", intf, s) {
				found++;
				dmuci_set_value_by_section(s, "masq", value);
				if (value[0] != '\0')
					break;
			}
			if (found == 0 && value[0] != '\0') {
				s = create_firewall_zone_config("nat", intf, "ACCEPT", "ACCEPT", "ACCEPT");
				dmuci_set_value_by_section(s, "masq", value);
			}
			return 0;
	}	
	return 0;
}



int get_wan_igmp_rule_idx(char *iface, struct uci_section **rule, struct uci_section **zone, char **enable)
{
	char *input, *proto, *target, *zname;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	*enable = "1";
	*rule = NULL;
	
	uci_foreach_option_cont("firewall", "zone", "network", iface, *zone) {
		dmuci_get_value_by_section_string(*zone, "input", &input);
		if (strcmp(input, "DROP") == 0)
			*enable = "0";
		dmuci_get_value_by_section_string(*zone, "name", &zname);
		uci_foreach_option_cont("firewall", "rule", "src", zname, *rule) {
			dmuci_get_value_by_section_string(*rule, "proto", &proto);
			if (strcmp(proto, "igmp") == 0) {
				dmuci_get_value_by_section_string(*rule, "enabled", enable);
				if ((*enable)[0] != '\0') {
					if ((*enable)[0] == '0')
						break;
				}
				dmuci_get_value_by_section_string(*rule, "target", &target);
				if (target[0] != '\0') {
					if (strcmp(target, "DROP") == 0)
						*enable = "0";
					else
						*enable = "1";
				}
				break;
			}
		}
		if (*rule != NULL)
			break;
	}
	return 0;
}

int get_wan_ip_link_connection_igmp_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf, *enable = "0";
	struct uci_section *rule = NULL;
	struct uci_section *zone = NULL;
	
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	get_wan_igmp_rule_idx(intf, &rule, &zone, &enable);
	*value = enable;
	return 0;
}

int set_wan_ip_link_connection_igmp_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	bool b;
	int found = 0;
	char *val, *intf, *enable, *zname, buf[32];
	struct uci_section *rule = NULL;
	struct uci_section *zone = NULL;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "ACCEPT";
			else
				value = "DROP";
			get_wan_igmp_rule_idx(intf, &rule, &zone, &enable);
			if(zone == NULL) {
				create_firewall_zone_config("igmp", intf, "ACCEPT", "ACCEPT", "ACCEPT");
				sprintf(buf, "igmp_%s", intf);
				zname = buf;
			} else {
				dmuci_get_value_by_section_string(zone, "name", &zname);
			}
			if(rule == NULL) {
				dmuci_add_section("firewall", "rule", &rule, &val);
				dmuci_set_value_by_section(rule, "src", zname);
				dmuci_set_value_by_section(rule, "proto", "igmp");
			}
			dmuci_set_value_by_section(rule, "target", value);
			dmuci_set_value_by_section(rule, "enabled", "1");
			return 0;
	}
	return 0;
}

int get_wan_ip_link_connection_dns_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "peerdns", value);
	if ((*value)[0] == '\0') {
		*value = "1";
	}
	return 0;
}

int set_wan_ip_link_connection_dns_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	bool b;
	char *intf;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	intf = section_name(wandcprotoargs->wancprotosection);
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				value = "1";
			else
				value = "0";
			dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "peerdns", value);
			return 0;
	}
}

int get_wan_device_ppp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf; 
	char *status = NULL;
	char *uptime = NULL;
	char *pending = NULL;
	json_object *res = NULL, *jobj = NULL;
	bool bstatus = false, bpend = false;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;

	intf = section_name(wandcprotoargs->wancprotosection);
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", intf, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	jobj = dmjson_get_obj(res, 1, "up");
	if (jobj)
	{
		status = dmjson_get_value(res, 1, "up");
		string_to_bool(status, &bstatus);
		if (bstatus) {
			uptime = dmjson_get_value(res, 1, "uptime");
			pending = dmjson_get_value(res, 1, "pending");
			string_to_bool(pending, &bpend);
		}
	}
	if (uptime && atoi(uptime) > 0)
		*value = "Connected";
	else if (pending && bpend)
		*value = "Pending Disconnect";
	else
		*value = "Disconnected";
	return 0;
}

int get_wan_device_ppp_interface_ip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf, *val;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;

	intf = section_name(wandcprotoargs->wancprotosection);
	network_get_ipaddr(value, intf);
	return 0;
}

int get_wan_device_mng_interface_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf, *device;
	json_object *res, *jobj;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	*value = "";
	intf = section_name(wandcprotoargs->wancprotosection);
	
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", intf, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	jobj = dmjson_get_obj(res, 1, "device");
	if (jobj) {
		device = dmjson_get_value(res, 1, "device");
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device, String}}, 1, &res);
		if (res) {
			*value = dmjson_get_value(res, 1, "macaddr");
			return 0;
		}
	}
	return 0;
}

int get_wan_device_ppp_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *intf;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "username", value);
	return 0;
}

int set_wan_device_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "username", value);
			return 0;
	}
	return 0;
} 

int set_wan_device_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "password", value);
			return 0;
	}
	return 0;
}

int get_wan_ip_link_connection_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *args = (struct wanargs *)data;
	*value = dmstrdup(section_name(args->wancprotosection));
	return 0;
}

int set_wan_ip_link_connection_connection_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct wanargs *args;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			args = (struct wanargs *)data;
			dmuci_rename_section_by_section(args->wancprotosection, value);
			return 0;
	}
	return 0;
}

int get_layer2_interface(char *wan_name, char **ifname)
{
	char *wtype, *wifname, *device, *dup, *pch, *spch;
	int vid;
	json_object *res;

	*ifname = "";
	dmuci_get_option_value_string("network", wan_name, "ifname", &wifname);
	if(wifname[0] == '\0')
		return 0;

	dmuci_get_option_value_string("network", wan_name, "type", &wtype);
	if(wtype[0] == '\0' || strcmp(wtype, "anywan") == 0)
	{
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", wan_name, String}}, 1, &res);
		if (res) {
			device = dmjson_get_value(res, 1, "device");
			if(device[0] != '\0') {
				if (atoi(device + 5) > 1) {
					*ifname = device;
					return 0;
				}
			}
		}
	}
	dup = dmstrdup(wifname); // MEM will be freed in the DMMCLEAN
	for (pch = strtok_r(dup, " ", &spch); pch != NULL; pch = strtok_r(NULL, " ", &spch)) {
		if (strstr(pch, "atm") ||
			strstr(pch, "ptm") ||
			strstr(pch, eth_wan)) {
			if (atoi(pch + 5) > 1) {
				*ifname = pch;
				return 0;
			}
		}
	}
	return  0;
}

int get_wan_ip_link_connection_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);
	struct uci_section *ss = NULL;
	char *ifname;

	*value = "0";
	get_layer2_interface(wan_name, &ifname);
	if (ifname[0] == '\0') {
		uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
		{
			dmuci_get_value_by_section_string(ss, "vid", value);
		}
		return 0;
	}
	else
		*value = ifname + 5;
	return 0;
}

int set_wan_ip_link_connection_vid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ss = NULL, *w_vlan, *s_last = NULL;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *add_value, *ifname, *vid, *prio;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);
	bool found = false;
	char *p, *q, *wifname, *baseifname="", *type="";
	char r_new_wifname[128] = "";
	char r1_new_wifname[128] = "";
	char a_new_wifname[128] = "";
	char v_ifname[16];
	char v1_baseifname[16];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_layer2_interface(wan_name, &ifname);
			dmuci_get_value_by_section_string(wandcprotoargs->wancprotosection, "type", &type);
			if (ifname[0] != '\0')
			{
				uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "wan_name", wan_name, ss) {
					dmuci_get_value_by_section_string(ss, "vlan8021q", &vid);
					if (strcmp(vid, value) == 0)
						return 0;
					dmuci_get_option_value_string("network", wan_name, "ifname", &wifname);
					if (wifname[0] == '\0')
						return 0;
					remove_vid_interfaces_from_ifname(vid, wifname, r_new_wifname);
					sprintf(r1_new_wifname, "%s", r_new_wifname);
					dmuci_get_value_by_section_string(ss, "baseifname", &baseifname);
					if (strcmp(type, "bridge") != 0 || strcmp(value, "1") == 0)
					{
						sprintf(v1_baseifname, "%s.1", baseifname);
						remove_interface_from_ifname(v1_baseifname, r1_new_wifname, r_new_wifname);
					}
					p = a_new_wifname;
					q = v_ifname;
					dmuci_get_value_by_section_string(ss, "vlan8021q", &vid);
					dmstrappendstr(q, baseifname);
					dmstrappendchr(q, '.');
					dmstrappendstr(q, value);
					dmstrappendend(q);
					dmstrappendstr(p, v_ifname);
					if (r_new_wifname[0] != '\0')
					dmstrappendchr(p, ' ');
					dmstrappendstr(p, r_new_wifname);
					dmstrappendend(p);
					dmuci_set_value_by_section(ss, "ifname", v_ifname);
					dmuci_set_value_by_section(ss, "vlan8021q", value);
					dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "ifname", a_new_wifname);
					DMUCI_SET_VALUE(icwmpd, "dmmap", wan_name, "vid", value);
				}
				return 0;
			}
			uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
			{
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "vid", value);
				dmuci_get_value_by_section_string(ss, "baseifname", &baseifname);
				dmuci_get_value_by_section_string(ss, "priority", &prio);
				if (strstr(baseifname, "atm") ||  strstr(baseifname, "ptm") || strstr(baseifname, eth_wan)) {
					dmuci_get_option_value_string("network", wan_name, "ifname", &wifname);
					if (strcmp(type, "bridge") != 0 || strcmp(value, "1") == 0)
					{
						sprintf(v1_baseifname, "%s.1", baseifname);
						remove_interface_from_ifname(v1_baseifname, wifname, r_new_wifname);
					}
					else
					{
						sprintf(r_new_wifname, "%s",wifname);
					}
					p = a_new_wifname;
					q = v_ifname;
					dmstrappendstr(q, baseifname);
					dmstrappendchr(q, '.');
					dmstrappendstr	(q, value);
					dmstrappendend(q);
					dmstrappendstr(p, v_ifname);
					if (r_new_wifname[0] != '\0')
					dmstrappendchr(p, ' ');
					dmstrappendstr(p, r_new_wifname);
					dmstrappendend(p);
					add_wvlan(baseifname, v_ifname, value, (prio) ? prio : "0", wan_name);
					dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "ifname", a_new_wifname);
					DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "vid", value);
					return 0;
				}
				return 0;
			}
			DMUCI_SET_VALUE(icwmpd, "dmmap", wan_name, NULL, wan_name);
			DMUCI_SET_VALUE(icwmpd, "dmmap", wan_name, "vid", value);
		return 0;
	}
	return 0;
}
int get_wan_ip_link_connection_vpriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);
	struct uci_section *ss = NULL;
	char *ifname;
	*value = "0";

	get_layer2_interface(wan_name, &ifname);
	if (ifname[0] == '\0') {
		uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
		{
			dmuci_get_value_by_section_string(ss, "priority", value);
		}
		return 0;
	}
	uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", ifname, ss)
	{
		dmuci_get_value_by_section_string(ss, "vlan8021p", value);
	}
	return 0;
}

int set_wan_ip_link_connection_vpriority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *ifname;
	struct uci_section *ss = NULL;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_layer2_interface(wan_name, &ifname);
			if (ifname[0] == '\0') {
				uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
				{
					DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "priority", value);
				}
				return 0;
			}
			uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", ifname, ss)
			{
				dmuci_set_value_by_section(ss, "vlan8021p", value);
			}
			return 0;
	}
	return 0;
}

int get_wan_ip_link_connection_layer2_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);
	struct uci_section *ss = NULL;
	char *ifname, *p;

	*value = "";
	get_layer2_interface(wan_name, &ifname);
	if (ifname[0] == '\0') {
		uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
		{
			dmuci_get_value_by_section_string(ss, "baseifname", value);
		}
		return 0;
	}
	*value = dmstrdup(ifname); // MEM will be freed in the DMMCLEAN
	p = strchr(*value, '.');
	if (p)
		*p = '\0';
	return 0;
}

int set_wan_ip_link_connection_layer2_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *ifname, *add_value;
	struct uci_section *ss = NULL, *w_vlan, *s_last;
	struct wanargs *wandcprotoargs = (struct wanargs *)data;
	char *wan_name = section_name(wandcprotoargs->wancprotosection);
	char *p, *q, *wifname, *baseifname = NULL, *vid = NULL, *prio = NULL;
	char r_new_wifname[128];
	char a_new_wifname[128];
	char ifname_buf[16];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_layer2_interface(wan_name, &ifname);
			if (ifname[0] != '\0')
			{
				uci_foreach_option_eq("layer2_interface_vlan", "vlan_interface", "ifname", ifname, ss) {
					dmuci_get_value_by_section_string(ss, "baseifname", &baseifname);
					if (strcmp(baseifname, value) == 0)
						return 0;
					dmuci_get_option_value_string("network", wan_name, "ifname", &wifname);
					if (wifname[0] == '\0')
						return 0;
					dmuci_get_value_by_section_string(ss, "vlan8021q", &vid);
					remove_vid_interfaces_from_ifname(vid, wifname, r_new_wifname);
					p = a_new_wifname;
					q = ifname_buf;
					dmstrappendstr(q, value);
					dmstrappendchr(q, '.');
					dmstrappendstr(q, vid);
					dmstrappendend(q);
					dmstrappendstr(p, ifname_buf);
					if (r_new_wifname[0] != '\0')
						dmstrappendchr(p, ' ');
					dmstrappendstr(p, r_new_wifname);
					dmstrappendend(p);
					dmuci_set_value_by_section(ss, "baseifname", value);
					dmuci_set_value_by_section(ss, "ifname", ifname_buf);
					dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "ifname", a_new_wifname);
				}
				return 0;
			}
			uci_path_foreach_sections(icwmpd, "dmmap", wan_name, ss)
			{
				DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "baseifname", value);
				dmuci_get_value_by_section_string(ss, "vid", &vid);
				dmuci_get_value_by_section_string(ss, "priority", &prio);
				if (atoi(vid) >= 1) {
					dmuci_get_option_value_string("network", wan_name, "ifname", &wifname);
					p = a_new_wifname;
					q = ifname_buf;
					dmstrappendstr(q, value);
					dmstrappendchr(q, '.');
					dmstrappendstr(q, vid);
					dmstrappendend(q);
					dmstrappendstr(p, ifname_buf);
					if (wifname[0] != '\0')
					dmstrappendchr(p, ' ');
					dmstrappendstr(p, wifname);
					dmstrappendend(p);
					add_wvlan(value, ifname_buf, vid, (prio) ? prio : "0", wan_name);
					dmuci_set_value_by_section(wandcprotoargs->wancprotosection, "ifname", a_new_wifname);
					return 0;
				}
				return 0;
			}
			DMUCI_SET_VALUE(icwmpd, "dmmap", wan_name, NULL, wan_name);
			DMUCI_SET_VALUE(icwmpd, "dmmap", wan_name, "baseifname", value);
		return 0;
	}
	return 0;
}

inline int ubus_get_wan_stats(json_object *res, char **value, char *stat_mod, struct wanargs *wanargs)
{
	char *proto;
	json_object *res1;
	char *device_name;

	dmuci_get_option_value_string("network", section_name(wanargs->wancprotosection), "proto", &proto);
	if (strcmp(proto, "dhcp") == 0 || strcmp(proto, "pppoe") == 0)
	{
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface",section_name(wanargs->wancprotosection)}, String}, 1, &res1);
		device_name = dmjson_get_value(res1, 1, "device");
		dmubus_call("network.device", "status", UBUS_ARGS{{"name", device_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		*value = dmjson_get_value(res, 2, "statistics", stat_mod);
		return 0;
	}
	*value = "";
	return 0;
}

int get_wan_link_connection_eth_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(res, value, "rx_bytes", (struct wanargs *)data);
	return 0;
}

int get_wan_link_connection_eth_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(res, value, "tx_bytes", (struct wanargs *)data);
	return 0;
}

int get_wan_link_connection_eth_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(res, value, "rx_packets", (struct wanargs *)data);
	return 0;
}

int get_wan_link_connection_eth_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_get_wan_stats(res, value, "tx_packets", (struct wanargs *)data);
	return 0;
}

////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_wan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wanargs *)data)->wandevsection, "wan_dev_alias", value);
	return 0;
}

int set_wan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wanargs *)data)->wandevsection, "wan_dev_alias", value);
			return 0;
	}
	return 0;
}

int get_wan_con_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wanargs *)data)->wancdsection, "wanalias", value);
	return 0;
}

int set_wan_con_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wanargs *)data)->wancdsection, "wanalias", value);
			return 0;
	}
	return 0;
}

int get_wan_ip_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wanargs *)data)->wancprotosection, "conipalias", value);
	return 0;
}

int set_wan_ip_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wanargs *)data)->wancprotosection, "conipalias", value);
			return 0;
	}
	return 0;
}

int get_wan_ppp_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	dmuci_get_value_by_section_string(((struct wanargs *)data)->wancprotosection, "conpppalias", value);
	return 0;
}

int set_wan_ppp_con_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wanargs *)data)->wancprotosection, "conpppalias", value);
			return 0;
	}
	return 0;
}
/////////////SUB ENTRIES///////////////
/*************************************************/

char *get_wan_connection_device_perm(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{

	int tmp = WAN_IDX_ETH + 1;
	if (((struct wanargs *)data)->instance == tmp )
		return "0";
	else
		return "1";
}

unsigned char get_wan_protocol_connection_forced_inform(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	return 1;
	struct wanargs *wancprotoarg = (struct wanargs *)data;
	if (strcmp(section_name(wancprotoarg->wancprotosection), default_wan) == 0)
		return 1;
	return 0;
}

bool check_wan_is_ethernet(struct dmctx *dmctx, void *data)
{
	struct wanargs *wanargs = (struct wanargs *)data;
	if(wanargs->instance == WAN_IDX_ETH + 1)
		return true;
	else
		return false;
}

bool check_wan_is_atm(struct dmctx *dmctx, void *data)
{
	struct wanargs *wanargs = (struct wanargs *)data;
	if(wanargs->instance == WAN_IDX_ATM + 1)
		return true;
	else
		return false;
}

char *get_wan_connection_device_notif(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	struct wanargs *wancprotoarg = (struct wanargs *)data;
	if (strcmp(section_name(wancprotoarg->wancprotosection), default_wan) == 0)
		return "2";
	return "0";
}

int browsewandeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	int i = 0;
	bool notif_permission;
	char *cwritable;
	char *defwanproto;
	struct uci_section *s = NULL;
	char *dev, *dev_last= NULL;
	struct wanargs curr_wanargs = {0};

	dmuci_get_option_value_string("cwmp", "cpe", "default_wan_interface", &default_wan);
	dmuci_get_option_value_string("network", default_wan, "ifname", &default_wan_ifname);
	dmuci_get_option_value_string("network", default_wan, "proto", &defwanproto);
	dmuci_get_option_value_string("ports", "WAN", "ifname", &eth_wan);
	wan_devices[WAN_IDX_ETH].fdev = eth_wan;

	if (strstr(defwanproto, "ppp"))
		default_wan_proto = WAN_PROTO_PPP;
	else if (strcmp(defwanproto, "dhcp") == 0 || strcmp(defwanproto, "static") == 0)
		default_wan_proto = WAN_PROTO_IP;
	else
		default_wan_proto = WAN_PROTO_NIL;
	update_section_list(DMMAP,"wan_dev", NULL, 3, NULL, NULL, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap", "wan_dev", s) {
#ifdef EX400
		if(i==WAN_IDX_ETH){
			init_wanargs(&curr_wanargs, i+1, eth_wan, s);
			if (strstr(default_wan_ifname, eth_wan))
				notif_permission = false;
			else
				notif_permission = true;
		}
#else
		init_wanargs(&curr_wanargs, i+1, wan_devices[i].fdev, s);
		if (strstr(default_wan_ifname, wan_devices[i].fdev))
			notif_permission = false;
		else
			notif_permission = true;
#endif

		if (i == WAN_IDX_ETH)
			cwritable = "0";
#ifndef EX400
		else
			cwritable = "1";
#endif

		dev = handle_update_instance(1, dmctx, &dev_last, update_instance_alias_icwmpd, 3, s, "wan_dev_instance", "wan_dev_alias");
#ifdef EX400
		if(i==WAN_IDX_ETH){
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wanargs, dev) == DM_STOP)
				break;
		}
#else
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wanargs, dev) == DM_STOP)
			break;
#endif
		i++;
	}
	return 0;
}

int browsewanconnectiondeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *fwan;
	char *wan_ifname;
	char *iwan = NULL, *iwan_last = NULL;
	char *pack, *stype, *sname;
	char uname[32] = "";
	bool ipn_perm = true;
	bool pppn_perm = true;
	bool notif_permission = true;
	int i;
	struct wanargs *curr_wanargs = (struct wanargs *)prev_data;

	i = curr_wanargs->instance - 1;

#ifdef EX400
	if (i == WAN_IDX_ETH){
		uci_foreach_sections("ports", "ethport", s) {
			if(!strcmp(s->e.name, "WAN")){
				dmuci_get_value_by_section_string(s, "ifname", &fwan);
				dmuci_get_option_value_string("network", "wan", "ifname", &wan_ifname);
				if (strstr(default_wan_ifname, fwan)) {
					notif_permission = false;
					if (default_wan_proto == WAN_PROTO_IP) ipn_perm = false;
					else if (default_wan_proto == WAN_PROTO_PPP) pppn_perm = false;
				}
				iwan = handle_update_instance(2, dmctx, &iwan_last, update_instance_alias, 3, s, "waninstance", "wanalias");
				init_wancdevargs(curr_wanargs, s, fwan, iwan_last, wan_ifname);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)curr_wanargs, iwan) == DM_STOP)
					break;
			}
		}
	}
#else
	pack = wan_devices[i].cdev;
	stype = wan_devices[i].stype;

	if(i == WAN_IDX_ETH)
	{
		uci_foreach_sections(pack, stype, s) {
			if(!strcmp(s->e.name, "WAN")){
				fwan = eth_wan; //eth0
				sprintf(uname, "%s.1", fwan);
				wan_ifname = dmstrdup(uname); //eth0.1

				if (strstr(default_wan_ifname, fwan)) {
					notif_permission = false;
					if (default_wan_proto == WAN_PROTO_IP) ipn_perm = false;
					else if (default_wan_proto == WAN_PROTO_PPP) pppn_perm = false;
				}
				iwan = handle_update_instance(2, dmctx, &iwan_last, update_instance_alias, 3, s, "waninstance", "wanalias");
				init_wancdevargs(curr_wanargs, s, fwan, iwan_last, wan_ifname);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)curr_wanargs, iwan) == DM_STOP)
					goto end;
			}
		}
	}
	else
	{
		uci_foreach_sections(pack, stype, s) {
			dmuci_get_value_by_section_string(s, "device", &fwan); //atm0
			sprintf(uname, "%s.1", fwan);
			wan_ifname = dmstrdup(uname); //atm0.1

			if (strstr(default_wan_ifname, fwan)) {
				notif_permission = false;
				if (default_wan_proto == WAN_PROTO_IP) ipn_perm = false;
				else if (default_wan_proto == WAN_PROTO_PPP) pppn_perm = false;
			}
			iwan = handle_update_instance(2, dmctx, &iwan_last, update_instance_alias, 3, s, "waninstance", "wanalias");
			init_wancdevargs(curr_wanargs, s, fwan, iwan_last, wan_ifname);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)curr_wanargs, iwan) == DM_STOP)
				goto end;
		}
	}
#endif
end:
	return 0;
}

int browsewanprotocolconnectionipInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *ss = NULL;
	char *pack, *stype, *p, *iconp_ip_last = NULL, *iconp_ppp_last = NULL;
	char *iconp = NULL, *iconp_nil = NULL;
	int proto;
	bool notif_permission = true;
	bool forced_inform_eip = false;
	int forced_notify = UNDEF;
	struct wanargs *curr_wanargs = (struct wanargs *)prev_data;
	char *lan_name;

	char *wan_interface;
	char* containsrepeater= NULL;
	int repeatindex;
	struct uci_section *s = NULL;
	char *freq, **value;
	json_object *res;
	char *isrepeater;
	dmuci_get_option_value_string("netmode", "setup", "curmode", &isrepeater);
	containsrepeater = strstr(isrepeater, "repeater");
	if(containsrepeater){
		repeatindex = (int)(containsrepeater - isrepeater);
		if(repeatindex == 0){
			uci_foreach_sections("wireless", "wifi-device", s) {
				dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", s->e.name, String}}, 1, &res);
				DM_ASSERT(res, *value = "");
				freq = dmjson_get_value(res, 1, "frequency");
				if(freq[0]=='5') dmasprintf(&wan_interface, "%s", s->e.name);
			}
		}else{
			dmasprintf(&wan_interface, "%s", curr_wanargs->fwan);
		}
	}else{
		dmasprintf(&wan_interface, "%s", curr_wanargs->fwan);
	}

	uci_foreach_option_cont("network", "interface", "ifname", wan_interface, ss) {
		dmuci_get_value_by_section_string(ss, "proto", &p);
		lan_name = section_name(ss);
		if (strcmp(p, "dhcp") == 0 || strcmp(p, "static") == 0)
			proto = WAN_PROTO_IP;
		else
			return 0;
		if (strcmp(lan_name, default_wan) == 0) {
			forced_inform_eip = true;
			forced_notify = 2;
			notif_permission = false;
		}
		if (check_multiwan_interface(ss, curr_wanargs->fwan) != 0)
			continue;
		init_wancprotoargs(curr_wanargs, ss);
		iconp = handle_update_instance(3, dmctx, &iconp_ip_last, update_instance_alias, 3, ss, "conipinstance", "conipalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)curr_wanargs, iconp) == DM_STOP)
			break;
	}
	return 0;
}

int browsewanprotocolconnectionpppInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *ss = NULL;
	char *pack, *stype, *p, *iconp_ip_last = NULL, *iconp_ppp_last = NULL;
	char *iconp = NULL, *iconp_nil = NULL;
	int proto;
	bool notif_permission = true;
	bool forced_inform_eip = false;
	int forced_notify = UNDEF;
	struct wanargs *curr_wanargs = (struct wanargs *)prev_data;
	char *lan_name;

	char *wan_interface;
	char* containsrepeater= NULL;
	int repeatindex;
	struct uci_section *s = NULL;
	char *freq, **value;
	json_object *res;
	char *isrepeater;
	dmuci_get_option_value_string("netmode", "setup", "curmode", &isrepeater);
	containsrepeater = strstr(isrepeater, "repeater");
	if(containsrepeater){
		repeatindex = (int)(containsrepeater - isrepeater);
		if(repeatindex == 0){
			uci_foreach_sections("wireless", "wifi-device", s) {
				dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", s->e.name, String}}, 1, &res);
				DM_ASSERT(res, *value = "");
				freq = dmjson_get_value(res, 1, "frequency");
				if(freq[0]=='5') dmasprintf(&wan_interface, "%s", s->e.name);
			}
		}else{
			dmasprintf(&wan_interface, "%s", curr_wanargs->fwan);
		}
	}else{
		dmasprintf(&wan_interface, "%s", curr_wanargs->fwan);
	}

	uci_foreach_option_cont("network", "interface", "ifname", wan_interface, ss) {
		dmuci_get_value_by_section_string(ss, "proto", &p);
		lan_name = section_name(ss);
		if (strstr(p, "ppp"))
			proto = WAN_PROTO_PPP;
		else
		return 0;
		if (strcmp(lan_name, default_wan) == 0) {
			forced_inform_eip = true;
			forced_notify = 2;
			notif_permission = false;
		}
		if (check_multiwan_interface(ss, curr_wanargs->fwan) != 0)
			continue;
		init_wancprotoargs(curr_wanargs, ss);
		iconp = handle_update_instance(3, dmctx, &iconp_ppp_last, update_instance_alias, 3, ss, "conpppinstance", "conpppalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)curr_wanargs, iconp) == DM_STOP)
			break;
	}
	return 0;
}

struct dm_permession_s DMWANConnectionDevice = {"0", &get_wan_connection_device_perm};
struct dm_notif_s DMWANConnectionDevicenotif = {NULL, &get_wan_connection_device_notif};
struct dm_forced_inform_s DMWANConnectionProtocolinform = {1, get_wan_protocol_connection_forced_inform};
