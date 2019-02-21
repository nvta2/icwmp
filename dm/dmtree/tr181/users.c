/*
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      Copyright (C) 2019 iopsys Software Solutions AB
 *
 *      Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include "dmcwmp.h"
#include "users.h"
#include "dmcommon.h"

DMOBJ tUsersObj[] = {
{"User", &DMWRITE, add_users_user, delete_users_user, NULL, browseUserInst, NULL, NULL, NULL, tUserParams, NULL},
{0}
};

DMLEAF tUsersParams[] = {
{"UserNumberOfEntries", &DMREAD, DMT_UNINT, get_users_user_number_of_entries, NULL, NULL, NULL},
{0}
};

DMLEAF tUserParams[] = {
{"Alias", &DMWRITE, DMT_STRING, get_user_alias, set_user_alias, NULL, NULL},
{"Enable", &DMWRITE, DMT_BOOL, get_user_enable, set_user_enable, NULL, NULL},
{"Username", &DMWRITE, DMT_STRING, get_user_username, set_user_username, NULL, NULL},
{"Password", &DMWRITE, DMT_STRING, get_user_password, set_user_password, NULL, NULL},
{0}
};

/***************************** Browse Functions ***********************************/

int browseUserInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *instnbr = NULL;
	struct dmmap_dup *p;
	LIST_HEAD(dup_list);


	synchronize_specific_config_sections_with_dmmap("users", "user", "dmmap_users", &dup_list);
	list_for_each_entry(p, &dup_list, list) {
		instance =  handle_update_instance(1, dmctx, &instnbr, update_instance_alias, 3, p->dmmap_section, "user_instance", "user_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, instance) == DM_STOP)
			return 0;
	}
	free_dmmap_config_dup_list(&dup_list);
	return 0;
}


int add_users_user(char *refparam, struct dmctx *ctx, void *data, char **instance){
	struct uci_section *s, *dmmap_user;
	char *last_inst= NULL, *sect_name= NULL, *username, *v;
	char ib[8];
	last_inst= get_last_instance_icwmpd("dmmap_users", "user", "user_instance");
	if (last_inst)
		sprintf(ib, "%s", last_inst);
	else
		sprintf(ib, "%s", "1");

	dmasprintf(&username, "user_%d", atoi(ib)+1);

	dmuci_add_section("users", "user", &s, &sect_name);
	dmuci_rename_section_by_section(s, username);
	dmuci_set_value_by_section(s, "enabled", "1");
	dmuci_set_value_by_section(s, "password", username);

	dmuci_add_section_icwmpd("dmmap_users", "user", &dmmap_user, &v);
	dmuci_set_value_by_section(dmmap_user, "section_name", sect_name);
	*instance = update_instance_icwmpd(dmmap_user, last_inst, "user_instance");
	return 0;
}

int delete_users_user(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action){
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *dmmap_section;
	int found = 0;

	switch (del_action) {
		case DEL_INST:
			if(is_section_unnamed(section_name((struct uci_section *)data))){
				LIST_HEAD(dup_list);
				delete_sections_save_next_sections("dmmap_users", "user", "user_instance", section_name((struct uci_section *)data), atoi(instance), &dup_list);
				update_dmmap_sections(&dup_list, "user_instance", "dmmap_users", "user");
				dmuci_delete_by_section_unnamed((struct uci_section *)data, NULL, NULL);
			} else {
				get_dmmap_section_of_config_section("dmmap_users", "user", section_name((struct uci_section *)data), &dmmap_section);
				dmuci_delete_by_section_unnamed_icwmpd(dmmap_section, NULL, NULL);
				dmuci_delete_by_section((struct uci_section *)data, NULL, NULL);
			}
			break;
		case DEL_ALL:
			uci_foreach_sections("users", "user", s) {
				if (found != 0){
					get_dmmap_section_of_config_section("dmmap_users", "user", section_name(ss), &dmmap_section);
					if(dmmap_section != NULL)
						dmuci_delete_by_section(dmmap_section, NULL, NULL);
					dmuci_delete_by_section(ss, NULL, NULL);
				}
				ss = s;
				found++;
			}
			if (ss != NULL){
				get_dmmap_section_of_config_section("dmmap_users", "user", section_name(ss), &dmmap_section);
				if(dmmap_section != NULL)
					dmuci_delete_by_section(dmmap_section, NULL, NULL);
				dmuci_delete_by_section(ss, NULL, NULL);
			}
			break;
	}
	return 0;
}
/***************************************** Set/Get Parameter functions ***********************/
int get_users_user_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	uci_foreach_sections("users", "user", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
}

int get_user_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_users", "user", section_name((struct uci_section *)data), &dmmap_section);
	dmuci_get_value_by_section_string(dmmap_section, "user_alias", value);
    return 0;
}

int get_user_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	dmuci_get_value_by_section_string((struct uci_section *)data, "enabled", &v);
	*value= dmstrdup(v);
    return 0;
}

int get_user_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value= dmstrdup(section_name((struct uci_section *)data));
    return 0;
}

int get_user_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	dmuci_get_value_by_section_string((struct uci_section *)data, "password", &v);
	*value= dmstrdup(v);
    return 0;
}

int set_user_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *dmmap_section;

	get_dmmap_section_of_config_section("dmmap_users", "user", section_name((struct uci_section *)data), &dmmap_section);

	switch (action) {
		case VALUECHECK:
				break;
		case VALUESET:
			dmuci_set_value_by_section(dmmap_section, "user_alias", value);
			return 0;
	}
	return 0;
}

int set_user_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
        switch (action) {
			case VALUECHECK:
					break;
			case VALUESET:
				dmuci_set_value_by_section((struct uci_section *)data, "enabled", value);
				break;
        }
        return 0;
}

int set_user_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *s= (struct uci_section *)data;
        switch (action) {
			case VALUECHECK:
					break;
			case VALUESET:
				dmuci_rename_section_by_section((struct uci_section *)data, value);
				break;
        }
        return 0;
}

int set_user_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
        switch (action) {
			case VALUECHECK:
					break;
			case VALUESET:
				dmuci_set_value_by_section((struct uci_section *)data, "password", value);
				break;
        }
        return 0;
}
