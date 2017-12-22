#ifndef UPNP_COMMON_H
#define UPNP_COMMON_H
char *upnp_get_softwareversion();
int upnp_get_NetworkInterfaceNumberOfEntries();
int upnp_get_IPInterfaceNumberOfEntries();
void upnp_getMacAddress(char *interfaceName, char **macAddress);
void upnp_getInterfaceStatus(char *interfaceName, char **status);
int upnp_getInterfaceTotalPacketSent(char *interfaceName, char **totalPktSent);
int upnp_getInterfaceTotalPacketReceived(char *interfaceName, char **totalPktReceived);
char *upnp_get_deviceid_manufactureroui();
#endif
