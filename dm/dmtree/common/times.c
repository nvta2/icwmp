/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *
 */

#include <uci.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmcwmp.h"
#include "dmubus.h"
#include "times.h"
#include "dmcommon.h"


/*** Time. ***/
DMLEAF tTimeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification, linker*/
{"Enable", &DMWRITE, DMT_BOOL, get_time_enable, set_time_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_time_status, NULL, NULL, NULL},
{"NTPServer1", &DMWRITE, DMT_STRING, get_time_ntpserver1, set_time_ntpserver1, NULL, NULL},
{"NTPServer2", &DMWRITE, DMT_STRING, get_time_ntpserver2, set_time_ntpserver2, NULL, NULL},
{"NTPServer3", &DMWRITE, DMT_STRING, get_time_ntpserver3, set_time_ntpserver3, NULL, NULL},
{"NTPServer4", &DMWRITE, DMT_STRING, get_time_ntpserver4, set_time_ntpserver4, NULL, NULL},
{"NTPServer5", &DMWRITE, DMT_STRING, get_time_ntpserver5, set_time_ntpserver5, NULL, NULL},
{"CurrentLocalTime", &DMREAD, DMT_TIME, get_time_CurrentLocalTime, NULL, NULL, NULL},
{"LocalTimeZone", &DMWRITE, DMT_STRING, get_time_LocalTimeZone, set_time_LocalTimeZone, NULL, NULL},
{CUSTOM_PREFIX"LocalTimeZoneOlson", &DMREAD, DMT_STRING, get_local_time_zone_olson, NULL, NULL, NULL},
{CUSTOM_PREFIX"SourceInterface", &DMWRITE, DMT_STRING, get_time_source_interface, set_time_source_interface, NULL, NULL},
{0}
};

int get_time_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*ntpd";
	
	if (check_file(path))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int set_time_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	int check; 
	pid_t pid;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				DMCMD("/etc/rc.common", 2, "/etc/init.d/ntpd", "enable"); //TODO wait ubus command
				pid = get_pid("ntpd");
				if (pid < 0) {
					DMCMD("/etc/rc.common", 2, "/etc/init.d/ntpd", "start"); //TODO wait ubus command
				}
			}
			else {
				DMCMD("/etc/rc.common", 2, "/etc/init.d/ntpd", "disable"); //TODO wait ubus command
				pid = get_pid("ntpd");
				if (pid > 0) {
					DMCMD("/etc/rc.common", 2, "/etc/init.d/ntpd", "stop"); //TODO may be should be updated with ubus call uci
				}
			}
			return 0;
	}
	return 0;
}

int get_time_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	char *path = "/etc/rc.d/*ntpd";

	if (check_file(path))
		*value = "Synchronized";
	else
		*value = "Disabled";
	return 0;
}

int get_time_CurrentLocalTime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char time_buf[26] = {0};
	struct tm *t_tm;

	*value = "0001-01-01T00:00:00Z";

	time_t t_time = time(NULL);
	t_tm = localtime(&t_time);
	if (t_tm == NULL)
		return 0;

	if(strftime(time_buf, sizeof(time_buf), "%FT%T%z", t_tm) == 0)
		return 0;

	time_buf[25] = time_buf[24];
	time_buf[24] = time_buf[23];
	time_buf[22] = ':';
	time_buf[26] = '\0';

	*value = dmstrdup(time_buf);
	return 0;
}

int get_time_LocalTimeZone(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("system", "@system[0]", "timezone", value);
	return 0;
}

int set_time_LocalTimeZone(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value("system", "@system[0]", "timezone", value);
			break;
	}
	return 0;
}

int get_local_time_zone_olson(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("system", "@system[0]", "zonename", value);
}

//WE CAN WORK WITHOUT FOUND VALUE TO UPDATE
int get_time_ntpserver(char *refparam, struct dmctx *ctx, char **value, int index)
{
	char *pch;
	bool found = 0;
	int element = 0;
	struct uci_list *v;
	struct uci_element *e;
	
	dmuci_get_option_value_list("system","ntp","server", &v);
	if (v) {
		uci_foreach_element(v, e) {
			element++;
			if (element == index) {
				*value = dmstrdup(e->name); // MEM WILL BE FREED IN DMMEMCLEAN
				found = 1; 
				break;
			}
		}
	}
	if (!found) {
		*value = "";
		return 0;
	}
	if (strcmp(*value, "none") == 0) {
		*value = "";
	}
	return 0;
}

int get_time_source_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	char *iface= NULL, *interface= NULL;
	*value= "";
	dmuci_get_option_value_string("system", "ntp", "interface", &iface);
	if (*iface == '\0' || strlen(iface)== 0)
		return 0;
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), iface, &interface);
	if (*interface == '\0')
		return 0;
	*value= dmstrdup(interface);
	return 0;
}

int get_time_ntpserver1(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_time_ntpserver(refparam, ctx, value, 1);
}

int get_time_ntpserver2(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_time_ntpserver(refparam, ctx, value, 2);
}

int get_time_ntpserver3(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_time_ntpserver(refparam, ctx, value, 3);
}

int get_time_ntpserver4(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_time_ntpserver(refparam, ctx, value, 4);
}

int get_time_ntpserver5(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	return get_time_ntpserver(refparam, ctx, value, 5);
}

int set_time_source_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *iface= NULL;
	switch (action) {
		case VALUECHECK:
			adm_entry_get_linker_value(ctx, value, &iface);
			if(iface == NULL ||  iface[0] == '\0')
				return FAULT_9007;
			break;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &iface);
			dmuci_set_value("system", "ntp", "interface", iface);
			return 0;
	}
	return 0;
}

int set_time_ntpserver(char *refparam, struct dmctx *ctx, int action, char *value, int index)
{
	char *pch, *path;
	int check;
	struct uci_list *v;
	struct uci_element *e;
	int count = 0;
	int i = 0;
	char *ntp[5] = {0};
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_list("system", "ntp", "server", &v);
			if (v) {
				uci_foreach_element(v, e) {
					if ((count+1) == index) {
						ntp[count] = dmstrdup(value);
					}
					else {
					ntp[count] = dmstrdup(e->name);
				}
					count++;
					if (count > 4)
						break;
				}
			}
			if (index > count) {
				ntp[index-1] = dmstrdup(value);
				count = index;
				}
			for (i = 0; i < 5; i++) {
				if (ntp[i] && (*ntp[i]) != '\0')
					count = i+1;
			}
			dmuci_delete("system", "ntp", "server", NULL);
			for (i = 0; i < count; i++) {
				dmuci_add_list_value("system", "ntp", "server", ntp[i] ? ntp[i] : "");
				dmfree(ntp[i]);
			}
			return 0;
	}
	return 0;
}

int set_time_ntpserver1(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_time_ntpserver(refparam, ctx, action, value, 1);
}

int set_time_ntpserver2(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_time_ntpserver(refparam, ctx, action, value, 2);
}

int set_time_ntpserver3(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_time_ntpserver(refparam, ctx, action, value, 3);
}

int set_time_ntpserver4(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_time_ntpserver(refparam, ctx, action, value, 4);
}

int set_time_ntpserver5(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	return set_time_ntpserver(refparam, ctx, action, value, 5);
}

