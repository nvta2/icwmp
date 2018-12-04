/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2016 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 */

#include "dmuci.h"
#include "dmcwmp.h"
#include "root.h"
#include "deviceinfo.h"
#include "managementserver.h"
#include "times.h"
#include "upnp.h"
#include "voice_services.h"
#include "x_iopsys_eu_ice.h"
#include "x_iopsys_eu_igmp.h"
#include "x_iopsys_eu_ipacccfg.h"
#include "x_iopsys_eu_logincfg.h"
#include "x_iopsys_eu_power_mgmt.h"
#include "x_iopsys_eu_syslog.h"
#include "softwaremodules.h"
#include "xmpp.h"
#include "x_iopsys_eu_owsd.h"
#include "x_iopsys_eu_dropbear.h"
#include "x_iopsys_eu_buttons.h"
#ifdef UPNP_TR064
#include "upnp_deviceinfo.h"
#include "upnp_configuration.h"
#include "upnp_monitoring.h"
#endif

#include "ip.h"
#include "ethernet.h"
#include "bridging.h"
#include "wifi.h"
#include "wan.h"
#include "dhcp.h"
#include "hosts.h"
#include "nat.h"
#include "ppp.h"
#include "routing.h"
#include "userinterface.h"
#include "landevice.h"
#include "wandevice.h"
#include "ippingdiagnostics.h"
#include "lan_interfaces.h"
#include "layer_3_forwarding.h"
#include "x_iopsys_eu_wifi.h"
#include "layer_2_bridging.h"
#include "downloaddiagnostic.h"
#include "uploaddiagnostic.h"
#include "deviceconfig.h"

/* *** CWMP *** */
DMOBJ tEntry098Obj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{(char *)&dmroot, &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot_098_Obj, NULL, NULL},
{0}
};

DMOBJ tEntry181Obj[] = {
/*OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{(char *)&dmroot, &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot_181_Obj, NULL, NULL},
{0}
};

DMOBJ tRoot_098_Obj[] = {
/* OBJ permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, notification, linker*/
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tDeviceInfoObj, tDeviceInfoParams, NULL},
{"ManagementServer", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tManagementServerParams, NULL},
{"Time", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tTimeParams, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tUPnPObj, NULL, NULL},
{"Services", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tServiceObj, NULL, NULL},
{CUSTOM_PREFIX"_ICE", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IceParam, NULL},
{CUSTOM_PREFIX"_IGMP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IgmpParam, NULL},
{CUSTOM_PREFIX"IpAccCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSe_IpAccObj, NULL, NULL},
{CUSTOM_PREFIX"LoginCfg", &DMREAD, NULL, NULL, NULL, NULL,NULL, &DMNONE,NULL, tSe_LoginCfgParam, NULL},
{CUSTOM_PREFIX"PowerManagement", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_PowerManagementParam, NULL},
{CUSTOM_PREFIX"SyslogCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_SyslogCfgParam, NULL},
{"SoftwareModules", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSoftwareModulesObj, NULL, NULL},
{CUSTOM_PREFIX"Owsd", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,XIopsysEuOwsdObj, XIopsysEuOwsdParams, NULL},
{CUSTOM_PREFIX"Dropbear", &DMWRITE, add_dropbear_instance, delete_dropbear_instance, NULL, browseXIopsysEuDropbear, NULL, &DMNONE, NULL, X_IOPSYS_EU_DropbearParams, NULL},
{CUSTOM_PREFIX"Buttons", &DMREAD, NULL, NULL, NULL, browseXIopsysEuButton, NULL, &DMNONE, NULL, X_IOPSYS_EU_ButtonParams, NULL},
{"LANDevice", &DMREAD, NULL, NULL, NULL, browselandeviceInst, &DMFINFRM, &DMNONE,tLANDeviceObj, tLANDeviceParam, NULL},
{"WANDevice", &DMREAD, NULL, NULL, NULL, browsewandeviceInst, &DMFINFRM, &DMWANConnectionDevicenotif,tWANDeviceObj, tWANDeviceParam, NULL},
{"LANInterfaces", &DMREAD, NULL, NULL, check_laninterfaces, NULL, &DMFINFRM, &DMNONE,tLANInterfacesObj, tLANInterfacesParam, NULL},
{"IPPingDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tIPPingDiagnosticsParam, NULL},
{"Layer3Forwarding", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tLayer3ForwardingObj, tLayer3ForwardingParam, NULL},
{"Layer2Bridging", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tLayer2BridgingObj, NULL, NULL},
{CUSTOM_PREFIX"Wifi", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tsewifiObj, tsewifiParam, NULL},
{"DownloadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tDownloadDiagnosticsParam, NULL},
{"UploadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tUploadDiagnosticsParam, NULL},
{"DeviceConfig", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tDeviceConfigParam, NULL},
#ifdef XMPP_ENABLE
{"XMPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL,tXMPPObj, tXMPPParams, NULL},
#endif
{0}
};

DMOBJ tRoot_181_Obj[] = {
/* OBJ permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, notification, linker*/
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tDeviceInfoObj, tDeviceInfoParams, NULL},
{"ManagementServer", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tManagementServerParams, NULL},
{"Time", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tTimeParams, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tUPnPObj, NULL, NULL},
{"Services", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tServiceObj, NULL, NULL},
{CUSTOM_PREFIX"ICE", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IceParam, NULL},
{CUSTOM_PREFIX"IGMP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IgmpParam, NULL},
{CUSTOM_PREFIX"IpAccCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSe_IpAccObj, NULL, NULL},
{CUSTOM_PREFIX"LoginCfg", &DMREAD, NULL, NULL, NULL, NULL,NULL, &DMNONE,NULL, tSe_LoginCfgParam, NULL},
{CUSTOM_PREFIX"PowerManagement", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_PowerManagementParam, NULL},
{CUSTOM_PREFIX"SyslogCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_SyslogCfgParam, NULL},
{"SoftwareModules", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSoftwareModulesObj, NULL, NULL},
{CUSTOM_PREFIX"Owsd", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,XIopsysEuOwsdObj, XIopsysEuOwsdParams, NULL},
{CUSTOM_PREFIX"Dropbear", &DMWRITE, add_dropbear_instance, delete_dropbear_instance, NULL, browseXIopsysEuDropbear, NULL, &DMNONE, NULL, X_IOPSYS_EU_DropbearParams, NULL},
{CUSTOM_PREFIX"Buttons", &DMREAD, NULL, NULL, NULL, browseXIopsysEuButton, NULL, &DMNONE, NULL, X_IOPSYS_EU_ButtonParams, NULL},
{"Bridging",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tBridgingObj, NULL, NULL},
{"WiFi",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tWifiObj, tWifiParams, NULL},
{"IP",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tIPObj, NULL, NULL},
{"Ethernet", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetObj, NULL, NULL},
{"DSL",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDslObj, NULL, NULL},
{"ATM",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tAtmObj, NULL, NULL},
{"PTM", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tPtmObj, NULL, NULL},
{"DHCPv4", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDhcpv4Obj, NULL, NULL},
{"Hosts", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, thostsObj, thostsParam, NULL},
{"NAT", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tnatObj, NULL, NULL},
{"PPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tpppObj, NULL, NULL},
{"Routing", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tRoutingObj, tRoutingParam, NULL},
{"UserInterface", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tUserInterfaceObj, NULL, NULL},
#ifdef XMPP_ENABLE
{"XMPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL,tXMPPObj, tXMPPParams, NULL},
#endif
{0}
};

/*** UPNP ***/
#ifdef UPNP_TR064
DMOBJ tEntry098ObjUPNP[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{(char *)&dmroot, &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot098ObjUPNP, NULL, NULL},
{0}
};

DMOBJ tRoot098ObjUPNP[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{"BBF", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot098ObjUPNPBBF, NULL, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot098ObjUPNPDMROOT, NULL, NULL},
{0}
};

DMOBJ tRoot098ObjUPNPDMROOT[] = {
{"DM", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot098ObjUPNPDM, NULL, NULL},
{0}
};

DMOBJ tRoot098ObjUPNPDM[] = {
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpDeviceInfoObj, upnpDeviceInfoParams, NULL},
{"Configuration", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpConfigurationObj, NULL, NULL},
{"Monitoring", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpMonitoringObj, upnpMonitoringParams, NULL},
{0}
};

DMOBJ tRoot098ObjUPNPBBF[] = {
/* OBJ permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, notification, linker*/
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tDeviceInfoObj, tDeviceInfoParams, NULL},
{"ManagementServer", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tManagementServerParams, NULL},
{"Time", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tTimeParams, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tUPnPObj, NULL, NULL},
{"VoiceService", &DMREAD, NULL, NULL, NULL, browseVoiceServiceInst, NULL, NULL, tVoiceServiceObj, tVoiceServiceParam, NULL},
{CUSTOM_PREFIX"ICE", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IceParam, NULL},
{CUSTOM_PREFIX"IGMP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IgmpParam, NULL},
{CUSTOM_PREFIX"IpAccCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSe_IpAccObj, NULL, NULL},
{CUSTOM_PREFIX"LoginCfg", &DMREAD, NULL, NULL, NULL, NULL,NULL, &DMNONE,NULL, tSe_LoginCfgParam, NULL},
{CUSTOM_PREFIX"PowerManagement", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_PowerManagementParam, NULL},
{CUSTOM_PREFIX"SyslogCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_SyslogCfgParam, NULL},
{"SoftwareModules", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSoftwareModulesObj, NULL, NULL},
{CUSTOM_PREFIX"Owsd", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,XIopsysEuOwsdObj, XIopsysEuOwsdParams, NULL},
{CUSTOM_PREFIX"Dropbear", &DMWRITE, add_dropbear_instance, delete_dropbear_instance, NULL, browseXIopsysEuDropbear, NULL, &DMNONE, NULL, X_IOPSYS_EU_DropbearParams, NULL},
{CUSTOM_PREFIX"Buttons", &DMREAD, NULL, NULL, NULL, browseXIopsysEuButton, NULL, &DMNONE, NULL, X_IOPSYS_EU_ButtonParams, NULL},
{"LANDevice", &DMREAD, NULL, NULL, NULL, browselandeviceInst, &DMFINFRM, &DMNONE,tLANDeviceObj, tLANDeviceParam, NULL},
{"WANDevice", &DMREAD, NULL, NULL, NULL, browsewandeviceInst, &DMFINFRM, &DMWANConnectionDevicenotif,tWANDeviceObj, tWANDeviceParam, NULL},
{"LANInterfaces", &DMREAD, NULL, NULL, check_laninterfaces, NULL, &DMFINFRM, &DMNONE,tLANInterfacesObj, tLANInterfacesParam, NULL},
{"IPPingDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tIPPingDiagnosticsParam, NULL},
{"Layer3Forwarding", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tLayer3ForwardingObj, tLayer3ForwardingParam, NULL},
{"Layer2Bridging", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tLayer2BridgingObj, NULL, NULL},
{CUSTOM_PREFIX"Wifi", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tsewifiObj, NULL, NULL},
{"DownloadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tDownloadDiagnosticsParam, NULL},
{"UploadDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tUploadDiagnosticsParam, NULL},
#ifdef XMPP_ENABLE
{"XMPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL,tXMPPObj, tXMPPParams, NULL},
#endif
{0}
};

DMOBJ tEntry181ObjUPNP[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{(char *)&dmroot, &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot181ObjUPNP, NULL, NULL},
{0}
};

DMOBJ tRoot181ObjUPNP[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, NOTIFICATION, nextobj, leaf, linker*/
{"BBF", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot181ObjUPNPBBF, NULL, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot181ObjUPNPDMROOT, NULL, NULL},
{0}
};

DMOBJ tRoot181ObjUPNPDMROOT[] = {
{"DM", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, tRoot181ObjUPNPDM, NULL, NULL},
{0}
};

DMOBJ tRoot181ObjUPNPDM[] = {
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpDeviceInfoObj, upnpDeviceInfoParams, NULL},
{"Configuration", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpConfigurationObj, NULL, NULL},
{"Monitoring", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,upnpMonitoringObj, upnpMonitoringParams, NULL},
{0}
};

DMOBJ tRoot181ObjUPNPBBF[] = {
/* OBJ permission, addobj, delobj, browseinstobj, finform, nextobj, leaf, notification, linker*/
{"DeviceInfo", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,tDeviceInfoObj, tDeviceInfoParams, NULL},
{"ManagementServer", &DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE,NULL, tManagementServerParams, NULL},
{"Time", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tTimeParams, NULL},
{"UPnP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tUPnPObj, NULL, NULL},
{"VoiceService", &DMREAD, NULL, NULL, NULL, browseVoiceServiceInst, NULL, NULL, tVoiceServiceObj, tVoiceServiceParam, NULL},
{CUSTOM_PREFIX"ICE", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IceParam, NULL},
{CUSTOM_PREFIX"IGMP", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_IgmpParam, NULL},
{CUSTOM_PREFIX"IpAccCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSe_IpAccObj, NULL, NULL},
{CUSTOM_PREFIX"LoginCfg", &DMREAD, NULL, NULL, NULL, NULL,NULL, &DMNONE,NULL, tSe_LoginCfgParam, NULL},
{CUSTOM_PREFIX"PowerManagement", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_PowerManagementParam, NULL},
{CUSTOM_PREFIX"SyslogCfg", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,NULL, tSe_SyslogCfgParam, NULL},
{"SoftwareModules", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,tSoftwareModulesObj, NULL, NULL},
{CUSTOM_PREFIX"Owsd", &DMREAD, NULL, NULL, NULL, NULL, NULL, &DMNONE,XIopsysEuOwsdObj, XIopsysEuOwsdParams, NULL},
{CUSTOM_PREFIX"Dropbear", &DMWRITE, add_dropbear_instance, delete_dropbear_instance, NULL, browseXIopsysEuDropbear, NULL, &DMNONE, NULL, X_IOPSYS_EU_DropbearParams, NULL},
{CUSTOM_PREFIX"Buttons", &DMREAD, NULL, NULL, NULL, browseXIopsysEuButton, NULL, &DMNONE, NULL, X_IOPSYS_EU_ButtonParams, NULL},
{"Bridging",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tBridgingObj, NULL, NULL},
{"WiFi",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tWifiObj, NULL, NULL},
{"IP",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tIPObj, NULL, NULL},
{"Ethernet", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tEthernetObj, NULL, NULL},
{"DSL",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDslObj, NULL, NULL},
{"ATM",&DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tAtmObj, NULL, NULL},
{"PTM", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tPtmObj, NULL, NULL},
{"DHCPv4", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDhcpv4Obj, NULL, NULL},
{"Hosts", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, thostsObj, thostsParam, NULL},
{"NAT", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tnatObj, NULL, NULL},
{"PPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tpppObj, NULL, NULL},
{"Routing", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tRoutingObj, tRoutingParam, NULL},
#ifdef XMPP_ENABLE
{"XMPP", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL,tXMPPObj, tXMPPParams, NULL},
#endif
{0}
};

UPNP_SUPPORTED_DM tUPNPSupportedDM[];
UPNP_SUPPORTED_DM tUPNPSupportedDM_098[] = {
{"/UPnP/DM/DeviceInfo/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_098, "UPnP DeviceInfo from "DMROOT_DESC_098, ""},
{"/UPnP/DM/Configuration/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_098, "Configuration from "DMROOT_DESC_098, ""},
{"/UPnP/DM/Monitoring/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_098, "Monitoring from "DMROOT_DESC_098, ""},
{"/BBF/DeviceInfo/", DMROOT_URI_098, DMROOT_URL_098, "DeviceInfo from "DMROOT_DESC_098, ""},
{"/BBF/ManagementServer/", DMROOT_URI_098, DMROOT_URL_098, "ManagementServer from "DMROOT_DESC_098, ""},
{"/BBF/Time/", DMROOT_URI_098, DMROOT_URL_098, "Time from "DMROOT_DESC_098, ""},
{"/BBF/UPnP/", DMROOT_URI_098, DMROOT_URL_098, "UPnP from "DMROOT_DESC_098, ""},
{"/BBF/VoiceService/", "urn:broadband-forum-org:wt-104-2-0-0", "https://www.broadband-forum.org/cwmp/tr-104-2-0-0.html", "TR-104 Voice:2 Service Object definition", ""},
{"/BBF/"CUSTOM_PREFIX"ICE/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for ICE", ""},
{"/BBF/"CUSTOM_PREFIX"IGMP/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for ICE", ""},
{"/BBF/"CUSTOM_PREFIX"IpAccCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for IGMP", ""},
{"/BBF/"CUSTOM_PREFIX"LoginCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for LoginCfg", ""},
{"/BBF/"CUSTOM_PREFIX"PowerManagement/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for PowerManagement", ""},
{"/BBF/"CUSTOM_PREFIX"SyslogCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for SyslogCfg", ""},
{"/BBF/SoftwareModules/", DMROOT_URI_098, DMROOT_URL_098, "SoftwareModules from "DMROOT_DESC_098, ""},
{"/BBF/"CUSTOM_PREFIX"Owsd/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Owsd", ""},
{"/BBF/"CUSTOM_PREFIX"Dropbear/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Dropbear", ""},
{"/BBF/"CUSTOM_PREFIX"Buttons/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Buttons", ""},
{"/BBF/LANDevice/", DMROOT_URI_098, DMROOT_URL_098, "LANDevice from "DMROOT_DESC_098, ""},
{"/BBF/WANDevice/", DMROOT_URI_098, DMROOT_URL_098, "WANDevice from "DMROOT_DESC_098, ""},
{"/BBF/LANInterfaces/", DMROOT_URI_098, DMROOT_URL_098, "LANInterfaces from "DMROOT_DESC_098, ""},
{"/BBF/IPPingDiagnostics/", DMROOT_URI_098, DMROOT_URL_098, "IPPingDiagnostics from "DMROOT_DESC_098, ""},
{"/BBF/Layer3Forwarding/", DMROOT_URI_098, DMROOT_URL_098, "Layer3Forwarding from "DMROOT_DESC_098, ""},
{"/BBF/Layer2Bridging/", DMROOT_URI_098, DMROOT_URL_098, "Layer2Bridging from "DMROOT_DESC_098, ""},
{"/BBF/"CUSTOM_PREFIX"Wifi/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for WiFi", ""},
{"/BBF/DownloadDiagnostics/", DMROOT_URI_098, DMROOT_URL_098, "DownloadDiagnostics from "DMROOT_DESC_098, ""},
{"/BBF/UploadDiagnostics/", DMROOT_URI_098, DMROOT_URL_098, "UploadDiagnostics from "DMROOT_DESC_098, ""},
#ifdef XMPP_ENABLE
{"/BBF/XMPP/", DMROOT_URI_098, DMROOT_URL_098, "XMPP from "DMROOT_DESC_098, ""},
#endif
{0}
};

UPNP_SUPPORTED_DM tUPNPSupportedDM_181[] = {
{"/UPnP/DM/DeviceInfo/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_181, "UPnP DeviceInfo from "DMROOT_DESC_181, ""},
{"/UPnP/DM/Configuration/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_181, "Configuration from "DMROOT_DESC_181, ""},
{"/UPnP/DM/Monitoring/", "urn:UPnP:Parent Device:1:ConfigurationManagement:2", DMROOT_URL_181, "Monitoring from "DMROOT_DESC_181, ""},
{"/BBF/DeviceInfo/", DMROOT_URI_181, DMROOT_URL_181, "DeviceInfo from "DMROOT_DESC_181, ""},
{"/BBF/ManagementServer/", DMROOT_URI_181, DMROOT_URL_181, "ManagementServer from "DMROOT_DESC_181, ""},
{"/BBF/Time/", DMROOT_URI_181, DMROOT_URL_181, "Time from "DMROOT_DESC_181, ""},
{"/BBF/UPnP/", DMROOT_URI_181, DMROOT_URL_181, "UPnP from "DMROOT_DESC_181, ""},
{"/BBF/VoiceService/", "urn:broadband-forum-org:wt-104-2-0-0", "https://www.broadband-forum.org/cwmp/tr-104-2-0-0.html", "TR-104 Voice:2 Service Object definition", ""},
{"/BBF/"CUSTOM_PREFIX"ICE/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for ICE", ""},
{"/BBF/"CUSTOM_PREFIX"IGMP/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for ICE", ""},
{"/BBF/"CUSTOM_PREFIX"IpAccCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for IGMP", ""},
{"/BBF/"CUSTOM_PREFIX"LoginCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for LoginCfg", ""},
{"/BBF/"CUSTOM_PREFIX"PowerManagement/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for PowerManagement", ""},
{"/BBF/"CUSTOM_PREFIX"SyslogCfg/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for SyslogCfg", ""},
{"/BBF/SoftwareModules/", DMROOT_URI_181, DMROOT_URL_181, "SoftwareModules from "DMROOT_DESC_181, ""},
{"/BBF/"CUSTOM_PREFIX"Owsd/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Owsd", ""},
{"/BBF/"CUSTOM_PREFIX"Dropbear/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Dropbear", ""},
{"/BBF/"CUSTOM_PREFIX"Buttons/", "urn:intenogroup-com:na", "https://www.intenogroup.com/", "Inteno extension for Buttons", ""},
{"/BBF/Bridging/", DMROOT_URI_181, DMROOT_URL_181, "Bridging from "DMROOT_DESC_181, ""},
{"/BBF/WiFi/", DMROOT_URI_181, DMROOT_URL_181, "WiFi from "DMROOT_DESC_181, ""},
{"/BBF/IP/", DMROOT_URI_181, DMROOT_URL_181, "IP from "DMROOT_DESC_181, ""},
{"/BBF/Ethernet/", DMROOT_URI_181, DMROOT_URL_181, "Ethernet from "DMROOT_DESC_181, ""},
{"/BBF/DSL/", DMROOT_URI_181, DMROOT_URL_181, "DSL from "DMROOT_DESC_181, ""},
{"/BBF/ATM/", DMROOT_URI_181, DMROOT_URL_181, "ATM from "DMROOT_DESC_181, ""},
{"/BBF/PTM/", DMROOT_URI_181, DMROOT_URL_181, "PTM from "DMROOT_DESC_181, ""},
{"/BBF/DHCPv4/", DMROOT_URI_181, DMROOT_URL_181, "DHCPv4 from "DMROOT_DESC_181, ""},
{"/BBF/Hosts/", DMROOT_URI_181, DMROOT_URL_181, "Hosts from "DMROOT_DESC_181, ""},
{"/BBF/NAT/", DMROOT_URI_181, DMROOT_URL_181, "NAT from "DMROOT_DESC_181, ""},
{"/BBF/PPP/", DMROOT_URI_181, DMROOT_URL_181, "PPP from "DMROOT_DESC_181, ""},
{"/BBF/Routing/", DMROOT_URI_181, DMROOT_URL_181, "Routing from "DMROOT_DESC_181, ""},
#ifdef XMPP_ENABLE
{"/BBF/XMPP/", DMROOT_URI_181, DMROOT_URL_181, "XMPP from "DMROOT_DESC_181, ""},
#endif
{0}
};

size_t tr98_size  = sizeof(tUPNPSupportedDM_098);
size_t tr181_size = sizeof(tUPNPSupportedDM_181);
#endif
