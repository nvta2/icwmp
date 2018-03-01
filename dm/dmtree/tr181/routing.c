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
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "routing.h"
#include "dmjson.h"

enum enum_route_type {
	ROUTE_STATIC,
	ROUTE_DYNAMIC,
	ROUTE_DISABLED
};

/*** Routing. ***/
DMOBJ tRoutingObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"Router", &DMREAD, NULL, NULL, NULL, browseRouterInst, NULL, NULL, tRouterObj, tRouterInstParam, NULL},
{0}
};

DMLEAF tRoutingParam[] = {
{"RouterNumberOfEntries", &DMREAD, DMT_UNINT, get_router_nbr_entry, NULL, NULL, NULL},
{0}
};

/*** Routing.Router. ***/
DMOBJ tRouterObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"IPv4Forwarding", &DMREAD, NULL, NULL, NULL, browseIPv4ForwardingInst, NULL, NULL, NULL, tIPv4ForwardingParam, NULL},
{0}
};
DMLEAF tRouterInstParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_router_alias, set_router_alias, NULL, NULL},
{0}
};

/*** Routing.Router.IPv4Forwarding. ***/
DMLEAF tIPv4ForwardingParam[] = {
{"Enable", &DMRouting, DMT_BOOL, get_router_ipv4forwarding_enable, set_router_ipv4forwarding_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_router_ipv4forwarding_status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_router_ipv4forwarding_alias, set_router_ipv4forwarding_alias, NULL, NULL},
{"DestIPAddress", &DMRouting, DMT_STRING, get_router_ipv4forwarding_destip, set_router_ipv4forwarding_destip, NULL, NULL},
{"DestSubnetMask", &DMRouting, DMT_STRING, get_router_ipv4forwarding_destmask, set_router_ipv4forwarding_destmask, NULL, NULL},
{"SourceIPAddress", &DMREAD, DMT_STRING, get_router_ipv4forwarding_src_address, NULL, NULL, NULL},
{"SourceSubnetMask", &DMREAD, DMT_STRING, get_router_ipv4forwarding_src_mask, NULL, NULL, NULL},
{"GatewayIPAddress", &DMRouting, DMT_STRING, get_router_ipv4forwarding_gatewayip, set_router_ipv4forwarding_gatewayip, NULL, NULL},
{"Interface", &DMRouting, DMT_STRING, get_router_ipv4forwarding_interface_linker_parameter, set_router_ipv4forwarding_interface_linker_parameter, NULL, NULL},
{"ForwardingMetric", &DMRouting, DMT_STRING, get_router_ipv4forwarding_metric, set_router_ipv4forwarding_metric, NULL, NULL},
{0}
};


/********************************
 * inti function
 ********************************/
inline int init_args_ipv4forward(struct routingfwdargs *args, struct uci_section *s, char *permission, struct proc_routing *proute, int type)
{
	args->permission = permission;
	args->routefwdsection = s;
	args->proute = proute;
	args->type = type;
	return 0;
}

/************************************************************************************* 
**** function related to get_object_router_ipv4forwarding ****
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

	fp = fopen(ROUTING_FILE, "r");
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

static struct uci_section *update_route_dynamic_section(struct proc_routing *proute)
{
	struct uci_section *s = NULL;
	char *name, *mask;
	uci_path_foreach_option_eq(icwmpd, "dmmap", "route_dynamic", "target", proute->destination, s) {
		dmuci_get_value_by_section_string(s, "netmask", &mask);
		if (strcmp(proute->mask, mask) == 0){
			return s;
		}
	}
	if (!s) {
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "route_dynamic", &s, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "target", proute->destination);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "netmask", proute->mask);
	}
	return s;
}

int get_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_router_ipv4forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_router_ipv4forwarding_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_router_ipv4forwarding_destip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_router_ipv4forwarding_destmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_router_ipv4forwarding_src_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_router_ipv4forwarding_src_mask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_router_ipv4forwarding_gatewayip(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

char *get_router_ipv4forwarding_interface(struct dmctx *ctx, void *data, char *instance)
{
	json_object *res;
	char *val, *bval, *ifname, *device;
	char *name;
	struct uci_section *ss;
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;
	
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

int get_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *iface, *linker;
			
	iface = get_router_ipv4forwarding_interface(ctx, data, instance);
	if (iface[0] != '\0') {
		dmasprintf(&linker, "%s", iface);
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL)
			*value = "";
		dmfree(linker);
	}
	return 0;
}

int set_router_ipv4forwarding_interface_linker_parameter(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *linker;
	struct routingfwdargs *routeargs = (struct routingfwdargs *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(ctx, value, &linker);
			if (linker) {
				dmuci_set_value_by_section(routeargs->routefwdsection, "interface", linker);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_router_ipv4forwarding_metric(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_router_nbr_entry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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
	fp = fopen(ROUTING_FILE, "r");
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

/*************************************************************
 * SET AND GET ALIAS FOR ROUTER OBJ
/*************************************************************/

int get_router_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "router_alias", value);
	return 0;
}

int set_router_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "router_alias", value);
			return 0;
	}
	return 0;
}

int get_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	if (((struct routingfwdargs *)data)->routefwdsection) dmuci_get_value_by_section_string(((struct routingfwdargs *)data)->routefwdsection, "routealias", value);
	return 0;
}

int set_router_ipv4forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (((struct routingfwdargs *)data)->routefwdsection) dmuci_set_value_by_section(((struct routingfwdargs *)data)->routefwdsection, "routealias", value);
			return 0;
	}
	return 0;
}

char *get_routing_perm(char *refparam, struct dmctx *dmctx, void *data, char *instance)
{
	return ((struct routingfwdargs *)data)->permission;
}

struct dm_permession_s DMRouting = {"0", &get_routing_perm};


/*************************************************************
 * SUB ENTRIES
/*************************************************************/

int browseIPv4ForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iroute = NULL, *iroute_last = NULL;
	char *permission = "1";
	struct uci_section *s = NULL, *ss = NULL;
	FILE* fp = NULL;
	char line[MAX_PROC_ROUTING];
	struct proc_routing proute = {0};
	bool find_max = true;
	struct routingfwdargs curr_routefwdargs = {0};

	uci_foreach_sections("network", "route", s) {
		init_args_ipv4forward(&curr_routefwdargs, s, "1", NULL, ROUTE_STATIC);
		iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias, 4, s, "routeinstance", "routealias", &find_max);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
			goto end;
	}
	uci_foreach_sections("network", "route_disabled", s) {
		init_args_ipv4forward(&curr_routefwdargs, s, "1", NULL, ROUTE_DISABLED);
		iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias, 4, s, "routeinstance", "routealias", &find_max);
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
			goto end;
	}
	fp = fopen(ROUTING_FILE, "r");
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
			init_args_ipv4forward(&curr_routefwdargs, ss, "0", &proute, ROUTE_DYNAMIC);
			iroute =  handle_update_instance(1, dmctx, &iroute_last, forwarding_update_instance_alias, 4, ss, "routeinstance", "routealias", &find_max);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_routefwdargs, iroute) == DM_STOP)
				goto end;
		}
		fclose(fp) ;
	}
end:
	return 0;
}

int browseRouterInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *r = NULL, *r_last = NULL;
	
	update_section_list(DMMAP,"router", NULL, 1, NULL, NULL, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap", "router", s) {
		r = handle_update_instance(1, dmctx, &r_last, update_instance_alias_icwmpd, 3, s, "router_instance", "router_alias");
		DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, r);
		break;
	}
	return 0;
}
