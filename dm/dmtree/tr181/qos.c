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
#include "qos.h"
#include <regex.h>

/* *** Device.QoS. *** */
DMOBJ tQoSObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Classification", &DMWRITE, addObjQoSClassification, delObjQoSClassification, NULL, browseQoSClassificationInst, NULL, NULL, NULL, tQoSClassificationParams, NULL},
{"App", &DMWRITE, addObjQoSApp, delObjQoSApp, NULL, browseQoSAppInst, NULL, NULL, NULL, tQoSAppParams, NULL},
{"Flow", &DMWRITE, addObjQoSFlow, delObjQoSFlow, NULL, browseQoSFlowInst, NULL, NULL, NULL, tQoSFlowParams, NULL},
{"Policer", &DMWRITE, addObjQoSPolicer, delObjQoSPolicer, NULL, browseQoSPolicerInst, NULL, NULL, NULL, tQoSPolicerParams, NULL},
{"Queue", &DMWRITE, addObjQoSQueue, delObjQoSQueue, NULL, browseQoSQueueInst, NULL, NULL, NULL, tQoSQueueParams, get_linker_qos_queue},
{"QueueStats", &DMWRITE, addObjQoSQueueStats, delObjQoSQueueStats, NULL, browseQoSQueueStatsInst, NULL, NULL, NULL, tQoSQueueStatsParams, NULL},
{"Shaper", &DMWRITE, addObjQoSShaper, delObjQoSShaper, NULL, browseQoSShaperInst, NULL, NULL, NULL, tQoSShaperParams, NULL},
{0}
};

DMLEAF tQoSParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"MaxClassificationEntries", &DMREAD, DMT_UNINT, get_QoS_MaxClassificationEntries, NULL, NULL, NULL},
{"ClassificationNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_ClassificationNumberOfEntries, NULL, NULL, NULL},
{"MaxAppEntries", &DMREAD, DMT_UNINT, get_QoS_MaxAppEntries, NULL, NULL, NULL},
{"AppNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_AppNumberOfEntries, NULL, NULL, NULL},
{"MaxFlowEntries", &DMREAD, DMT_UNINT, get_QoS_MaxFlowEntries, NULL, NULL, NULL},
{"FlowNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_FlowNumberOfEntries, NULL, NULL, NULL},
{"MaxPolicerEntries", &DMREAD, DMT_UNINT, get_QoS_MaxPolicerEntries, NULL, NULL, NULL},
{"PolicerNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_PolicerNumberOfEntries, NULL, NULL, NULL},
{"MaxQueueEntries", &DMREAD, DMT_UNINT, get_QoS_MaxQueueEntries, NULL, NULL, NULL},
{"QueueNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_QueueNumberOfEntries, NULL, NULL, NULL},
{"QueueStatsNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_QueueStatsNumberOfEntries, NULL, NULL, NULL},
{"MaxShaperEntries", &DMREAD, DMT_UNINT, get_QoS_MaxShaperEntries, NULL, NULL, NULL},
{"ShaperNumberOfEntries", &DMREAD, DMT_UNINT, get_QoS_ShaperNumberOfEntries, NULL, NULL, NULL},
{"DefaultForwardingPolicy", &DMWRITE, DMT_UNINT, get_QoS_DefaultForwardingPolicy, set_QoS_DefaultForwardingPolicy, NULL, NULL},
{"DefaultTrafficClass", &DMWRITE, DMT_UNINT, get_QoS_DefaultTrafficClass, set_QoS_DefaultTrafficClass, NULL, NULL},
{"DefaultPolicer", &DMWRITE, DMT_STRING, get_QoS_DefaultPolicer, set_QoS_DefaultPolicer, NULL, NULL},
{"DefaultQueue", &DMWRITE, DMT_STRING, get_QoS_DefaultQueue, set_QoS_DefaultQueue, NULL, NULL},
{"DefaultDSCPMark", &DMWRITE, DMT_INT, get_QoS_DefaultDSCPMark, set_QoS_DefaultDSCPMark, NULL, NULL},
{"DefaultEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoS_DefaultEthernetPriorityMark, set_QoS_DefaultEthernetPriorityMark, NULL, NULL},
{"DefaultInnerEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoS_DefaultInnerEthernetPriorityMark, set_QoS_DefaultInnerEthernetPriorityMark, NULL, NULL},
{"AvailableAppList", &DMREAD, DMT_STRING, get_QoS_AvailableAppList, NULL, NULL, NULL},
{0}
};

/* *** Device.QoS.Classification.{i}. *** */
DMLEAF tQoSClassificationParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSClassification_Enable, set_QoSClassification_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSClassification_Status, NULL, NULL, NULL},
{"Order", &DMWRITE, DMT_UNINT, get_QoSClassification_Order, set_QoSClassification_Order, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSClassification_Alias, set_QoSClassification_Alias, NULL, NULL},
{"DHCPType", &DMWRITE, DMT_STRING, get_QoSClassification_DHCPType, set_QoSClassification_DHCPType, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_QoSClassification_Interface, set_QoSClassification_Interface, NULL, NULL},
{"AllInterfaces", &DMWRITE, DMT_BOOL, get_QoSClassification_AllInterfaces, set_QoSClassification_AllInterfaces, NULL, NULL},
{"DestIP", &DMWRITE, DMT_STRING, get_QoSClassification_DestIP, set_QoSClassification_DestIP, NULL, NULL},
{"DestMask", &DMWRITE, DMT_STRING, get_QoSClassification_DestMask, set_QoSClassification_DestMask, NULL, NULL},
{"DestIPExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestIPExclude, set_QoSClassification_DestIPExclude, NULL, NULL},
{"SourceIP", &DMWRITE, DMT_STRING, get_QoSClassification_SourceIP, set_QoSClassification_SourceIP, NULL, NULL},
{"SourceMask", &DMWRITE, DMT_STRING, get_QoSClassification_SourceMask, set_QoSClassification_SourceMask, NULL, NULL},
{"SourceIPExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceIPExclude, set_QoSClassification_SourceIPExclude, NULL, NULL},
{"Protocol", &DMWRITE, DMT_INT, get_QoSClassification_Protocol, set_QoSClassification_Protocol, NULL, NULL},
{"ProtocolExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_ProtocolExclude, set_QoSClassification_ProtocolExclude, NULL, NULL},
{"DestPort", &DMWRITE, DMT_INT, get_QoSClassification_DestPort, set_QoSClassification_DestPort, NULL, NULL},
{"DestPortRangeMax", &DMWRITE, DMT_INT, get_QoSClassification_DestPortRangeMax, set_QoSClassification_DestPortRangeMax, NULL, NULL},
{"DestPortExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestPortExclude, set_QoSClassification_DestPortExclude, NULL, NULL},
{"SourcePort", &DMWRITE, DMT_INT, get_QoSClassification_SourcePort, set_QoSClassification_SourcePort, NULL, NULL},
{"SourcePortRangeMax", &DMWRITE, DMT_INT, get_QoSClassification_SourcePortRangeMax, set_QoSClassification_SourcePortRangeMax, NULL, NULL},
{"SourcePortExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourcePortExclude, set_QoSClassification_SourcePortExclude, NULL, NULL},
{"SourceMACAddress", &DMWRITE, DMT_STRING, get_QoSClassification_SourceMACAddress, set_QoSClassification_SourceMACAddress, NULL, NULL},
{"SourceMACMask", &DMWRITE, DMT_STRING, get_QoSClassification_SourceMACMask, set_QoSClassification_SourceMACMask, NULL, NULL},
{"SourceMACExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceMACExclude, set_QoSClassification_SourceMACExclude, NULL, NULL},
{"DestMACAddress", &DMWRITE, DMT_STRING, get_QoSClassification_DestMACAddress, set_QoSClassification_DestMACAddress, NULL, NULL},
{"DestMACMask", &DMWRITE, DMT_STRING, get_QoSClassification_DestMACMask, set_QoSClassification_DestMACMask, NULL, NULL},
{"DestMACExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestMACExclude, set_QoSClassification_DestMACExclude, NULL, NULL},
{"Ethertype", &DMWRITE, DMT_INT, get_QoSClassification_Ethertype, set_QoSClassification_Ethertype, NULL, NULL},
{"EthertypeExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_EthertypeExclude, set_QoSClassification_EthertypeExclude, NULL, NULL},
{"SSAP", &DMWRITE, DMT_INT, get_QoSClassification_SSAP, set_QoSClassification_SSAP, NULL, NULL},
{"SSAPExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SSAPExclude, set_QoSClassification_SSAPExclude, NULL, NULL},
{"DSAP", &DMWRITE, DMT_INT, get_QoSClassification_DSAP, set_QoSClassification_DSAP, NULL, NULL},
{"DSAPExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DSAPExclude, set_QoSClassification_DSAPExclude, NULL, NULL},
{"LLCControl", &DMWRITE, DMT_INT, get_QoSClassification_LLCControl, set_QoSClassification_LLCControl, NULL, NULL},
{"LLCControlExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_LLCControlExclude, set_QoSClassification_LLCControlExclude, NULL, NULL},
{"SNAPOUI", &DMWRITE, DMT_INT, get_QoSClassification_SNAPOUI, set_QoSClassification_SNAPOUI, NULL, NULL},
{"SNAPOUIExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SNAPOUIExclude, set_QoSClassification_SNAPOUIExclude, NULL, NULL},
{"SourceVendorClassID", &DMWRITE, DMT_STRING, get_QoSClassification_SourceVendorClassID, set_QoSClassification_SourceVendorClassID, NULL, NULL},
{"SourceVendorClassIDv6", &DMWRITE, DMT_HEXBIN, get_QoSClassification_SourceVendorClassIDv6, set_QoSClassification_SourceVendorClassIDv6, NULL, NULL},
{"SourceVendorClassIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceVendorClassIDExclude, set_QoSClassification_SourceVendorClassIDExclude, NULL, NULL},
{"SourceVendorClassIDMode", &DMWRITE, DMT_STRING, get_QoSClassification_SourceVendorClassIDMode, set_QoSClassification_SourceVendorClassIDMode, NULL, NULL},
{"DestVendorClassID", &DMWRITE, DMT_STRING, get_QoSClassification_DestVendorClassID, set_QoSClassification_DestVendorClassID, NULL, NULL},
{"DestVendorClassIDv6", &DMWRITE, DMT_HEXBIN, get_QoSClassification_DestVendorClassIDv6, set_QoSClassification_DestVendorClassIDv6, NULL, NULL},
{"DestVendorClassIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestVendorClassIDExclude, set_QoSClassification_DestVendorClassIDExclude, NULL, NULL},
{"DestVendorClassIDMode", &DMWRITE, DMT_STRING, get_QoSClassification_DestVendorClassIDMode, set_QoSClassification_DestVendorClassIDMode, NULL, NULL},
{"SourceClientID", &DMWRITE, DMT_HEXBIN, get_QoSClassification_SourceClientID, set_QoSClassification_SourceClientID, NULL, NULL},
{"SourceClientIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceClientIDExclude, set_QoSClassification_SourceClientIDExclude, NULL, NULL},
{"DestClientID", &DMWRITE, DMT_HEXBIN, get_QoSClassification_DestClientID, set_QoSClassification_DestClientID, NULL, NULL},
{"DestClientIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestClientIDExclude, set_QoSClassification_DestClientIDExclude, NULL, NULL},
{"SourceUserClassID", &DMWRITE, DMT_HEXBIN, get_QoSClassification_SourceUserClassID, set_QoSClassification_SourceUserClassID, NULL, NULL},
{"SourceUserClassIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceUserClassIDExclude, set_QoSClassification_SourceUserClassIDExclude, NULL, NULL},
{"DestUserClassID", &DMWRITE, DMT_HEXBIN, get_QoSClassification_DestUserClassID, set_QoSClassification_DestUserClassID, NULL, NULL},
{"DestUserClassIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestUserClassIDExclude, set_QoSClassification_DestUserClassIDExclude, NULL, NULL},
{"SourceVendorSpecificInfo", &DMWRITE, DMT_HEXBIN, get_QoSClassification_SourceVendorSpecificInfo, set_QoSClassification_SourceVendorSpecificInfo, NULL, NULL},
{"SourceVendorSpecificInfoExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_SourceVendorSpecificInfoExclude, set_QoSClassification_SourceVendorSpecificInfoExclude, NULL, NULL},
{"SourceVendorSpecificInfoEnterprise", &DMWRITE, DMT_UNINT, get_QoSClassification_SourceVendorSpecificInfoEnterprise, set_QoSClassification_SourceVendorSpecificInfoEnterprise, NULL, NULL},
{"SourceVendorSpecificInfoSubOption", &DMWRITE, DMT_INT, get_QoSClassification_SourceVendorSpecificInfoSubOption, set_QoSClassification_SourceVendorSpecificInfoSubOption, NULL, NULL},
{"DestVendorSpecificInfo", &DMWRITE, DMT_HEXBIN, get_QoSClassification_DestVendorSpecificInfo, set_QoSClassification_DestVendorSpecificInfo, NULL, NULL},
{"DestVendorSpecificInfoExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DestVendorSpecificInfoExclude, set_QoSClassification_DestVendorSpecificInfoExclude, NULL, NULL},
{"DestVendorSpecificInfoEnterprise", &DMWRITE, DMT_UNINT, get_QoSClassification_DestVendorSpecificInfoEnterprise, set_QoSClassification_DestVendorSpecificInfoEnterprise, NULL, NULL},
{"DestVendorSpecificInfoSubOption", &DMWRITE, DMT_INT, get_QoSClassification_DestVendorSpecificInfoSubOption, set_QoSClassification_DestVendorSpecificInfoSubOption, NULL, NULL},
{"TCPACK", &DMWRITE, DMT_BOOL, get_QoSClassification_TCPACK, set_QoSClassification_TCPACK, NULL, NULL},
{"TCPACKExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_TCPACKExclude, set_QoSClassification_TCPACKExclude, NULL, NULL},
{"IPLengthMin", &DMWRITE, DMT_UNINT, get_QoSClassification_IPLengthMin, set_QoSClassification_IPLengthMin, NULL, NULL},
{"IPLengthMax", &DMWRITE, DMT_UNINT, get_QoSClassification_IPLengthMax, set_QoSClassification_IPLengthMax, NULL, NULL},
{"IPLengthExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_IPLengthExclude, set_QoSClassification_IPLengthExclude, NULL, NULL},
{"DSCPCheck", &DMWRITE, DMT_INT, get_QoSClassification_DSCPCheck, set_QoSClassification_DSCPCheck, NULL, NULL},
{"DSCPExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_DSCPExclude, set_QoSClassification_DSCPExclude, NULL, NULL},
{"DSCPMark", &DMWRITE, DMT_INT, get_QoSClassification_DSCPMark, set_QoSClassification_DSCPMark, NULL, NULL},
{"EthernetPriorityCheck", &DMWRITE, DMT_INT, get_QoSClassification_EthernetPriorityCheck, set_QoSClassification_EthernetPriorityCheck, NULL, NULL},
{"EthernetPriorityExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_EthernetPriorityExclude, set_QoSClassification_EthernetPriorityExclude, NULL, NULL},
{"EthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSClassification_EthernetPriorityMark, set_QoSClassification_EthernetPriorityMark, NULL, NULL},
{"InnerEthernetPriorityCheck", &DMWRITE, DMT_INT, get_QoSClassification_InnerEthernetPriorityCheck, set_QoSClassification_InnerEthernetPriorityCheck, NULL, NULL},
{"InnerEthernetPriorityExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_InnerEthernetPriorityExclude, set_QoSClassification_InnerEthernetPriorityExclude, NULL, NULL},
{"InnerEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSClassification_InnerEthernetPriorityMark, set_QoSClassification_InnerEthernetPriorityMark, NULL, NULL},
{"EthernetDEICheck", &DMWRITE, DMT_INT, get_QoSClassification_EthernetDEICheck, set_QoSClassification_EthernetDEICheck, NULL, NULL},
{"EthernetDEIExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_EthernetDEIExclude, set_QoSClassification_EthernetDEIExclude, NULL, NULL},
{"VLANIDCheck", &DMWRITE, DMT_INT, get_QoSClassification_VLANIDCheck, set_QoSClassification_VLANIDCheck, NULL, NULL},
{"VLANIDExclude", &DMWRITE, DMT_BOOL, get_QoSClassification_VLANIDExclude, set_QoSClassification_VLANIDExclude, NULL, NULL},
{"OutOfBandInfo", &DMWRITE, DMT_INT, get_QoSClassification_OutOfBandInfo, set_QoSClassification_OutOfBandInfo, NULL, NULL},
{"ForwardingPolicy", &DMWRITE, DMT_UNINT, get_QoSClassification_ForwardingPolicy, set_QoSClassification_ForwardingPolicy, NULL, NULL},
{"TrafficClass", &DMWRITE, DMT_INT, get_QoSClassification_TrafficClass, set_QoSClassification_TrafficClass, NULL, NULL},
{"Policer", &DMWRITE, DMT_STRING, get_QoSClassification_Policer, set_QoSClassification_Policer, NULL, NULL},
{"App", &DMWRITE, DMT_STRING, get_QoSClassification_App, set_QoSClassification_App, NULL, NULL},
{0}
};

/* *** Device.QoS.App.{i}. *** */
DMLEAF tQoSAppParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSApp_Enable, set_QoSApp_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSApp_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSApp_Alias, set_QoSApp_Alias, NULL, NULL},
{"ProtocolIdentifier", &DMWRITE, DMT_STRING, get_QoSApp_ProtocolIdentifier, set_QoSApp_ProtocolIdentifier, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_QoSApp_Name, set_QoSApp_Name, NULL, NULL},
{"DefaultForwardingPolicy", &DMWRITE, DMT_UNINT, get_QoSApp_DefaultForwardingPolicy, set_QoSApp_DefaultForwardingPolicy, NULL, NULL},
{"DefaultTrafficClass", &DMWRITE, DMT_UNINT, get_QoSApp_DefaultTrafficClass, set_QoSApp_DefaultTrafficClass, NULL, NULL},
{"DefaultPolicer", &DMWRITE, DMT_STRING, get_QoSApp_DefaultPolicer, set_QoSApp_DefaultPolicer, NULL, NULL},
{"DefaultDSCPMark", &DMWRITE, DMT_INT, get_QoSApp_DefaultDSCPMark, set_QoSApp_DefaultDSCPMark, NULL, NULL},
{"DefaultEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSApp_DefaultEthernetPriorityMark, set_QoSApp_DefaultEthernetPriorityMark, NULL, NULL},
{"DefaultInnerEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSApp_DefaultInnerEthernetPriorityMark, set_QoSApp_DefaultInnerEthernetPriorityMark, NULL, NULL},
{0}
};

/* *** Device.QoS.Flow.{i}. *** */
DMLEAF tQoSFlowParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSFlow_Enable, set_QoSFlow_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSFlow_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSFlow_Alias, set_QoSFlow_Alias, NULL, NULL},
{"Type", &DMWRITE, DMT_STRING, get_QoSFlow_Type, set_QoSFlow_Type, NULL, NULL},
{"TypeParameters", &DMWRITE, DMT_STRING, get_QoSFlow_TypeParameters, set_QoSFlow_TypeParameters, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_QoSFlow_Name, set_QoSFlow_Name, NULL, NULL},
{"App", &DMWRITE, DMT_STRING, get_QoSFlow_App, set_QoSFlow_App, NULL, NULL},
{"ForwardingPolicy", &DMWRITE, DMT_UNINT, get_QoSFlow_ForwardingPolicy, set_QoSFlow_ForwardingPolicy, NULL, NULL},
{"TrafficClass", &DMWRITE, DMT_UNINT, get_QoSFlow_TrafficClass, set_QoSFlow_TrafficClass, NULL, NULL},
{"Policer", &DMWRITE, DMT_STRING, get_QoSFlow_Policer, set_QoSFlow_Policer, NULL, NULL},
{"DSCPMark", &DMWRITE, DMT_INT, get_QoSFlow_DSCPMark, set_QoSFlow_DSCPMark, NULL, NULL},
{"EthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSFlow_EthernetPriorityMark, set_QoSFlow_EthernetPriorityMark, NULL, NULL},
{"InnerEthernetPriorityMark", &DMWRITE, DMT_INT, get_QoSFlow_InnerEthernetPriorityMark, set_QoSFlow_InnerEthernetPriorityMark, NULL, NULL},
{0}
};

/* *** Device.QoS.Policer.{i}. *** */
DMLEAF tQoSPolicerParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSPolicer_Enable, set_QoSPolicer_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSPolicer_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSPolicer_Alias, set_QoSPolicer_Alias, NULL, NULL},
{"CommittedRate", &DMWRITE, DMT_UNINT, get_QoSPolicer_CommittedRate, set_QoSPolicer_CommittedRate, NULL, NULL},
{"CommittedBurstSize", &DMWRITE, DMT_UNINT, get_QoSPolicer_CommittedBurstSize, set_QoSPolicer_CommittedBurstSize, NULL, NULL},
{"ExcessBurstSize", &DMWRITE, DMT_UNINT, get_QoSPolicer_ExcessBurstSize, set_QoSPolicer_ExcessBurstSize, NULL, NULL},
{"PeakRate", &DMWRITE, DMT_UNINT, get_QoSPolicer_PeakRate, set_QoSPolicer_PeakRate, NULL, NULL},
{"PeakBurstSize", &DMWRITE, DMT_UNINT, get_QoSPolicer_PeakBurstSize, set_QoSPolicer_PeakBurstSize, NULL, NULL},
{"MeterType", &DMWRITE, DMT_STRING, get_QoSPolicer_MeterType, set_QoSPolicer_MeterType, NULL, NULL},
{"PossibleMeterTypes", &DMREAD, DMT_STRING, get_QoSPolicer_PossibleMeterTypes, NULL, NULL, NULL},
{"ConformingAction", &DMWRITE, DMT_STRING, get_QoSPolicer_ConformingAction, set_QoSPolicer_ConformingAction, NULL, NULL},
{"PartialConformingAction", &DMWRITE, DMT_STRING, get_QoSPolicer_PartialConformingAction, set_QoSPolicer_PartialConformingAction, NULL, NULL},
{"NonConformingAction", &DMWRITE, DMT_STRING, get_QoSPolicer_NonConformingAction, set_QoSPolicer_NonConformingAction, NULL, NULL},
{"TotalCountedPackets", &DMREAD, DMT_UNINT, get_QoSPolicer_TotalCountedPackets, NULL, NULL, NULL},
{"TotalCountedBytes", &DMREAD, DMT_UNINT, get_QoSPolicer_TotalCountedBytes, NULL, NULL, NULL},
{"ConformingCountedPackets", &DMREAD, DMT_UNINT, get_QoSPolicer_ConformingCountedPackets, NULL, NULL, NULL},
{"ConformingCountedBytes", &DMREAD, DMT_UNINT, get_QoSPolicer_ConformingCountedBytes, NULL, NULL, NULL},
{"PartiallyConformingCountedPackets", &DMREAD, DMT_UNINT, get_QoSPolicer_PartiallyConformingCountedPackets, NULL, NULL, NULL},
{"PartiallyConformingCountedBytes", &DMREAD, DMT_UNINT, get_QoSPolicer_PartiallyConformingCountedBytes, NULL, NULL, NULL},
{"NonConformingCountedPackets", &DMREAD, DMT_UNINT, get_QoSPolicer_NonConformingCountedPackets, NULL, NULL, NULL},
{"NonConformingCountedBytes", &DMREAD, DMT_UNINT, get_QoSPolicer_NonConformingCountedBytes, NULL, NULL, NULL},
{0}
};

/* *** Device.QoS.Queue.{i}. *** */
DMLEAF tQoSQueueParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSQueue_Enable, set_QoSQueue_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSQueue_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSQueue_Alias, set_QoSQueue_Alias, NULL, NULL},
{"TrafficClasses", &DMWRITE, DMT_STRING, get_QoSQueue_TrafficClasses, set_QoSQueue_TrafficClasses, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_QoSQueue_Interface, set_QoSQueue_Interface, NULL, NULL},
{"AllInterfaces", &DMWRITE, DMT_BOOL, get_QoSQueue_AllInterfaces, set_QoSQueue_AllInterfaces, NULL, NULL},
{"HardwareAssisted", &DMREAD, DMT_BOOL, get_QoSQueue_HardwareAssisted, NULL, NULL, NULL},
{"BufferLength", &DMREAD, DMT_UNINT, get_QoSQueue_BufferLength, NULL, NULL, NULL},
{"Weight", &DMWRITE, DMT_UNINT, get_QoSQueue_Weight, set_QoSQueue_Weight, NULL, NULL},
{"Precedence", &DMWRITE, DMT_UNINT, get_QoSQueue_Precedence, set_QoSQueue_Precedence, NULL, NULL},
{"REDThreshold", &DMWRITE, DMT_UNINT, get_QoSQueue_REDThreshold, set_QoSQueue_REDThreshold, NULL, NULL},
{"REDPercentage", &DMWRITE, DMT_UNINT, get_QoSQueue_REDPercentage, set_QoSQueue_REDPercentage, NULL, NULL},
{"DropAlgorithm", &DMWRITE, DMT_STRING, get_QoSQueue_DropAlgorithm, set_QoSQueue_DropAlgorithm, NULL, NULL},
{"SchedulerAlgorithm", &DMWRITE, DMT_STRING, get_QoSQueue_SchedulerAlgorithm, set_QoSQueue_SchedulerAlgorithm, NULL, NULL},
{"ShapingRate", &DMWRITE, DMT_INT, get_QoSQueue_ShapingRate, set_QoSQueue_ShapingRate, NULL, NULL},
{"ShapingBurstSize", &DMWRITE, DMT_UNINT, get_QoSQueue_ShapingBurstSize, set_QoSQueue_ShapingBurstSize, NULL, NULL},
{0}
};

/* *** Device.QoS.QueueStats.{i}. *** */
DMLEAF tQoSQueueStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSQueueStats_Enable, set_QoSQueueStats_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSQueueStats_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSQueueStats_Alias, set_QoSQueueStats_Alias, NULL, NULL},
{"Queue", &DMWRITE, DMT_STRING, get_QoSQueueStats_Queue, set_QoSQueueStats_Queue, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_QoSQueueStats_Interface, set_QoSQueueStats_Interface, NULL, NULL},
{"OutputPackets", &DMREAD, DMT_UNINT, get_QoSQueueStats_OutputPackets, NULL, NULL, NULL},
{"OutputBytes", &DMREAD, DMT_UNINT, get_QoSQueueStats_OutputBytes, NULL, NULL, NULL},
{"DroppedPackets", &DMREAD, DMT_UNINT, get_QoSQueueStats_DroppedPackets, NULL, NULL, NULL},
{"DroppedBytes", &DMREAD, DMT_UNINT, get_QoSQueueStats_DroppedBytes, NULL, NULL, NULL},
{"QueueOccupancyPackets", &DMREAD, DMT_UNINT, get_QoSQueueStats_QueueOccupancyPackets, NULL, NULL, NULL},
{"QueueOccupancyPercentage", &DMREAD, DMT_UNINT, get_QoSQueueStats_QueueOccupancyPercentage, NULL, NULL, NULL},
{0}
};

/* *** Device.QoS.Shaper.{i}. *** */
DMLEAF tQoSShaperParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_QoSShaper_Enable, set_QoSShaper_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_QoSShaper_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_QoSShaper_Alias, set_QoSShaper_Alias, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_QoSShaper_Interface, set_QoSShaper_Interface, NULL, NULL},
{"ShapingRate", &DMWRITE, DMT_INT, get_QoSShaper_ShapingRate, set_QoSShaper_ShapingRate, NULL, NULL},
{"ShapingBurstSize", &DMWRITE, DMT_UNINT, get_QoSShaper_ShapingBurstSize, set_QoSShaper_ShapingBurstSize, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_linker_qos_queue(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if(data && ((struct dmmap_dup *)data)->config_section) {
		dmasprintf(linker,"%s", section_name(((struct dmmap_dup *)data)->config_section));
		return 0;
	} else {
		*linker = "";
		return 0;
	}
}

/**************************************************************************
* Browse functions
***************************************************************************/
int browseQoSClassificationInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL;
	char buf[12];
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("qos", "classify", "dmmap_qos", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, p->dmmap_section, "classifinstance", "classifalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseQoSAppInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseQoSFlowInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseQoSPolicerInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseQoSQueueInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL;
	char buf[12];
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("qos", "class", "dmmap_qos", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, p->dmmap_section, "queueinstance", "queuealias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

struct uci_section *get_dup_qos_stats_section_in_dmmap(char *dmmap_package, char *section_type, char *dev)
{
	struct uci_section *s;

	uci_path_foreach_option_eq(icwmpd, dmmap_package, section_type, "dev_link", dev, s)
	{
		return s;
	}

	return NULL;
}

int browseQoSQueueStatsInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *dmmap_sect;
	char *questatsout[256], *wnum= NULL, *instance= NULL, *inst_last= NULL, *v, *lastinstancestore= NULL, *instancestore= NULL, dev[50]= "", user[50]= "";
	int length, i, ret;
	struct queuestats queuests= {0}, emptyquestats= {0};
	regex_t regex1, regex2;

	regcomp(&regex1, queuessts1, 0);
	regcomp(&regex2, queuessts2, 0);
	check_create_dmmap_package("dmmap_qos");
	command_exec_output_to_array("tc -s qdisc", questatsout, &length);
	for(i=0; i<length; i++){
		switch(i%3) {
			case 0: ret= regexec(&regex1, questatsout[i], 0, NULL, 0);
				 	if (ret == 0)
				 		sscanf(questatsout[i], "qdisc noqueue %d: dev %s %s refcnt %d\n", &queuests.noqueue, dev, user, &queuests.refcnt);
				 	else {
				 		ret= regexec(&regex2, questatsout[i], 0, NULL, 0);
				 		if (ret == 0)
				 			sscanf(questatsout[i], "qdisc pfifo_fast %d: dev %s %s refcnt %d\n", &queuests.pfifo_fast, dev, user, &queuests.refcnt);
				 	}
					strcpy(queuests.dev, dev);
					break;
			case 1: sscanf(questatsout[i], " Sent %d bytes %d pkt (dropped %d, overlimits %d requeues %d)\n", &queuests.bytes_sent, &queuests.pkt_sent, &queuests.pkt_dropped, &queuests.pkt_overlimits, &queuests.pkt_requeues);
					break;
			case 2: sscanf(questatsout[i], " backlog %db %dp requeues %d\n", &queuests.backlog_b, &queuests.backlog_p, &queuests.backlog_requeues);
					if ((dmmap_sect = get_dup_qos_stats_section_in_dmmap("dmmap_qos", "qos_queue_stats", queuests.dev)) == NULL) {
						dmuci_add_section_icwmpd("dmmap_qos", "qos_queue_stats", &dmmap_sect, &v);
						DMUCI_SET_VALUE_BY_SECTION(icwmpd, dmmap_sect, "dev_link", queuests.dev);
					}
					queuests.dmsect= dmmap_sect;

					if(lastinstancestore != NULL && inst_last !=NULL)
						inst_last= dmstrdup(lastinstancestore);
					instance =  handle_update_instance(1, dmctx, &inst_last, update_instance_alias, 3, dmmap_sect, "queuestatsinstance", "queuestatsalias");
					lastinstancestore= dmstrdup(inst_last);
					instancestore= dmstrdup(instance);
					if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&queuests, instance) == DM_STOP)
						goto end;
					queuests= emptyquestats;
					dmfree(instance);
					break;
		}
	}
	end:
		return 0;
}

int browseQoSShaperInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL;
	char buf[12];
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	char *limitrate= NULL;
	LIST_HEAD(dup_list);
	synchronize_specific_config_sections_with_dmmap("qos", "class", "dmmap_qos", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		dmuci_get_value_by_section_string(p->config_section, "limitrate", &limitrate);
		if(limitrate == NULL || strlen(limitrate) == 0)
			continue;
		wnum =  handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, p->dmmap_section, "shaperinstance", "shaperalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p, wnum) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}


int addObjQoSClassification(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_qos_classify;
	char *last_inst= NULL, *sect_name= NULL, *qos_comment, *v;
	char ib[8];
	last_inst= get_last_instance_icwmpd("dmmap_qos", "classify", "classifinstance");
	if (last_inst)
		sprintf(ib, "%s", last_inst);
	else
		sprintf(ib, "%s", "1");
	dmasprintf(&qos_comment, "QoS classify %d", atoi(ib)+1);

	dmuci_add_section("qos", "classify", &s, &sect_name);
	dmuci_set_value_by_section(s, "comment", qos_comment);

	dmuci_add_section_icwmpd("dmmap_qos", "classify", &dmmap_qos_classify, &v);
	dmuci_set_value_by_section(dmmap_qos_classify, "section_name", sect_name);
	*instance = update_instance_icwmpd(dmmap_qos_classify, last_inst, "classifinstance");
	return 0;
}

int delObjQoSClassification(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_section;
	int found = 0;

	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name(p->config_section))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_qos", "classify", "classifinstance", section_name(p->config_section), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "classifinstance", "dmmap_qos", "classify");
				dmuci_delete_by_section_unnamed(p->config_section, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_qos", "classify", section_name(p->config_section), &dmmap_section);
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(p->config_section, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("qos", "classify", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_qos", "classify", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_qos", "classify", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjQoSApp(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjQoSApp(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjQoSFlow(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjQoSFlow(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjQoSPolicer(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjQoSPolicer(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjQoSQueue(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_qos_class;
	char *last_inst= NULL, *sect_name= NULL, *qos_comment, *v;
	char ib[8];
	last_inst= get_last_instance_icwmpd("dmmap_qos", "class", "queueinstance");
	if (last_inst)
		sprintf(ib, "%s", last_inst);
	else
		sprintf(ib, "%s", "1");

	dmuci_add_section("qos", "class", &s, &sect_name);
	dmuci_set_value_by_section(s, "packetsize", "1000");

	dmuci_add_section_icwmpd("dmmap_qos", "class", &dmmap_qos_class, &v);
	dmuci_set_value_by_section(dmmap_qos_class, "section_name", sect_name);
	*instance = update_instance_icwmpd(dmmap_qos_class, last_inst, "queueinstance");
	return 0;
}

int delObjQoSQueue(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_section;
	int found = 0;

	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name(p->config_section))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_qos", "class", "queueinstance", section_name(p->config_section), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "queueinstance", "dmmap_qos", "class");
				dmuci_delete_by_section_unnamed(p->config_section, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_qos", "class", section_name(p->config_section), &dmmap_section);
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(p->config_section, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("qos", "class", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_qos", "class", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_qos", "class", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}

int addObjQoSQueueStats(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	//TODO
	return 0;
}

int delObjQoSQueueStats(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
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

int addObjQoSShaper(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s, *dmmap_qos_class;
	char *last_inst= NULL, *sect_name= NULL, *qos_comment, *v;
	char ib[8];
	last_inst= get_last_instance_icwmpd_without_update("dmmap_qos", "class", "shaperinstance");
	if (last_inst)
		sprintf(ib, "%s", last_inst);
	else
		sprintf(ib, "%s", "1");

	dmuci_add_section("qos", "class", &s, &sect_name);
	dmuci_set_value_by_section(s, "limitrate", "1000");

	dmuci_add_section_icwmpd("dmmap_qos", "class", &dmmap_qos_class, &v);
	dmuci_set_value_by_section(dmmap_qos_class, "section_name", sect_name);
	*instance = update_instance_icwmpd(dmmap_qos_class, last_inst, "shaperinstance");
	return 0;
}

int delObjQoSShaper(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	struct uci_section *s, *dmmap_sect;
	switch (del_action) {
		case DEL_INST:
			dmuci_set_value_by_section(p->config_section, "limitrate", "");
			dmuci_set_value_by_section(p->dmmap_section, "shaperinstance", "");
			break;
		case DEL_ALL:
			uci_foreach_sections("qos", "class", s) {
				get_dmmap_section_of_config_section("dmmap_qos", "class", section_name(s), &dmmap_sect);
				dmuci_set_value_by_section(s, "limitrate", "");
				dmuci_set_value_by_section(dmmap_sect, "shaperinstance", "");
			}
			break;
	}
	return 0;
}


int get_QoS_MaxClassificationEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_ClassificationNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;
	uci_foreach_sections("qos", "classify", s) {
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_QoS_MaxAppEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_AppNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_MaxFlowEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_FlowNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_MaxPolicerEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_PolicerNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_MaxQueueEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_QueueNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;

	uci_foreach_sections("qos", "class", s) {
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_QoS_QueueStatsNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *questatsout[256];
	int length;

	command_exec_output_to_array("tc -s qdisc", questatsout, &length);
	dmasprintf(value, "%d", length/3);
	return 0;
}

int get_QoS_MaxShaperEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoS_ShaperNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int nbre= 0;
	char *rate= NULL;

	uci_foreach_sections("qos", "class", s) {

		dmuci_get_value_by_section_string(s, "limitrate", &rate);

		if(rate == NULL || strlen(rate)<=0)
			continue;
		nbre++;
	}
	dmasprintf(value, "%d", nbre);
	return 0;
}

int get_QoS_DefaultForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoS_DefaultForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_DefaultTrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoS_DefaultTrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_DefaultPolicer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoS_DefaultPolicer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_DefaultQueue(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;

	dmuci_get_option_value_string("qos", "Default", "default", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cQoS%cQueue%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_QoS_DefaultQueue(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			dmuci_set_value("qos", "Default", "default", linker);
			break;
	}
	return 0;
}

int get_QoS_DefaultDSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	char *linker, *classtarget;

	dmuci_get_option_value_string("qos", "Default", "default", &linker);
	uci_foreach_sections("qos", "classify", s) {
		dmuci_get_value_by_section_string(s, "target", &classtarget);
		if (strcmp(classtarget, linker) == 0) {
			dmuci_get_value_by_section_string(s, "dscp", value);
			return 0;
		}
	}
	*value= "";
	return 0;
}

int set_QoS_DefaultDSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_DefaultEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoS_DefaultEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_DefaultInnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoS_DefaultInnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoS_AvailableAppList(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSClassification_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSClassification_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_Order(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->dmmap_section, "classifalias", value);
	return 0;
}

int set_QoSClassification_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->dmmap_section, "classifalias", value);
			break;
	}
	return 0;
}

int get_QoSClassification_DHCPType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DHCPType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup *) data;
	struct uci_section *s;
	char *classes= NULL, **classesarr, *classgroup= NULL, *ifaceclassgrp, *targetclass;
	int nbre= 0;
	size_t length;

	dmuci_get_value_by_section_string(p->config_section, "target", &targetclass);
	uci_foreach_sections("qos", "classgroup", s) {
		dmuci_get_value_by_section_string(s, "classes", &classes);
		classesarr= strsplit(classes, " ", &length);
		if(classes!=NULL && is_array_elt_exist(classesarr, targetclass, length)){
			dmasprintf(&classgroup, "%s", section_name(s));
			break;
		}
	}
	if(classgroup == NULL)
		return 0;
	uci_foreach_sections("qos", "interface", s) {
		dmuci_get_value_by_section_string(s, "classgroup", &ifaceclassgrp);
		if(ifaceclassgrp != NULL && strcmp(ifaceclassgrp, classgroup) == 0){
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cPPP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				*value = "";
		}
	}
	return 0;
}

int set_QoSClassification_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_AllInterfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_AllInterfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestIP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "dsthost", value);
	return 0;
}

int set_QoSClassification_DestIP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "dsthost", value);
			break;
	}
	return 0;
}

int get_QoSClassification_DestMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestIPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestIPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceIP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "srchost", value);
	return 0;
}

int set_QoSClassification_SourceIP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "srchost", value);
			break;
	}
	return 0;
}

int get_QoSClassification_SourceMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceIPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceIPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "proto", value);
	return 0;
}

int set_QoSClassification_Protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "proto", value);
			break;
	}
	return 0;
}

int get_QoSClassification_ProtocolExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_ProtocolExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "dstports", value);
	return 0;
}

int set_QoSClassification_DestPort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "dstports", value);
			break;
	}
	return 0;
}

int get_QoSClassification_DestPortRangeMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "portrange", value);
	return 0;
}

int set_QoSClassification_DestPortRangeMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "portrange", value);
			break;
	}
	return 0;
}

int get_QoSClassification_DestPortExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestPortExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourcePort(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "srcports", value);
	return 0;
}

int set_QoSClassification_SourcePort(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "srcports", value);
			break;
	}
	return 0;
}

int get_QoSClassification_SourcePortRangeMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourcePortRangeMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourcePortExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourcePortExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceMACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceMACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceMACMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceMACMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceMACExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceMACExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestMACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestMACAddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestMACMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestMACMask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestMACExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestMACExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Ethertype(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_Ethertype(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_EthertypeExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthertypeExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SSAP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SSAP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SSAPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SSAPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DSAP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DSAP(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DSAPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DSAPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_LLCControl(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_LLCControl(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_LLCControlExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_LLCControlExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SNAPOUI(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SNAPOUI(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SNAPOUIExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SNAPOUIExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorClassIDv6(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorClassIDv6(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorClassIDv6(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorClassIDv6(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorClassIDMode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestClientID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestClientIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceUserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceUserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceUserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceUserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestUserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestUserClassID(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestUserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestUserClassIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorSpecificInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorSpecificInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorSpecificInfoExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorSpecificInfoExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorSpecificInfoEnterprise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorSpecificInfoEnterprise(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_SourceVendorSpecificInfoSubOption(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_SourceVendorSpecificInfoSubOption(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorSpecificInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorSpecificInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorSpecificInfoExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorSpecificInfoExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorSpecificInfoEnterprise(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorSpecificInfoEnterprise(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DestVendorSpecificInfoSubOption(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DestVendorSpecificInfoSubOption(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_TCPACK(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_TCPACK(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_TCPACKExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_TCPACKExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_IPLengthMin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_IPLengthMin(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_IPLengthMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_IPLengthMax(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_IPLengthExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_IPLengthExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DSCPCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DSCPCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DSCPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_DSCPExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_DSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup *)data;
	dmuci_get_value_by_section_string(p->config_section, "dscp", value);
	return 0;
}

int set_QoSClassification_DSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup *)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->config_section, "dscp", value);
			break;
	}
	return 0;
}

int get_QoSClassification_EthernetPriorityCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthernetPriorityCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_EthernetPriorityExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthernetPriorityExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_EthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_InnerEthernetPriorityCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_InnerEthernetPriorityCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_InnerEthernetPriorityExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_InnerEthernetPriorityExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_InnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_InnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_EthernetDEICheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthernetDEICheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_EthernetDEIExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_EthernetDEIExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_VLANIDCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_VLANIDCheck(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_VLANIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_VLANIDExclude(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_OutOfBandInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_OutOfBandInfo(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_TrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_TrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_Policer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_Policer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSClassification_App(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSClassification_App(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSApp_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_ProtocolIdentifier(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_ProtocolIdentifier(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultTrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultTrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultPolicer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultPolicer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultDSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultDSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSApp_DefaultInnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSApp_DefaultInnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSFlow_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_Type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_TypeParameters(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_TypeParameters(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_App(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_App(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_ForwardingPolicy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_TrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_TrafficClass(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_Policer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_Policer(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_DSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_DSCPMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_EthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_EthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSFlow_InnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSFlow_InnerEthernetPriorityMark(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_CommittedRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_CommittedRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_CommittedBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_CommittedBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_ExcessBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_ExcessBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_PeakRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_PeakRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_PeakBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_PeakBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_MeterType(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_MeterType(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_PossibleMeterTypes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_ConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_ConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_PartialConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_PartialConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_NonConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSPolicer_NonConformingAction(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSPolicer_TotalCountedPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_TotalCountedBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_ConformingCountedPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_ConformingCountedBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_PartiallyConformingCountedPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_PartiallyConformingCountedBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_NonConformingCountedPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSPolicer_NonConformingCountedBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSQueue_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSQueue_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->dmmap_section, "queuealias", value);
	return 0;
}

int set_QoSQueue_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->dmmap_section, "queuealias", value);
			break;
	}
	return 0;
}

int get_QoSQueue_TrafficClasses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_TrafficClasses(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup *) data;
	struct uci_section *s;
	char *classes= NULL, **classesarr, *classgroup= NULL, *ifaceclassgrp, *targetclass;
	int nbre= 0;
	size_t length;

	uci_foreach_sections("qos", "classgroup", s) {
		dmuci_get_value_by_section_string(s, "classes", &classes);
		classesarr= strsplit(classes, " ", &length);
		if(classes!=NULL && is_array_elt_exist(classesarr, section_name(p->config_section), length)){
			dmasprintf(&classgroup, "%s", section_name(s));
			break;
		}
	}
	if(classgroup == NULL)
		return 0;
	uci_foreach_sections("qos", "interface", s) {
		dmuci_get_value_by_section_string(s, "classgroup", &ifaceclassgrp);
		if(ifaceclassgrp != NULL && strcmp(ifaceclassgrp, classgroup) == 0){
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cPPP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				*value = "";
		}
	}
	return 0;
}

int set_QoSQueue_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_AllInterfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_AllInterfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_HardwareAssisted(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSQueue_BufferLength(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->config_section, "maxsize", value);
	return 0;
}

int get_QoSQueue_Weight(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_Weight(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_Precedence(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_Precedence(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_REDThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_REDThreshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_REDPercentage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_REDPercentage(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_DropAlgorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_DropAlgorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_SchedulerAlgorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_SchedulerAlgorithm(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueue_ShapingRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	char *rate= NULL;

	dmuci_get_value_by_section_string(p->config_section, "limitrate", &rate);
	if (rate != NULL && atoi(rate)>=0)
		dmasprintf(value, "%s", rate);
	else
		dmasprintf(value, "%s", "-1");
	return 0;
}

int set_QoSQueue_ShapingRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			if(atoi(value)>=0)
				dmuci_set_value_by_section(p->config_section, "limitrate", value);
			else
				dmuci_set_value_by_section(p->config_section, "limitrate", "");

			break;
	}
	return 0;
}

int get_QoSQueue_ShapingBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueue_ShapingBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueueStats_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueueStats_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueueStats_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSQueueStats_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *qts= (struct queuestats*)data;
	dmuci_get_value_by_section_string(qts->dmsect, "queuestatsalias", value);
	return 0;
}

int set_QoSQueueStats_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct queuestats *qts= (struct queuestats*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(qts->dmsect, "queuestatsalias", value);
			break;
	}
	return 0;
}

int get_QoSQueueStats_Queue(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSQueueStats_Queue(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueueStats_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *qts= (struct queuestats*)data;

	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), qts->dev, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cPPP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), qts->dev, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), qts->dev, value);
	if (*value == NULL)
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWiFi%cRadio%c", dmroot, dm_delim, dm_delim, dm_delim), qts->dev, value);
	if (*value == NULL)
		*value = "";
	return 0;
}

int set_QoSQueueStats_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSQueueStats_OutputPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *queuests= (struct queuestats*)data;
	dmasprintf(value, "%d", queuests->pkt_sent);
	return 0;
}

int get_QoSQueueStats_OutputBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *queuests= (struct queuestats*)data;
	dmasprintf(value, "%d", queuests->bytes_sent);
	return 0;
}

int get_QoSQueueStats_DroppedPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *queuests= (struct queuestats*)data;
	dmasprintf(value, "%d", queuests->pkt_dropped);
	return 0;
}

int get_QoSQueueStats_DroppedBytes(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return 0;
}

int get_QoSQueueStats_QueueOccupancyPackets(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct queuestats *queuests= (struct queuestats*)data;
	dmasprintf(value, "%d", queuests->pkt_requeues);
	return 0;
}

int get_QoSQueueStats_QueueOccupancyPercentage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSShaper_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSShaper_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSShaper_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int get_QoSShaper_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	dmuci_get_value_by_section_string(p->dmmap_section, "shaperalias", value);
	return 0;
}

int set_QoSShaper_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(p->dmmap_section, "shaperalias", value);
			break;
	}
	return 0;
}

int get_QoSShaper_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup *) data;
	struct uci_section *s;
	char *classes= NULL, **classesarr, *classgroup= NULL, *ifaceclassgrp, *targetclass;
	int nbre= 0;
	size_t length;

	uci_foreach_sections("qos", "classgroup", s) {
		dmuci_get_value_by_section_string(s, "classes", &classes);
		classesarr= strsplit(classes, " ", &length);
		if(classes!=NULL && is_array_elt_exist(classesarr, section_name(p->config_section), length)){
			dmasprintf(&classgroup, "%s", section_name(s));
			break;
		}
	}
	if(classgroup == NULL)
		return 0;
	uci_foreach_sections("qos", "interface", s) {
		dmuci_get_value_by_section_string(s, "classgroup", &ifaceclassgrp);
		if(ifaceclassgrp != NULL && strcmp(ifaceclassgrp, classgroup) == 0){
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cPPP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cEthernet%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), section_name(s), value);
			if (*value == NULL)
				*value = "";
		}
	}
	return 0;
}

int set_QoSShaper_Interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_QoSShaper_ShapingRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;
	char *rate= NULL;

	dmuci_get_value_by_section_string(p->config_section, "limitrate", &rate);
	if (rate != NULL && atoi(rate)>=0)
		dmasprintf(value, "%s", rate);
	else
		dmasprintf(value, "%s", "-1");
	return 0;
}

int set_QoSShaper_ShapingRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct dmmap_dup *p= (struct dmmap_dup*)data;

	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			if(atoi(value)>=0)
				dmuci_set_value_by_section(p->config_section, "limitrate", value);
			else
				dmuci_set_value_by_section(p->config_section, "limitrate", "");

			break;
	}
	return 0;
}

int get_QoSShaper_ShapingBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	//TODO
	return 0;
}

int set_QoSShaper_ShapingBurstSize(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

