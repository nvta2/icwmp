#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <libubox/blobmsg_json.h>
#include "dmjson.h"
#include "dmmem.h"

static json_object *dmjson_jobj = NULL;

void dm_add_json_obj(json_object *json_obj_out, char *object, char *string)
{
	json_object *json_obj_tmp = json_object_new_string(string);
	json_object_object_add(json_obj_out, object, json_obj_tmp);
}

static void inline __dmjson_fprintf(FILE *fp, int argc, struct dmjson_arg dmarg[])
{
	int i;
	char *arg;
	json_object *json_obj_out = json_object_new_object();
	if (json_obj_out == NULL)
		return;

	if (argc) {
		for (i = 0; i < argc; i++) {
			dm_add_json_obj(json_obj_out, dmarg[i].key, dmarg[i].val);
		}
		arg = (char *)json_object_to_json_string(json_obj_out);
		fprintf(fp, "%s\n", arg);
	}

	json_object_put(json_obj_out);
}

void dmjson_fprintf(FILE *fp, int argc, struct dmjson_arg dmarg[])
{
	__dmjson_fprintf(fp, argc, dmarg);
}

void dmjson_parse_init(char *msg)
{
	if (dmjson_jobj) {
		json_object_put(dmjson_jobj);
		dmjson_jobj = NULL;
	}
	dmjson_jobj = json_tokener_parse(msg);
}

void dmjson_parse_fini(void)
{
	if (dmjson_jobj) {
		json_object_put(dmjson_jobj);
		dmjson_jobj = NULL;
	}
}

static char *dmjson_print_value(json_object *jobj)
{
	enum json_type type;
	char *ret = "";

	if (!jobj)
		return ret;

	type = json_object_get_type(jobj);
	switch (type) {
	case json_type_boolean:
	case json_type_double:
	case json_type_int:
	case json_type_string:
		ret = (char *)json_object_get_string(jobj);
		break;
	}
	return ret;
}

char *____dmjson_get_value_in_obj(json_object *mainjobj, char *argv[])
{
	json_object *jobj = NULL;
	char *value = "";

	jobj = dmjson_select_obj(mainjobj, argv);
	value = dmjson_print_value(jobj);

	return value;
}

char *__dmjson_get_value_in_obj(json_object *mainjobj, int argc, ...)
{
	va_list arg;
	char *argv[64], *v;
	int i;

	va_start(arg, argc);
	for (i = 0; i < argc; i++)
	{
		argv[i] = va_arg(arg, char *);
	}
	argv[argc] = NULL;
	va_end(arg);
	v = ____dmjson_get_value_in_obj(mainjobj, argv);
	return v;
}

json_object *__dmjson_get_obj(json_object *mainjobj, int argc, ...)
{
	va_list arg;
	char *argv[64];
	int i;

	va_start(arg, argc);
	for (i = 0; i < argc; i++)
	{
		argv[i] = va_arg(arg, char *);
	}
	argv[argc] = NULL;
	va_end(arg);
	return dmjson_select_obj(mainjobj, argv);
	//return v;
}


json_object *dmjson_select_obj(json_object * jobj, char *argv[])
{
	int i;
	for (i = 0; argv[i]; i++) {
		if (jobj == NULL)
			return NULL;
		json_object_object_get_ex(jobj, argv[i], &jobj);
	}
	return jobj;
}


json_object *____dmjson_select_obj_in_array_idx(json_object *mainjobj, json_object **arrobj, int index, char *argv[])
{
	json_object *jobj = NULL;
	int i;

	if (arrobj == NULL || *arrobj == NULL) {
		jobj = dmjson_select_obj(mainjobj, argv);
		if (arrobj)
			*arrobj = jobj;
		if (jobj && json_object_get_type(jobj) == json_type_array) {
			jobj = json_object_array_get_idx(jobj, index);
			return jobj;
		}
		else {
			return NULL;
		}
	}
	else {
		jobj = json_object_array_get_idx(*arrobj, index);
		return jobj;
	}

	return NULL;
}

json_object *__dmjson_select_obj_in_array_idx(json_object *mainjobj, json_object **arrobj, int index, int argc, ...)
{
	va_list arg;
	json_object *jobj;
	char *argv[64];
	int i;

	if (mainjobj == NULL)
		return NULL;

	va_start(arg, argc);
	for (i = 0; i < argc; i++)
	{
		argv[i] = va_arg(arg, char *);
	}
	argv[argc] = NULL;
	va_end(arg);
	jobj = ____dmjson_select_obj_in_array_idx(mainjobj, arrobj, index, argv);
	return jobj;
}

char *____dmjson_get_value_in_array_idx(json_object *mainjobj, json_object **arrobj, int index, char *argv[])
{
	json_object *jobj = NULL;
	char *value = NULL;
	int i;

	if (arrobj == NULL || *arrobj == NULL) {
		jobj = dmjson_select_obj(mainjobj, argv);
		if (arrobj)
			*arrobj = jobj;
		if (jobj && json_object_get_type(jobj) == json_type_array) {
			jobj = json_object_array_get_idx(jobj, index);
			if (jobj == NULL)
				return NULL;
			value = dmjson_print_value(jobj);
			return value;
		}
	}
	else {
		jobj = json_object_array_get_idx(*arrobj, index);
		if (jobj == NULL)
			return NULL;
		value = dmjson_print_value(jobj);
		return value;
	}

	return value;
}

char *__dmjson_get_value_in_array_idx(json_object *mainjobj, json_object **arrobj, char *defret, int index, int argc, ...)
{
	va_list arg;
	char *argv[64], *v;
	int i;

	if (mainjobj == NULL)
		return defret;

	va_start(arg, argc);
	for (i = 0; i < argc; i++)
	{
		argv[i] = va_arg(arg, char *);
	}
	argv[argc] = NULL;
	va_end(arg);
	v = ____dmjson_get_value_in_array_idx(mainjobj, arrobj, index, argv);
	return (v ? v : defret) ;
}


char *____dmjson_get_value_array_all(json_object *mainjobj, char *delim, char *argv[])
{
	json_object *arrobj;
	char *v, *ret = "";
	int i, dlen;

	delim = (delim) ? delim : ",";
	dlen = strlen(delim);

	for (i = 0, arrobj = NULL, v = ____dmjson_get_value_in_array_idx(mainjobj, &arrobj, i, argv);
		v;
		v = ____dmjson_get_value_in_array_idx(mainjobj, &arrobj, ++i, argv)) {

		if (*ret == '\0') {
			ret = dmstrdup(v);
		}
		else if (*v) {
			ret = dmrealloc(ret, strlen(ret) + dlen + strlen(v) + 1);
			strcat(ret, delim);
			strcat(ret, v);
		}
	}
	return ret;
}

char *__dmjson_get_value_array_all(json_object *mainjobj, char *delim, int argc, ...)
{
	char *argv[64], *ret;
	va_list arg;
	int i;

	va_start(arg, argc);
	for (i = 0; i < argc; i++)
	{
		argv[i] = va_arg(arg, char *);
	}
	argv[argc] = NULL;
	va_end(arg);
	ret = ____dmjson_get_value_array_all(mainjobj, delim, argv);
	return ret;
}

void dmjson_get_var(char *jkey, char **jval)
{
	enum json_type type;
	*jval = "";

	if (dmjson_jobj == NULL)
		return;

	json_object_object_foreach(dmjson_jobj, key, val) {
		if (strcmp(jkey, key) == 0) {
			*jval = dmjson_print_value(val);
			return;
		}
	}
}
