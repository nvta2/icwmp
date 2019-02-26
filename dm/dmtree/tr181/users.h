/*
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      Copyright (C) 2019 iopsys Software Solutions AB
 *
 *      Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef _USERS_H
#define _USERS_H

#include "dmcwmp.h"


extern DMOBJ tUsersObj[];
extern DMLEAF tUsersParams[];
extern DMLEAF tUserParams[];


int browseUserInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int add_users_user(char *refparam, struct dmctx *ctx, void *data, char **instance);
int delete_users_user(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int get_users_user_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_remote_accessable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_user_language(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_user_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_user_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_user_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_user_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_user_remote_accessable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_user_language(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
#endif
