#ifndef __TIMES_H
#define __TIMES_H

extern DMLEAF tTimeParams[];

int get_time_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_ntpserver1(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_ntpserver2(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_ntpserver3(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_ntpserver4(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_time_ntpserver5(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_time_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_ntpserver1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_ntpserver2(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_ntpserver3(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_ntpserver4(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_time_ntpserver5(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
