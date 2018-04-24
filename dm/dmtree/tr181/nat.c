/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 Inteno Broadband Technology AB
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
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
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf*/
{"InterfaceSetting", &DMREAD, NULL, NULL, NULL, browseInterfaceSettingInst, NULL, NULL, NULL, tInterfaceSettingParam, NULL},
{0}
};

/*** NAT.InterfaceSetting. ***/
DMLEAF tInterfaceSettingParam[] = {
{"Enable", &DMREAD, DMT_BOOL, get_nat_enable, NULL, NULL, &DMNONE},
{"Alias", &DMWRITE, DMT_STRING, get_nat_alias, set_nat_alias, NULL, &DMNONE},
{"Interface", &DMREAD, DMT_STRING, get_nat_interface, NULL, NULL, &DMNONE},
{0}
};


int get_nat_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name((struct uci_section *)data), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "natalias", value);
	return 0;
}

int set_nat_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_network", "interface", section_name((struct uci_section *)data), &dmmap_section);
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
	char *linker;
	linker = dmstrdup(section_name(((struct uci_section *)data)));
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	dmfree(linker);
	return 0;
}

int get_nat_last_inst()
{
	char *rinst = "0", *tmp;
	int max;
	struct uci_section *s;
	int cnt = 0;
	
	uci_foreach_sections("dmmap_network", "interface", s) {
		dmuci_get_value_by_section_string(s, "natinstance", &tmp);
		if (tmp[0] == '\0')
			continue;
		else if (atoi(rinst) < atoi(tmp))
		{
			rinst = tmp;
		}		
	}
	max = atoi(rinst);
	return max;
}

char *nat_update_instance_alias(int action, char **last_inst, void *argv[])
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
			int m = get_nat_last_inst();
			sprintf(buf, "%d", m+1);
			*find_max = false;
		}
		else if (last_inst == NULL) {
			sprintf(buf, "%d", 1);
		}
		else {
			sprintf(buf, "%d", atoi(*last_inst)+1);
		}
		instance= DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, inst_opt, buf);
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


/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseInterfaceSettingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *net_sec = NULL, *s = NULL, *dmmap_sect;
	char *nat = NULL, *instance= NULL;
	char *nati, *nati_last = NULL, *v= NULL, *stmp= NULL;
	int id = 0;
	bool find_max = true;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);

	synchronize_specific_config_sections_with_dmmap("network", "interface", "dmmap_network", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		uci_foreach_list_cont("firewall", "zone", "network", section_name(p->config_section), s) {
			dmuci_get_value_by_section_string(s, "masq", &nat);
			if(nat[0] == '1') {
				nati =  handle_update_instance(1, dmctx, &nati_last, nat_update_instance_alias, 4, p->dmmap_section, "natinstance", "natalias", &find_max);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, nati) == DM_STOP)
					goto end;
				break;
			}
		}
	}
	free_dmmap_config_dup_list(&dup_list);
end:
	return 0;
}

