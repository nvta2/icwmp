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
#include <stdio.h>

struct cwmp_json_arg {
	char *key;
	char *val;
};

#define CWMP_JSON_ARGS (struct cwmp_json_arg[])
int cwmp_handle_downloadFault(char *msg);
int cwmp_handle_uploadFault(char *msg);
int cwmp_handle_dustate_changeFault(char *msg);
int cwmp_handle_uninstallFault(char *msg);
int cwmp_handle_getParamValues(char *msg);
int cwmp_handle_setParamValues(char *msg);
int cwmp_handle_getParamNames(char *msg);
int cwmp_handle_getParamAttributes(char *msg);
int cwmp_handle_setParamAttributes(char *msg);
int cwmp_handle_addObject(char *msg);
int cwmp_handle_delObject(char *msg);
void cwmp_json_fprintf(FILE *fp, int argc, struct cwmp_json_arg cwmp_arg[]);
void cwmp_json_get_string(json_object *obj, char* key, char** value);
void cwmp_json_obj_init(char *str, json_object **obj);
void cwmp_json_obj_clean(json_object **obj);

#define foreach_jsonobj_in_array(param_obj, parameters) \
	int k, array_length = json_object_array_length(parameters); \
	for (k=0, param_obj = json_object_array_get_idx(parameters, 0); k< array_length; k++, param_obj = json_object_array_get_idx(parameters, k)) \

#endif /* _JSHN_H__ */
