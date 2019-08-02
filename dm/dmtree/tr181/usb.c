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
#include "usb.h"
#include <libusb-1.0/libusb.h>

/* *** Device.USB. *** */
DMOBJ tUSBObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Interface", &DMREAD, NULL, NULL, NULL, browseUSBInterfaceInst, NULL, NULL, tUSBInterfaceObj, tUSBInterfaceParams, NULL},
{"Port", &DMREAD, NULL, NULL, NULL, browseUSBPortInst, NULL, NULL, NULL, tUSBPortParams, get_linker_usb_port},
{"USBHosts", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tUSBUSBHostsObj, tUSBUSBHostsParams, NULL},
{0}
};

DMLEAF tUSBParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"InterfaceNumberOfEntries", &DMREAD, DMT_UNINT, get_USB_InterfaceNumberOfEntries, NULL, NULL, NULL},
{"PortNumberOfEntries", &DMREAD, DMT_UNINT, get_USB_PortNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.Interface.{i}. *** */
DMOBJ tUSBInterfaceObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tUSBInterfaceStatsParams, NULL},
{0}
};

DMLEAF tUSBInterfaceParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_USBInterface_Enable, set_USBInterface_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_USBInterface_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_USBInterface_Alias, set_USBInterface_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_USBInterface_Name, NULL, NULL, NULL},
{"LastChange", &DMREAD, DMT_UNINT, get_USBInterface_LastChange, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_USBInterface_LowerLayers, set_USBInterface_LowerLayers, NULL, NULL},
{"Upstream", &DMREAD, DMT_BOOL, get_USBInterface_Upstream, NULL, NULL, NULL},
{"MACAddress", &DMREAD, DMT_STRING, get_USBInterface_MACAddress, NULL, NULL, NULL},
{"MaxBitRate", &DMREAD, DMT_UNINT, get_USBInterface_MaxBitRate, NULL, NULL, NULL},
{"Port", &DMREAD, DMT_STRING, get_USBInterface_Port, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.Interface.{i}.Stats. *** */
DMLEAF tUSBInterfaceStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"BytesSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_BytesSent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_BytesReceived, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_PacketsSent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_PacketsReceived, NULL, NULL, NULL},
{"ErrorsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_ErrorsSent, NULL, NULL, NULL},
{"ErrorsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_ErrorsReceived, NULL, NULL, NULL},
{"UnicastPacketsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_UnicastPacketsSent, NULL, NULL, NULL},
{"UnicastPacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_UnicastPacketsReceived, NULL, NULL, NULL},
{"DiscardPacketsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_DiscardPacketsSent, NULL, NULL, NULL},
{"DiscardPacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_DiscardPacketsReceived, NULL, NULL, NULL},
{"MulticastPacketsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_MulticastPacketsSent, NULL, NULL, NULL},
{"MulticastPacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_MulticastPacketsReceived, NULL, NULL, NULL},
{"BroadcastPacketsSent", &DMREAD, DMT_UNINT, get_USBInterfaceStats_BroadcastPacketsSent, NULL, NULL, NULL},
{"BroadcastPacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_BroadcastPacketsReceived, NULL, NULL, NULL},
{"UnknownProtoPacketsReceived", &DMREAD, DMT_UNINT, get_USBInterfaceStats_UnknownProtoPacketsReceived, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.Port.{i}. *** */
DMLEAF tUSBPortParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_USBPort_Alias, set_USBPort_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_USBPort_Name, NULL, NULL, NULL},
{"Standard", &DMREAD, DMT_STRING, get_USBPort_Standard, NULL, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_USBPort_Type, NULL, NULL, NULL},
{"Receptacle", &DMREAD, DMT_STRING, get_USBPort_Receptacle, NULL, NULL, NULL},
{"Rate", &DMREAD, DMT_STRING, get_USBPort_Rate, NULL, NULL, NULL},
{"Power", &DMREAD, DMT_STRING, get_USBPort_Power, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.USBHosts. *** */
DMOBJ tUSBUSBHostsObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Host", &DMREAD, NULL, NULL, NULL, browseUSBUSBHostsHostInst, NULL, NULL, tUSBUSBHostsHostObj, tUSBUSBHostsHostParams, NULL},
{0}
};

DMLEAF tUSBUSBHostsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"HostNumberOfEntries", &DMREAD, DMT_UNINT, get_USBUSBHosts_HostNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.USBHosts.Host.{i}. *** */
DMOBJ tUSBUSBHostsHostObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Device", &DMREAD, NULL, NULL, NULL, browseUSBUSBHostsHostDeviceInst, NULL, NULL, tUSBUSBHostsHostDeviceObj, tUSBUSBHostsHostDeviceParams, NULL},
{0}
};

DMLEAF tUSBUSBHostsHostParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_USBUSBHostsHost_Alias, set_USBUSBHostsHost_Alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_USBUSBHostsHost_Enable, set_USBUSBHostsHost_Enable, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_USBUSBHostsHost_Name, NULL, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_USBUSBHostsHost_Type, NULL, NULL, NULL},
{"Reset", &DMWRITE, DMT_BOOL, get_USBUSBHostsHost_Reset, set_USBUSBHostsHost_Reset, NULL, NULL},
{"PowerManagementEnable", &DMWRITE, DMT_BOOL, get_USBUSBHostsHost_PowerManagementEnable, set_USBUSBHostsHost_PowerManagementEnable, NULL, NULL},
{"USBVersion", &DMREAD, DMT_STRING, get_USBUSBHostsHost_USBVersion, NULL, NULL, NULL},
{"DeviceNumberOfEntries", &DMREAD, DMT_UNINT, get_USBUSBHostsHost_DeviceNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.USBHosts.Host.{i}.Device.{i}. *** */
DMOBJ tUSBUSBHostsHostDeviceObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Configuration", &DMREAD, NULL, NULL, NULL, browseUSBUSBHostsHostDeviceConfigurationInst, NULL, NULL, tUSBUSBHostsHostDeviceConfigurationObj, tUSBUSBHostsHostDeviceConfigurationParams, NULL},
{0}
};

DMLEAF tUSBUSBHostsHostDeviceParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"DeviceNumber", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_DeviceNumber, NULL, NULL, NULL},
{"USBVersion", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_USBVersion, NULL, NULL, NULL},
{"DeviceClass", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDevice_DeviceClass, NULL, NULL, NULL},
{"DeviceSubClass", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDevice_DeviceSubClass, NULL, NULL, NULL},
{"DeviceVersion", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_DeviceVersion, NULL, NULL, NULL},
{"DeviceProtocol", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDevice_DeviceProtocol, NULL, NULL, NULL},
{"ProductID", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_ProductID, NULL, NULL, NULL},
{"VendorID", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_VendorID, NULL, NULL, NULL},
{"Manufacturer", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_Manufacturer, NULL, NULL, NULL},
{"ProductClass", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_ProductClass, NULL, NULL, NULL},
{"SerialNumber", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_SerialNumber, NULL, NULL, NULL},
{"Port", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_Port, NULL, NULL, NULL},
{"USBPort", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_USBPort, NULL, NULL, NULL},
{"Rate", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_Rate, NULL, NULL, NULL},
{"Parent", &DMREAD, DMT_STRING, get_USBUSBHostsHostDevice_Parent, NULL, NULL, NULL},
{"MaxChildren", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_MaxChildren, NULL, NULL, NULL},
{"IsSuspended", &DMREAD, DMT_BOOL, get_USBUSBHostsHostDevice_IsSuspended, NULL, NULL, NULL},
{"IsSelfPowered", &DMREAD, DMT_BOOL, get_USBUSBHostsHostDevice_IsSelfPowered, NULL, NULL, NULL},
{"ConfigurationNumberOfEntries", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDevice_ConfigurationNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.USBHosts.Host.{i}.Device.{i}.Configuration.{i}. *** */
DMOBJ tUSBUSBHostsHostDeviceConfigurationObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Interface", &DMREAD, NULL, NULL, NULL, browseUSBUSBHostsHostDeviceConfigurationInterfaceInst, NULL, NULL, NULL, tUSBUSBHostsHostDeviceConfigurationInterfaceParams, NULL},
{0}
};

DMLEAF tUSBUSBHostsHostDeviceConfigurationParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ConfigurationNumber", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDeviceConfiguration_ConfigurationNumber, NULL, NULL, NULL},
{"InterfaceNumberOfEntries", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDeviceConfiguration_InterfaceNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.USB.USBHosts.Host.{i}.Device.{i}.Configuration.{i}.Interface.{i}. *** */
DMLEAF tUSBUSBHostsHostDeviceConfigurationInterfaceParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"InterfaceNumber", &DMREAD, DMT_UNINT, get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceNumber, NULL, NULL, NULL},
{"InterfaceClass", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceClass, NULL, NULL, NULL},
{"InterfaceSubClass", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceSubClass, NULL, NULL, NULL},
{"InterfaceProtocol", &DMREAD, DMT_HEXBIN, get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceProtocol, NULL, NULL, NULL},
{0}
};

void init_usb_port(struct uci_section *dm, struct libusb_device_descriptor desc, libusb_device *dev, char *name, struct libusb_config_descriptor *config, const struct libusb_interface_descriptor *interface, struct usbPort *port){
	port->dm_usb= dm;
	port->dev= dev;
	port->desc= desc;
	port->name= dmstrdup(name);
	port->config= config;
	port->interface= interface;
}

libusb_device *get_libusb_device_grand_parent_host(libusb_device *dev){
	if(libusb_get_parent(dev) == NULL)
		return dev;
	return get_libusb_device_grand_parent_host(libusb_get_parent(dev));
}

int browseUSBInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseUSBPortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	libusb_context *context= NULL;
	libusb_device **devs;
	struct libusb_device_descriptor desc;
	struct usbPort port= {};

	int ret;
	size_t list, i;
	char *instance, *port_id, *v, *instnbr= NULL;

	ret= libusb_init(&context);
	if(ret<0)
		return FAULT_9002;
	list= libusb_get_device_list(context, &devs);
	if(list<0){
        libusb_free_device_list(devs, 1);
        libusb_exit(context);
        return FAULT_9002;
	}

	check_create_dmmap_package("dmmap_usb");

	for(i=0; i< list; i++) {
		ret = libusb_get_device_descriptor(devs[i], &desc);
		if (ret<0)
			continue;
		dmasprintf(&port_id, "port_%02x_%02x", libusb_get_bus_number(devs[i]), libusb_get_port_number(devs[i]));
		s= is_dmmap_section_exist_eq("dmmap_usb", "port", "port_id", port_id);
		if(!s){
			dmuci_add_section_icwmpd("dmmap_usb", "port", &s, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "port_id", port_id);
		}
		init_usb_port(s, desc, devs[i], port_id, NULL, NULL, &port);
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, s, "usb_port_instance", "usb_port_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &port, instance) == DM_STOP)
			return 0;

	}

    libusb_free_device_list(devs, 1);
    libusb_exit(context);
	return 0;
}

int browseUSBUSBHostsHostInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s;
	libusb_context *context= NULL;
	libusb_device **devs;
	struct libusb_device_descriptor desc;
	struct usbPort port= {};

	int ret;
	size_t list, i;
	char *instance, *host_id, *v, *instnbr= NULL;

	ret= libusb_init(&context);
	if(ret<0)
		return FAULT_9002;
	list= libusb_get_device_list(context, &devs);
	if(list<0){
        libusb_free_device_list(devs, 1);
        libusb_exit(context);
        return FAULT_9002;
	}

	check_create_dmmap_package("dmmap_usb");

	for(i=0; i< list; i++) {
		if(libusb_get_parent(devs[i]) != NULL)
			continue;

		ret = libusb_get_device_descriptor(devs[i], &desc);
		if (ret<0)
			continue;
		dmasprintf(&host_id, "host_%02x_%02x", libusb_get_bus_number(devs[i]), libusb_get_port_number(devs[i]));
		s= is_dmmap_section_exist_eq("dmmap_usb", "host", "host_id", host_id);
		if(!s){
			dmuci_add_section_icwmpd("dmmap_usb", "host", &s, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "host_id", host_id);
		}
		init_usb_port(s, desc, devs[i], host_id, NULL, NULL, &port);
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, s, "usb_host_instance", "usb_host_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &port, instance) == DM_STOP)
			return 0;

	}

    libusb_free_device_list(devs, 1);
    libusb_exit(context);
	return 0;
}

int browseUSBUSBHostsHostDeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct usbPort *parentHostPort= (struct usbPort *)prev_data;
	struct uci_section *s;
	libusb_context *context= NULL;
	libusb_device **devs, *myparent;
	struct libusb_device_descriptor desc;
	struct usbPort port= {};

	int ret;
	size_t list, i;
	char *instance, *device_id, *v, *instnbr= NULL;

	ret= libusb_init(&context);
	if(ret<0)
		return FAULT_9002;
	list= libusb_get_device_list(context, &devs);
	if(list<0){
        libusb_free_device_list(devs, 1);
        libusb_exit(context);
        return FAULT_9002;
	}

	check_create_dmmap_package("dmmap_usb");

	for(i=0; i< list; i++) {
		if(libusb_get_parent(devs[i]) == NULL)
			continue;

		ret = libusb_get_device_descriptor(devs[i], &desc);
		if(desc.bDeviceClass == 0x09)
			continue;
		myparent= get_libusb_device_grand_parent_host(devs[i]);
		if(libusb_get_bus_number(parentHostPort->dev) != libusb_get_bus_number(myparent) || libusb_get_port_number(parentHostPort->dev) != libusb_get_port_number(myparent))
			continue;
		dmasprintf(&device_id, "dev_%02x_%02x", libusb_get_bus_number(devs[i]), libusb_get_port_number(devs[i]));
		s= is_dmmap_section_exist_eq("dmmap_usb", "device", "dev_id", device_id);
		if(!s){
			dmuci_add_section_icwmpd("dmmap_usb", "device", &s, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "dev_id", device_id);
		}
		init_usb_port(s, desc, devs[i], device_id, NULL, NULL, &port);
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, s, "usb_dev_instance", "usb_dev_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &port, instance) == DM_STOP)
			return 0;
	}

    libusb_free_device_list(devs, 1);
    libusb_exit(context);
	return 0;
}

int browseUSBUSBHostsHostDeviceConfigurationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct usbPort *parentDevicePort= (struct usbPort *)prev_data;
	struct uci_section *s;
	libusb_context *context= NULL;
	libusb_device **devs, *myparent;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	struct usbPort port= {};
	int ret;
	size_t list, i, nbreConfigs;
	char *instance, *conf_id, *v, *instnbr= NULL;

	nbreConfigs= parentDevicePort->desc.bNumConfigurations;
	libusb_get_config_descriptor(parentDevicePort->dev, 0, &config);
	check_create_dmmap_package("dmmap_usb");
	for(i=0; i<nbreConfigs; i++){
		dmasprintf(&conf_id, "%s_%02x", parentDevicePort->name, config[i].bConfigurationValue);
		s= is_dmmap_section_exist_eq("dmmap_usb", "device_configuration", "conf_id", conf_id);
		if(!s){
			dmuci_add_section_icwmpd("dmmap_usb", "device_configuration", &s, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "device_id", parentDevicePort->name);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "conf_id", conf_id);
		}
		init_usb_port(s, parentDevicePort->desc, parentDevicePort->dev, conf_id, &config[i], NULL, &port);
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, s, "usb_conf_instance", "usb_conf_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &port, instance) == DM_STOP)
			return 0;
	}
	return 0;
}

int browseUSBUSBHostsHostDeviceConfigurationInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct usbPort *parentConfigPort= (struct usbPort *)prev_data;
	struct uci_section *s;
	libusb_context *context= NULL;
	libusb_device **devs, *myparent;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	struct usbPort port= {};
	int ret;
	size_t list, i;
	char *instance, *iface_id, *v, *instnbr= NULL;

	check_create_dmmap_package("dmmap_usb");
	for(i=0; i<parentConfigPort->config->bNumInterfaces; i++){
		dmasprintf(&iface_id, "%s_%02x", parentConfigPort->name, parentConfigPort->config->interface[i]);
		s= is_dmmap_section_exist_eq("dmmap_usb", "configuration_interface", "iface_id", iface_id);
		if(!s){
			dmuci_add_section_icwmpd("dmmap_usb", "configuration_interface", &s, &v);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "config_id", parentConfigPort->name);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "iface_id", iface_id);
		}
		init_usb_port(s, parentConfigPort->desc, parentConfigPort->dev, iface_id, parentConfigPort->config, &parentConfigPort->config->interface[i], &port);
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, s, "usb_iface_instance", "usb_iface_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, &port, instance) == DM_STOP)
			return 0;
	}
	return 0;
}


int get_USB_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USB_PortNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	libusb_context *context= NULL;
	libusb_device **devs;
	struct usbPort port= {};

	int ret;
	size_t list;

	ret= libusb_init(&context);
	if(ret<0)
		return FAULT_9002;
	list= libusb_get_device_list(context, &devs);
	if(list<0){
        libusb_free_device_list(devs, 1);
        libusb_exit(context);
        return FAULT_9002;
	}
	dmasprintf(value, "%d", list);

	return 0;
}

int get_USBInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBInterface_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterface_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBPort_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port=(struct usbPort *)data;
	dmuci_get_value_by_section_string(port->dm_usb, "usb_port_alias", value);
	return 0;
}

int set_USBPort_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct usbPort* port=(struct usbPort *)data;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(port->dm_usb, "usb_port_alias", value);
			break;
	}
	return 0;
}

int get_USBPort_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port=(struct usbPort *)data;
	dmasprintf(value, "%s", port->name);
	return 0;
}

int get_USBPort_Standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port= (struct usbPort *) data;

	char *strhex;
	dmasprintf(&strhex, "%04X", port->desc.bcdUSB);
	dmasprintf(value, "%c.%c", strhex[1], strhex[2]);
	return 0;
}

int get_USBPort_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	if(libusb_get_parent(port->dev) == NULL)
		*value= "Host";
	else if(port->desc.bDeviceClass == 0x09)
		*value= "Hub";
	else
		*value= "Device";
	return 0;
}

int get_USBPort_Receptacle(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return 0;
}

int get_USBPort_Rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	switch(libusb_get_device_speed(port->dev)){
	case LIBUSB_SPEED_LOW:
		*value= "Low";
		break;
	case LIBUSB_SPEED_FULL:
		*value= "Full";
		break;
	case LIBUSB_SPEED_HIGH:
		*value= "High";
		break;
	default:
		*value= "Super";
	}
	return 0;
}

int get_USBPort_Power(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{

	return 0;
}

int get_USBUSBHosts_HostNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	libusb_context *context= NULL;
	libusb_device **devs;

	size_t list;
	int ret, i, nbre= 0;

	ret= libusb_init(&context);
	if(ret<0){
		*value= "0";
		return 0;
	}
	list= libusb_get_device_list(context, &devs);
	for (i=0; i<list; i++)
	{
		if(libusb_get_parent(devs[i]) == NULL)
			nbre ++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_USBUSBHostsHost_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port=(struct usbPort *)data;
	dmuci_get_value_by_section_string(port->dm_usb, "usb_host_alias", value);
	return 0;
}

int set_USBUSBHostsHost_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct usbPort* port=(struct usbPort *)data;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(port->dm_usb, "usb_host_alias", value);
			break;
	}
	return 0;
}

int get_USBUSBHostsHost_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBUSBHostsHost_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBUSBHostsHost_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port=(struct usbPort *)data;
	dmasprintf(value, "%s", port->name);
	return 0;
}

int get_USBUSBHostsHost_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHost_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBUSBHostsHost_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBUSBHostsHost_PowerManagementEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_USBUSBHostsHost_PowerManagementEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_USBUSBHostsHost_USBVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port= (struct usbPort *) data;

	char *strhex;
	dmasprintf(&strhex, "%04X", port->desc.bcdUSB);
	dmasprintf(value, "%c.%c", strhex[1], strhex[2]);
	return 0;
}

int get_USBUSBHostsHost_DeviceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* hostPort= (struct usbPort *) data;
	int dev_nbre= 0;
	int i, ret;
	size_t list;
	libusb_context *context= NULL;
	libusb_device **devs, *myparent;
	struct libusb_device_descriptor desc;

	ret= libusb_init(&context);
	if(ret<0)
		return FAULT_9002;
	list= libusb_get_device_list(context, &devs);
	for(i=0; i< list; i++) {
		if(libusb_get_parent(devs[i]) == NULL)
			continue;

		ret = libusb_get_device_descriptor(devs[i], &desc);
		if(desc.bDeviceClass == 0x09)
			continue;
		myparent= get_libusb_device_grand_parent_host(devs[i]);
		if(libusb_get_bus_number(hostPort->dev) != libusb_get_bus_number(myparent) || libusb_get_port_number(hostPort->dev) != libusb_get_port_number(myparent))
			continue;
		dev_nbre++;
	}
	dmasprintf(value, "%d", dev_nbre);
}

int get_USBUSBHostsHostDevice_DeviceNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value ,"%d", libusb_get_port_number(port->dev));
	return 0;
}

int get_USBUSBHostsHostDevice_USBVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort* port= (struct usbPort *) data;

	char *strhex;
	dmasprintf(&strhex, "%04X", port->desc.bcdUSB);
	dmasprintf(value, "%c.%c", strhex[1], strhex[2]);
	return 0;
}

int get_USBUSBHostsHostDevice_DeviceClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "0x%02x", port->desc.bDeviceClass);
	return 0;
}

int get_USBUSBHostsHostDevice_DeviceSubClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "0x%02", port->desc.bDeviceSubClass);
	return 0;
}

int get_USBUSBHostsHostDevice_DeviceVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHostDevice_DeviceProtocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "0x%02", port->desc.bDeviceProtocol);
	return 0;
}

int get_USBUSBHostsHostDevice_ProductID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "%d", port->desc.idProduct);
	return 0;
}

int get_USBUSBHostsHostDevice_VendorID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "%d", port->desc.idVendor);
	return 0;
}

int get_USBUSBHostsHostDevice_Manufacturer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "%d", port->desc.iManufacturer);
	return 0;
}

int get_USBUSBHostsHostDevice_ProductClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	//dmasprintf(value, "", port->desc.)
	return 0;
}

int get_USBUSBHostsHostDevice_SerialNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value, "%d", port->desc.iSerialNumber);
	return 0;
}

int get_USBUSBHostsHostDevice_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	dmasprintf(value ,"%d", libusb_get_parent(port->dev)?libusb_get_port_number(libusb_get_parent(port->dev)):0);
	return 0;
}

int get_USBUSBHostsHostDevice_USBPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	char *linker= NULL;
	dmasprintf(&linker, "%02x_%02x", libusb_get_bus_number(port->dev), libusb_get_port_number(port->dev));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cUSB%cPort%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	return 0;
}

int get_USBUSBHostsHostDevice_Rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *port= (struct usbPort *)data;
	switch(libusb_get_device_speed(port->dev)){
	case LIBUSB_SPEED_LOW:
		*value= "Low";
		break;
	case LIBUSB_SPEED_FULL:
		*value= "Full";
		break;
	case LIBUSB_SPEED_HIGH:
		*value= "High";
		break;
	default:
		*value= "Super";
	}
	return 0;
}

int get_USBUSBHostsHostDevice_Parent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHostDevice_MaxChildren(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHostDevice_IsSuspended(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHostDevice_IsSelfPowered(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_USBUSBHostsHostDevice_ConfigurationNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	struct libusb_config_descriptor *config= NULL;
	int nbre= libusb_get_config_descriptor(configPort->dev, 0, &config);
	dmasprintf(value, "%d", configPort->desc.bNumConfigurations);
	return 0;
}

int get_USBUSBHostsHostDeviceConfiguration_ConfigurationNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	struct libusb_config_descriptor *config= NULL;
	int nbre= libusb_get_config_descriptor(configPort->dev, 0, &config);
	dmasprintf(value, "%d", configPort->config->bConfigurationValue);
	return 0;
}

int get_USBUSBHostsHostDeviceConfiguration_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	dmasprintf(value, "%d", configPort->config->bNumInterfaces);
	return 0;
}

int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	dmasprintf(value, "%d", configPort->interface->bInterfaceNumber);
	return 0;
}

int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	dmasprintf(value, "0x%02x", configPort->interface->bInterfaceClass);
	return 0;
}

int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceSubClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	dmasprintf(value, "0x%02x", configPort->interface->bInterfaceSubClass);
	return 0;
}

int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceProtocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct usbPort *configPort= (struct usbPort *)data;
	dmasprintf(value, "0x%02x", configPort->interface->bInterfaceProtocol);
	return 0;
}

int get_linker_usb_port(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	struct usbPort *port= (struct usbPort *)data;
	if(port && port->dev) {
		dmasprintf(linker, "%02x_%02x", libusb_get_bus_number(port->dev),libusb_get_port_number(port->dev));
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}
