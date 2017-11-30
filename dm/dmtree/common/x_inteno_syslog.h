#ifndef __SE_SYSLOG_H
#define __SE_SYSLOG_H

extern DMLEAF tSe_SyslogCfgParam[];

int get_server_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_port_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_remote_log_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_server_ip_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_server_port_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_remote_log_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
