/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#include <getopt.h>

#include "common.h"

char *commandKey = NULL;

struct option cwmp_long_options[] = {
	{"boot-event", no_argument, NULL, 'b'},
	{"get-rpc-methods", no_argument, NULL, 'g'},
	{"command-input", no_argument, NULL, 'c'},
	{"shell-cli", required_argument, NULL, 'm'},
	{"alias-based-addressing", no_argument, NULL, 'a'},
	{"instance-mode-number", no_argument, NULL, 'N'},
	{"instance-mode-alias", no_argument, NULL, 'A'},
	{"upnp", no_argument, NULL, 'U'},
	{"user-acl", required_argument, NULL, 'u'},
	{"amendment", required_argument, NULL, 'M'},
	{"time-tracking", no_argument, NULL, 't'},
	{"evaluating-test", no_argument, NULL, 'E'},
	{"file", required_argument, NULL, 'f'},
	{"wep", required_argument, NULL, 'w'},
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{NULL, 0, NULL, 0}
};

static void show_help(void)
{
	printf("Usage: icwmpd [OPTIONS]\n");
	printf(" -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event\n");
	printf(" -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS\n");
	printf(" -h, --help                                          Display this help text\n");
	printf(" -v, --version                                       Display the version\n");
}

static void show_version()
{
#ifndef CWMP_REVISION
    fprintf(stdout, "\nVersion: %s\n\n",CWMP_VERSION);
#else
    fprintf(stdout, "\nVersion: %s revision %s\n\n",CWMP_VERSION,CWMP_REVISION);
#endif
}

int global_env_init (int argc, char** argv, struct env *env)
{

	int c, option_index = 0;

	while ((c = getopt_long(argc, argv, "bghv", cwmp_long_options, &option_index)) != -1) {

		switch (c)
		{
		case 'b':
			env->boot = CWMP_START_BOOT;
			break;

		case 'g':
			env->periodic = CWMP_START_PERIODIC;
			break;

		case 'h':
			show_help();
			exit(0);

		case 'v':
			show_version();
			exit(0);
		}
	}
	return CWMP_OK;
}

void add_dm_parameter_tolist(struct list_head *head, char *param_name, char *param_data, char *param_type)
{
	struct cwmp_dm_parameter *dm_parameter;
	struct list_head *ilist;
	int cmp;
	list_for_each (ilist, head) {
		dm_parameter = list_entry(ilist, struct cwmp_dm_parameter, list);
		cmp = strcmp(dm_parameter->name, param_name);
		if (cmp == 0) {
			if (param_data && strcmp(dm_parameter->data, param_data) != 0)
			{
				free(dm_parameter->data);
				dm_parameter->data = strdup(param_data);
			}
			return;
		} else if (cmp > 0) {
			break;
		}
	}
	dm_parameter = calloc(1, sizeof(struct cwmp_dm_parameter));
	_list_add(&dm_parameter->list, ilist->prev, ilist);
	if (param_name) dm_parameter->name = strdup(param_name);
	if (param_data) dm_parameter->data = strdup(param_data);
	if (param_type) dm_parameter->type = strdup(param_type ? param_type : "xsd:string");
}

void delete_dm_parameter_fromlist(struct cwmp_dm_parameter *dm_parameter)
{
	list_del(&dm_parameter->list);
	free(dm_parameter->name);
	free(dm_parameter->data);
	free(dm_parameter->type);
	free(dm_parameter);
}

void free_dm_parameter_all_fromlist(struct list_head *list)
{
	struct cwmp_dm_parameter *dm_parameter;
	while (list->next!=list) {
		dm_parameter = list_entry(list->next, struct cwmp_dm_parameter, list);
		delete_dm_parameter_fromlist(dm_parameter);
	}
}
