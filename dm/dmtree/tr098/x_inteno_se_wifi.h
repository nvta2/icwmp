#ifndef __SE_WIFI_H
#define __SE_WIFI_H

extern DMLEAF tsewifiradioParam[];
extern DMLEAF tsewifiParam[];
extern DMOBJ tsewifiObj[];

int get_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_bandsteering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_bandsteering_rssi_threshold(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_bandsteering_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_bandsteering_policy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_bandsteering_bw_util(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_bandsteering_bw_util(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

int browsesewifiradioInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_frequency(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wifi_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_radio_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wifi_maxassoc(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wifi_dfsenable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
