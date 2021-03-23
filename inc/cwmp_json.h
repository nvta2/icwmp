/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef _JSHN_H__
#define _JSHN_H__
#include <json-c/json.h>
#include "common.h"
struct cwmp_json_arg {
	char *key;
	char *val;
};

#define CWMP_JSON_ARGS (struct cwmp_json_arg[])
int cwmp_handle_download_fault(char *msg);
int cwmp_handle_upload_fault(char *msg);
int cwmp_handle_dustate_change_fault(char *msg);
int cwmp_handle_uninstall_fault(char *msg);
void cwmp_json_fprintf(FILE *fp, int argc, struct cwmp_json_arg cwmp_arg[]);

#define foreach_jsonobj_in_array(param_obj, parameters)             \
	int k, array_length = json_object_array_length(parameters); \
	for (k = 0, param_obj = json_object_array_get_idx(parameters, 0); k < array_length; k++, param_obj = json_object_array_get_idx(parameters, k))

#endif /* _JSHN_H__ */
