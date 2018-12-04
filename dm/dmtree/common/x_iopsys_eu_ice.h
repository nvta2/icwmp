#ifndef __SE_ICE_H
#define __SE_ICE_H

extern DMLEAF tSe_IceParam[];

int get_ice_cloud_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ice_cloud_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_ice_cloud_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ice_cloud_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
