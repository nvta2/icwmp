#ifndef __ROUTING_H
#define __ROUTING_H


extern struct dm_permession_s DMRouting;

extern DMLEAF tRouterInstParam[];
extern DMLEAF tIPv4ForwardingParam[];
extern DMOBJ tRoutingObj[];
extern DMLEAF tRoutingParam[];
extern DMOBJ tRouterObj[];

int browseRouterInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseIPv4ForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_router_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_src_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_src_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_router_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_router_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
