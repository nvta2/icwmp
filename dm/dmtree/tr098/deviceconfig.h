#ifndef __SE_CONFIG_FILE_H
#define __SE_CONFIG_FILE_H

extern DMLEAF tDeviceConfigParam[];
int get_config_file(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_config_file(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
