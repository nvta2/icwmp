#include <stdio.h>
#include "dmcwmp.h"
#include "upnp_deviceinfo.h"
#include <sys/utsname.h>
#include "dmuci.h"
#include "dmmem.h"
#include "dmcommon.h"
#include <uci.h>
#include "upnp_common.h"
#include<stdbool.h>

int upnp_browseNetworkInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

/**************************************************************************
*
* /UPnP/DM/DeviceInfo/ datamodel tree
*
***************************************************************************/

/*** /UPnP/DM/DeviceInfo/ objects ***/
DMOBJ upnpDeviceInfoObj[] ={
{"PhysicalDevice", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpPhysicalDeviceObj, upnpPhysicalDeviceParams, NULL},
{"OperatingSystem",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, upnpOperatingSystemParams, NULL},
{"ExecutionEnvironment",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, upnpExecutionEnvironmentParams, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/ parameters ***/
DMLEAF upnpDeviceInfoParams[] = {
{"ProvisioningCode", &DMWRITE, DMT_STRING, upnp_deviceinfo_get_provisionning_code, upnp_deviceinfo_set_provisionning_code, &DMFINFRM, NULL},
{"SoftwareVersion", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_version, NULL, &DMFINFRM, NULL},
{"SoftwareDescription", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_description, NULL, &DMFINFRM, NULL},
{"UpTime", &DMREAD, DMT_UNINT, upnp_deviceinfo_get_up_time, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/PhysicalDevice/ objects ***/
DMOBJ upnpPhysicalDeviceObj[] = {
{"DeviceID", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, upnpDeviceIdParams, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/PhysicalDevice/ parameters ***/
DMLEAF upnpPhysicalDeviceParams[] = {
{"HardwareVersion", &DMREAD, DMT_STRING, upnp_deviceinfo_get_hardware_version, NULL, &DMFINFRM, NULL},
{"NetworkInterfaceNumberOfEntries", &DMREAD, DMT_UNINT, upnp_deviceinfo_get_network_interface_number_entries, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/OperatingSystem/ parameters ***/
DMLEAF upnpOperatingSystemParams[] = {
{"SoftwareVersion", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_version, NULL, &DMFINFRM, NULL},
{"SoftwareDescription", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_description, NULL, &DMFINFRM, NULL},
{"UpTime", &DMREAD, DMT_UNINT, upnp_deviceinfo_get_up_time, NULL, &DMFINFRM, NULL},
{"WillReboot", &DMREAD, DMT_BOOL, upnp_deviceinfo_get_will_reboot, NULL, &DMFINFRM, NULL},
{"WillBaselineReset", &DMREAD, DMT_BOOL, upnp_deviceinfo_get_will_base_line_reset, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/ExecutionEnvironment/ parameters ***/
DMLEAF upnpExecutionEnvironmentParams[] = {
{"Status", &DMREAD, DMT_STRING, upnp_deviceinfo_get_status, NULL, &DMFINFRM, NULL},
{"Uptime", &DMREAD, DMT_UNINT, upnp_deviceinfo_get_up_time, NULL, &DMFINFRM, NULL},
{"SoftwareVersion", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_version, NULL, &DMFINFRM, NULL},
{"SoftwareDescription", &DMREAD, DMT_STRING, upnp_deviceinfo_get_software_description, NULL, &DMFINFRM, NULL},
{"WillReboot", &DMREAD, DMT_BOOL, upnp_deviceinfo_get_will_reboot, NULL, &DMFINFRM, NULL},
{"WillBaselineReset", &DMREAD, DMT_BOOL, upnp_deviceinfo_get_will_base_line_reset, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/DeviceInfo/DeviceID/ parameters ***/
DMLEAF upnpDeviceIdParams[] =  {
{"ManufacturerOUI", &DMREAD, DMT_HEXBIN, upnp_deviceinfo_get_manufacturer_oui, NULL, &DMFINFRM, NULL},
{"ProductClass", &DMREAD, DMT_STRING, upnp_deviceinfo_get_product_class, NULL, &DMFINFRM, NULL},
{"SerialNumber", &DMREAD, DMT_STRING, upnp_deviceinfo_get_serial_number, NULL, &DMFINFRM, NULL},
{0}
};


/**************************************************************************
*
* /UPnP/DM/DeviceInfo/* parameters functions
*
***************************************************************************/

int upnp_deviceinfo_get_provisionning_code(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("cwmp", "cpe", "provisioning_code", value);
	return 0;
}

int upnp_deviceinfo_get_software_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	char *v = NULL, *tmp = NULL, *val = NULL;
	char delimiter[] = "_";
	db_get_value_string("hw", "board", "iopVersion", &v);
	if(v == NULL || strlen(v)<=0) {
		*value = NULL;
		return 0;
	}
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	*value = cut_fx(tmp, delimiter, 2);
	return 0;
}

int upnp_deviceinfo_get_software_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
    struct utsname unameData;
    uname(&unameData);

    dmasprintf(value, "GNU/%s",unameData.sysname);
	return 0;
}

int upnp_deviceinfo_get_up_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	FILE* fp = NULL;
	char *pch = NULL, *spch = NULL;
	char buf[64];
	*value = "0";

	fp = fopen("/proc/uptime", "r");
	if (fp != NULL) {
		fgets(buf, 64, fp);
		pch = strtok_r(buf, ".", &spch);
		if (pch)
			*value = dmstrdup(pch); // MEM WILL BE FREED IN DMMEMCLEAN
		fclose(fp);
	}
	return 0;
}

int upnp_deviceinfo_get_hardware_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	db_get_value_string("hw", "board", "hardwareVersion", value);
	return 0;
}

int upnp_deviceinfo_get_network_interface_number_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	int n = upnp_get_NetworkInterfaceNumberOfEntries();
	dmasprintf(value, "%d",n);
	return 0;
}

int upnp_deviceinfo_get_manufacturer_oui(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("cwmp", "cpe", "override_oui", value);
	if(*value == NULL || strlen(value)<=0) {
		*value = NULL;
		return 0;
	}
	if (*value[0] == '\0')
		*value = upnp_get_deviceid_manufactureroui();
	return 0;
}


int upnp_deviceinfo_get_product_class(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	char *v = NULL, *tmp = NULL, *val = NULL;
	char delimiter[] = "_";

	db_get_value_string("hw", "board", "iopVersion", &v);
	if(v == NULL || strlen(v)<=NULL){
		*value = NULL;
		return 0;
	}
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	val = cut_fx(tmp, delimiter, 1);
	return 0;
}

int upnp_deviceinfo_get_serial_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	db_get_value_string("hw", "board", "serialNumber", value);
	return 0;
}

int upnp_deviceinfo_get_system_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_deviceinfo_get_mac_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_deviceinfo_get_interface_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_deviceinfo_get_will_reboot(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_deviceinfo_get_will_base_line_reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_deviceinfo_get_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}


int upnp_deviceinfo_set_provisionning_code(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	bool b;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("cwmp", "cpe", "provisioning_code", value);
			return 0;
	}
	return 0;
}


/*************************************************
 *
 * MultiInstance objects browsing functions
 *
 *************************************************/

int upnp_deviceinfo_networkinterface_createinstance(char *refparam, struct dmctx *ctx, void *data, char **instance){
	char *value = NULL;
	char *iface_instance = NULL, ib[8], ip_name[32];
	char *p = ip_name;
	struct uci_section *iface_sec = NULL;

	iface_instance = get_last_instance("network","interface","upnp_iface_int_instance");
	sprintf(ib, "%d", iface_instance ? atoi(iface_instance)+1 : 1);
	dmstrappendstr(p, "ip_interface_");
	dmstrappendstr(p, ib);
	dmstrappendend(p);
	sprintf(ib, "%d", iface_instance ? atoi(iface_instance)+1 : 1);
	dmuci_add_section("network", "interface", &iface_sec, &value);
	dmuci_set_value("network", ip_name, "", "interface");
	dmuci_set_value("network", ip_name, "proto", "dhcp");
	*instance = update_instance(iface_sec, iface_instance, "upnp_iface_int_instance");
	return 0;
}

int upnp_deviceinfo_networkinterface_deleteinstance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action){
	switch (del_action) {
	case DEL_INST:
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "proto", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "type", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "bridge_instance", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ip_int_instance", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ipv4_instance", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ipv6_instance", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ifname", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ipaddr", "");
		dmuci_set_value_by_section(((struct upnp_dvinf_args *)data)->upnp_deviceinfo_sec, "ip6addr", "");
		break;
	case DEL_ALL:
		return FAULT_9005;
	}
	return 0;
}

int upnp_browseNetworkInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance){
	struct uci_section *net_sec = NULL;
	char *iface_int = NULL, *iface_int_last = NULL, *interfaceType = NULL, *macAddress = NULL;
	struct upnp_dvinf_args curr_upnp_deviceinfo_args = {0};
	uci_foreach_sections("network", "interface", net_sec) {
		curr_upnp_deviceinfo_args.upnp_deviceinfo_sec = net_sec;
		dmuci_get_value_by_section_string(net_sec, "type", &interfaceType);
		upnp_getMacAddress(section_name(net_sec), &macAddress);
		dmasprintf(&curr_upnp_deviceinfo_args.systemName, "%s", section_name(net_sec));
		dmasprintf(&curr_upnp_deviceinfo_args.macAddress, "%s", macAddress);
		dmasprintf(&curr_upnp_deviceinfo_args.interfaceType, "%s", interfaceType);
		iface_int = handle_update_instance(1, dmctx, &iface_int_last, update_instance_alias, 3, net_sec, "upnp_iface_int_instance", "upnp_iface_int_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_upnp_deviceinfo_args, iface_int) == DM_STOP) break;
	}
}
