#ifndef __LAYER3_FORWORDING_H
#define __LAYER3_FORWORDING_H

#define ROUTE_FILE "/proc/net/route"

enum enum_route_type {
	ROUTE_STATIC,
	ROUTE_DYNAMIC,
	ROUTE_DISABLED
};

extern struct dm_permession_s DMForwarding_perm;

extern DMLEAF tForwardingInstParam[];
extern DMOBJ tLayer3ForwardingObj[];
extern DMLEAF tLayer3ForwardingParam[];

int browseForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
char *get_forwording_perm(char *refparam, struct dmctx *dmctx, void *data, char *instance);

int get_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_src_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_src_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_layer3_mtu(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_mtu(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_layer3_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
