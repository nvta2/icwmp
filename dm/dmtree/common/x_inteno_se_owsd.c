/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */


#include <uci.h>
#include <ctype.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmentry.h"
#include "x_inteno_se_owsd.h"

/*** DMROOT.X_INTENO_SE_Owsd. ***/
DMLEAF XIntenoSeOwsdParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"Socket", &DMWRITE, DMT_STRING, get_x_inteno_owsd_global_sock, set_x_inteno_owsd_global_sock, NULL, NULL},
{"Redirect", &DMWRITE, DMT_STRING, get_x_inteno_owsd_global_redirect, set_x_inteno_owsd_global_redirect, NULL, NULL},
{0}
};

DMOBJ XIntenoSeOwsdObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, nextobj, leaf*/
{CUSTOM_PREFIX"UbusProxy", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, UbusProxyParams, NULL},
{CUSTOM_PREFIX"ListenObj", &DMWRITE, add_owsd_listen, delete_owsd_listen_instance, NULL, browseXIntenoOwsdListenObj, NULL, NULL, NULL, X_INTENO_SE_ListenObjParams, NULL},
{0}
};

DMLEAF UbusProxyParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"Enable", &DMWRITE, DMT_BOOL, get_x_inteno_owsd_ubus_proxy_enable, set_x_inteno_owsd_ubus_proxy_enable, NULL, NULL},
{"UbusProxyCert", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_proxy_cert, set_x_inteno_owsd_ubus_proxy_cert, NULL, NULL},
{"UbusProxyKey", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_proxy_key, set_x_inteno_owsd_ubus_proxy_key, NULL, NULL},
{"UbusProxyCa", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_proxy_ca, set_x_inteno_owsd_ubus_proxy_ca, NULL, NULL},
{0}
};

DMLEAF X_INTENO_SE_ListenObjParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"Alias", &DMWRITE, DMT_STRING, get_x_inteno_owsd_listenobj_alias, set_x_inteno_owsd_listenobj_alias, NULL, NULL},
{"Port", &DMWRITE, DMT_UNINT, get_x_inteno_owsd_listenobj_port, set_x_inteno_owsd_listenobj_port, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_x_inteno_owsd_listenobj_interface, set_x_inteno_owsd_listenobj_interface, NULL, NULL},
{"Ipv6", &DMWRITE, DMT_BOOL, get_x_inteno_owsd_listenobj_ipv6_enable, set_x_inteno_owsd_listenobj_ipv6_enable, NULL, NULL},
{"Whitelist_interface", &DMWRITE, DMT_BOOL, get_x_inteno_owsd_listenobj_whitelist_interface, set_x_inteno_owsd_listenobj_whitelist_interface, NULL, NULL},
{"Whitelist_dhcp", &DMWRITE, DMT_BOOL, get_x_inteno_owsd_listenobj_whitelist_dhcp, set_x_inteno_owsd_listenobj_whitelist_dhcp, NULL, NULL},
{"Origin", &DMWRITE, DMT_STRING, get_x_inteno_owsd_listenobj_origin, set_x_inteno_owsd_listenobj_origin, NULL, NULL},
{"UbusCert", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_cert, set_x_inteno_owsd_ubus_cert, NULL, NULL},
{"UbusKey", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_key, set_x_inteno_owsd_ubus_key, NULL, NULL},
{"UbusCa", &DMWRITE, DMT_STRING, get_x_inteno_owsd_ubus_ca, set_x_inteno_owsd_ubus_ca, NULL, NULL},
{0}
};


int browseXIntenoOwsdListenObj(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iowsd_listen = NULL, *iowsd_listen_last = NULL;
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("owsd", "owsd-listen", "dmmap_owsd", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		iowsd_listen =  handle_update_instance(1, dmctx, &iowsd_listen_last, update_instance_alias, 3, p->dmmap_section, "olisteninstance", "olistenalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, iowsd_listen) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

/************************************************************************************* 
**** function related to owsd_origin ****
**************************************************************************************/
int get_x_inteno_owsd_global_sock(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("owsd", "global", "sock", value);
	return 0;
}

int set_x_inteno_owsd_global_sock(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "global", "sock", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_global_redirect(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("owsd", "global", "redirect", value);
	return 0;
}

int set_x_inteno_owsd_global_redirect(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "global", "redirect", value);
			return 0;
	}
	return 0;
}

/*************************************************************************************
**** function related to owsd_listenobj ****
**************************************************************************************/

int get_x_inteno_owsd_listenobj_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	dmuci_get_value_by_section_string(owsd_listensection, "port", value);
	if ((*value)[0] == '\0') {
		*value = "";
	}		
	return 0;
}

int set_x_inteno_owsd_listenobj_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(owsd_listensection, "port", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_listenobj_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *iface, *linker;
	char *uci_datamodel = NULL;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(owsd_listensection, "interface", &iface);
	dmuci_get_option_value_string("cwmp", "cpe", "datamodel", &uci_datamodel);
	if( (strcasecmp(uci_datamodel, "tr181") == 0) || (strcasecmp(uci_datamodel, "tr-181") == 0))
	{
		if (iface[0] != '\0') {
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), iface, value); // MEM WILL BE FREED IN DMMEMCLEAN
			if (*value == NULL)
				*value = "";
		}
	}
	else
	{
		if (iface[0] != '\0') {
			dmastrcat(&linker, "linker_interface:", iface);
			adm_entry_get_linker_param(ctx, dm_print_path("%s%cWANDevice%c", dmroot, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
			if (*value == NULL) {
				adm_entry_get_linker_param(ctx, dm_print_path("%s%cLANDevice%c", dmroot, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
				if (*value == NULL)
					*value = "";
			}
			dmfree(linker);
		}
	}
	return 0;
}

int set_x_inteno_owsd_listenobj_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int check;
	char *linker, *iface;
	char *uci_datamodel = NULL;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_get_option_value_string("cwmp", "cpe", "datamodel", &uci_datamodel);
				if( (strcasecmp(uci_datamodel, "tr098") == 0) || (strcasecmp(uci_datamodel, "tr-098") == 0) || (strcasecmp(uci_datamodel, "tr98") == 0) || (strcasecmp(uci_datamodel, "tr-98") == 0) ) {
					iface = linker + sizeof("linker_interface:") - 1;
				}
				else
					iface = linker;
				dmuci_set_value_by_section(owsd_listensection, "interface", iface);
				dmfree(linker);
			}
		return 0;
	}
	return 0;
}

int get_x_inteno_owsd_listenobj_ipv6_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(owsd_listensection, "ipv6", value);
	if ((*value)[0] != '\0' && (*value)[0] == 'o' && (*value)[1] == 'n' ) {
		*value = "1";
	}
	else
		*value = "0";
	return 0;
}

int set_x_inteno_owsd_listenobj_ipv6_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(owsd_listensection, "ipv6", "on");
			else
				dmuci_set_value_by_section(owsd_listensection, "ipv6", "off");
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_listenobj_whitelist_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	dmuci_get_value_by_section_string(owsd_listensection, "whitelist_interface_as_origin", value);
	if ((*value)[0] == '\0' ) {
		*value = "0";
	}
	return 0;
}

int set_x_inteno_owsd_listenobj_whitelist_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(owsd_listensection, "whitelist_interface_as_origin", "1");
			else
				dmuci_set_value_by_section(owsd_listensection, "whitelist_interface_as_origin", "0");
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_listenobj_whitelist_dhcp(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	dmuci_get_value_by_section_string(owsd_listensection, "whitelist_dhcp_domains", value);
	if ((*value)[0] == '\0') {
		*value = "0";
	}
	return 0;
}

int set_x_inteno_owsd_listenobj_whitelist_dhcp(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(owsd_listensection, "whitelist_dhcp_domains", "1");
			else
				dmuci_set_value_by_section(owsd_listensection, "whitelist_dhcp_domains", "0");
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_listenobj_origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_list *val;
	struct uci_section *owsd_listensection = (struct uci_section *)data;

	dmuci_get_value_by_section_list(owsd_listensection, "origin", &val);
	if (val)
		*value = dmuci_list_to_string(val, " ");
	else
		*value = "";
	return 0;
}

int set_x_inteno_owsd_listenobj_origin(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *spch;
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_delete_by_section(owsd_listensection, "origin", NULL);
			value = dmstrdup(value);
			pch = strtok_r(value, " ", &spch);
			while (pch != NULL) {
				dmuci_add_list_value_by_section(owsd_listensection, "origin", pch);
				pch = strtok_r(NULL, " ", &spch);
			}
			dmfree(value);
			return 0;
	}
	return 0;
}
////////////////////////SET AND GET ALIAS/////////////////////////////////

int get_x_inteno_owsd_listenobj_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_owsd", "owsd-listen", section_name(owsd_listensection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "olistenalias", value);
	return 0;
}

int set_x_inteno_owsd_listenobj_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_owsd", "owsd-listen", section_name(owsd_listensection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "olistenalias", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_proxy_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("owsd","ubusproxy","enable", value);
	return 0;
}

int set_x_inteno_owsd_ubus_proxy_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "ubusproxy", "enable", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_proxy_cert(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("owsd","ubusproxy","peer_cert", value);
	return 0;
}

int set_x_inteno_owsd_ubus_proxy_cert(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "ubusproxy", "peer_cert", value);
			return 0;
	}
}

int get_x_inteno_owsd_ubus_proxy_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("owsd","ubusproxy","peer_key", value);
	return 0;
}

int set_x_inteno_owsd_ubus_proxy_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	char *net_cur_mode= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "ubusproxy", "peer_key", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_proxy_ca(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	dmuci_get_option_value_string("owsd","ubusproxy","peer_ca", value);
	return 0;
}

int set_x_inteno_owsd_ubus_proxy_ca(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	char *net_cur_mode= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value("owsd", "ubusproxy", "peer_ca", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_cert(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;
	dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);

	if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
		dmuci_get_value_by_section_string(owsd_listensection, "cert", value);
	}
	return 0;
}

int set_x_inteno_owsd_ubus_cert(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);
			if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
				if(strcmp(section_name(owsd_listensection), "wan_https")== 0)
					dmuci_set_value_by_section(owsd_listensection, "cert", *value);
			}
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;

	dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);
	if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
		dmuci_get_value_by_section_string(owsd_listensection, "key", value);
	}
	return 0;
}

int set_x_inteno_owsd_ubus_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);
			if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
				if(strcmp(section_name(owsd_listensection), "wan_https")== 0)
					dmuci_set_value_by_section(owsd_listensection, "key", *value);
			}
			return 0;
	}
	return 0;
}

int get_x_inteno_owsd_ubus_ca(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;

	dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);
	if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
		dmuci_get_value_by_section_string(owsd_listensection, "ca", value);
	}
	return 0;
}

int set_x_inteno_owsd_ubus_ca(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action){
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	char *net_cur_mode= NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_option_value_string("netmode", "setup", "curmode", &net_cur_mode);
			if(strstr(net_cur_mode, "repeater") || strstr(net_cur_mode, "extender")){
				if(strcmp(section_name(owsd_listensection), "wan_https")== 0)
					dmuci_set_value_by_section(owsd_listensection, "ca", *value);
			}
			return 0;
	}
	return 0;
}

/***** ADD DEL OBJ *******/
int add_owsd_listen(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *listen_sec = NULL, *dmmap_sec= NULL;

	check_create_dmmap_package("dmmap_owsd");
	instance = get_last_instance_icwmpd("dmmap_owsd", "owsd-listen", "olisteninstance");

	dmuci_add_section("owsd", "owsd-listen", &listen_sec, &value);
	dmuci_set_value_by_section(listen_sec, "ipv6", "on");
	dmuci_set_value_by_section(listen_sec, "whitelist_interface_as_origin", "1");
	dmuci_add_list_value_by_section(listen_sec, "origin", "*");

	dmuci_add_section_icwmpd("dmmap_owsd", "owsd-listen", &dmmap_sec, &v);
	dmuci_set_value_by_section(dmmap_sec, "section_name", section_name(listen_sec));
	*instancepara = update_instance_icwmpd(dmmap_sec, instance, "olisteninstance");

	return 0;
}

int delete_owsd_listen_instance(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *owsd_listensection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	int found = 0;
	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_owsd", "owsd-listen", section_name(owsd_listensection), &dmmap_section);
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(owsd_listensection, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("owsd", "owsd-listen", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_owsd", "listen", section_name(s), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_owsd", "listen", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}
