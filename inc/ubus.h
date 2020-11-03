/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012 Luka Perkov <freecwmp@lukaperkov.net>
 */


#ifndef _FREECWMP_UBUS_H__
#define _FREECWMP_UBUS_H__
#include <stdbool.h>

#define ARRAY_MAX 8

int ubus_init(struct cwmp *cwmp);
void ubus_exit(void);

enum cwmp_ubus_arg_type {
	UBUS_String,
	UBUS_Integer,
	UBUS_Array_Obj,
	UBUS_Array_Str,
	UBUS_Bool
};

struct key_value {
	char *key;
	char *value;
};

union array_membre {
        char* str_value;
        struct key_value param_value;
};

union ubus_value {
	char* str_val;
	int int_val;
	bool bool_val;
	union array_membre array_value[ARRAY_MAX];
};

struct cwmp_ubus_arg {
	const char *key;
	const union ubus_value val;
	enum cwmp_ubus_arg_type type;
};

#define CWMP_UBUS_ARGS (struct cwmp_ubus_arg[])

int cwmp_ubus_call(const char *obj, const char *method, const struct cwmp_ubus_arg u_args[], int u_args_size, json_object **json_ret);

#endif /* UBUS_H_ */
