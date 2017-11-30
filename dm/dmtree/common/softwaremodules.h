/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 */
#ifndef __SOFTWARE_MODULE_H
#define __SOFTWARE_MODULE_H

int update_softwaremodules_url(char *uuid, char *url);
char *get_softwaremodules_uuid(char *url);
char *get_softwaremodules_username(char *uuid);
char *get_softwaremodules_pass(char *uuid);
char *get_softwaremodules_instance(char *uuid);
char *get_softwaremodules_version(char *uuid);
char *add_softwaremodules_deploymentunit(char *uuid, char*url, char *username, char *password, char *name, char *version);
char *get_softwaremodules_name(char *uuid);
char *get_softwaremodules_url(char *uuid);
extern DMLEAF tDeploymentUnitParams[];
extern DMOBJ tSoftwareModulesObj[];

int browsesoftwaremodules_deploymentunitInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int get_deploymentunit_uuid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_resolved(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_url(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_vendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_deploymentunit_execution_env_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif
