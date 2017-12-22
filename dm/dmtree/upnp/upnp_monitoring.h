#ifndef UPNP_MONITORING_H
#define UPNP_MONITORING_H
#include "dmcwmp.h"
struct upnp_ip_usage_args{
	char *systemName;
	char *status;
	char *totalpacketsent;
	char *totalpacketreceived;
};

int upnp_monitoring_get_IPUsageNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_StorageNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_CurrentTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_CPUUsage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_MemoryUsage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_SystemName(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_TotalPacketsSent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int upnp_monitoring_get_TotalPacketsReceived(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int upnp_BrowseIPUsage(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int upnp_CreateIPUsageInstance(char *refparam, struct dmctx *ctx, void *data, char **instance);
int upnp_DeleteIPUsageInstance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int upnp_BrowseStorage(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int upnp_CreateStorageInstance(char *refparam, struct dmctx *ctx, void *data, char **instance);
int upnp_DeleteStorageInstance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

extern DMLEAF upnpMonitoringOperatingSystemParams[];
extern DMLEAF upnpMonitoringExecutionEnvironmentParams[];
extern DMLEAF upnpMonitoringIPUsageParams[];
extern DMLEAF upnpMonitoringStorageParams[];
extern DMOBJ upnpMonitoringObj[];
extern DMLEAF upnpMonitoringParams[];
#endif
