/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */

#include <ctype.h>
#include <uci.h>
#include <stdio.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "softwaremodules.h"



/*** SoftwareModules. ***/
DMOBJ tSoftwareModulesObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"DeploymentUnit", &DMREAD, NULL, NULL, NULL, browsesoftwaremodules_deploymentunitInst, NULL, NULL, NULL, tDeploymentUnitParams, NULL},
{0}
};

DMLEAF tDeploymentUnitParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"UUID", &DMREAD, DMT_STRING, get_deploymentunit_uuid, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_deploymentunit_name, NULL, NULL, NULL},
{"Resolved", &DMREAD, DMT_BOOL, get_deploymentunit_resolved, NULL, NULL, NULL},
{"URL", &DMREAD, DMT_STRING, get_deploymentunit_url, NULL, NULL, NULL},
{"Vendor", &DMREAD, DMT_STRING, get_deploymentunit_vendor, NULL, NULL, NULL},
{"Version", &DMREAD, DMT_STRING, get_deploymentunit_version, NULL, NULL, NULL},
{"ExecutionEnvRef", &DMREAD, DMT_STRING, get_deploymentunit_execution_env_ref, NULL, NULL, NULL},
{0}
};

int get_deploymentunit_uuid(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "uuid", value);
	
	return 0;
}

int get_deploymentunit_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "name", value);
	return 0;
}

int get_deploymentunit_resolved(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "resolved", value);
	return 0;
}

int get_deploymentunit_url(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "url", value);
	return 0;
}

int get_deploymentunit_vendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "vendor", value);
	return 0;
}

int get_deploymentunit_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "version", value);
	return 0;
}

int get_deploymentunit_execution_env_ref(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *softsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(softsection, "execution_env_ref", value);
	return 0;
}

char *add_softwaremodules_deploymentunit(char *uuid, char*url, char *username, char *password, char *name, char *version)
{
	char *value;
	char *instance;
	struct uci_section *deploymentsection = NULL;
	char duname[16];
	
	
	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "UUID", uuid, deploymentsection) {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, deploymentsection, "URL", url);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "URL", url);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "Name", name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "Version", version);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "username", username);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "password", password);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd,deploymentsection, "Resolved", "1");
		dmuci_get_value_by_section_string(deploymentsection, "duinstance", &instance);
		goto end;
	}
	instance = get_last_instance(DMMAP, "deploymentunit", "duinstance");
	if (!instance)
		sprintf(duname, "du%d", 0);
	else
		sprintf(duname, "du%s", instance);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, NULL, "deploymentunit");
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "UUID", uuid);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "URL", url);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "Name", name);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "Version", version);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "username", username);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "password", password);
	DMUCI_SET_VALUE(icwmpd, "dmmap", duname, "Resolved", "1");
	instance = get_last_instance(DMMAP, "deploymentunit", "duinstance");
	return instance;
end:
	return instance;
}

int update_softwaremodules_url(char *uuid, char *url)
{
	struct uci_section *s = NULL;
	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "UUID", uuid, s) {
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "URL", url);
		return 1;
	}
	return 0;
}

char *get_softwaremodules_uuid(char *url)
{
	char *uuid;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "url", url, s) {
		dmuci_get_value_by_section_string(s, "UUID", &uuid);
	
		return uuid;
	}
	return "";
}

char *get_softwaremodules_url(char *uuid)
{	
	char *url;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "URL", &url);
		return url;
	}
	return "";
}

char *get_softwaremodules_username(char *uuid)
{	
	char *url;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "username", &url);
		return url;
	}
	return "";
}

char *get_softwaremodules_pass(char *uuid)
{	
	char *url;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "password", &url);
		return url;
	}
	return "";
}

char *get_softwaremodules_instance(char *uuid)
{	
	char *url;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "duinstance", &url);
		return url;
	}
	return "";
}

char *get_softwaremodules_name(char *uuid)
{	
	char *name;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "Name", &name);
		return name;
	}
	return "";
}

char *get_softwaremodules_version(char *uuid)
{	
	char *version;
	struct uci_section *s = NULL;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "deploymentunit", "uuid", uuid, s) {
		dmuci_get_value_by_section_string(s, "Version", &version);
		return version;
	}
	return "";
}




int browsesoftwaremodules_deploymentunitInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *idu = NULL, *idu_last = NULL;
	char *permission = "1";
	struct uci_section *s = NULL;

	uci_path_foreach_sections(icwmpd, "dmmap", "deploymentunit", s) {
		idu = handle_update_instance(1, dmctx, &idu_last, update_instance_alias_icwmpd, 3, s, "duinstance", "duinstance_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, idu) == DM_STOP)
			break;
	}
	return 0;
}


