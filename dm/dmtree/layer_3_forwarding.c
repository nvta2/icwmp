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
#include "layer_3_forwarding.h"

enum enum_route_type {
	ROUTE_STATIC,
	ROUTE_DYNAMIC,
	ROUTE_DISABLED
};

struct routefwdargs cur_routefwdargs = {0};

inline int init_args_rentry(struct dmctx *ctx, struct uci_section *s, char *permission, struct proc_route *proute, int type)
{
	struct routefwdargs *args = &cur_routefwdargs;
	ctx->args = (void *)args;
	args->permission = permission;
	args->routefwdsection = s;
	args->proute = proute;
	args->type = type;
	return 0;
}

/************************************************************************************* 
**** function related to get_object_layer3 ****
**************************************************************************************/
void ip_to_hex(char *address, char *ret) //TODO Move to the common.c
{
	int i;
	int ip[4] = {0};
	
	sscanf(address, "%d.%d.%d.%d", &(ip[0]), &(ip[1]), &(ip[2]), &(ip[3]));
	sprintf(ret, "%02X%02X%02X%02X", ip[0], ip[1], ip[2], ip[3]);
}

void hex_to_ip(char *address, char *ret) //TODO Move to the common.c
{
	int i;
	int ip[4] = {0};
	sscanf(address, "%2x%2x%2x%2x", &(ip[0]), &(ip[1]), &(ip[2]), &(ip[3]));
	sprintf(ret, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

void parse_proc_route_line(char *line, struct proc_route *proute)
{
	char *pch;
	proute->iface = strtok(line, " \t");

	pch = strtok(NULL, " \t");
	hex_to_ip(pch, proute->destination);
	pch = strtok(NULL, " \t");
	hex_to_ip(pch, proute->gateway);
	proute->flags = strtok(NULL, " \t");
	proute->refcnt = strtok(NULL, " \t");
	proute->use = strtok(NULL, " \t");
	proute->metric = strtok(NULL, " \t");
	pch = strtok(NULL, " \t");
	hex_to_ip(pch, proute->mask);
	proute->mtu = strtok(NULL, " \t");
	proute->window = strtok(NULL, " \t");
	proute->irtt = strtok(NULL, " \t\n\r");
}

bool is_proute_static(struct proc_route *proute)
{
	char *mask;
	struct uci_section *s;
	uci_foreach_option_eq("network", "route", "target", proute->destination, s) {
		dmuci_get_value_by_section_string(s, "netmask", &mask);
		if (strcmp(proute->mask, mask) == 0)
			return true;
	}
	return false;
}

bool is_cfg_route_active(struct uci_section *s)
{
	FILE *fp;
	char line[MAX_PROC_ROUTE];
	struct proc_route proute;
	char *dest, *mask;

	dmuci_get_value_by_section_string(s, "target", &dest);
	dmuci_get_value_by_section_string(s, "netmask", &mask);

	fp = fopen(ROUTE_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTE, fp);
		while (fgets(line, MAX_PROC_ROUTE, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			parse_proc_route_line(line, &proute);
			if (strcmp(dest, proute.destination) == 0 &&
				strcmp(mask, proute.mask) == 0)
				return true;
		}
		fclose(fp) ;
	}
	return false;
}

int get_forwarding_last_inst()
{
	char *rinst = NULL, *drinst = NULL, *dsinst = NULL, *tmp;
	int r = 0, dr = 0, ds = 0, max;
	struct uci_section *s;
	int cnt = 0;
	FILE *fp;
	char line[MAX_PROC_ROUTE];
	struct proc_route proute;

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
	uci_foreach_sections("dmmap", "route_dynamic", s) {
		dmuci_get_value_by_section_string(s, "routeinstance", &tmp);
		if (tmp[0] == '\0')
			break;
		drinst = tmp;
	}
	if (rinst) r = atoi();
	if (drinst) ds = atoi();
	if (drinst) dr = atoi();
	max = (r>ds&&r>dr?r:ds>dr?ds:dr);
	return max;
}

char *forwarding_update_instance(struct uci_section *s, char *last_inst, char *inst_opt, bool *find_max)
{
	char *instance;
	char buf[8] = {0};

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
			*find_max = false;
		}
		else {
			if (*find_max) {
				int m = get_forwarding_last_inst();
				sprintf(buf, "%d", m+1);
				*find_max = false;
			} else {
				sprintf(buf, "%d", atoi(last_inst)+1);
			}
		}
		instance = dmuci_set_value_by_section(s, inst_opt, buf);
	}
	return instance;
}

char *forwarding_update_instance_dynamic(struct proc_route *proute, char *last_inst, char *inst_opt, bool *find_max)
{
	struct uci_section *s;
	char *instance, *name;
	char buf[8] = {0};

	uci_foreach_option_eq("dmmap", "route_dynamic", "target", proute->destination, s) {
		dmuci_get_value_by_section_string(s, "netmask", &mask);
		if (strcmp(proute->mask, mask) == 0)
			break;
	}
	if (!s) {
		dmuci_add_section("dmmap", "route_dynamic", &s, &name);
		dmuci_set_value_by_section(s, "target", proute->destination);
		dmuci_set_value_by_section(s, "netmask", proute->mask);
	}
	instance = forwarding_update_instance(s, last_inst, inst_opt, find_max);
	return instance;
}

int get_layer3_enable(char *refparam, struct dmctx *ctx, char **value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;

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

int set_layer3_enable(char *refparam, struct dmctx *ctx, int action, char *value)
{
	static bool b;
	char *pch;
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			if (b) {
				if (routeargs->type == ROUTE_STATIC)
					return 0;
				dmuci_set_value_by_section(routeargs->routefwdsection, NULL, "route");
			}
			else {
				if (routeargs->type == ROUTE_DISABLED)
					return 0;
				dmuci_set_value_by_section(routeargs->routefwdsection, NULL, "route_disabled");
			}
			return 0;
	}
	return 0;
}

int get_layer3_status(char *refparam, struct dmctx *ctx, char **value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;

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

int get_layer3_type(char *refparam, struct dmctx *ctx, char **value)
{
	char *netmask;

	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "netmask", value);
	}	
	else {
		struct proc_route *proute = routeargs->proute;
		*value = proute->mask;
	}
	*value = (strcmp(*value, "255.255.255.255") == 0 || (*value)[0] == '\0') ? "Host" : "Network";
	return 0;		
}

int get_layer3_destip(char *refparam, struct dmctx *ctx, char **value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "target", value);
		if ((*value)[0] == '\0') {
			*value = "0.0.0.0";
		}
	}
	else {
		struct proc_route *proute = routeargs->proute;
		*value = dmstrdup(proute->destination); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;		
}

int set_layer3_destip(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "target", value);
			return 0;
	}
	return 0;
}

int get_layer3_destmask(char *refparam, struct dmctx *ctx, char **value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "netmask", value);
		if ((*value)[0] == '\0') {
			*value = "255.255.255.255";
		}
	}
	else {
		struct proc_route *proute = routeargs->proute;
		*value = dmstrdup(proute->mask); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
}

int set_layer3_destmask(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "netmask", value);
			return 0;
	}
	return 0;
}

int get_layer3_src_address(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_layer3_src_mask(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "0.0.0.0";
	return 0;
}

int get_layer3_gatewayip(char *refparam, struct dmctx *ctx, char **value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "gateway", value);
		if ((*value)[0] == '\0') {
			*value = "0.0.0.0";
		}
	}
	else {
		struct proc_route *proute = routeargs->proute;
		*value = dmstrdup(proute->gateway); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	return 0;
} 

int set_layer3_gatewayip(char *refparam, struct dmctx *ctx, int action, char *value)
{	
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "gateway", value);
			return 0;
	}
	return 0;
}

char *get_layer3_interface(struct dmctx *ctx)
{
	json_object *res;
	char *val, *bval, *ifname, *device;
	char *name;
	struct uci_section *ss;
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "interface", &val);
		return val;
	}
	else {
		struct proc_route *proute = routeargs->proute;
		bval = proute->iface;
		if (!strstr(bval, "br-")) {
			uci_foreach_option_cont("network", "interface", "ifname", bval, ss) {
				ifname = section_name(ss);
				dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", ifname}}, 1, &res);
				if (res) {
					json_select(res, "device", 0, NULL, &device, NULL);
					if (strcmp(bval, device) == 0) {
						return ifname;
					}
				}
			}			
		}
	}
	return "";
}

int get_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, char **value)
{
	char *iface, *linker;
			
	iface = get_layer3_interface(ctx);
	if (iface[0] != '\0') {
		dmastrcat(&linker, "linker_interface:", iface);
		adm_entry_get_linker_param(DMROOT"WANDevice.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL) {
			adm_entry_get_linker_param(DMROOT"LANDevice.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
			if (*value == NULL)
				*value = "";
		}
		dmfree(linker);
	}
	return 0;
}

int set_layer3_interface_linker_parameter(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *linker, *iface;
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(value, &linker);
			if (linker) {
				iface = linker + sizeof("linker_interface:") - 1;
				dmuci_set_value_by_section(routeargs->routefwdsection, "interface", iface);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_layer3_metric(char *refparam, struct dmctx *ctx, char **value)
{
	char *name;
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;	
	
	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "metric", value);
	}
	else {
		struct proc_route *proute = routeargs->proute;
		*value = dmstrdup(proute->metric); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	if ((*value)[0] == '\0') {
		*value = "0";
	}	
	return 0;
}

int set_layer3_metric(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "metric", value);
			return 0;
	}
	return 0;
}

int get_layer3_mtu(char *refparam, struct dmctx *ctx, char **value)
{	
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;

	if (routeargs->routefwdsection != NULL)	{
		dmuci_get_value_by_section_string(routeargs->routefwdsection, "mtu", value);
	}
	else {
		struct proc_route *proute = routeargs->proute;
		*value = dmstrdup(proute->mtu); // MEM WILL BE FREED IN DMMEMCLEAN
	}
	if ((*value)[0] == '\0') {
		*value = "1500";
	}
	return 0;
}

int set_layer3_mtu(char *refparam, struct dmctx *ctx, int action, char *value)
{
	struct routefwdargs *routeargs = (struct routefwdargs *)ctx->args;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(routeargs->routefwdsection, "mtu", value);
			return 0;
	}
	return 0;
}

inline int get_object_layer3(struct dmctx *ctx, char *iroute, char *permission)
{
	DMOBJECT(DMROOT"Layer3Forwarding.Forwarding.%s.", ctx, "0", 1, NULL, NULL, NULL, iroute);
	DMPARAM("Enable", ctx, permission, get_layer3_enable, set_layer3_enable, "xsd:boolean", 0, 1, UNDEF, NULL);
	DMPARAM("Status", ctx, "0", get_layer3_status, NULL, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("Type", ctx, "0", get_layer3_type, NULL, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("DestIPAddress", ctx, permission, get_layer3_destip, set_layer3_destip, "xsd:boolean", 0, 1, UNDEF, NULL);
	DMPARAM("DestSubnetMask", ctx, permission, get_layer3_destmask, set_layer3_destmask, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("SourceIPAddress", ctx, "0", get_layer3_src_address, NULL, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("SourceSubnetMask", ctx, "0", get_layer3_src_mask, NULL, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("GatewayIPAddress", ctx, permission, get_layer3_gatewayip, set_layer3_gatewayip, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("Interface", ctx, permission, get_layer3_interface_linker_parameter, set_layer3_interface_linker_parameter, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("ForwardingMetric", ctx, permission, get_layer3_metric, set_layer3_metric, NULL, 0, 1, UNDEF, NULL);
	DMPARAM("MTU", ctx, permission, get_layer3_mtu, set_layer3_mtu, NULL, 0, 1, UNDEF, NULL);	
	return 0;
}

/****************************************/

/*
get_linker_by_parameter() {
	local param="$1"
	local linker=""
	jmsg=`cat "$cache_path/"* $cache_linker_dynamic | grep "\"linker\"" | grep "\"$param\"" | head -1`
	[ "$jmsg" = "" ] && return
	json_load "$jmsg"
	json_get_var linker linker
	echo "$linker"
}
*/

int get_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, char **value)
{
	char *iface, *linker;

	dmuci_get_option_value_string("cwmp", "cpe", "default_wan_interface", &iface);
	if (iface[0] != '\0') {
		dmastrcat(&linker, "linker_interface:", iface);
		adm_entry_get_linker_param(DMROOT"WANDevice.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*value == NULL) {
			*value = "";
		}
		dmfree(linker);
	}
	return 0;
}
int set_layer3_def_conn_serv(char *refparam, struct dmctx *ctx, int action, char *value)
{
	int i;
	char *linker, *iface;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			adm_entry_get_linker_value(value, &linker);
			if (linker) {
				iface = linker + sizeof("linker_interface:") - 1;
				dmuci_set_value("cwmp", "cpe", "default_wan_interface", iface);
				dmfree(linker);
			}
			return 0;
	}
	return 0;
}

int get_layer3_nbr_entry(char *refparam, struct dmctx *ctx, char **value)
{
	struct uci_section *s;
	int cnt = 0;
	FILE *fp;
	char line[MAX_PROC_ROUTE];
	struct proc_route proute;

	uci_foreach_sections("network", "route", s) {
		cnt++;
	}
	uci_foreach_sections("network", "route_disabled", s) {
		cnt++;
	}
	fp = fopen(ROUTE_FILE, "r");
	if ( fp != NULL)
	{
		fgets(line, MAX_PROC_ROUTE, fp);
		while (fgets(line, MAX_PROC_ROUTE, fp) != NULL )
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

int entry_method_root_layer3_forwarding(struct dmctx *ctx)
{
	char *iroute = NULL;
	char *permission = "1";
	struct uci_section *s = NULL;
	FILE* fp = NULL;
	char line[MAX_PROC_ROUTE];
	struct proc_route proute;
	bool find_max = true;
	
	IF_MATCH(ctx, DMROOT"Layer3Forwarding.") {
		DMOBJECT(DMROOT"Layer3Forwarding.", ctx, "0", 1, NULL, NULL, NULL);
		DMOBJECT(DMROOT"Layer3Forwarding.Forwarding.", ctx, "0", 1, NULL, NULL, NULL);
		DMPARAM("DefaultConnectionService", ctx, "1", get_parameter_by_linker, set_layer3_def_conn_serv, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("ForwardNumberOfEntries", ctx, "0", get_layer3_nbr_entry, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		uci_foreach_sections("network", "route", s) {
			init_args_rentry(ctx, s, "1", NULL, ROUTE_STATIC);
			iroute = forwarding_update_instance(s, iroute, "routeinstance", &find_max);
			SUBENTRY(get_object_layer3, ctx, iroute, permission);
		}
		uci_foreach_sections("network", "route_disabled", s) {
			init_args_rentry(ctx, s, "1", NULL, ROUTE_DISABLED);
			iroute = forwarding_update_instance(s, iroute, "routeinstance", &find_max);
			SUBENTRY(get_object_layer3, ctx, iroute, permission);
		}
		fp = fopen(ROUTE_FILE, "r");
		if ( fp != NULL)
		{
			fgets(line, MAX_PROC_ROUTE, fp);
			while (fgets(line, MAX_PROC_ROUTE, fp) != NULL )
			{
				if (line[0] == '\n')
					continue;
				parse_proc_route_line(line, &proute);
				if (is_proute_static(&proute))
					continue;
				init_args_rentry(ctx, NULL, "0", &proute, ROUTE_DYNAMIC);
				iroute = forwarding_update_instance_dynamic(&proute, iroute, "routeinstance", &find_max);
				SUBENTRY(get_object_layer3, ctx, iroute, permission);
			}
			fclose(fp) ;
		}
		return 0;
	}
	return FAULT_9005;
}
