#include "dmcwmp.h"
#include "upnp_monitoring.h"
#include "upnp_common.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmjson.h"
#include <time.h>

/**************************************************************************
*
* /UPnP/DM/Monitoring/ datamodel tree
*
***************************************************************************/

/*** /UPnP/DM/Monitoring/ objects ***/
DMOBJ upnpMonitoringObj[] = {
{"OperatingSystem",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, NULL, upnpMonitoringOperatingSystemParams, NULL},
{"ExecutionEnvironment",&DMREAD, NULL, NULL, NULL, NULL, &DMFINFRM, &DMNONE, NULL, upnpMonitoringExecutionEnvironmentParams, NULL},
{"IPUsage",&DMREAD, NULL, NULL, NULL, upnp_BrowseIPUsage, &DMFINFRM, &DMNONE, NULL, upnpMonitoringIPUsageParams, NULL},
{0}
};

/*** /UPnP/DM/Monitoring/ parameters ***/
DMLEAF upnpMonitoringParams[] = {
{"IPUsageNumberOfEntries", &DMREAD, DMT_UNINT, upnp_monitoring_get_IPUsageNumberOfEntries, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/Monitoring/OperatingSystem parameters ***/
DMLEAF upnpMonitoringOperatingSystemParams[] = {
{"CurrentTime", &DMREAD, DMT_TIME, upnp_monitoring_get_CurrentTime, NULL, &DMFINFRM, NULL},
{"CPUUsage", &DMREAD, DMT_UNINT, upnp_monitoring_get_CPUUsage, NULL, &DMFINFRM, NULL},
{"MemoryUsage", &DMREAD, DMT_UNINT, upnp_monitoring_get_MemoryUsage, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/Monitoring/ExecutionEnvironement parameters ***/
 DMLEAF upnpMonitoringExecutionEnvironmentParams[] = {
{"CPUUsage", &DMREAD, DMT_UNINT, upnp_monitoring_get_CPUUsage, NULL, &DMFINFRM, NULL},
{"MemoryUsage", &DMREAD, DMT_UNINT, upnp_monitoring_get_MemoryUsage, NULL, &DMFINFRM, NULL},
{0}
};

/*** /UPnP/DM/Monitoring/Usage parameters ***/
DMLEAF upnpMonitoringIPUsageParams[] = {
{"SystemName", &DMREAD, DMT_STRING, upnp_monitoring_get_SystemName, NULL, &DMFINFRM, NULL},
{"Status", &DMREAD, DMT_STRING, upnp_monitoring_get_Status, NULL, &DMFINFRM, NULL},
{"TotalPacketsSent", &DMREAD, DMT_UNINT, upnp_monitoring_get_TotalPacketsSent, NULL, &DMFINFRM, NULL},
{"TotalPacketsReceived", &DMREAD, DMT_UNINT, upnp_monitoring_get_TotalPacketsReceived, NULL, &DMFINFRM, NULL},
{0}
};

/**************************************************************************
*
* /UPnP/DM/Monitoring/* parameters functions
*
***************************************************************************/
int upnp_monitoring_get_IPUsageNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	int n = upnp_get_IPInterfaceNumberOfEntries();
	dmasprintf(value, "%d", n);
	return 0;
}


int upnp_monitoring_get_CurrentTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
    time_t rawtime;
    struct tm * timeinfo = NULL;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    dmasprintf(value, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	return 0;
}

int upnp_monitoring_get_CPUUsage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	return 0;
}

int upnp_monitoring_get_MemoryUsage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res = NULL;
	char *totalmemory = NULL, *freememory = NULL;
	int total, free;

	dmubus_call("system", "info", UBUS_ARGS{{}}, 0, &res);
	if(res == NULL) {
		*value = NULL;
		return 0;
	}
	DM_ASSERT(res, freememory = "");
	DM_ASSERT(res, totalmemory = "");
	totalmemory = dmjson_get_value(res, 2, "memory", "total");
	freememory = dmjson_get_value(res, 2, "memory", "freememory");
	total = atoi(totalmemory);
	free = atoi(freememory);
	dmasprintf(value, "%d", total - free);
	return 0;
}

int upnp_monitoring_get_SystemName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_ip_usage_args *ipusage_args = (struct upnp_ip_usage_args *)data;
	if(ipusage_args->systemName != NULL && strlen(ipusage_args->systemName)>0)
	dmasprintf(value, "%s", ipusage_args->systemName);
	return 0;
}

int upnp_monitoring_get_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_ip_usage_args *ipusage_args = (struct upnp_ip_usage_args *)data;
	if(ipusage_args->status != NULL && strlen(ipusage_args->status)>0)
	dmasprintf(value, "%s", ipusage_args->status);
	return 0;
}

int upnp_monitoring_get_TotalPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_ip_usage_args *ipusage_args = (struct upnp_ip_usage_args *)data;
	if(ipusage_args->totalpacketsent != NULL && strlen(ipusage_args->totalpacketsent)>0)
	dmasprintf(value, "%s", ipusage_args->totalpacketsent);
	return 0;
}

int upnp_monitoring_get_TotalPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct upnp_ip_usage_args *ipusage_args = (struct upnp_ip_usage_args *)data;
	if(ipusage_args->totalpacketreceived != NULL && strlen(ipusage_args->totalpacketreceived)>0)
	dmasprintf(value, "%s", ipusage_args->totalpacketreceived);
	return 0;
}

int upnp_BrowseIPUsage(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance){
	struct uci_section *net_sec = NULL;
	struct upnp_ip_usage_args curr_ip_usage_args = {0};
	char *iface_int = NULL, *iface_int_last = NULL, *totalPktSnt = NULL, *totalPktRcvd= NULL, *status = NULL;
	int i = 0;
	uci_foreach_sections("network", "interface", net_sec) {
		dmasprintf(&curr_ip_usage_args.systemName, "%s", section_name(net_sec));
		upnp_getInterfaceStatus(section_name(net_sec), &status);
		if(status !=NULL) dmasprintf(&curr_ip_usage_args.status, "%s", status); else dmasprintf(&curr_ip_usage_args.status, "");
		upnp_getInterfaceTotalPacketSent(section_name(net_sec), &totalPktSnt);
		if(totalPktSnt != NULL) dmasprintf(&curr_ip_usage_args.totalpacketsent, "%s", totalPktSnt); else dmasprintf(&curr_ip_usage_args.totalpacketsent, "");
		upnp_getInterfaceTotalPacketReceived(section_name(net_sec), &totalPktRcvd);
		if(totalPktRcvd !=NULL) dmasprintf(&curr_ip_usage_args.totalpacketreceived, "%s", totalPktRcvd); else dmasprintf(&curr_ip_usage_args.totalpacketreceived, "");
		iface_int = handle_update_instance(1, dmctx, &iface_int_last, update_instance_alias, 3, net_sec, "upnp_ip_usage_instance", "upnp_ip_usage_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ip_usage_args, iface_int) == DM_STOP) break;
		i++;
	}
	return 0;
}
