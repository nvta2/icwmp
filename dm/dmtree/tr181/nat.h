/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Imen BHIRI <imen.bhiri@pivasoftware.com>
 *
 */
#ifndef __NAT_H
#define __NAT_H

extern DMLEAF tInterfaceSettingParam[];
extern DMOBJ tnatObj[];

int browseInterfaceSettingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_nat_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nat_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
