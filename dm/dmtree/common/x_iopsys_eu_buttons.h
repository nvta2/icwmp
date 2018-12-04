#ifndef __SE_BUTTONS_H
#define __SE_BUTTONS_H

extern DMLEAF X_IOPSYS_EU_ButtonParams[];
int browseXIopsysEuButton(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int get_x_iopsys_eu_button_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_button_hotplug(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_button_hotplug_long(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_x_iopsys_eu_button_minpress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_x_iopsys_eu_button_minpress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_x_iopsys_eu_button_longpress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_x_iopsys_eu_button_longpress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_x_iopsys_eu_button_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_x_iopsys_eu_button_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_x_iopsys_eu_button_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_x_iopsys_eu_button_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
