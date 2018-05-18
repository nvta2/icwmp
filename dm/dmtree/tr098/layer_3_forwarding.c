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
#include <arpa/inet.h>
#include "dmcwmp.h"
#include "dmentry.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "dmjson.h"
#include "layer_3_forwarding.h"


/*** Layer3Forwarding. ***/
DMOBJ tLayer3ForwardingObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Forwarding", &DMREAD, NULL, NULL, NULL, browseForwardingInst, NULL, NULL, NULL, tForwardingInstParam, NULL},
{0}
};

DMLEAF tLayer3ForwardingParam[] = {
{"DefaultConnectionService", &DMWRITE, DMT_STRING, get_layer3_def_conn_serv, set_layer3_def_conn_serv, NULL, NULL},
{"ForwardNumberOfEntries", &DMREAD, DMT_UNINT, get_layer3_nbr_entry, NULL, NULL, NULL},
{0}
};

/*** Layer3Forwarding.Forwarding.{i}. ***/
DMLEAF tForwardingInstParam[] = {
{"Enable", &DMForwarding_perm, DMT_BOOL, get_layer3_enable, set_layer3_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_layer3_status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_layer3_alias, set_layer3_alias, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_layer3_type, NULL, NULL, NULL},
{"DestIPAddress", &DMForwarding_perm, DMT_STRING, get_layer3_destip, set_layer3_destip, NULL, NULL},
{"DestSubnetMask", &DMForwarding_perm, DMT_STRING, get_layer3_destmask,  set_layer3_destmask, NULL, NULL},
{"SourceIPAddress", &DMREAD, DMT_STRING, get_layer3_src_address, NULL, NULL, NULL},
{"SourceSubnetMask", &DMREAD, DMT_STRING, get_layer3_src_mask, NULL, NULL, NULL},
{"GatewayIPAddress", &DMForwarding_perm, DMT_STRING, get_layer3_gatewayip, set_layer3_gatewayip, NULL, NULL},
{"Interface", &DMForwarding_perm, DMT_STRING, get_layer3_interface_linker_parameter, set_layer3_interface_linker_parameter, NULL, NULL},
{"ForwardingMetric", &DMForwarding_perm, DMT_STRING, get_layer3_metric, set_layer3_metric, NULL, NULL},
{"MTU", &DMForwarding_perm, DMT_STRING, get_layer3_mtu, set_layer3_mtu, NULL, NULL},
{0}
};


inline int init_args_rentry(struct routingfwdargs *args, struct uci_section *s, char *permission, struct proc_routing *proute, int type)
{
	args->permission = permission;
	args->routefwdsection = s;
	args->proute = proute;
	args->type = type;
	return 0;
}

/************************************************************************************* 
**** function related to get_object_layer3 ****
**************************************************************************************/
static bool is_proute_static(struct proc_routing *proute)
{
	char *mask;
	struct uci_section *s;
	uci_foreach_option_eq("network", "route", "target", proute->destination, s) {
		dmuci_get_value_by_section_string(s, "netmask", &mask);
		if (mask[0] == '\0' || strcmp(proute->mask, mask) == 0)
			return true;
	}
	uci_foreach_option_eq("network", "route_disabled", "target", proute->destination, s) {
		dmuci_get_value_by_section_string(s, "netmask", &mask);
		if (mask[0] == '\0' || strcmp(proute->mask, mask) == 0)
			return true;
	}
	return false;
}

static bool is_cfg_route_active(struct uci_section *s)
{
	FILE *fp;
	char line[MAX_PROC_ROUTING];
	struct proc_routing proute;
	char *dest, *mask;

	dmuci_get_value_by_section_string(s, "target", &dest);
	dmuci_get_value_by_section_string(s, "netmask", &mask);

	fp = fopen(ROUTE_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTING, fp);
		while (fgets(line, MAX_PROC_ROUTING, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			parse_proc_route_line(line, &proute);
			if (strcmp(dest, proute.destination) == 0 &&
				(mask[0] == '\0' || strcmp(mask, proute.mask) == 0)) {
				fclose(fp) ;
				return true;
			}
		}
		fclose(fp) ;
	}
	return false;
}

static int get_forwarding_last_inst()
{
	char *rinst = NULL, *drinst = NULL, *dsinst = NULL, *tmp;
	int r = 0, dr = 0, ds = 0, max;
	struct uci_section *s;
	int cnt = 0;
	FILE *fp;
	char line[MAX_PROC_ROUTING];
	struct proc_routing proute;

	uci_foreach_sections("network", "route", s) {
		dmuci_get_value_by_section_string(s, "routeinstance", &tmp);
		if (tmp[0] == '\0')
			break;
		rinst = tmp;
	}
	uci_foreach_sections("network", "route_disabled", s) {
		dmuci_get_value_by_section_string(s, "routeinstance", &tmp);
		if (tmp[0] == '\0')
			break;
		dsinst = tmp;
	}
	uci_path_foreach_sections(icwmpd, "dmmap", "route_dynamic", s) {
		dmuci_get_value_by_section_string(s, "routeinstance", &tmp);
		if (tmp[0] == '\0')
			break;
		drinst = tmp;
	}
	if (rinst) r = atoi(rinst);
	if (dsinst) ds = atoi(dsinst);
	if (drinst) dr = atoi(drinst);
	max = (r>ds&&r>dr?r:ds>dr?ds:dr);
	return max;
}

static char *forwarding_update_instance_alias(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	char *inst_opt = (char *) argv[1];
	char *alias_opt = (char *) argv[2];
	bool *find_max = (bool *) argv[3];

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			int m = get_forwarding_last_inst();
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance = dmuci_set_value_by_section(s, inst_opt, buf);
	}
	*last_inst = instance;
	if (action == INSTANCE_MODE_ALIAS) {
		dmuci_get_value_by_section_string(s, alias_opt, &alias);
		if (alias[0] == '\0') {
			sprintf(buf, "cpe-%s", instance);
			alias = dmuci_set_value_by_section(s, alias_opt, buf);
		}
		sprintf(buf, "[%s]", alias);
		instance = dmstrdup(buf);
	}
	return instance;
}

char *forwarding_update_instance_alias_icwmpd(int action, char **last_inst, void *argv[])
{
	char *instance, *alias;
	char buf[8] = {0};

	struct uci_section *s = (struct uci_section *) argv[0];
	char *inst_opt = (char *) argv[1];
	char *alias_opt = (char *) argv[2];
	bool *find_max = (bool *) argv[3];

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (*find_max) {
			int m = get_forwarding_last_inst();
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, inst_opt, buf);
	}
	*last_inst = instance;
	if (action == INSTANCE_MODE_ALIAS) {
		dmuci_get_value_by_section_string(s, alias_opt, &alias);
		if (alias[0] == '\0') {
			sprintf(buf, "cpe-%s", instance);
			alias = DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, alias_opt, buf);
		}
		sprintf(buf, "[%s]", alias);
		instance = dmstrdup(buf);
	}
	return instance;
}

static struct uci_section *update_route_dynamic_section(struct proc_routing *proute)
{
	struct uci_section *s = NULL;
	char *name, *mask;
	uci_path_foreach_option_eq(icwmpd, "dmmap_route_forwarding", "route_dynamic", "target", proute->destination, s) {
			dmuci_get_value_by_section_string(s, "netmask", &mask);
			if (strcmp(proute->mask, mask) == 0)
				return s;
		}
		if (!s) {
			DMUCI_ADD_SECTION(icwmpd, "dmmap_route_forwarding", "route_dynamic", &s, &name);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "target", proute->destination);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "netmask", proute->mask);
		}
		return s;
}

int get_layer3_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;

	if (routeargs->routefwdsection == NULL) {
		*value = "1";
		return 0;
	}
	if(strcmp(routeargs->routefwdsection->type, "route_disabled") == 0)
		*value = "0";
	else {
		*value = "1";
	}
	return 0;
}

int set_layer3_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	char *pch;
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b) {
				if (routeargs->type == ROUTE_STATIC)
					return 0;
				dmuci_set_value_by_section(routeargs->routefwdsection, NULL, "route"); //TODO test
			}
			else {
				if (routeargs->type == ROUTE_DISABLED)
					return 0;
				dmuci_set_value_by_section(routeargs->routefwdsection, NULL, "route_disabled"); //TODO test
			}
			return 0;
	}
	return 0;
}

int get_layer3_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;

	if (routeargs->routefwdsection == NULL) {
		*value = "Enabled";
		return 0;
	}
	if(strcmp(routeargs->routefwdsection->type, "route_disabled") == 0) {
		*value = "Disabled";
	} else {
		if (is_cfg_route_active(routeargs->routefwdsection))
			*value = "Enabled";
		else
			*value = "Error";
	}
	return 0;	
}

int get_layer3_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *netmask;

	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "netmask", value);
	}	
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = proute->mask;
	}
	*value = (strcmp(*value, "255.255.255.255") == 0 || (*value)[0] == '\0') ? "Host" : "Network";
	return 0;		
}

int get_layer3_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "target", value);
		if ((*value)[0] == '\0') {
			*value = "0.0.0.0";
		}
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = dmstrdup(proute->destination); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;		
}

int set_layer3_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "target", value);
			return 0;
	}
	return 0;
}

int get_layer3_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "netmask", value);
		if ((*value)[0] == '\0') {
			*value = "255.255.255.255";
		}
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = dmstrdup(proute->mask); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_layer3_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "netmask", value);
			return 0;
	}
	return 0;
}

int get_layer3_src_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_layer3_src_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_layer3_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "gateway", value);
		if ((*value)[0] == '\0') {
			*value = "0.0.0.0";
		}
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = dmstrdup(proute->gateway); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
} 

int set_layer3_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{	
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "gateway", value);
			return 0;
	}
	return 0;
}

char *get_layer3_interface(struct dmctx *ctx, struct routingfwdargs *routeargs)
{
	json_object *res;
	char *val, *bval, *ifname, *device;
	char *name;
	struct uci_section *ss;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "interface", &val);
		return val;
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		bval = proute->iface;
		val = bval;
		if (!strstr(bval, "br-")) {
			uci_foreach_option_cont("network", "interface", "ifname", bval, ss) {
				ifname = section_name(ss);
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ifname, String}}, 1, &res);
				if (res) {
					device = dmjson_get_value(res, 1, "device");
					if (strcmp(bval, device) == 0) {
						return ifname;
					}
				}
			}			
		}
		else {
			val = bval + sizeof("br-") - 1;
		}
		return val;
	}
	return "";
}

int get_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *iface, *linker;
	*value = "";
			
	iface = get_layer3_interface(ctx, (struct routingfwdargs *)data);
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
	return 0;
}

int set_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker, *iface;
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				iface = linker + sizeof("linker_interface:") - 1;
				dmuci_set_value_by_section(routeargs->routefwdsection, "interface", iface);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_layer3_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *name;
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "metric", value);
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = dmstrdup(proute->metric); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	if ((*value)[0] == '\0') {
		*value = "0";
	}	
	return 0;
}

int set_layer3_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "metric", value);
			return 0;
	}
	return 0;
}

int get_layer3_mtu(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{	
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;

	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "mtu", value);
	}
	else {
		struct proc_routing *proute = routeargs->proute;
		*value = dmstrdup(proute->mtu); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	if ((*value)[0] == '\0') {
		*value = "1500";
	}
	return 0;
}

int set_layer3_mtu(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "mtu", value);
			return 0;
	}
	return 0;
}


int get_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *iface, *linker;

	dmuci_get_option_value_string("cwmp", "cpe", "default_wan_interface", &iface);
	if (iface[0] != '\0') {
		dmasprintf(&linker, "linker_interface:%s", iface);
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cWANDevice%c", dmroot, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL) {
			*value = "";
		}
		dmfree(linker);
	}
	return 0;
}
int set_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int i;
	char *linker, *iface;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				iface = linker + sizeof("linker_interface:") - 1;
				dmuci_set_value("cwmp", "cpe", "default_wan_interface", iface);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_layer3_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s;
	int cnt = 0;
	FILE *fp;
	char line[MAX_PROC_ROUTING];
	struct proc_routing proute;

	uci_foreach_sections("network", "route", s) {
		cnt++;
	}
	uci_foreach_sections("network", "route_disabled", s) {
		cnt++;
	}
	fp = fopen(ROUTE_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTING, fp);
		while (fgets(line, MAX_PROC_ROUTING, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			parse_proc_route_line(line, &proute);
			if (is_proute_static(&proute))
				continue;
			cnt++;
		}
		fclose(fp) ;
	}
	dmasprintf(value, "%d", cnt); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_layer3_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;
	*value = "";
	if(((struct routingfwdargs *)data)->type == ROUTE_DYNAMIC)
		dmmap_section= ((struct routingfwdargs *)data)->routefwdsection;
	else if (((struct routingfwdargs *)data)->type == ROUTE_STATIC)
		get_dmmap_section_of_config_section("dmmap_route_forwarding", "route", section_name(((struct routingfwdargs *)data)->routefwdsection), &dmmap_section);
	else
		get_dmmap_section_of_config_section("dmmap_route_forwarding", "route_disabled", section_name(((struct routingfwdargs *)data)->routefwdsection), &dmmap_section);

		dmuci_get_value_by_section_string(dmmap_section, "routealias", value);
	return 0;
}

int set_layer3_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	if(((struct routingfwdargs *)data)->type == ROUTE_DYNAMIC)
		dmmap_section= ((struct routingfwdargs *)data)->routefwdsection;
	else if (((struct routingfwdargs *)data)->type == ROUTE_STATIC)
		get_dmmap_section_of_config_section("dmmap_route_forwarding", "route", section_name(((struct routingfwdargs *)data)->routefwdsection), &dmmap_section);
	else
		get_dmmap_section_of_config_section("dmmap_route_forwarding", "route_disabled", section_name(((struct routingfwdargs *)data)->routefwdsection), &dmmap_section);

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "routealias", value);
			return 0;
	}
	return 0;
}
/////////////SUB ENTRIES///////////////
struct dm_permession_s DMForwarding_perm = {"0", &get_forwording_perm};

char *get_forwording_perm(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	return ((struct routingfwdargs *)data)->permission;
}

int browseForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iroute = NULL, *iroute_last = NULL;
	char *permission = "1";
	struct uci_section *s = NULL, *ss = NULL;
	FILE* fp = NULL;
	char line[MAX_PROC_ROUTING];
	struct proc_routing proute;
	bool find_max = true;
	struct routingfwdargs curr_routefwdargs = {0};
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "route", "dmmap_route_forwarding", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		init_args_rentry(&curr_routefwdargs, p->config_section, "1", NULL, ROUTE_STATIC);
		iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias, 4, p->dmmap_section, "routeinstance", "routealias", &find_max);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
			goto end;
	}
	free_dmmap_config_dup_list(&dup_list);
	synchronize_specific_config_sections_with_dmmap("network", "route_disabled", "dmmap_route_forwarding", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		init_args_rentry(&curr_routefwdargs, p->config_section, "1", NULL, ROUTE_DISABLED);
		iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias, 4, p->dmmap_section, "routeinstance", "routealias", &find_max);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
			goto end;
	}
	free_dmmap_config_dup_list(&dup_list);
	fp = fopen(ROUTE_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTING, fp);
		while (fgets(line, MAX_PROC_ROUTING, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			parse_proc_route_line(line, &proute);
			if (is_proute_static(&proute))
				continue;
			ss = update_route_dynamic_section(&proute);
			init_args_rentry(&curr_routefwdargs, ss, "0", &proute, ROUTE_DYNAMIC);
			iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias_icwmpd, 4, ss, "routeinstance", "routealias", &find_max);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
				goto end;
		}
		fclose(fp) ;
	}
end:
	return 0;
}
