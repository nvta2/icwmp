/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef __INTERFACESTACK_H
#define __INTERFACESTACK_H

extern DMLEAF tInterfaceStackParams[];

struct interfacestack_data {
	char *lowerlayer;
	char *higherlayer;
	char *loweralias;
	char *higheralias;
};

int browseInterfaceStackInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_Device_InterfaceStackNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int get_InterfaceStack_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_InterfaceStack_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_InterfaceStack_HigherLayer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_InterfaceStack_LowerLayer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_InterfaceStack_HigherAlias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_InterfaceStack_LowerAlias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif //__INTERFACESTACK_H
