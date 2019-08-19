/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*/

#ifndef __USB_H
#define __USB_H

#include <libusb-1.0/libusb.h>
#include <regex.h>

extern DMOBJ tUSBObj[];
extern DMLEAF tUSBParams[];
extern DMOBJ tUSBInterfaceObj[];
extern DMLEAF tUSBInterfaceParams[];
extern DMLEAF tUSBInterfaceStatsParams[];
extern DMLEAF tUSBPortParams[];
extern DMOBJ tUSBUSBHostsObj[];
extern DMLEAF tUSBUSBHostsParams[];
extern DMOBJ tUSBUSBHostsHostObj[];
extern DMLEAF tUSBUSBHostsHostParams[];
extern DMOBJ tUSBUSBHostsHostDeviceObj[];
extern DMLEAF tUSBUSBHostsHostDeviceParams[];
extern DMOBJ tUSBUSBHostsHostDeviceConfigurationObj[];
extern DMLEAF tUSBUSBHostsHostDeviceConfigurationParams[];
extern DMLEAF tUSBUSBHostsHostDeviceConfigurationInterfaceParams[];

#define SYSFS_USB_DEVICES_PATH "/sys/bus/usb/devices"

regex_t regex1, regex2;
struct usbPort {
	struct uci_section *dm_usb;
	libusb_device *dev;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	const struct libusb_interface_descriptor *interface;
	char *name;
};

struct usb_port {
	struct uci_section *dm_usb_port;
	char *folder_name;
	char *folder_path;
};

struct usb_interface {
	struct uci_section *dm_usb_iface;
	char *iface_name;
	char *iface_path;
	char *statistics_path;
	char *portlink;
};

int browseUSBInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseUSBPortInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseUSBUSBHostsHostInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseUSBUSBHostsHostDeviceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseUSBUSBHostsHostDeviceConfigurationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseUSBUSBHostsHostDeviceConfigurationInterfaceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_USB_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USB_PortNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBInterface_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBInterface_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBInterface_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBInterface_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_LastChange(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBInterface_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBInterface_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_MACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_MaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterface_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_BytesSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_BytesReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_PacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_PacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_ErrorsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_ErrorsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_UnicastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_UnicastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_DiscardPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_DiscardPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_MulticastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_MulticastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_BroadcastPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_BroadcastPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBInterfaceStats_UnknownProtoPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBPort_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBPort_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Receptacle(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBPort_Power(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHosts_HostNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHost_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBUSBHostsHost_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBUSBHostsHost_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBUSBHostsHost_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBUSBHostsHost_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHost_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHost_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBUSBHostsHost_Reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBUSBHostsHost_PowerManagementEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_USBUSBHostsHost_PowerManagementEnable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_USBUSBHostsHost_USBVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHost_DeviceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_DeviceNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_USBVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_DeviceClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_DeviceSubClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_DeviceVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_DeviceProtocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_ProductID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_VendorID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_Manufacturer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_ProductClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_SerialNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_Port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_USBPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_Rate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_Parent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_MaxChildren(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_IsSuspended(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_IsSelfPowered(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDevice_ConfigurationNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfiguration_ConfigurationNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfiguration_InterfaceNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceSubClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_USBUSBHostsHostDeviceConfigurationInterface_InterfaceProtocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_linker_usb_port(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
#endif //__USB_H

