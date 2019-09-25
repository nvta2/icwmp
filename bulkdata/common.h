/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <sys/param.h>

#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/utils.h>
#include <libubus.h>

#include <libbbfdm/dmubus.h>
#include <libbbfdm/dmcommon.h>
#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmjson.h>

#include "config.h"
#include "log.h"
#include "bulkdata.h"

typedef struct pathnode {
	char *ref_path;
	struct pathnode *next;
} pathnode;

typedef struct resultsnode {
	struct list_head list;
	char *name;
	char *data;
	char *type;
} resultsnode;

struct failed_reports {
	struct list_head list;
	char *freport;
};

#define GLOB_CHAR "[[+*]+"

int bulkdata_dm_ctx_init(struct dmctx *ctx);
int bulkdata_dm_ctx_clean(struct dmctx *ctx);

char *bulkdata_get_value_param(char *path);
void bulkdata_get_value(char *path, struct list_head *list);

void bulkdata_free_data_from_list(struct list_head *dup_list);
void bulkdata_get_value_results(char *path, struct list_head *list);
char *create_request_url(struct profile *profile);
char *get_bulkdata_profile_parameter_name(char *paramref, char *paramname, char *param);
void append_string_to_string(char *strappend, char **target);
void bulkdata_add_failed_report(struct profile *profile, char *freport);
void bulkdata_delete_failed_report(struct failed_reports *report);

struct failed_reports *empty_failed_reports_list(struct profile *profile);
void add_failed_reports_to_report_csv(struct profile *profile, char **report, int isnext);

#endif //__COMMON_H
