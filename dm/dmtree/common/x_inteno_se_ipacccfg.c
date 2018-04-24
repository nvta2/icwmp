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
#include "x_inteno_se_ipacccfg.h"

/*** DMROOT.X_INTENO_SE_IpAccCfg. ***/
DMOBJ tSe_IpAccObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"X_INTENO_SE_IpAccListCfgObj", &DMWRITE, add_ipacccfg_rule, delete_ipacccfg_rule, NULL, browseAccListInst, NULL, NULL, NULL, tSe_IpAccCfgParam, NULL},
{"X_INTENO_SE_PortForwarding", &DMWRITE, add_ipacccfg_port_forwarding, delete_ipacccfg_port_forwarding, NULL, browseport_forwardingInst, NULL, NULL, NULL, tSe_PortForwardingParam, NULL},
{0}
};

DMLEAF tSe_PortForwardingParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_port_forwarding_alias, set_port_forwarding_alias, NULL, NULL},
{"Name", &DMWRITE, DMT_STRING, get_port_forwarding_name, set_port_forwarding_name, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_port_forwarding_enable, set_port_forwarding_enable, NULL, NULL},
{"EnalbeNatLoopback", &DMWRITE, DMT_BOOL, get_port_forwarding_loopback, set_port_forwarding_loopback, NULL, NULL},
{"Protocol", &DMWRITE, DMT_STRING, get_port_forwarding_protocol, set_port_forwarding_protocol, NULL, NULL},
{"ExternalZone", &DMWRITE, DMT_STRING, get_port_forwarding_external_zone, set_port_forwarding_external_zone, NULL, NULL},
{"InternalZone", &DMWRITE, DMT_STRING, get_port_forwarding_internal_zone, set_port_forwarding_internal_zone, NULL, NULL},
{"ExternalPort", &DMWRITE, DMT_STRING, get_port_forwarding_external_port, set_port_forwarding_external_port, NULL, NULL},
{"InternalPort", &DMWRITE, DMT_STRING, get_port_forwarding_internal_port, set_port_forwarding_internal_port, NULL, NULL},
{"SourcePort", &DMWRITE, DMT_STRING, get_port_forwarding_source_port, set_port_forwarding_source_port, NULL, NULL},
{"InternalIpAddress", &DMWRITE, DMT_STRING, get_port_forwarding_internal_ipaddress, set_port_forwarding_internal_ipaddress, NULL, NULL},
{"ExternalIpAddress", &DMWRITE, DMT_STRING, get_port_forwarding_external_ipaddress, set_port_forwarding_external_ipaddress, NULL, NULL},
{"SourceIpAddress", &DMWRITE, DMT_STRING, get_port_forwarding_source_ipaddress, set_port_forwarding_source_ipaddress, NULL, NULL},
{"SourceMacAddress", &DMWRITE, DMT_STRING, get_port_forwarding_src_mac, set_port_forwarding_src_mac, NULL, NULL},
{0}
};

DMLEAF tSe_IpAccCfgParam[] = {
{"Alias", &DMWRITE, DMT_STRING, get_x_inteno_cfgobj_address_alias, set_x_inteno_cfgobj_address_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_x_bcm_com_ip_acc_list_cfgobj_enable, set_x_bcm_com_ip_acc_list_cfgobj_enable, NULL, NULL},
{"IPVersion", &DMWRITE, DMT_STRING, get_x_bcm_com_ip_acc_list_cfgobj_ipversion, set_x_bcm_com_ip_acc_list_cfgobj_ipversion, NULL, NULL},
{"Protocol", &DMWRITE, DMT_STRING, get_x_bcm_com_ip_acc_list_cfgobj_protocol, set_x_bcm_com_ip_acc_list_cfgobj_protocol, NULL, NULL},
{"FilterName", &DMWRITE, DMT_STRING, get_x_bcm_com_ip_acc_list_cfgobj_name, set_x_bcm_com_ip_acc_list_cfgobj_name, NULL, NULL},
{"AccAddressAndNetMask", &DMWRITE, DMT_STRING, get_x_inteno_cfgobj_address_netmask, set_x_inteno_cfgobj_address_netmask, NULL, NULL},
{"AccPort", &DMWRITE, DMT_STRING, get_x_bcm_com_ip_acc_list_cfgobj_acc_port, set_x_bcm_com_ip_acc_list_cfgobj_acc_port, NULL, NULL},
{"Target", &DMWRITE, DMT_STRING, get_x_bcm_com_ip_acc_list_cfgobj_target, set_x_bcm_com_ip_acc_list_cfgobj_target, NULL, NULL},
{0}
};



/************************************************************************************* 
**** function related to get_object_ip_acc_list_cfgobj ****
**************************************************************************************/

int get_x_bcm_com_ip_acc_list_cfgobj_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;
		
	dmuci_get_value_by_section_string(ipaccsection, "enabled", value);
	if ((*value)[0] == '\0') {
		*value = "1";
	}		
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	int check;
	struct uci_section *ipaccsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				value = "";
			}
			else {
				value = "0";
			}
			dmuci_set_value_by_section(ipaccsection, "enabled", value);
			return 0;
	}
	return 0;
}

int get_x_bcm_com_ip_acc_list_cfgobj_ipversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(ipaccsection, "family", value);
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_ipversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(ipaccsection, "family", value);
			return 0;
	}
	return 0;
}

int get_x_bcm_com_ip_acc_list_cfgobj_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(ipaccsection, "proto", value);
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(ipaccsection, "proto", value);
			return 0;
	}
	return 0;
}

int get_x_bcm_com_ip_acc_list_cfgobj_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(ipaccsection, "name", value);
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(ipaccsection, "name", value);
			return 0;
	}
	return 0;
}

int get_x_inteno_cfgobj_address_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *ipaccsection = (struct uci_section *)data;
	struct uci_list *list = NULL;
	
	dmuci_get_value_by_section_list(ipaccsection, "src_ip", &val);
	if (val) {
		*value = dmuci_list_to_string(val, ",");
	}
	else
		*value = "";
	if ((*value)[0] == '\0') {
		*value = "0.0.0.0/0";
		return 0;
	}
	return 0;
}

int set_x_inteno_cfgobj_address_netmask(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *spch, *val;
	struct uci_section *ipaccsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_delete_by_section(ipaccsection, "src_ip", "");
			val = dmstrdup(value);
			pch = strtok_r(val, " ,", &spch);
			while (pch != NULL) {
				dmuci_add_list_value_by_section(ipaccsection, "src_ip", pch);
				pch = strtok_r(NULL, " ,", &spch);
			}
			dmfree(val);
			return 0;
	}
	return 0;
}

int get_x_bcm_com_ip_acc_list_cfgobj_acc_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;
	
	dmuci_get_value_by_section_string(ipaccsection, "dest_port", value);
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_acc_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(ipaccsection, "dest_port", value);
			return 0;
	}
	return 0;
}

int get_x_bcm_com_ip_acc_list_cfgobj_target(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(ipaccsection, "target", value);
	return 0;
}

int set_x_bcm_com_ip_acc_list_cfgobj_target(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(ipaccsection, "target", value);
			return 0;
	}
	return 0;
}



/************************************************************************************* 
**** function related to get_cache_object_port_forwarding ****
**************************************************************************************/

int get_port_forwarding_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "name", value);
	return 0;
}

int set_port_forwarding_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "name", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "enabled", value);
	return 0;
}

int set_port_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	int check;
	struct uci_section *forwardsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(forwardsection, "enabled", "1");
			else 
				dmuci_set_value_by_section(forwardsection, "enabled", "0");
			return 0;
	}
	return 0;
}

int get_port_forwarding_loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;
		
	dmuci_get_value_by_section_string(forwardsection, "reflection", value);
	if((*value)[0] == '\0') {
		*value = "1";
	}
	return 0;
}

int set_port_forwarding_loopback(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	struct uci_section *forwardsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value_by_section(forwardsection, "reflection", "1");
			else 
				dmuci_set_value_by_section(forwardsection, "reflection", "0");
			return 0;
	}
	return 0;
}

int get_port_forwarding_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;
		
	dmuci_get_value_by_section_string(forwardsection, "proto", value);
	return 0;
}

int set_port_forwarding_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "proto", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_external_zone(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "src", value);
	return 0;
}

int set_port_forwarding_external_zone(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "src", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_internal_zone(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "dest", value);
	return 0;
}

int set_port_forwarding_internal_zone(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "dest", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_external_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "src_dport", value);
	return 0;
}

int set_port_forwarding_external_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "src_dport", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_internal_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "dest_port", value);
	if ((*value)[0] == '\0') {
		*value = "any";
	}
	return 0;
}

int set_port_forwarding_internal_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;
	
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "any") == 0) {
				value = "";
			}
			dmuci_set_value_by_section(forwardsection, "dest_port", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_source_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "src_port", value);
	if ((*value)[0] == '\0') {
		*value = "any";
	}
	return 0;
}

int set_port_forwarding_source_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "any") == 0) {
				value = "";
			}
			dmuci_set_value_by_section(forwardsection, "src_port", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_internal_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "dest_ip", value);
	return 0;
}

int set_port_forwarding_internal_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(forwardsection, "dest_ip", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_external_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_string(forwardsection, "src_dip", value);
	if ((*value)[0] == '\0') {
		*value = "any";
	}
	return 0;
}

int set_port_forwarding_external_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "any") == 0) {
				value = "";
			}
			dmuci_set_value_by_section(forwardsection, "src_dip", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_source_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_list *val;
	struct uci_element *e = NULL;
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_list(forwardsection, "src_ip", &val);
	if (val) {
		*value = dmuci_list_to_string(val, ",");
	}
	else {
		*value = "";
	}
	if ((*value)[0] == '\0') {
		*value = "any";
	}
	return 0;
}

int set_port_forwarding_source_ipaddress(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *val, *spch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcasecmp(value, "any") == 0) {
				dmuci_delete_by_section(forwardsection, "src_ip", "");
			}
			else {
				dmuci_delete_by_section(forwardsection, "src_ip", "");
				val = dmstrdup(value);
				pch = strtok_r(val, " ,", &spch);
				while (pch != NULL) {
					dmuci_add_list_value_by_section(forwardsection, "src_ip", pch);
					pch = strtok_r(NULL, " ,", &spch);
				}
				dmfree(val);
			}						
			return 0;
	}
	return 0;
}

int get_port_forwarding_src_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_list *list = NULL;
	struct uci_section *forwardsection = (struct uci_section *)data;

	dmuci_get_value_by_section_list(forwardsection, "src_mac", &list);
	*value = dmuci_list_to_string(list, " ");
	return 0;
}

int set_port_forwarding_src_mac(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *pch, *spch;
	struct uci_section *forwardsection = (struct uci_section *)data;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_delete_by_section(forwardsection, "src_mac", NULL);
			value = dmstrdup(value);
			pch = strtok_r(value, " ", &spch);
			while (pch != NULL) {
				dmuci_add_list_value_by_section(forwardsection, "src_mac", pch);
				pch = strtok_r(NULL, " ", &spch);
			}
			dmfree(value);
			return 0;
	}
	return 0;
}

/***** ADD DEL OBJ *******/
int add_ipacccfg_rule(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *rule = NULL, *dmmap_rule= NULL;
	
	check_create_dmmap_package("dmmap_firewall");
	instance = get_last_instance_icwmpd("dmmap_firewall", "rule", "fruleinstance");
	dmuci_add_section("firewall", "rule", &rule, &value);
	dmuci_set_value_by_section(rule, "type", "generic");
	dmuci_set_value_by_section(rule, "name", "new_rule");
	dmuci_set_value_by_section(rule, "proto", "all");
	dmuci_set_value_by_section(rule, "target", "ACCPET");
	dmuci_set_value_by_section(rule, "family", "ipv4");
	dmuci_set_value_by_section(rule, "enabled", "1");
	dmuci_set_value_by_section(rule, "hidden", "1");
	dmuci_set_value_by_section(rule, "parental", "0");

	dmuci_add_section_icwmpd("dmmap_firewall", "rule", &dmmap_rule, &v);
	dmuci_set_value_by_section(dmmap_rule, "section_name", section_name(rule));
	*instancepara = update_instance_icwmpd(dmmap_rule, instance, "fruleinstance");

	return 0;
}

int delete_ipacccfg_rule(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL; 
	struct uci_section *ss = NULL;
	int found = 0;
	struct uci_section *ipaccsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_firewall", "rule", section_name(ipaccsection), &dmmap_section);
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(ipaccsection, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("firewall", "rule", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_firewall", "rule", section_name(s), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_firewall", "rule", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}

	return 0;
}

int add_ipacccfg_port_forwarding(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
{
	char *value, *v;
	char *instance;
	struct uci_section *redirect = NULL, *dmmap_redirect= NULL;
	
	check_create_dmmap_package("dmmap_firewall");
	instance = get_last_instance_icwmpd("dmmap_firewall", "redirect", "forwardinstance");
	dmuci_add_section("firewall", "redirect", &redirect, &value);
	dmuci_set_value_by_section(redirect, "enabled", "0");
	dmuci_set_value_by_section(redirect, "target", "DNAT");
	dmuci_set_value_by_section(redirect, "proto", "tcp udp");

	dmuci_add_section_icwmpd("dmmap_firewall", "redirect", &dmmap_redirect, &v);
	dmuci_set_value_by_section(dmmap_redirect, "section_name", section_name(redirect));
	*instancepara = update_instance_icwmpd(dmmap_redirect, instance, "forwardinstance");
	return 0;
}


int delete_ipacccfg_port_forwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	struct pforwardrgs *forwardargs;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *forwardsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;
	
	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_firewall", "redirect", section_name(forwardsection), &dmmap_section);
			dmuci_delete_by_section(dmmap_section, NULL, NULL);
			dmuci_delete_by_section(forwardsection, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_option_eq("firewall", "redirect", "target", "DNAT", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_firewall", "redirect", section_name(s), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_firewall", "redirect", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}

////////////////////////SET AND GET ALIAS/////////////////////////////////
int get_x_inteno_cfgobj_address_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "rule", section_name(ipaccsection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "frulealias", value);
	return 0;
}

int set_x_inteno_cfgobj_address_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *ipaccsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "rule", section_name(ipaccsection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "frulealias", value);
			return 0;
	}
	return 0;
}

int get_port_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *forwardsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "redirect", section_name(forwardsection), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "forwardalias", value);
	return 0;
}

int set_port_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *forwardsection = (struct uci_section *)data;
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "redirect", section_name(forwardsection), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "forwardalias", value);
			return 0;
	}
	return 0;
}

int browseAccListInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *irule = NULL, *irule_last = NULL;
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("firewall", "rule", "dmmap_firewall", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		irule =  handle_update_instance(1, dmctx, &irule_last, update_instance_alias_icwmpd, 3, p->dmmap_section, "fruleinstance", "frulealias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, irule) == DM_STOP)
			return 0;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

int browseport_forwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *iforward = NULL, *iforward_last = NULL;
	struct uci_section *s = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap_eq("firewall", "redirect", "dmmap_firewall", "target", "DNAT", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		iforward =  handle_update_instance(1, dmctx, &iforward_last, update_instance_alias, 3, p->dmmap_section, "forwardinstance", "forwardalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, iforward) == DM_STOP)
			break;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

