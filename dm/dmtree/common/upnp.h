#ifndef __UPNP_H
#define __UPNP_H

extern DMLEAF tUPnPDeviceParams[];
extern DMOBJ tUPnPObj[];

int get_upnp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_upnp_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_upnp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
