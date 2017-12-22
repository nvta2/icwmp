#ifndef UPNP_DEVICEINFO_H
#define UPNP_DEVICEINFO_H
#include "dmcwmp.h"

struct upnp_dvinf_args
{
	struct uci_section *upnp_deviceinfo_sec;
	char* systemName;
	char* macAddress;
	char* interfaceType;
};

int upnp_deviceinfo_get_provisionning_code(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_software_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_software_description(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_up_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_hardware_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_network_interface_number_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_manufacturer_oui(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_product_class(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_serial_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_system_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_mac_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_interface_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_will_reboot(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_will_base_line_reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_deviceinfo_get_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int upnp_deviceinfo_set_provisionning_code(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int upnp_deviceinfo_networkinterface_createinstance(char *refparam, struct dmctx *ctx, void *data, char **instance);
int upnp_deviceinfo_networkinterface_deleteinstance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
extern DMLEAF upnpDeviceIdParams[];
extern DMLEAF upnpNetworkInterfaceParams[];
extern DMOBJ upnpPhysicalDeviceObj[];
extern DMLEAF upnpPhysicalDeviceParams[];
extern DMLEAF upnpOperatingSystemParams[];
extern DMLEAF upnpExecutionEnvironmentParams[];
extern DMOBJ upnpDeviceInfoObj[];
extern DMLEAF upnpDeviceInfoParams[];
#endif
