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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cwmp.h"
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmentry.h"
#include "landevice.h"
#include "wepkey.h"
#include "dmubus.h"
#include "dmjson.h"
#define DELIMITOR ","
#define TAILLE 10
#define MAX_PROC_ARP 256
#define DHCP_LEASE_FILE "/var/dhcp.leases"


/*** LANDevice.{i}. ***/
DMLEAF tLANDeviceParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_lan_dev_alias, set_lan_dev_alias, NULL, NULL},
{0}
};

DMOBJ tLANDeviceObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"LANHostConfigManagement", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tLanhost_Config_ManagementObj, tLanhost_Config_ManagementParam, NULL},
{"Hosts", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tlandevice_hostObj, tlandevice_hostsParam, NULL},
{"LANEthernetInterfaceConfig", &DMREAD, NULL, NULL, NULL, browselanethernetinterfaceconfigInst, NULL, NULL, tlanethernetinterfaceconfigObj, tlanethernetinterfaceconfigParam, NULL},
{"WLANConfiguration", &DMWRITE, add_landevice_wlanconfiguration, delete_landevice_wlanconfiguration, NULL, browseWlanConfigurationInst, NULL, NULL, tWlanConfigurationObj, tWlanConfigurationParam, NULL},
{0}
};

/*** LANDevice.{i}.LANHostConfigManagement. ***/
DMOBJ tLanhost_Config_ManagementObj[] = {
{"IPInterface", &DMREAD, NULL, NULL, NULL, browseIPInterfaceInst, NULL, NULL, NULL, tIPInterfaceParam, get_linker_lanhost_interface},
{"DHCPStaticAddress", &DMWRITE, add_landevice_dhcpstaticaddress, delete_landevice_dhcpstaticaddress, NULL, browseDhcp_static_addressInst, NULL, NULL, NULL, tDHCPStaticAddressParam, NULL},
{"DHCPConditionalServingPool", &DMWRITE, add_dhcp_conditional_serving_pool, delete_dhcp_conditional_serving_pool, NULL, browselandevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance, NULL, NULL, tDHCPConditionalServingPoolobj, tDHCPConditionalServingPoolParam, NULL},
{0}
};

DMLEAF tLanhost_Config_ManagementParam[] = {
{"DNSServers", &DMWRITE, DMT_STRING, get_lan_dns, set_lan_dns, NULL, NULL},
{"DHCPServerConfigurable", &DMWRITE, DMT_BOOL, get_lan_dhcp_server_configurable, set_lan_dhcp_server_configurable, NULL, NULL},
{"DHCPServerEnable", &DMWRITE, DMT_BOOL, get_lan_dhcp_server_enable, set_lan_dhcp_server_enable, NULL, NULL},
{"MinAddress", &DMWRITE, DMT_STRING, get_lan_dhcp_interval_address_start, set_lan_dhcp_address_start, NULL, NULL},
{"MaxAddress", &DMWRITE, DMT_STRING, get_lan_dhcp_interval_address_end, set_lan_dhcp_address_end, NULL, NULL},
{"ReservedAddresses", &DMWRITE, DMT_STRING, get_lan_dhcp_reserved_addresses, set_lan_dhcp_reserved_addresses, NULL, NULL},
{"SubnetMask", &DMWRITE, DMT_STRING, get_lan_dhcp_subnetmask, set_lan_dhcp_subnetmask, NULL, NULL},
{"IPRouters", &DMWRITE, DMT_STRING, get_lan_dhcp_iprouters, set_lan_dhcp_iprouters, NULL, NULL},
{"DHCPLeaseTime", &DMWRITE, DMT_STRING, get_lan_dhcp_leasetime, set_lan_dhcp_leasetime, NULL, NULL},
{"DomainName", &DMWRITE, DMT_STRING, get_lan_dhcp_domainname, set_lan_dhcp_domainname, NULL, NULL},
{0}
};

/*** LANDevice.{i}.LANHostConfigManagement.IPInterface.{i}. ***/
DMLEAF tIPInterfaceParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_lan_ip_int_alias, set_lan_ip_int_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_interface_enable_ipinterface, set_interface_enable_ipinterface, NULL, NULL},
{"X_BROADCOM_COM_FirewallEnabled", &DMWRITE, DMT_BOOL, get_interface_firewall_enabled_ipinterface, set_interface_firewall_enabled_ipinterface, NULL, NULL},
{"IPInterfaceIPAddress", &DMWRITE, DMT_STRING, get_interface_ipaddress, set_interface_ipaddress, NULL, NULL},
{"IPInterfaceSubnetMask", &DMWRITE, DMT_STRING, get_interface_subnetmask, set_interface_subnetmask, NULL, NULL},
{"IPInterfaceAddressingType", &DMWRITE, DMT_STRING, get_interface_addressingtype, set_interface_addressingtype, NULL, NULL},
{0}
};

/*** LANDevice.{i}.LANHostConfigManagement.DHCPStaticAddress.{i}. ***/
DMLEAF tDHCPStaticAddressParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_dhcp_alias, set_dhcp_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_dhcpstaticaddress_enable, set_dhcpstaticaddress_enable, NULL, NULL},
{"Chaddr", &DMWRITE, DMT_STRING, get_dhcpstaticaddress_chaddr, set_dhcpstaticaddress_chaddr, NULL, NULL},
{"Yiaddr", &DMWRITE, DMT_STRING, get_dhcpstaticaddress_yiaddr, set_dhcpstaticaddress_yiaddr, NULL, NULL},
{0}
};


/*** LANDevice.{i}.Hosts. ***/
DMOBJ tlandevice_hostObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Host", &DMREAD, NULL, NULL, NULL, browselandevice_hostInst, NULL, NULL, NULL, tlandevice_hostParam, NULL},
{0}
};

DMLEAF tlandevice_hostsParam[] = {
{"HostNumberOfEntries", &DMREAD, DMT_UNINT, get_lan_host_nbr_entries, NULL, NULL, &DMNONE},
{0}
};

/*** LANDevice.{i}.DHCPConditionalServingPool. ***/
DMOBJ tDHCPConditionalServingPoolobj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"DHCPOption", &DMWRITE, add_dhcp_serving_pool_option, delete_dhcp_serving_pool_option, NULL, browseentry_landevice_dhcpconditionalservingpool_option_instance, NULL, NULL, NULL, tDHCPOptionParam, NULL},
{0}
};

DMLEAF tDHCPConditionalServingPoolParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_dhcp_conditional_servingpool_alias, set_dhcp_conditional_servingpool_alias, NULL, &DMNONE},
{"Enable", &DMWRITE, DMT_BOOL, get_dhcp_conditional_servingpool_enable, set_dhcp_conditionalservingpool_enable, NULL, &DMNONE},
{"VendorClassID", &DMWRITE, DMT_STRING, get_dhcp_conditional_servingpool_vendorclassid, set_dhcp_conditional_servingpool_vendorclassid, NULL, &DMNONE},
{"X_INTENO_COM_Networkid", &DMWRITE, DMT_STRING, get_dhcp_conditional_servingpool_network_id, set_dhcp_conditional_servingpool_network_id, NULL, &DMNONE},
{0}
};

DMLEAF tDHCPOptionParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_dhcp_servingpool_alias, set_dhcp_servingpool_alias, NULL, &DMNONE},
{"Tag", &DMWRITE, DMT_BOOL, get_dhcp_servingpool_tag, set_dhcp_servingpool_tag, NULL, &DMNONE},
{"Value", &DMWRITE, DMT_UNINT, get_dhcp_servingpool_value, set_dhcp_servingpool_value, NULL, &DMNONE},
{0}
};

/*** LANDevice.{i}.Hosts.Host.{i}. ***/
DMLEAF tlandevice_hostParam[] = {
{"IPAddress", &DMREAD, DMT_STRING, get_lan_host_ipaddress, NULL, NULL, &DMNONE},
{"HostName", &DMREAD, DMT_STRING, get_lan_host_hostname, NULL, NULL, &DMNONE},
{"Active", &DMREAD, DMT_BOOL, get_lan_host_active, NULL, NULL, &DMNONE},
{"MACAddress", &DMREAD, DMT_STRING, get_lan_host_macaddress, NULL, NULL, &DMNONE},
{"InterfaceType", &DMREAD, DMT_STRING, get_lan_host_interfacetype, NULL, NULL, &DMNONE},
{"AddressSource", &DMREAD, DMT_STRING, get_lan_host_addresssource, NULL, NULL, &DMNONE},
{"LeaseTimeRemaining", &DMREAD, DMT_STRING, get_lan_host_leasetimeremaining, NULL, NULL, &DMNONE},
{0}
};

/*** LANDevice.{i}.LANEthernetInterfaceConfig.{i}. ***/
DMOBJ tlanethernetinterfaceconfigObj[] = {
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tlanethernetinterfaceStatsParam, NULL},
{0}
};

DMLEAF tlanethernetinterfaceconfigParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_lan_eth_alias, set_lan_eth_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_lan_eth_iface_cfg_enable, set_lan_eth_iface_cfg_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_lan_eth_iface_cfg_status, NULL, NULL, NULL},
{"MaxBitRate", &DMWRITE, DMT_STRING, get_lan_eth_iface_cfg_maxbitrate, set_lan_eth_iface_cfg_maxbitrate, NULL, NULL},
{"DuplexMode", &DMWRITE, DMT_STRING, get_lan_eth_iface_cfg_duplexmode, set_lan_eth_iface_cfg_duplexmode, NULL, NULL},
{0}
};

/*** LANDevice.{i}.LANEthernetInterfaceConfig.{i}.Stats. ***/
DMLEAF tlanethernetinterfaceStatsParam[] = {
{"BytesSent", &DMREAD, DMT_UNINT, get_lan_eth_iface_cfg_stats_tx_bytes, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_lan_eth_iface_cfg_stats_rx_bytes, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_lan_eth_iface_cfg_stats_tx_packets, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_lan_eth_iface_cfg_stats_rx_packets, NULL, NULL, NULL},
{0}
};

/*** LANDevice.{i}.WLANConfiguration.{i}. ***/
DMOBJ tWlanConfigurationObj[] = {
{"WPS", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tWPSParam, NULL},
{"WEPKey", &DMREAD, NULL, NULL, NULL, browseWepKeyInst, NULL, NULL, NULL, tWepKeyParam, NULL},
{"PreSharedKey", &DMREAD, NULL, NULL, NULL, browsepresharedkeyInst, NULL, NULL, NULL, tpresharedkeyParam, NULL},
{"AssociatedDevice", &DMREAD, NULL, NULL, NULL, browseassociateddeviceInst, NULL, NULL, NULL, tassociateddeviceParam, NULL},
{0}
};

DMLEAF tWlanConfigurationParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wlan_alias, set_wlan_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_wlan_enable, set_wlan_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_wlan_status, NULL, NULL, NULL},
{"BSSID", &DMREAD, DMT_STRING, get_wlan_bssid, NULL, NULL, NULL},
{"MaxBitRate", &DMWRITE, DMT_STRING, get_wlan_max_bit_rate, set_wlan_max_bit_rate, NULL, NULL},
{"Channel", &DMWRITE, DMT_UNINT, get_wlan_channel, set_wlan_channel, NULL, NULL},
{"AutoChannelEnable", &DMWRITE, DMT_BOOL, get_wlan_auto_channel_enable, set_wlan_auto_channel_enable, NULL, NULL},
{"SSID", &DMWRITE, DMT_STRING, get_wlan_ssid, set_wlan_ssid, NULL, NULL},
{"BeaconType", &DMWRITE, DMT_STRING, get_wlan_beacon_type, set_wlan_beacon_type, NULL, NULL},
{"MACAddressControlEnabled", &DMWRITE, DMT_BOOL, get_wlan_mac_control_enable, set_wlan_mac_control_enable, NULL, NULL},
{"PossibleChannels", &DMREAD, DMT_STRING, get_wlan_possible_channels, NULL, NULL, NULL},
{"Standard", &DMWRITE, DMT_STRING, get_wlan_standard, set_wlan_standard, NULL, NULL},
{"WEPKeyIndex", &DMWRITE, DMT_UNINT, get_wlan_wep_key_index, set_wlan_wep_key_index, NULL, NULL},
{"KeyPassphrase", &DMWRITE, DMT_STRING, get_empty, set_wlan_key_passphrase, NULL, NULL},
{"WEPEncryptionLevel", &DMREAD, DMT_STRING, get_wlan_wep_encryption_level, NULL, NULL, NULL},
{"BasicEncryptionModes", &DMWRITE, DMT_STRING, get_wlan_basic_encryption_modes, set_wlan_basic_encryption_modes, NULL, NULL},
{"BasicAuthenticationMode", &DMWRITE, DMT_STRING, get_wlan_basic_authentication_mode, set_wlan_basic_authentication_mode, NULL, NULL},
{"WPAEncryptionModes", &DMWRITE, DMT_STRING, get_wlan_wpa_encryption_modes, set_wlan_wpa_encryption_modes, NULL, NULL},
{"WPAAuthenticationMode", &DMWRITE, DMT_STRING, get_wlan_wpa_authentication_mode, set_wlan_wpa_authentication_mode, NULL, NULL},
{"IEEE11iEncryptionModes", &DMWRITE, DMT_STRING, get_wlan_ieee_11i_encryption_modes, set_wlan_ieee_11i_encryption_modes, NULL, NULL},
{"IEEE11iAuthenticationMode", &DMWRITE, DMT_STRING, get_wlan_ieee_11i_authentication_mode, set_wlan_ieee_11i_authentication_mode, NULL, NULL},
{"RadioEnabled", &DMWRITE, DMT_BOOL, get_wlan_radio_enabled, set_wlan_radio_enabled, NULL, NULL},
{"DeviceOperationMode", &DMWRITE, DMT_STRING, get_wlan_device_operation_mode, set_wlan_device_operation_mode, NULL, NULL},
{"AuthenticationServiceMode", &DMWRITE, DMT_STRING, get_wlan_authentication_service_mode, set_wlan_authentication_service_mode, NULL, NULL},
{"TotalAssociations", &DMREAD, DMT_UNINT, get_wlan_total_associations, NULL, NULL, NULL},
{"ChannelsInUse", &DMWRITE, DMT_STRING, get_wlan_channel, set_wlan_channel, NULL, NULL},
{"TotalBytesSent", &DMREAD, DMT_UNINT, get_wlan_devstatus_statistics_tx_bytes, NULL, NULL, NULL},
{"TotalBytesReceived", &DMREAD, DMT_UNINT, get_wlan_devstatus_statistics_rx_bytes, NULL, NULL, NULL},
{"TotalPacketsSent", &DMREAD, DMT_UNINT, get_wlan_devstatus_statistics_tx_packets, NULL, NULL, NULL},
{"TotalPacketsReceived", &DMREAD, DMT_UNINT, get_wlan_devstatus_statistics_rx_packets, NULL, NULL, NULL},
{"SSIDAdvertisementEnabled", &DMWRITE, DMT_BOOL, get_wlan_ssid_advertisement_enable, set_wlan_ssid_advertisement_enable, NULL, NULL},
{"WMMEnable", &DMWRITE, DMT_BOOL, get_wmm_enabled, set_wmm_enabled, NULL, NULL},
{"X_INTENO_SE_ChannelMode", &DMWRITE, DMT_STRING, get_x_inteno_se_channelmode, set_x_inteno_se_channelmode, NULL, NULL},
{"X_INTENO_SE_SupportedStandards", &DMREAD, DMT_STRING, get_x_inteno_se_supported_standard, NULL, NULL, NULL},
{"X_INTENO_SE_OperatingChannelBandwidth", &DMWRITE, DMT_STRING, get_x_inteno_se_operating_channel_bandwidth, set_x_inteno_se_operating_channel_bandwidth, NULL, NULL},
{"X_INTENO_SE_MaxSSID", &DMWRITE, DMT_STRING, get_x_inteno_se_maxssid, set_x_inteno_se_maxssid, NULL, NULL},
{"X_INTENO_SE_ScanTimer", &DMWRITE, DMT_STRING, get_x_inteno_se_scantimer, set_x_inteno_se_scantimer, NULL, NULL},
{"X_INTENO_SE_Frequency", &DMWRITE, DMT_STRING, get_x_inteno_se_frequency, set_x_inteno_se_frequency, NULL, NULL},
{0}
};

/*** LANDevice.{i}.WLANConfiguration.{i}.WPS. ***/
DMLEAF tWPSParam[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_wlan_wps_enable, set_wlan_wps_enable, NULL, NULL},
{0}
};

/*** LANDevice.{i}.WLANConfiguration.{i}.WEPKey.{i}. ***/
DMLEAF tWepKeyParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wlan_wep_alias, set_wlan_wep_alias, NULL, NULL},
{"WEPKey", &DMWRITE, DMT_STRING, get_empty, set_wlan_wep_key1, NULL, NULL},
{0}
};

/*** LANDevice.{i}.WLANConfiguration.{i}.PreSharedKey.{i}. ***/
DMLEAF tpresharedkeyParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_wlan_psk_alias, set_wlan_psk_alias, NULL, NULL},
{"PreSharedKey", &DMWRITE, DMT_STRING, get_empty, set_wlan_pre_shared_key, NULL, NULL},
{"KeyPassphrase", &DMWRITE, DMT_STRING, get_empty, set_wlan_preshared_key_passphrase, NULL, NULL},
{"AssociatedDeviceMACAddress", &DMREAD, DMT_STRING, get_wlan_psk_assoc_MACAddress, NULL, NULL, NULL},
{0}
};

/*** LANDevice.{i}.WLANConfiguration.{i}.AssociatedDevice.{i}. ***/
DMLEAF tassociateddeviceParam[] = {
{"AssociatedDeviceMACAddress", &DMREAD, DMT_STRING, get_wlan_associated_macaddress, NULL, NULL, &DMNONE},
{"AssociatedDeviceIPAddress", &DMREAD, DMT_STRING, get_wlan_associated_ipddress, NULL, NULL, &DMNONE},
{"AssociatedDeviceAuthenticationState", &DMREAD, DMT_BOOL, get_wlan_associated_authenticationstate, NULL, NULL, &DMNONE},
{0}
};

inline int init_ldargs_lan(struct ldlanargs *args, struct uci_section *s, char *iwan)
{
	args->ldlansection = s;
	args->ldinstance = iwan;
	return 0;
}

inline int init_ldargs_wlan(struct ldwlanargs *args, struct uci_section *wifisection, int wlctl_num,
						struct uci_section *device_section, char *wunit, char *wiface, json_object *res, int pki)
{
	args->lwlansection = wifisection;
	args->device_section = device_section;
	args->res = res;
	args->wlctl_num = wlctl_num;
	args->wiface = wiface;
	args->wunit = wunit;
	args->pki = pki;
	return 0;
}

inline int init_ldargs_eth_cfg(struct ldethargs *args, char *eth, struct uci_section *eth_section)
{
	args->eth = eth;
	args->lan_ethsection = eth_section;
	return 0;
}

inline int init_client_args(struct clientargs *args, json_object *clients, char *lan_name)
{
	args->client = clients;
	args->lan_name = lan_name;
	return 0;
}

inline int init_wl_client_args(struct wl_clientargs *args, char *value, char *wiface)
{
	args->mac = value;
	args->wiface = wiface;
	return 0;
}

inline int init_args_pool_option(struct dhcppooloptionargs *args, struct uci_section *s, struct uci_section *ss)
{
	args->dhcppooloptionsection = s;
	args->dhcppoolsection = ss;
	return 0;
}
void update_dhcp_conf_start(struct execute_end_session *ees)
{
		json_object *res, *jobj;
		struct dmctx dmctx = {0};
		struct dhcp_param *dhcp_param = (struct dhcp_param *)(ees->data);
		char *mask, *start, *dhcp_name, *ipaddr, buf[16];
		
		dm_ctx_init(&dmctx, ees->dm_type, ees->amd_version, ees->instance_mode);
		dmuci_get_option_value_string("network", dhcp_param->interface, "ipaddr", &ipaddr);
		if (ipaddr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipaddr = dmjson_get_value(jobj, 1, "address");
			}
		}
		if (ipaddr[0] == '\0')
			goto end;

		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask);
		if (mask[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				mask = dmjson_get_value(jobj, 1, "mask");
				if (mask[0] == '\0')
					goto end;
				mask = cidr2netmask(atoi(mask));
			}
		}
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "start", &start);
		ipcalc_rev_start(ipaddr, mask, start, buf);
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "dhcp_sec", &dhcp_name);
		dmuci_set_value("dhcp", dhcp_name, "start", buf);
		dmuci_set_varstate_value("cwmp", dhcp_param->state_sec, "start", "");
		dmuci_commit();
		dm_ctx_clean(&dmctx);
end:
		FREE(dhcp_param->state_sec);
		FREE(dhcp_param->interface);
		FREE(dhcp_param);
		return;
}

void update_dhcp_conf_end(struct execute_end_session *ees)
{
		json_object *res, *jobj;
		char *ipaddr, *mask, *start, *dhcp_name, *limit, buf[16], buf_start[16] = "";
		struct dhcp_param *dhcp_param = (struct dhcp_param *)(ees->data);
		struct dmctx dmctx = {0};

		dm_ctx_init(&dmctx, ees->dm_type, ees->amd_version, ees->instance_mode);
		dmuci_get_option_value_string("network", dhcp_param->interface, "ipaddr", &ipaddr);
		if (ipaddr[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				ipaddr = dmjson_get_value(jobj, 1, "address");
			}
		}
		if (ipaddr[0] == '\0')
			goto end;

		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask);
		if (mask[0] == '\0') {
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", dhcp_param->interface, String}}, 1, &res);
			if (res) {
				jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
				mask = dmjson_get_value(jobj, 1, "mask");
				if (mask[0] == '\0')
					goto end;
				mask = cidr2netmask(atoi(mask));
			}
		}
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "dhcp_sec", &dhcp_name);
		dmuci_get_option_value_string("network", dhcp_param->interface, "netmask", &mask); //TODO
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "start", &start);
		if (!start || start[0] == '\0')
			dmuci_get_option_value_string("dhcp", dhcp_name, "start", &start);
		else
			ipcalc_rev_start(ipaddr, mask, start, buf_start);
		if (!start || start[0] == '\0')
			goto end;
		dmuci_get_varstate_string("cwmp", dhcp_param->state_sec, "limit", &limit);
		if (buf_start[0] != '\0')
			ipcalc_rev_end(ipaddr, mask, buf_start, limit, buf);
		else
		ipcalc_rev_end(ipaddr, mask, start, limit, buf);
		dmuci_set_value("dhcp", dhcp_name, "limit", buf);
		dmuci_set_varstate_value("cwmp", dhcp_param->state_sec, "limit", "");
		dmuci_commit();
		dm_ctx_clean(&dmctx);
end:
		FREE(dhcp_param->state_sec);
		FREE(dhcp_param->interface);
		FREE(dhcp_param);
		return;
}

int get_dhcp_option_last_inst(struct uci_section *ss)
{
	char *drinst = NULL, *tmp;
	int dr = 0;
	struct uci_section *s;

	uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), s) {
		dmuci_get_value_by_section_string(s, "optioninst", &tmp);
		if (tmp[0] == '\0')
			break;
		drinst = tmp;
	}
	if (drinst) dr = atoi(drinst);
	return dr;
}

char *dhcp_option_update_instance_alias_icwmpd(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	struct uci_section *ss = (struct uci_section *) argv[1];
	char *inst_opt = (char *) argv[2];
	char *alias_opt = (char *) argv[3];
	bool *find_max = (bool *) argv[4];

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			int m = get_dhcp_option_last_inst(ss);
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, inst_opt, buf);
	}
	*last_inst = instance;
	if (action == INSTANCE_MODE_ALIAS) {
		dmuci_get_value_by_section_string(s, alias_opt, &alias);
		if (alias[0] == '\0') {
			sprintf(buf, "cpe-%s", instance);
			alias = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, alias_opt, buf);
		}
		sprintf(buf, "[%s]", alias);
		instance = dmstrdup(buf);
	}
	return instance;
}

/*******************ADD-DEL OBJECT*********************/
int add_dhcp_serving_pool_option(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char val[64];
	char *value, *instance, *tmp;
	struct uci_section *s = NULL;
	struct uci_section *poolargs = (struct uci_section *)data;

	instance = get_last_instance(DMMAP, section_name(poolargs), "optioninst");
	DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name(poolargs), &s, &value);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "dhcp_option", "0");
	*instancepara = update_instance_icwmpd(s, instance, "optioninst");
	sprintf(val, "vendorclass%s", *instancepara);
	DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "value", val);
	sprintf(val, "0,vendorclass%s", *instancepara);
	dmuci_add_list_value_by_section( poolargs, "dhcp_option", val);
	return 0;
}

int delete_dhcp_serving_pool_option(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	char *value, *tag, *bufopt;
	struct uci_list *val;
	struct uci_section *dmmap_s = NULL, *dhcp_sec;
	struct uci_section *dmmap_ss = NULL;
	struct uci_element *e = NULL, *tmp;
	int dmmap = 0;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	switch (del_action) {
		case DEL_INST:
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &tag);
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", &value);
			dmasprintf(&bufopt, "%s,%s", tag, value);
			dmuci_get_value_by_section_list(pooloptionargs->dhcppoolsection, "dhcp_option", &val);
			if (val) {
				uci_foreach_element_safe(val, e, tmp)
				{
					if (strcmp(tmp->name, bufopt) == 0) {
						dmuci_del_list_value_by_section(pooloptionargs->dhcppoolsection, "dhcp_option", tmp->name); //TODO test it
						break;
					}
				}
			}
			dmfree(bufopt);
			DMUCI_DELETE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, NULL, NULL);
			return 0;
		case DEL_ALL:
			dhcp_sec = (struct uci_section *)data;
			uci_path_foreach_sections(icwmpd, "dmmap", section_name(dhcp_sec), dmmap_s)
			{
				if (dmmap)
					DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
				dmmap_ss = dmmap_s;
				dmmap++;
			}
			if (dmmap_ss != NULL)
				DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
			dmuci_get_value_by_section_list(dhcp_sec, "dhcp_option", &val);
			if (val) {
				uci_foreach_element_safe(val, e, tmp)
				{
					dmuci_del_list_value_by_section(dhcp_sec, "dhcp_option", tmp->name); //TODO test it
				}
			}
		}
			return 0;
}


int add_dhcp_conditional_serving_pool(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *s = NULL;
	struct uci_section *dmmap_serving_pool=NULL;

	check_create_dmmap_package("dmmap_dhcp");
	instance = get_last_instance_icwmpd("dmmap_dhcp", "vendorclass", "poulinstance");
	dmuci_add_section("dhcp", "vendorclass", &s, &value);
	dmuci_set_value_by_section(s, "dhcp_option", "");
	*instancepara = update_instance(s, instance, "poulinstance");

	dmuci_add_section_icwmpd("dmmap_dhcp", "vendorclass", &dmmap_serving_pool, &v);
	dmuci_set_value_by_section(dmmap_serving_pool, "section_name", section_name(s));
	*instancepara = update_instance_icwmpd(dmmap_serving_pool, instance, "poulinstance");
	return 0;
}

int delete_dhcp_conditional_serving_pool(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *poolargs = (struct uci_section *)data;
	int dmmap = 0;
	int found = 0;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_s = NULL;
	struct uci_section *dmmap_ss = NULL, *dmmap_section= NULL;

	switch (del_action) {
		case DEL_INST:
			uci_path_foreach_sections(icwmpd, "dmmap", section_name(poolargs), dmmap_s)
			{
				if (dmmap)
					DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
				dmmap_ss = dmmap_s;
				dmmap++;
			}
			if (dmmap_ss != NULL)
				DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);

			get_dmmap_section_of_config_section("dmmap_dhcp", "vendorclass", section_name(poolargs), &dmmap_section);
			if(dmmap_section != NULL)
				dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(poolargs, NULL, NULL);
			return 0;
		case DEL_ALL:
			uci_foreach_sections("dhcp", "vendorclass", s) {
				if (found != 0)
				{
					uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), dmmap_s)
					{
						if (dmmap != 0)
							DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
						dmmap_ss = dmmap_s;
						dmmap++;
					}
					if (dmmap_ss != NULL)
						DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
					dmmap = 0;
					dmmap_ss = NULL;
					get_dmmap_section_of_config_section("dmmap_dhcp", "vendorclass", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL)
			{
				uci_path_foreach_sections(icwmpd, "dmmap", section_name(ss), dmmap_s)
				{
					if (dmmap != 0)
						DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
					dmmap_ss = dmmap_s;
					dmmap++;
				}
				if (dmmap_ss != NULL)
					DMUCI_DELETE_BY_SECTION(icwmpd, dmmap_ss, NULL, NULL);
				get_dmmap_section_of_config_section("dmmap_dhcp", "vendorclass", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
		}
	return 0;
}

int add_landevice_dhcpstaticaddress(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value;
	char *instance;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
instance = get_last_instance_lev2("dhcp", "host", "ldhcpinstance", "dhcp", lan_name);
	dmuci_add_section("dhcp", "host", &s, &value);
	dmuci_set_value_by_section(s, "dhcp", lan_name);
	*instancepara = update_instance(s, instance, "ldhcpinstance");
	return 0;
}

int delete_landevice_dhcpstaticaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct ldlanargs *lanargs;
	struct uci_section *lddhcpsection;
	
	switch (del_action) {
		case DEL_INST:
			lddhcpsection = (struct uci_section *)data;
			dmuci_delete_by_section(lddhcpsection, NULL, NULL);
			return 0;
		case DEL_ALL:
			lanargs = (struct ldlanargs *)data;
			lan_name = section_name(lanargs->ldlansection);
			uci_foreach_option_eq("dhcp", "host", "dhcp", lan_name, s) {
				if (found != 0)
					dmuci_delete_by_section(ss, NULL, NULL);
				ss = s;
				found++;
			}
			if (ss != NULL)
				dmuci_delete_by_section(ss, NULL, NULL);
			return 0;
	}
	return 0;
}


int add_landevice_wlanconfiguration(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value;
	char ssid[16] = {0};
	char *instance;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	instance = get_last_instance_lev2("wireless", "wifi-iface", "lwlaninstance", "network", lan_name);
	sprintf(ssid, "Inteno_%s_%d", lan_name, instance ? (atoi(instance)+1) : 1);
	dmuci_add_section("wireless", "wifi-iface", &s, &value);
	dmuci_set_value_by_section(s, "device", "wl0");
	dmuci_set_value_by_section(s, "encryption", "none");
	dmuci_set_value_by_section(s, "macfilter", "0");
	dmuci_set_value_by_section(s, "mode", "ap");
	dmuci_set_value_by_section(s, "network", lan_name);
	dmuci_set_value_by_section(s, "ssid", ssid);
	*instancepara = update_instance(s, instance, "lwlaninstance");
	return 0;
}

int delete_landevice_wlanconfiguration(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct ldlanargs *lanargs;
	struct ldwlanargs *wlanargs;
	
	switch (del_action) {
		case DEL_INST:
			wlanargs = (struct ldwlanargs *)data;
			dmuci_delete_by_section(wlanargs->lwlansection, NULL, NULL);
			return 0;
		case DEL_ALL:
			lanargs = (struct ldlanargs *)data;
			lan_name = section_name(lanargs->ldlansection);
			uci_foreach_option_eq("wireless", "wifi-iface", "network", lan_name, s) {
				if (found != 0)
						dmuci_delete_by_section(ss, NULL, NULL);
				ss = s;
				found++;
			}
			if (ss != NULL)
				dmuci_delete_by_section(ss, NULL, NULL);
			return 0;
	}
	return 0;
}
/******************************************************/
/************************************************************************** 
**** ****  function related to landevice_lanhostconfigmanagement  **** ****
***************************************************************************/

int get_lan_dns(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	int len;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value_array_all(res, DELIMITOR, 1, "dns-server");
	if ((*value)[0] == '\0') {
		dmuci_get_value_by_section_string(lanargs->ldlansection, "dns", value);
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
		dmuci_get_value_by_section_string(lanargs->ldlansection, "ipaddr", value);
	}
	return 0;
}

int set_lan_dns(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
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
			dmuci_set_value_by_section(lanargs->ldlansection, "dns", dup);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;

	uci_foreach_option_eq("dhcp", "dhcp", "interface", section_name(lanargs->ldlansection), s) {
		*value = "1";
		return 0;
	}
	*value = "0";
	return 0;
}

int set_lan_dhcp_server_configurable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	bool b;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				if (!b) {
					dmuci_delete_by_section(s, NULL, NULL);
				}
				break;
			}
			if (s == NULL && b) {
				dmuci_set_value("dhcp",lan_name, NULL, "dhcp");
				dmuci_set_value("dhcp", lan_name, "interface", lan_name);
				dmuci_set_value("dhcp", lan_name, "start", "100");
				dmuci_set_value("dhcp", lan_name, "limit", "150");
				dmuci_set_value("dhcp", lan_name, "leasetime", "12h");
			}
			return 0;
	}
	return 0;
}

int get_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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

int set_lan_dhcp_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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

int get_lan_dhcp_interval_address(struct dmctx *ctx, void *data, char **value, int option)
{
	json_object *res, *jobj;
	char *ipaddr = "" , *mask = "", *start , *limit;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	struct uci_section *s = NULL;
	char bufipstart[16], bufipend[16];

	*value = "";
	
	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
		dmuci_get_value_by_section_string(s, "start", &start);
		if (option == LANIP_INTERVAL_END)
			dmuci_get_value_by_section_string(s, "limit", &limit);
		break;
	}
	if (s == NULL) {
		goto end;
	}
	dmuci_get_value_by_section_string(lanargs->ldlansection, "ipaddr", &ipaddr);
	if (ipaddr[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		if (res)
		{
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			ipaddr = dmjson_get_value(jobj, 1, "address");
		}
	}
	if (ipaddr[0] == '\0') {
		goto end;
	}
	dmuci_get_value_by_section_string(lanargs->ldlansection, "netmask", &mask);
	if (mask[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		if (res) {
			jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
			mask = dmjson_get_value(jobj, 1, "mask");
			if (mask[0] == '\0') {
				goto end;
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

end:
	return 0;
}

int get_lan_dhcp_interval_address_start(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *s_name, *tmp;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	dmasprintf(&s_name, "@%s[0]", lan_name);
	dmuci_get_varstate_string("cwmp", s_name, "start", &tmp);
	dmfree(s_name);
	if(tmp[0] != '\0')
	{
		*value = tmp;
		return 0;
	}
	int ret = get_lan_dhcp_interval_address(ctx, data, value, LANIP_INTERVAL_START);
	return ret;
}

int get_lan_dhcp_interval_address_end(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *s_name, *tmp;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	dmasprintf(&s_name, "@%s[0]", lan_name);
	dmuci_get_varstate_string("cwmp", s_name, "limit", &tmp);
	dmfree(s_name);	
	if(tmp[0] != '\0')
	{
		*value = tmp;
		return 0;
	}
	int ret = get_lan_dhcp_interval_address(ctx, data, value, LANIP_INTERVAL_END);
	return ret;
}

int set_lan_dhcp_address_start(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	json_object *res;
	char *ipaddr = "", *mask = "", *start , *limit, buf[16];
	char *s_name = "", *tmp, *dhcp_name = NULL;
	struct uci_section *s = NULL;
	struct uci_section *curr_section = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	struct dhcp_param *dhcp_param_1;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dhcp_name = section_name(s);
				break;
			}
			if (!s) return 0;
			dmasprintf(&s_name, "@%s[0]", lan_name);
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", lan_name, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", lan_name, &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", s_name, "start", value);
			dmuci_set_varstate_value("cwmp", s_name, "dhcp_sec", dhcp_name);
			dmfree(s_name);
			dhcp_param_1 = calloc(1, sizeof(struct dhcp_param));
			dhcp_param_1->interface = strdup(lan_name);
			dhcp_param_1->state_sec = strdup((curr_section)->e.name);
			dm_add_end_session(ctx, &update_dhcp_conf_start, 0, (void*)(dhcp_param_1));
			return 0;
	}
	return 0;
}

int set_lan_dhcp_address_end(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int i_val;
	json_object *res;
	char *ipaddr = "", *mask = "", *start, buf[16], *tmp, *s_name = NULL;
	struct uci_section *s = NULL;
	struct uci_section *curr_section = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	char *dhcp_name = NULL;
	struct dhcp_param *dhcp_param;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dmuci_get_value_by_section_string(s, "start", &start);
				dhcp_name = section_name(s);
				break;
			}
			if (!s) return 0;

			dmasprintf(&s_name, "@%s[0]", lan_name);
			curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", lan_name, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", lan_name, &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", s_name, "limit", value);
			dmuci_set_varstate_value("cwmp", s_name, "dhcp_sec", dhcp_name);
			dmfree(s_name);
			dhcp_param = calloc(1, sizeof(struct dhcp_param));
			dhcp_param->interface = strdup(lan_name);
			dhcp_param->state_sec = strdup((curr_section)->e.name);
			dm_add_end_session(ctx, &update_dhcp_conf_end, 0, (void*)(dhcp_param));
			return 0;
	}
	return 0;
}

int get_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) 
{	
	char val[512] = {0}, *p;
	struct uci_section *s = NULL;
	char *min, *max, *ip, *s_n_ip;
	unsigned int n_min, n_max, n_ip;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	*value = "";
	
	get_lan_dhcp_interval_address(ctx, data, &min, LANIP_INTERVAL_START);
	get_lan_dhcp_interval_address(ctx, data, &max, LANIP_INTERVAL_END);
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

int set_lan_dhcp_reserved_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	struct uci_section *dhcp_section = NULL;
	char *min, *max, *ip, *val, *local_value;
	char *pch, *spch;
	unsigned int n_min, n_max, n_ip;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			get_lan_dhcp_interval_address(ctx, data, &min, LANIP_INTERVAL_START);
			get_lan_dhcp_interval_address(ctx, data, &max, LANIP_INTERVAL_END);
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
					dmuci_add_section("dhcp", "host", &dhcp_section, &val);
					dmuci_set_value_by_section(dhcp_section, "dhcp", lan_name);
					dmuci_set_value_by_section(dhcp_section, "ip", pch);
				}
			}
			dmfree(local_value);
			return 0;
	}
	return 0;
}

int get_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	char *mask;
	json_object *res, *jobj;
	struct uci_section *s = NULL;
	char *val;
	*value = "";
	
	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
		dmuci_get_value_by_section_string(s, "netmask", value);
		break;
	}
	if (s == NULL || (*value)[0] == '\0')
		dmuci_get_value_by_section_string(lanargs->ldlansection, "netmask", value);
	if ((*value)[0] == '\0') {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		mask = dmjson_get_value(jobj, 1, "mask");
		int i_mask = atoi(mask);
		val = cidr2netmask(i_mask);
		*value = dmstrdup(val);// MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_lan_dhcp_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				dmuci_set_value_by_section(s, "netmask", value);
				return 0;
			}
			return 0;
	}
	return 0;
}

int get_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) 
{
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	
	dmuci_get_value_by_section_string(lanargs->ldlansection, "gateway", value);
	if ((*value)[0] == '\0') {
		dmuci_get_value_by_section_string(lanargs->ldlansection, "ipaddr", value);
	}
	return 0;
}

int set_lan_dhcp_iprouters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct ldlanargs *lanargs = (struct ldlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(lanargs->ldlansection, "gateway", value);
			return 0;
	}
	return 0;
}

int get_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int len, mtime = 0;
	char *ltime = "", *pch, *spch, *ltime_ini, *tmp, *tmp_ini;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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
	if (strcmp(ltime, "infinite") == 0)
		mtime = -1;
	dmfree(tmp);
	dmfree(tmp_ini);

	dmasprintf(value, "%d", mtime); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_lan_dhcp_leasetime(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action) 
{
	struct uci_section *s = NULL;
	char buf[32];
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
				int val = atoi(value);
				if (val == -1)
					sprintf(buf, "infinite");
				else
					sprintf(buf, "%ds", val);
				dmuci_set_value_by_section(s, "leasetime",  buf);
				break;
			}
			return 0;
	}	
	return 0;
}

int get_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) 
{
	char *result, *str;
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	*value = "";

	uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
		dmuci_get_value_by_section_list(s, "dhcp_option", &val);
		if (val) {
			uci_foreach_element(val, e)
			{
				if (str = strstr(e->name, "15,")) {
					*value = dmstrdup(str + sizeof("15,") - 1); //MEM WILL BE FREED IN DMMEMCLEAN
					return 0;
				}
			}
		}
	}
	return 0;
}

int set_lan_dhcp_domainname(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *result, *dn, *pch;
	struct uci_list *val;
	struct uci_section *s = NULL;
	struct uci_element *e = NULL, *tmp;
	char *option = "dhcp_option", buf[64];
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("dhcp", "dhcp", "interface", lan_name, s) {
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
	dmuci_add_list_value_by_section(s, "dhcp_option", buf);
	return 0;
}

int get_lan_host_nbr_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) 
{
	int entries = 0;
	char *network;
	json_object *res;
	struct ldlanargs *lanargs = (struct ldlanargs *)data;
	char *lan_name = section_name(lanargs->ldlansection);
	
	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, val) {
		network = dmjson_get_value(val, 1, "network");
		if (strcmp(network, lan_name) == 0)
			entries++;
	}
	dmasprintf(value, "%d", entries); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}
/***************************************************************************/


/************************************************************************** 
**** function related to landevice_lanhostconfigmanagement_ipinterface ****
***************************************************************************/

int get_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *lan_name = section_name(((struct uci_section *)data));
	return get_interface_enable_ubus(lan_name, refparam, ctx, value);
}

int set_interface_enable_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *lan_name = section_name(((struct uci_section *)data));
	return set_interface_enable_ubus(lan_name, refparam, ctx, action, value);
}

int get_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *lan_name = section_name(((struct uci_section *)data));
	return get_interface_firewall_enabled(lan_name, refparam, ctx, value);
}

int set_interface_firewall_enabled_ipinterface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *lan_name = section_name(((struct uci_section *)data));
	return set_interface_firewall_enabled(lan_name, refparam, ctx, action, value);
}

int get_interface_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *proto;
	json_object *res, *jobj;
	struct uci_section *ldipsection = (struct uci_section *)data;
	char *lan_name = section_name(ldipsection);
	
	dmuci_get_value_by_section_string(ldipsection, "proto", &proto);
	if (strcmp(proto, "static") == 0)
		dmuci_get_value_by_section_string(ldipsection, "ipaddr", value);
	else {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
		*value = dmjson_get_value(jobj, 1, "address");
	}
	return 0;
}

int set_interface_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	char *lan_name = section_name(ldipsection);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
			dmuci_set_value_by_section(ldipsection, "ipaddr", value);
			return 0;
	}
	return 0;
}

int get_interface_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	char *proto;
	char *val = NULL;
	json_object *res, *jobj;
	char *tmp;
	char *lan_name = section_name(ldipsection);
	
	dmuci_get_value_by_section_string(ldipsection, "proto", &proto);
	if (strcmp(proto, "static") == 0)
		dmuci_get_value_by_section_string(ldipsection, "netmask", value);
	else {
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", lan_name, String}}, 1, &res);
		DM_ASSERT(res, *value = "");
		jobj = dmjson_select_obj_in_array_idx(res, 0, 1, "ipv4-address");
    	val = dmjson_get_value(jobj, 1, "mask");
		tmp = cidr2netmask(atoi(val));
		*value = dmstrdup(tmp); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_interface_subnetmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (value[0] == '\0')
				return 0;
			dmuci_set_value_by_section(ldipsection, "netmask", value);
			return 0;
	}
	return 0;
}

int get_interface_addressingtype (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	*value = "";

	dmuci_get_value_by_section_string(ldipsection, "proto", value);
	if (strcmp(*value, "static") == 0)
		*value = "Static";
	else if (strcmp(*value, "dhcp") == 0)
		*value = "DHCP";
	return 0;
}

int set_interface_addressingtype(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ldipsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Static") == 0)
				dmuci_set_value_by_section(ldipsection, "proto", "static");
			else if (strcmp(value, "DHCP") == 0)
				dmuci_set_value_by_section(ldipsection, "proto", "dhcp");
			return 0;
	}
	return 0;
}

/************************************************************************************* 
**** function related to get_landevice_lanhostconfigmanagement_dhcpstaticaddress ****
**************************************************************************************/

int get_dhcpstaticaddress_enable (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mac;
	struct uci_section *lddhcpsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(lddhcpsection, "mac", &mac);
	*value = "1";
	return 0;
}

int set_dhcpstaticaddress_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *chaddr;
	bool b;
	struct uci_section *lddhcpsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			if (b == 0)
				return FAULT_9001;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			dmuci_get_value_by_section_string(lddhcpsection, "mac", &chaddr);
			if (b) {
				if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0) {
					char *orig_chaddr;
					dmuci_get_value_by_section_string(lddhcpsection, "mac_orig", &orig_chaddr);
					dmuci_set_value_by_section(lddhcpsection, "mac", orig_chaddr);
				} else {
					return 0;
				}
			} else {
				if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
					return 0;
				else {
					char *orig_chaddr;
					dmuci_get_value_by_section_string(lddhcpsection, "mac", &orig_chaddr);
					dmuci_set_value_by_section(lddhcpsection, "mac_orig", orig_chaddr);
					dmuci_set_value_by_section(lddhcpsection, "mac", DHCPSTATICADDRESS_DISABLED_CHADDR);
				}
			}
			return 0;
	}
	return 0;
}

int get_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *chaddr;
	struct uci_section *lddhcpsection = (struct uci_section *)data;
	
	dmuci_get_value_by_section_string(lddhcpsection, "mac", &chaddr);
	if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
		dmuci_get_value_by_section_string(lddhcpsection, "mac_orig", value);
	else 
		*value = chaddr;
	return 0;
}

int set_dhcpstaticaddress_chaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	char *chaddr;
	struct uci_section *lddhcpsection = (struct uci_section *)data;
		
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(lddhcpsection, "mac", &chaddr);
			if (strcmp(chaddr, DHCPSTATICADDRESS_DISABLED_CHADDR) == 0)
				dmuci_set_value_by_section(lddhcpsection, "mac_orig", value);
			else
				dmuci_set_value_by_section(lddhcpsection, "mac", value);
			return 0;
	}
	return 0;
}

int get_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *lddhcpsection = (struct uci_section *)data;
	
	dmuci_get_value_by_section_string(lddhcpsection, "ip", value);
	return 0;
}

int set_dhcpstaticaddress_yiaddr(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *lddhcpsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(lddhcpsection, "ip", value);
			return 0;
	}
	return 0;
}
/*************************************/


/************************************************************************************* 
**** function related to get_landevice_ethernet_interface_config ****
**************************************************************************************/

int get_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)data;
		
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 1, "up");
	return 0;
}

int set_lan_eth_iface_cfg_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldethargs *ethargs = (struct ldethargs *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				DMCMD("ethctl", 3, ethargs->eth, "phy-power", "up"); //TODO wait ubus command
			}
			else {
				DMCMD("ethctl", 3, ethargs->eth, "phy-power", "down"); //TODO wait ubus command
			}
			return 0;
	}
	return 0;
}

int get_lan_eth_iface_cfg_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldethargs *ethargs = (struct ldethargs *)data;
	bool b;
		
	get_lan_eth_iface_cfg_enable(refparam, ctx, data, instance, value);
	string_to_bool(*value, &b);
	if (b)
		*value = "Up";
	else 
		*value = "Disabled";
	return 0;
}

int get_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *pch, *spch, *v;
	int len;
	struct ldethargs *ethargs = (struct ldethargs *)data;
	struct uci_section *s;
	
	*value = "";
	
	uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
		dmuci_get_value_by_section_string(s, "speed", value);
	}
	if ((*value)[0] == '\0')
		return 0;
	else {
		if (strcmp(*value, "auto") == 0)
			*value = "Auto";
		else {
			v = dmstrdup(*value); // MEM WILL BE FREED IN DMMEMCLEAN
			pch = strtok_r(v, "FHfh", &spch);
			len = strlen(pch) + 1;
			*value = pch;
		}
	}
	return 0;
}

int set_lan_eth_iface_cfg_maxbitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *val = NULL;
	char *duplex;
	struct ldethargs *ethargs = (struct ldethargs *)data;
	struct uci_section *s, *sec;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
				sec = s;
				break;
			}
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(sec, "speed", value);
				return 0;
			} else {
				dmuci_get_value_by_section_string(sec, "speed", &duplex);
				if (strcmp(duplex, "auto") == 0)
					duplex = "FD";
				else {
					if (strstr(duplex, "FD"))
						duplex = "FD";
					else if (strstr(duplex, "HD"))
						duplex = "HD";
					else
						duplex = "FD";
				}
				dmastrcat(&val, value, duplex);
				dmuci_set_value_by_section(sec, "speed", val);
				dmfree(val);
			}
			return 0;

	}
	return 0;
}

int get_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp = "";
	struct ldethargs *ethargs = (struct ldethargs *)data;
	struct uci_section *s;

	uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
		dmuci_get_value_by_section_string(s, "speed", &tmp);
	}
	if (strcmp(tmp, "auto") == 0) {
		*value = "Auto";
	}
	else if (tmp[0] == '\0') {
		*value = "";
	}	
	else {
		if (strstr(tmp, "FD"))
			*value = "Full";
		else if (strstr(tmp, "HD"))
			*value = "Half";
		else
			*value = "";
	}
	return 0;
}

int set_lan_eth_iface_cfg_duplexmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *m, *spch, *rate, *val = NULL;
	struct ldethargs *ethargs = (struct ldethargs *)data;
	struct uci_section *s, *sec;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			uci_foreach_option_eq("ports", "ethport", "ifname", ethargs->eth, s) {
				sec = s;
				break;
			}
			if (strcasecmp(value, "auto") == 0) {
				dmuci_set_value_by_section(sec, "speed", "auto");
				return 0;
			}
			dmuci_get_value_by_section_string(sec, "speed", &m);
			m = dmstrdup(m);
			rate = m;
			if (strcasecmp(rate, "auto") == 0)
				rate = "100";
			else {
				strtok_r(rate, "FHfh", &spch);
			}
			if (strcasecmp(value, "full") == 0)
				dmastrcat(&val, rate, "FD");
			else if (strcasecmp(value, "half") == 0)
				dmastrcat(&val, rate, "HD");
			else {
				dmfree(m);
				return 0;
			}
			dmuci_set_value_by_section(sec, "speed", val);
			dmfree(m);
			dmfree(val);
			return 0;
	}
	return 0;
}

int get_lan_eth_iface_cfg_stats_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)data;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "tx_bytes");
	return 0;
}

int get_lan_eth_iface_cfg_stats_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)data;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "rx_bytes");
	return 0;
}

int get_lan_eth_iface_cfg_stats_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)data;

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "tx_packets");
	return 0;
}

int get_lan_eth_iface_cfg_stats_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldethargs *ethargs = (struct ldethargs *)data;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ethargs->eth, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "statistics", "rx_packets");
	return 0;
}

//HOST DYNAMIC
static char *get_interface_type(char *mac, char *ndev)
{
	json_object *res;
	int wlctl_num;
	struct uci_section *s, *d;
	char *network, *device, *value, *wunit;
	char buf[8], *p;
	
	uci_foreach_sections("wireless", "wifi-device", d) {
		wlctl_num = 0;
		wunit = section_name(d);
		uci_foreach_option_eq("wireless", "wifi-iface", "device", wunit, s) {
			dmuci_get_value_by_section_string(s, "network", &network);
			if (strcmp(network, ndev) == 0) {
				if (wlctl_num != 0) {
					sprintf(buf, "%s.%d", wunit, wlctl_num);
					p = buf;
				}
				else {
					p = wunit;
				}
				dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", p, String}}, 1, &res);
				if(res) {
					json_object_object_foreach(res, key, val) {
						value = dmjson_get_value(val, 1, "assoc_mac");
						if (strcasecmp(value, mac) == 0)
							return "802.11";
					}
				}
				wlctl_num++;
			}
		}
	}
	return "Ethernet";
}

int get_lan_host_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "ipaddr");
	return 0;	
}

int get_lan_host_hostname(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "hostname");
	return 0;	
}

int get_lan_host_active(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "connected");
	return 0;	
}

int get_lan_host_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	*value = dm_ubus_get_value(clienlargs->client, 1, "macaddr");
	return 0;	
}

int get_lan_host_interfacetype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mac;
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	mac = dm_ubus_get_value(clienlargs->client, 1, "macaddr");
	*value = get_interface_type(mac, clienlargs->lan_name);
	return 0;	
}

int get_lan_host_addresssource(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	char *dhcp;
	struct clientargs *clienlargs = (struct clientargs *)data;
	
	dhcp = dm_ubus_get_value(clienlargs->client, 1, "dhcp");
	if (strcasecmp(dhcp, "true") == 0)
		*value = "DHCP";
	else 
		*value = "Static";
	return 0;	
}

int get_lan_host_leasetimeremaining(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char buf[80], *dhcp;
	FILE *fp;
	char line[MAX_DHCP_LEASES];
	struct tm ts;
	char *leasetime, *mac_f, *mac, *line1;
	struct clientargs *clientlargs = (struct clientargs *)data;
	char delimiter[] = " \t";
	
	dhcp = dm_ubus_get_value(clientlargs->client, 1, "dhcp");
	if (strcmp(dhcp, "false") == 0) {
		*value = "0";
	}
	else {
		mac = dm_ubus_get_value(clientlargs->client, 1, "macaddr");
		fp = fopen(ARP_FILE, "r");
		if ( fp != NULL)
		{
			while (fgets(line, MAX_DHCP_LEASES, fp) != NULL )
			{
				if (line[0] == '\n')
					continue;
				line1 = dmstrdup(line);			
				leasetime = cut_fx(line, delimiter, 1);
				mac_f = cut_fx(line1, delimiter, 2);
				if (strcasecmp(mac, mac_f) == 0) {
					int rem_lease = atoi(leasetime) - time(NULL);
					if (rem_lease < 0)
						*value = "-1";
					else
						dmasprintf(value, "%d", rem_lease); // MEM WILL BE FREED IN DMMEMCLEAN					
					fclose(fp) ;
					return 0;
				}
			}
			fclose(fp);			
		}
	}		
	return 0;	
}

/**************************************************************************************
**** **** ****function related to get_landevice_wlanconfiguration_generic **** ********
***************************************************************************************/
int get_wlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int i;
	char *val;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "disabled", &val);

	if (val[0] == '1')
		*value = "0";
	else
		*value = "1";
	return 0;
}

int set_wlan_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "disabled", "0");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "disabled", "1");
			return 0;
	}
	return 0;
}

int get_wlan_status (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	dmuci_get_value_by_section_string(wlanargs->device_section, "disabled", &tmp);
	if (tmp[0] == '0' || tmp[0] == '\0')
		*value = "Up";
	else if (tmp[0] == '1')
		*value = "Disabled";		
	return 0;
}

static int get_wlan_bssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	DM_ASSERT(wlanargs->res, *value = "");
	*value = dmjson_get_value(wlanargs->res, 1, "bssid");
	return 0;
}

int get_wlan_max_bit_rate (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->device_section, "hwmode", value);
	return 0;
}

int set_wlan_max_bit_rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "hwmode", value);
			return 0;
	}
	return 0;
}

int get_wlan_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0') {
		DM_ASSERT(wlanargs->res, *value ="");
		*value = dmjson_get_value(wlanargs->res, 1, "channel");
	}
	return 0;
}

int set_wlan_channel(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "channel", value);
			return 0;
	}
	return 0;
}

int get_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_auto_channel_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "auto";
			else {
				if(wlanargs->res == NULL)
					return 0;
				else
					value = dmjson_get_value(wlanargs->res, 1, "channel");
			}
			dmuci_set_value_by_section(wlanargs->device_section, "channel", value);
			return 0;
	}
	return 0;
}

static int get_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "ssid", value);
	return 0;
}

static int set_wlan_ssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->lwlansection, "ssid", value);
			return 0;
	}
	return 0;
}

int get_wlan_beacon_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", value);
	if (strcmp(*value, "none") == 0)
		*value = "None";
	else if (strcmp(*value, "wep-shared") == 0 || strcmp(*value, "wep-open") == 0)
		*value = "Basic";
	else if (strcmp(*value, "psk") == 0 || strcmp(*value, "psk+") == 0 || strcmp(*value, "wpa") == 0)
		*value = "WPA";
	else if (strcmp(*value, "psk2") == 0 || strcmp(*value, "psk2+") == 0 || strcmp(*value, "wpa2") == 0)
		*value = "11i";
	else if (strcmp(*value, "mixed-psk") == 0 || strcmp(*value, "mixed-psk+") == 0 || strcmp(*value, "mixed-wpa") == 0)
		*value = "WPAand11i";
	return 0;
}

int set_wlan_beacon_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	char *encryption, *option;
	char strk64[4][11];

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "None") == 0) {
				value = "none";
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if (strcmp(value, "Basic") == 0) {
				value = "wep-open";
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					wepkey64("Inteno", strk64);
					int i = 0;
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "WPA") == 0) {
				value = "psk";
				char *encryption;
				dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
				if (!strstr(encryption, "psk")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "tkip");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "11i") == 0) {
				value = "psk2";
				if (!strstr(encryption, "psk2")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "ccmp");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			else if(strcmp(value, "WPAand11i") == 0) {
				value = "mixed-psk";
				if (!strstr(encryption, "mixed-psk")){
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "cipher", "tkip+ccmp");
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", value);
			}
			return 0;
	}
	return 0;
}

int get_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	char *macfilter;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "macfilter", &macfilter);
	if (macfilter[0] != '0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_mac_control_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				value = "2";
			else
				value = "0";
			dmuci_set_value_by_section(wlanargs->lwlansection, "macfilter", value);
			return 0;
	}
	return 0;
}

int get_wlan_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->device_section, "hwmode", value);
	if (strcmp(*value, "11b") == 0)
		*value = "b";
	else if (strcmp(*value, "11bg") == 0)
		*value = "g";
	else if (strcmp(*value, "11g") == 0 || strcmp(*value, "11gst") == 0 || strcmp(*value, "11lrs") == 0)
		*value = "g-only";
	else if (strcmp(*value, "11n") == 0 || strcmp(*value, "auto") == 0)
		*value = "n";
	return 0;
}

int set_wlan_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "b") == 0)
				value = "11b";
			else if (strcmp(value, "g") == 0)
				value = "11bg";
			else if (strcmp(value, "g-only") == 0)
				value = "11g";
			else if (strcmp(value, "n") == 0)
				value = "auto";
			dmuci_set_value_by_section(wlanargs->device_section, "hwmode", value);
			return 0;
	}
	return 0;
}

int get_wlan_possible_channels(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res, *jobj;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	*value = "";
	dmubus_call("router.wireless", "radios", UBUS_ARGS{}, 0, &res);
	if(res)
	*value = dmjson_get_value_array_all(res, DELIMITOR, 2, wlanargs->wunit, "channels");
	return 0;
}

int get_wlan_wep_key_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	char *encryption;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0)
		dmuci_get_value_by_section_string(wlanargs->lwlansection, "key", value);
	else
		*value = "";
	return 0;
}

int set_wlan_wep_key_index(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0)
			{
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
			}
			dmuci_set_value_by_section(wlanargs->lwlansection, "key", value);
			return 0;
	}	
	return 0;
}

int set_wlan_key_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "wep-open") == 0) {
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
			} else if (strcmp(encryption, "none") == 0)
				return 0;
			else
				return set_wlan_pre_shared_key(refparam, ctx, data, instance, value, action);
			return 0;
	}
	return 0;
}

int get_wlan_wep_encryption_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "40-bit, 104-bit";
	return 0;
}

int get_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "wep-open") == 0)
		*value = "WEPEncryption";
	else 
		*value = "None";
	return 0;
}

int set_wlan_basic_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *option, *encryption;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "WEPEncryption") == 0) {
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					int i = 0;
					wepkey64("Inteno", strk64);
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
			} else if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			return 0;
	}
	return 0;
}

int get_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wep-shared") == 0)
		*value = "SharedAuthentication";
	else
		*value = "None";
	return 0;
}

int set_wlan_basic_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption, *option;
	char strk64[4][11];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "SharedAuthentication") == 0) {
				if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", "1");
					int i = 0;
					wepkey64("Inteno", strk64);
					while (i < 4) {
						dmasprintf(&option, "key%d", i + 1);
						dmuci_set_value_by_section(wlanargs->lwlansection, option, strk64[i]);
						dmfree(option);
						i++;
					}
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wep-open");
			} else if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			return 0;
	}
	return 0;
}

int get_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	*value = "";
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "psk+tkip") == 0 || strcmp(encryption, "mixed-psk+tkip") == 0)
		*value = "TKIPEncryption";
	else if (strcmp(encryption, "psk+ccmp") == 0 || strcmp(encryption, "mixed-psk+ccmp") == 0)
		*value = "AESEncryption";
	else if (strcmp(encryption, "psk+tkip+ccmp") == 0 || strcmp(encryption, "mixed-psk+tkip+ccmp") == 0)
		*value = "TKIPandAESEncryption";
	return 0;
}

int set_wlan_wpa_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "TKIPEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+tkip");
			}
			else if (strcmp(value, "AESEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+ccmp");
			}
			else if (strcmp(value, "TKIPandAESEncryption") == 0) {
				if(!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk+tkip+ccmp");
			}
			return 0;
	}
	return 0;
}

int get_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk") == 0 || strncmp(encryption, "psk+", 4) == 0 || strncmp(encryption, "mixed-psk", 9) == 0)
		*value = "PSKAuthentication";
	else if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "EAPAuthentication";
	return 0;
}

int set_wlan_wpa_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "PSKAuthentication") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk");
			}
			else if (strcmp(value, "EAPAuthentication") == 0) {
				if(strcmp(encryption, "wpa") != 0 && strcmp(encryption, "wpa2") != 0 && strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa");
			}
			return 0;
	}
	return 0;
}

int get_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk2+tkip") == 0 || strcmp(encryption, "mixed-psk+tkip") == 0)
		*value = "TKIPEncryption";
	else if (strcmp(encryption, "psk2+ccmp") == 0 || strcmp(encryption, "mixed-psk+ccmp") == 0)
		*value = "AESEncryption";
	else if (strcmp(encryption, "psk2+tkip+ccmp") == 0 || strcmp(encryption, "mixed-psk+tkip+ccmp") == 0)
		*value = "TKIPandAESEncryption";
	return 0;
}

int set_wlan_ieee_11i_encryption_modes(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "TKIPEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+tkip");
			}
			else if (strcmp(value, "AESEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+ccmp");
			}
			else if (strcmp(value, "TKIPandAESEncryption") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2+tkip+ccmp");
			}
			return 0;
	}
	return 0;
}

int get_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	*value = "";
	if (strcmp(encryption, "psk2") == 0 || strncmp(encryption, "psk2+", 5) == 0 || strncmp(encryption, "mixed-psk", 9) == 0 )
		*value = "PSKAuthentication";
	else if (strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "EAPAuthentication";
	return 0;
}

int set_wlan_ieee_11i_authentication_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "PSKAuthentication") == 0) {
				if (!strstr(encryption, "psk")) {
					reset_wlan(wlanargs->lwlansection);
					char *gnw = get_nvram_wpakey();
					dmuci_set_value_by_section(wlanargs->lwlansection, "key", gnw);
					dmuci_set_value_by_section(wlanargs->lwlansection, "gtk_rekey", "3600");
					dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
					dmfree(gnw);
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "psk2");
			}
			else if (strcmp(value, "EAPAuthentication") == 0) {
				if (strcmp(encryption, "wpa") != 0 && strcmp(encryption, "wpa2") != 0 && strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa2");
			}
			return 0;
	}	
	return 0;
}

int get_wlan_radio_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *tmp;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->device_section, "disabled", &tmp);	
	if (tmp[0] == '0' || tmp[0] == '\0')
		*value = "1";
	else if (tmp[0] == '1')
		*value = "0";
	return 0;
}

int set_wlan_radio_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *wunit, buf[8];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->device_section, "disabled", "0");
			else
				dmuci_set_value_by_section(wlanargs->device_section, "disabled", "1");			
			return 0;
	}
	return 0;
}

int get_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "mode", value);
	if (strcmp(*value, "ap") == 0)
		*value = "InfrastructureAccessPoint";
	else 
		*value = "";
	return 0;
}

int set_wlan_device_operation_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "InfrastructureAccessPoint") == 0)
				dmuci_set_value_by_section(wlanargs->lwlansection, "mode", "ap");
			return 0;
	}
	return 0;
}

int get_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
	if (strcmp(encryption, "wpa") == 0 || strcmp(encryption, "wpa2") == 0 || strcmp(encryption, "mixed-wpa") == 0)
		*value = "RadiusClient";
	else 
		*value = "None";
	return 0;
}

int set_wlan_authentication_service_mode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wlanargs->lwlansection, "encryption", &encryption);
			if (strcmp(value, "None") == 0) {
				reset_wlan(wlanargs->lwlansection);
				dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "none");
			}
			else if (strcmp(value, "RadiusClient") == 0) {
				if (strcmp(encryption, "wpa") != 0 || strcmp(encryption, "wpa2") != 0 || strcmp(encryption, "mixed-wpa") != 0) {
					reset_wlan(wlanargs->lwlansection);
					dmuci_set_value_by_section(wlanargs->lwlansection, "encryption", "wpa");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_server", "");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_port", "1812");
					dmuci_set_value_by_section(wlanargs->lwlansection, "radius_secret", "");
				}
			}
			return 0;
	}
	return 0;
}

int get_wlan_total_associations(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	int i = 0;
	json_object *res;
	char *wunit, buf[8];
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "0");
	json_object_object_foreach(res, key, val) {
		if (strstr(key, "sta-"))
			i++;
	}
	dmasprintf(value, "%d", i); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_wlan_devstatus_statistics_tx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	json_object *res;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","tx_bytes");
	return 0;
}

int get_wlan_devstatus_statistics_rx_bytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","rx_bytes");
	return 0;
}

int get_wlan_devstatus_statistics_tx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","tx_packets");
	return 0;
}

int get_wlan_devstatus_statistics_rx_packets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *val = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmubus_call("network.device", "status", UBUS_ARGS{{"name",  wlanargs->wiface, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics","rx_packets");
	return 0;
}

static int get_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *hidden;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "hidden", &hidden);
	if (hidden[0] == '1' && hidden[1] == '\0')
		*value = "0";
	else
		*value = "1";
	return 0;
}

static int set_wlan_ssid_advertisement_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "hidden", "");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "hidden", "1");
			return 0;

	}
	return 0;
}

int get_wlan_wps_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *wps_pbc;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "wps_pbc", &wps_pbc);
	if (wps_pbc[0] == '1' && wps_pbc[1] == '\0')
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_wlan_wps_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "1");
			else
				dmuci_set_value_by_section(wlanargs->lwlansection, "wps_pbc", "");
			return 0;
	}
	return 0;
}

int get_x_inteno_se_channelmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	dmuci_get_value_by_section_string(wlanargs->device_section, "channel", value);
	if (strcmp(*value, "auto") == 0 || (*value)[0] == '\0')
		*value = "Auto";
	else
		*value = "Manual";
	return 0;
}

int set_x_inteno_se_channelmode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *channel;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Auto") == 0)
				dmuci_set_value_by_section(wlanargs->device_section, "channel", "auto");
			else if (strcmp(value, "Manual") == 0) {
				if (wlanargs->res != NULL) {
					channel = dmjson_get_value(wlanargs->res, 1, "channel");
					if (channel[0] != '\0')
						dmuci_set_value_by_section(wlanargs->device_section, "channel", channel);
				}
			}
			return 0;
	}
	return 0;
}

int get_x_inteno_se_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *freq;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	DM_ASSERT(wlanargs->res, *value = "b, g, n, gst, lrs");
	freq = dmjson_get_value(wlanargs->res, 1, "frequency");
	if (strcmp(freq, "5") == 0)
		*value = "a, n, ac";
	else
		*value = "b, g, n, gst, lrs";
	return 0;
}

int get_x_inteno_se_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *bandwith;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	dmuci_get_value_by_section_string(wlanargs->device_section, "bandwidth", value);
	if (*value[0] == '\0')
	{
		DM_ASSERT(wlanargs->res, *value = "");
		bandwith = dmjson_get_value(wlanargs->res, 1, "bandwidth");
	}
	dmastrcat(value, *value, "MHz"); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_x_inteno_se_operating_channel_bandwidth(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *spch, *dup;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dup = dmstrdup(value);
			pch = strtok_r(dup, "Mm", &spch);
			dmuci_set_value_by_section(wlanargs->device_section, "bandwidth", pch);
			dmfree(dup);
			return 0;
	}
	return 0;
}

int get_x_inteno_se_maxssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	dmuci_get_value_by_section_string(wlanargs->lwlansection, "bss_max", value);
	return 0;
}

int set_x_inteno_se_maxssid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->lwlansection, "bss_max", value);
			return 0;
	}

	return 0;
}

int set_wlan_wep_key(char *refparam, struct dmctx *ctx, void *data, char *instance, int action, char *value, char *key_index)
{
	char *encryption, *option;
	char strk64[4][11];
	struct wlan_wep *wepargs = (struct wlan_wep *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(wepargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") != 0 && strcmp(encryption, "wep-open") != 0) {
				reset_wlan(wepargs->lwlansection);
				dmuci_set_value_by_section(wepargs->lwlansection, "encryption", "wep-open");
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(wepargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
				dmuci_set_value_by_section(wepargs->lwlansection, "key", "1");
			}
			dmuci_set_value_by_section(wepargs->lwlansection, key_index, value);
			return 0;
	}
	return 0;
}

int set_wlan_wep_key1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char buf[8];
	sprintf(buf, "key%d", ((struct wlan_wep *)data)->key_index);
	return set_wlan_wep_key(refparam, ctx, data, instance, action, value, buf);
}
/****************************************************************************************/

int get_wlan_associated_macaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)data;
	
	*value = dmstrdup(clientwlargs->mac);
	return 0;
}

int get_wlan_associated_ipddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	FILE *fp;
	char *ip, *mac, *line1;
	char delimiter[] = " \t";
	char line[MAX_PROC_ARP];
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)data;
	
	fp = fopen(ARP_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ARP, fp);
		while (fgets(line, MAX_PROC_ARP, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			line1 = dmstrdup(line); // MEM WILL BE FREED IN DMMEMCLEAN
			mac = cut_fx(line, delimiter, 4);
			if (strcasecmp(mac, clientwlargs->mac) == 0) {
				*value = cut_fx(line1, delimiter, 1);
				fclose(fp) ;
				return 0;
			}
		}
		fclose(fp);
	}
	*value = "";
	return 0;
}

int get_wlan_associated_authenticationstate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value) {
	struct wl_clientargs *clientwlargs = (struct wl_clientargs *)data;
	char buf[256];
	int pp, r;
	*value = "0";

	pp = dmcmd("/usr/sbin/wlctl", 3, "-i", clientwlargs->wiface, "authe_sta_list"); //TODO wait ubus command
	if (pp) {
		r = dmcmd_read(pp, buf, 256);
		close(pp);
		if (r > 0 && strcasestr(buf, clientwlargs->mac))
			*value = "1";
		else
			*value = "0";
	}
	return 0;
}

int set_wlan_pre_shared_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *encryption;
	struct wlan_psk *pskargs = (struct wlan_psk *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pskargs->lwlansection, "encryption", &encryption);
			if (!strstr(encryption, "psk")) {
				reset_wlan(pskargs->lwlansection);
				dmuci_set_value_by_section(pskargs->lwlansection, "gtk_rekey", "3600");
				dmuci_set_value_by_section(pskargs->lwlansection, "encryption", "psk");
			}
			dmuci_set_value_by_section(pskargs->lwlansection, "key", value);
			return 0;
	}
	return 0;
}

int set_wlan_preshared_key_passphrase(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *option, *encryption;
	char strk64[4][11];
	struct wlan_psk *pskargs = (struct wlan_psk *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pskargs->lwlansection, "encryption", &encryption);
			if (strcmp(encryption, "wep-shared") == 0 || strcmp(encryption, "wep-open") == 0) {
				wepkey64("Inteno", strk64);
				int i = 0;
				while (i < 4) {
					dmasprintf(&option, "key%d", i + 1);
					dmuci_set_value_by_section(pskargs->lwlansection, option, strk64[i]);
					dmfree(option);
					i++;
				}
			} else if (strcmp(encryption, "none") == 0)
				return 0;
			else
				return set_wlan_pre_shared_key(refparam, ctx, data, instance, value, action);
			return 0;
	}
	return 0;
}

int get_wlan_psk_assoc_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	char *wunit, *encryption, buf[8];
	char sta_pki[8];
	struct wlan_psk *pskargs = (struct wlan_psk *)data;
	
	dmuci_get_value_by_section_string(pskargs->lwlansection, "encryption", &encryption);
	if (strstr(encryption, "psk")) {
		sprintf(sta_pki, "sta-%d", pskargs->pki);
		dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", pskargs->wiface}}, 1, &res);
		DM_ASSERT(res, *value = "");
		*value = dmjson_get_value(res, 2, sta_pki, "macaddr");
		return 0;
	}
	*value = "";
	return 0;
}

int get_x_inteno_se_scantimer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	*value = "0";
	dmuci_get_value_by_section_string(wlanargs->device_section, "scantimer", value);
	return 0;
}

int set_x_inteno_se_scantimer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(wlanargs->device_section, "scantimer", value);
			return 0;
	}
	return 0;
}

static int get_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	bool b;

	dmuci_get_value_by_section_string(wlanargs->device_section, "wmm", value);
	string_to_bool(*value, &b);
		if (b)
			*value = "1";
		else
			*value = "0";

	return 0;
}

int get_x_inteno_se_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *freq;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	DM_ASSERT(wlanargs->res, *value = "");
	freq = dmjson_get_value(wlanargs->res, 1, "frequency");
	dmastrcat(value, freq, "GHz");
	return 0;
}

int set_x_inteno_se_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *freq = NULL;
	json_object *res = NULL;
	struct uci_section *s = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if(wlanargs->res)
				freq = dm_ubus_get_value(wlanargs->res, 1, "frequency");
			if (freq && strcmp(freq, value) == 0)
			{
				return 0;
			}
			else if (value[0] == '5' || value[0] == '2')
			{
				uci_foreach_sections("wireless", "wifi-device", s) {
					dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", section_name(s), String}}, 1, &res);
					if(res)
					{
						freq = dmjson_get_value(res, 1, "frequency");
						if (strcmp(freq, value) == 0)
						{
							dmuci_set_value_by_section(wlanargs->lwlansection, "device", section_name(s));
							return 0;
						}
					}
				}
			}
			else
				return 0;
	}
	return 0;
}
static int set_wmm_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)data;
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				dmuci_set_value_by_section(wlanargs->device_section, "wmm", "1");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_noack", "1");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_apsd", "1");
			}
			else {
				dmuci_set_value_by_section(wlanargs->device_section, "wmm", "0");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_noack", "");
				dmuci_set_value_by_section(wlanargs->device_section, "wmm_apsd", "");
			}
			return 0;
	}
	return 0;
}

void lan_eth_update_section_option_list (char *name, char *sec_name, char *wan_eth)
{
	char *pch, *spch, *ifname;

	if (name[0] == '\0') {
		update_section_option_list(DMMAP, "lan_eth", "ifname", "network", "", sec_name, name);
	}
	ifname = dmstrdup(name);
	for (pch = strtok_r(ifname, " ,", &spch);
		pch != NULL;
		pch = strtok_r(NULL, " ,", &spch)) {
		if (strncmp(pch, "eth", 3) != 0 || strncmp(pch, wan_eth, 4) == 0)
			continue;
		update_section_option_list(DMMAP, "lan_eth", "ifname", "network", pch, sec_name, name);
	}
	dmfree(ifname);
}
/////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_lan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ldlanargs *)data)->ldlansection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "ldalias", value);
	return 0;
}

int set_lan_dev_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(((struct ldlanargs *)data)->ldlansection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "ldalias", value);
			return 0;
	}
	return 0;
}

int get_lan_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(ldipsection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "lipalias", value);
	return 0;
}

int set_lan_ip_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ldipsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name(ldipsection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "lipalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *lddhcpsection = (struct uci_section *)data;
	dmuci_get_value_by_section_string(lddhcpsection, "ldhcpalias", value);
	return 0;
}

int set_dhcp_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *lddhcpsection = (struct uci_section *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(lddhcpsection, "ldhcpalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ldwlanargs *)data)->lwlansection, "lwlanalias", value);
	return 0;
}

int set_wlan_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct ldwlanargs *)data)->lwlansection, "lwlanalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_psk_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wlan_psk *)data)->wlanpsk, "pskalias", value);
	return 0;
}

int set_wlan_psk_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wlan_psk *)data)->wlanpsk, "pskalias", value);
			return 0;
	}
	return 0;
}

int get_wlan_wep_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct wlan_wep *)data)->wlanwep, "wepalias", value);
	return 0;
}

int set_wlan_wep_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct wlan_wep *)data)->wlanwep, "wepalias", value);
			return 0;
	}
	return 0;
}

int get_lan_eth_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ldethargs *)data)->lan_ethsection, "ethalias", value);
	return 0;
}

int set_lan_eth_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct ldethargs *)data)->lan_ethsection, "ethalias", value);
			return 0;
	}
	return 0;
}

int get_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_dhcp", "vendorclass", section_name((struct uci_section *)data), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "poulalias", value);
	return 0;
}

int set_dhcp_conditional_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_dhcp", "vendorclass", section_name((struct uci_section *)data), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "poulalias", value);
			return 0;
	}
	return 0;
}


int get_dhcp_conditional_servingpool_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "enable", value);
	return 0;
}

int set_dhcp_conditionalservingpool_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				dmuci_set_value_by_section((struct uci_section *)data, "enable", "");
			}
			else {
				dmuci_set_value_by_section((struct uci_section *)data, "enable", "0");
			}
			return 0;
	}
	return 0;
}

int get_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "vendorclass", value);
	return 0;
}

int set_dhcp_conditional_servingpool_vendorclassid(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "vendorclass", value);
			return 0;
	}
	return 0;
}

int get_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "networkid", value);
	return 0;
}

int set_dhcp_conditional_servingpool_network_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "networkid", value);
			return 0;
	}
	return 0;
}

int get_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "optionalias", value);
	return 0;
}

int set_dhcp_servingpool_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "optionalias", value);
			return 0;
	}
	return 0;
}


int get_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", value);
	return 0;
}

int set_dhcp_servingpool_tag(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp = NULL;
	char *option_tmp = NULL;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", &tmp);
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &option_tmp);
			update_uci_dhcpserver_option(ctx, pooloptionargs->dhcppoolsection, option_tmp, value, tmp);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "dhcp_option", value);
			return 0;
	}
	return 0;
}

int get_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "value", value);
	return 0;
}

int set_dhcp_servingpool_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{

	char *tmp;
	struct dhcppooloptionargs *pooloptionargs = (struct dhcppooloptionargs *)data;

	switch (action) {
		case VALUECHECK:
				return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(pooloptionargs->dhcppooloptionsection, "dhcp_option", &tmp);
			set_uci_dhcpserver_option(ctx, pooloptionargs->dhcppoolsection, tmp, value);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, pooloptionargs->dhcppooloptionsection, "value", value);

			return 0;
	}
	return 0;
}
/**************************************************************************
* LINKER
***************************************************************************/

int get_linker_lanhost_interface(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	
	struct uci_section *ldipsection = (struct uci_section *)data;
	if(ldipsection) {
		dmasprintf(linker,"linker_interface:%s", section_name(ldipsection));
		return 0;
	}
	*linker = "";
	return 0;
}



int browselandeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *idev = NULL, *idev_last = NULL;
	struct ldlanargs curr_lanargs = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		if(filter_lan_device_interface(p->config_section, "")!=0) continue;
		idev = handle_update_instance(1, dmctx, &idev_last, update_instance_alias, 3, p->dmmap_section, "ldinstance", "ldalias");
		init_ldargs_lan(&curr_lanargs, p->config_section, idev);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_lanargs, idev) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseIPInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *ss = NULL;
	struct uci_section *sss = NULL;
	char *ilan = NULL, *ilan_last = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)prev_data;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		if(filter_lan_ip_interface(p->config_section, lanargs->ldlansection)!=0) continue;
		ilan = handle_update_instance(2, dmctx, &ilan_last, update_instance_alias, 3, p->dmmap_section, "lipinstance", "lipalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, ilan) == DM_STOP)
			break;
		/*SUBENTRY(entry_landevice_ipinterface_instance, ctx, idev, ilan);
	uci_foreach_option_cont("dhcp", "host", "interface", section_name(ss), sss) {
		idhcp = handle_update_instance(2, ctx, &idhcp_last, update_instance_alias, 3, sss, "ldhcpinstance", "ldhcpalias");
		init_ldargs_dhcp(ctx, sss);
		SUBENTRY(entry_landevice_dhcpstaticaddress_instance, ctx, idev, idhcp);
		}*/
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseDhcp_static_addressInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *ss = NULL;
	struct uci_section *sss = NULL;
	char *ilan = NULL, *ilan_last = NULL;
	char *idhcp = NULL, *idhcp_last = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)prev_data;

	uci_foreach_filter_func("network", "interface", lanargs->ldlansection, filter_lan_ip_interface, ss) {
		ilan = handle_update_instance(2, dmctx, &ilan_last, update_instance_alias, 3, ss, "lipinstance", "lipalias");
		uci_foreach_option_cont("dhcp", "host", "dhcp", section_name(ss), sss) {
			idhcp = handle_update_instance(2, dmctx, &idhcp_last, update_instance_alias, 3, sss, "ldhcpinstance", "ldhcpalias");
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)sss, idhcp) == DM_STOP)
				goto end;
		}
	}
end:
	return 0;
}

int browselanethernetinterfaceconfigInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	int i = 0;
	char *ifname, *wan_eth, *baseifname;
	char *ieth = NULL, *ieth_last = NULL;
	struct uci_section *s = NULL;
	struct ldlanargs *lanargs = (struct ldlanargs *)prev_data;
	struct ldethargs curr_ethargs = {0};

	dmuci_get_option_value_string("ports", "WAN", "ifname", &wan_eth);
	dmuci_get_value_by_section_string(lanargs->ldlansection, "ifname", &ifname);
	lan_eth_update_section_option_list(ifname, section_name(lanargs->ldlansection), wan_eth);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "lan_eth", "network", section_name(lanargs->ldlansection), s) {
		dmuci_get_value_by_section_string(s, "ifname", &baseifname);
		init_ldargs_eth_cfg(&curr_ethargs, baseifname, s);
		ieth =  handle_update_instance(2, dmctx, &ieth_last, update_instance_alias_icwmpd, 3, s, "ethinstance", "ethalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ethargs, ieth) == DM_STOP)
			break;
	}
	return 0;
}

int browseWlanConfigurationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *ss = NULL;
	struct uci_section *sss = NULL;
	json_object *res;
	char *iwlan = NULL, *iwlan_last = NULL;
	char *network , *wiface, buf[8], *lan_sec;
	struct ldlanargs *lanargs = (struct ldlanargs *)prev_data;
	struct ldwlanargs curr_wlanargs = {0};

	lan_sec = section_name(lanargs->ldlansection);
	iwlan = get_last_instance_lev2("wireless", "wifi-iface", "lwlaninstance", "network", lan_sec);
	uci_foreach_sections("wireless", "wifi-device", ss) {
		int wlctl_num=0;
		uci_foreach_option_eq("wireless", "wifi-iface", "device", section_name(ss), sss) {
			dmuci_get_value_by_section_string(sss, "network", &network);
			if (strcmp(network, lan_sec) != 0)
				continue;
			iwlan = handle_update_instance(2, dmctx, &iwlan_last, update_instance_alias, 3, sss, "lwlaninstance", "lwlanalias");
			wiface = section_name(ss);
			if (wlctl_num != 0) {
				sprintf(buf, "%s.%d", wiface, wlctl_num);
				wiface = buf;
			}
			dmubus_call("router.wireless", "status", UBUS_ARGS{{"vif", wiface, String}}, 1, &res);
			init_ldargs_wlan(&curr_wlanargs, sss, wlctl_num, ss, section_name(ss), wiface, res, 0);
			wlctl_num++;
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wlanargs, iwlan) == DM_STOP)
				goto end;
		}
	}
end:
	return 0;
}

int browseWepKeyInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	int i = 0;
	char *iwep = NULL, *iwep_last = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)prev_data;
	struct uci_section *s = NULL;
	struct wlan_wep curr_wepargs = {0};

	update_section_list(DMMAP,"wlan-wepkey", "wlan", 4, section_name(wlanargs->lwlansection), NULL, NULL, NULL, NULL);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "wlan-wepkey", "wlan", section_name(wlanargs->lwlansection), s) {
		curr_wepargs.wlanwep = s;
		curr_wepargs.key_index = ++i;
		curr_wepargs.lwlansection = wlanargs->lwlansection;
		iwep =  handle_update_instance(3, dmctx, &iwep_last, update_instance_alias_icwmpd, 3, s, "wepinstance", "wepalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wepargs, iwep) == DM_STOP)
			break;
	}
	return 0;
}

int browsepresharedkeyInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *ipk = NULL, *ipk_last = NULL ;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)prev_data;
	struct uci_section *s = NULL;
	struct wlan_psk curr_pskargs = {0};

	curr_pskargs.pki = 0;
	//update section list of wlan-psk before update instance
	update_section_list(DMMAP,"wlan-psk", "wlan", 10, section_name(wlanargs->lwlansection), NULL, NULL, NULL, NULL);
	uci_path_foreach_option_eq(icwmpd, "dmmap", "wlan-psk", "wlan", section_name(wlanargs->lwlansection), s) {
		curr_pskargs.pki++;
		curr_pskargs.wlanpsk = s;
		curr_pskargs.lwlansection = wlanargs->lwlansection;
		curr_pskargs.wiface = wlanargs->wiface;
		ipk =  handle_update_instance(3, dmctx, &ipk_last, update_instance_alias_icwmpd, 3, s, "pskinstance", "pskalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_pskargs, ipk) == DM_STOP)
			break;
	}
	return 0;
}

int browseassociateddeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	int id = 0;
	json_object *res, *wl_client_obj;
	char *idx, *idx_last = NULL;
	struct ldwlanargs *wlanargs = (struct ldwlanargs *)prev_data;
	struct wl_clientargs curr_wl_clientargs = {0};

	dmubus_call("router.wireless", "stas", UBUS_ARGS{{"vif", wlanargs->wiface, String}}, 1, &res);
	if (res) {
		char *value;
		json_object_object_foreach(res, key, wl_client_obj) {
			idx = handle_update_instance(3, dmctx, &idx_last, update_instance_without_section, 1, ++id);
			value = dmjson_get_value(wl_client_obj, 1, "macaddr");
			init_wl_client_args(&curr_wl_clientargs, value, wlanargs->wiface);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_wl_clientargs, idx) == DM_STOP)
				break;
		}
	}
	return 0;
}

int browselandevice_hostInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	json_object *res, *client_obj;
	char *network;
	char *idx, *idx_last = NULL;
	int id = 0;
	struct ldlanargs *lanargs = (struct ldlanargs *)prev_data;
	struct clientargs curr_clientargs = {0};

	dmubus_call("router.network", "clients", UBUS_ARGS{}, 0, &res);
	if (res) {
		json_object_object_foreach(res, key, client_obj) {
			network = dmjson_get_value(client_obj, 1, "network");
			if (strcmp(network, section_name(lanargs->ldlansection)) == 0) {
				init_client_args(&curr_clientargs, client_obj, section_name(lanargs->ldlansection));
				idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_clientargs, idx) == DM_STOP)
					break;
			}
		}
	}
	return 0;
}


int browselandevice_lanhostconfigmanagement_dhcpconditionalservingpool_instance(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *icondpool = NULL, *icondpool_last = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("dhcp", "vendorclass", "dmmap_dhcp", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		icondpool =  handle_update_instance(1, dmctx, &icondpool_last, update_instance_alias, 3, p->dmmap_section, "poulinstance", "poulalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, icondpool) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseentry_landevice_dhcpconditionalservingpool_option_instance(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	int id = 0;
		char *idx = NULL, *pch, *spch, *name, *value;
		char *idx_last = NULL;
		struct uci_list *val;
		struct uci_element *e = NULL, *tmp;
		struct uci_section *ss = NULL;
		bool find_max = true;
		char *tt;
		struct dhcppooloptionargs cur_dhcp_option = {0};
		int found = 0;

		dmuci_get_value_by_section_list((struct uci_section *)prev_data, "dhcp_option", &val);
		if (val) {
			uci_foreach_element_safe(val, e, tmp)
			{
				tt = dmstrdup(tmp->name);
				pch = strtok_r(tt, ",", &spch);
				found = 0;
				uci_path_foreach_option_eq(icwmpd, "dmmap", section_name((struct uci_section *)prev_data), "dhcp_option", pch, ss)
				{
					dmuci_get_value_by_section_string(ss, "value", &value);
					if (strcmp(spch, value) == 0) {
						dmuci_get_value_by_section_string(ss, "optioninst", &idx);
						found = 1;
					}
					else
						continue;
					init_args_pool_option(&cur_dhcp_option, ss, (struct uci_section *)prev_data);
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_dhcp_option, idx) == DM_STOP) {
						dmfree(tt);
						break;
					}
				}
				if (!found)
				{
					DMUCI_ADD_SECTION(icwmpd, "dmmap", section_name((struct uci_section *)prev_data), &ss, &name);
					DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "dhcp_option", pch);
					DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "value", spch);
					init_args_pool_option(&cur_dhcp_option, ss, (struct uci_section *)prev_data);
					idx =  handle_update_instance(1, dmctx, &idx_last, dhcp_option_update_instance_alias_icwmpd, 5, ss, (struct uci_section *)prev_data, "optioninst", "optionalias", &find_max);
					DMUCI_SET_VALUE_BY_SECTION(icwmpd, ss, "optioninst", idx);
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_dhcp_option, idx) == DM_STOP) {
						dmfree(tt);
						break;
					}
				}
			}
		}
}
