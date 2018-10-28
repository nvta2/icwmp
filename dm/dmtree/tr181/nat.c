/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2018 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "nat.h"

/*** NAT. ***/
DMOBJ tnatObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, finform, notification, nextobj, leaf*/
{"InterfaceSetting", &DMWRITE, add_NAT_InterfaceSetting, delete_NAT_InterfaceSetting, NULL, browseInterfaceSettingInst, NULL, NULL, NULL, tInterfaceSettingParam, NULL},
{0}
};

/*** NAT.InterfaceSetting. ***/
DMLEAF tInterfaceSettingParam[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_nat_enable, set_nat_enable, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_nat_alias, set_nat_alias, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_nat_interface, set_nat_interface, NULL, NULL},
{0}
};

/*************************************************************
 * ADD DEL OBJ
/*************************************************************/
int add_NAT_InterfaceSetting(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	char *value, *v;
	char name[8];
	char *inst;
	struct uci_section *s = NULL;
	struct uci_section *dmmap_firewall=NULL;

	check_create_dmmap_package("dmmap_firewall");
	inst = get_last_instance_icwmpd("dmmap_firewall", "zone", "natinstance");
	sprintf(name, "NAT_%d", inst ? (atoi(inst)+1) : 1);
	dmuci_add_section("firewall", "zone", &s, &value);
	dmuci_set_value_by_section(s, "input", "REJECT");
	dmuci_set_value_by_section(s, "output", "ACCEPT");
	dmuci_set_value_by_section(s, "forward", "REJECT");
	dmuci_set_value_by_section(s, "name", name);

	dmuci_add_section_icwmpd("dmmap_firewall", "zone", &dmmap_firewall, &v);
	dmuci_set_value_by_section(dmmap_firewall, "section_name", section_name(s));
	*instance = update_instance_icwmpd(dmmap_firewall, inst, "natinstance");
	return 0;

}

int delete_NAT_InterfaceSetting(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	int found = 0;
	char *lan_name;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL, *dmmap_firewall= NULL;

	switch (del_action) {
		case DEL_INST:
			get_dmmap_section_of_config_section("dmmap_firewall", "zone", section_name((struct uci_section *)data), &dmmap_firewall);
			if(dmmap_firewall != NULL)
				dmuci_delete_by_section(dmmap_firewall, NULL, NULL);
			dmuci_delete_by_section((struct uci_section *)data, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("firewall", "zone", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_firewall", "zone", section_name(ss), &dmmap_firewall);
					if(dmmap_firewall != NULL)
						dmuci_delete_by_section(dmmap_firewall, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_firewall", "zone", section_name(ss), &dmmap_firewall);
				if(dmmap_firewall != NULL)
					dmuci_delete_by_section(dmmap_firewall, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			return 0;
	}
	return 0;
}

/**************************************************************************
* SET & GET VALUE
***************************************************************************/
int get_nat_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val;
	dmuci_get_value_by_section_string((struct uci_section *)data, "masq", &val);
	*value = (*val == '1') ? "1" : "0";
	return 0;
}

int set_nat_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				dmuci_set_value_by_section((struct uci_section *)data, "masq", "1");
			else
				dmuci_set_value_by_section((struct uci_section *)data, "masq", "0");
			return 0;
	}
	return 0;
}


int get_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "zone", section_name((struct uci_section *)data), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "natalias", value);
	return 0;
}

int set_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_firewall", "zone", section_name((struct uci_section *)data), &dmmap_section);
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "natalias", value);
			return 0;
	}
	return 0;
}

int get_nat_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_list *v;
	struct uci_element *e;
	char *ifaceobj, buf[256] = "";

	*value = "";
	dmuci_get_value_by_section_list((struct uci_section *)data, "network", &v);
	if (v == NULL)
		return 0;
	uci_foreach_element(v, e) {
		adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), e->name, &ifaceobj); // MEM WILL BE FREED IN DMMEMCLEAN
		if (*ifaceobj == '\0')
			continue;
		if (*buf != '\0')
			strcat(buf, ",");
		strcat(buf, ifaceobj);
	}
	*value = dmstrdup(buf);
	return 0;
}

int set_nat_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *iface, *pch, *pchr, buf[256] = "";

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			strcpy(buf, value);
			dmuci_set_value_by_section((struct uci_section *)data, "network", "");
			for(pch = strtok_r(buf, ",", &pchr); pch != NULL; pch = strtok_r(NULL, ",", &pchr)) {
				adm_entry_get_linker_value(ctx, pch, &iface);
				if (iface) {
					dmuci_add_list_value_by_section((struct uci_section *)data, "network", iface);
					free(iface);
				}
			}
			return 0;
	}
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseInterfaceSettingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *nati, *nati_last = NULL;
	bool find_max = true;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("firewall", "zone", "dmmap_firewall", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		nati =  handle_update_instance(1, dmctx, &nati_last, update_instance_alias, 3, p->dmmap_section, "natinstance", "natalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, nati) == DM_STOP)
			return 0;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}

