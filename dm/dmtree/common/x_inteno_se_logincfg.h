#ifndef __SE_LOGINCFG_H
#define __SE_LOGINCFG_H

extern DMLEAF tSe_LoginCfgParam[];

int set_x_bcm_admin_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_bcm_support_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_bcm_user_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_x_bcm_root_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
