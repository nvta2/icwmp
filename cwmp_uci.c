/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include <ctype.h>
#include "common.h"
#include "log.h"
#include "cwmp_uci.h"

struct uci_paths uci_save_conf_paths[] = {
		[UCI_STANDARD_CONFIG] = { "/etc/config", "/tmp/.uci", NULL },
		[UCI_VARSTATE_CONFIG] = { "/var/state", NULL, NULL }
};

/*
 * UCI LOOKUP
 */
static inline bool cwmp_check_section_name(const char *str, bool name)
{
	if (!*str)
		return false;
	for (; *str; str++) {
		unsigned char c = *str;
		if (isalnum(c) || c == '_')
			continue;
		if (name || (c < 33) || (c > 126))
			return false;
	}
	return true;
}

int cwmp_uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value)
{
	/*value*/
	ptr->value = value;
	/*package*/
	if (!package)
		return UCI_ERR_IO;
	ptr->package = package;

	/*section*/
	if (!section || !section[0]) {
		ptr->target = UCI_TYPE_PACKAGE;
		goto lookup;
	}
	ptr->section = section;
	if (ptr->section && !cwmp_check_section_name(ptr->section, true))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	/*option*/
	if (!option || !option[0]) {
		ptr->target = UCI_TYPE_SECTION;
		goto lookup;
	}
	ptr->target = UCI_TYPE_OPTION;
	ptr->option = option;

lookup:
	if (uci_lookup_ptr(ctx, ptr, NULL, true) != UCI_OK) {
		return UCI_ERR_PARSE;
	}
	return UCI_OK;
}

static int cwmp_uci_lookup_ptr_by_section(struct uci_context *ctx, struct uci_ptr *ptr, struct uci_section *s, char *option, char *value)
{
	if (s == NULL || s->package == NULL)
		return -1;

	/*value*/
	ptr->value = value;

	/*package*/
	ptr->package = s->package->e.name;
	ptr->p = s->package;

	/* section */
	ptr->section = s->e.name;
	ptr->s = s;

	/*option*/
	if (!option || !option[0]) {
		ptr->target = UCI_TYPE_SECTION;
		goto lookup;
	}
	ptr->target = UCI_TYPE_OPTION;
	ptr->option = option;
	ptr->flags |= UCI_LOOKUP_EXTENDED;
lookup:
	if (uci_lookup_ptr(ctx, ptr, NULL, false) != UCI_OK || !UCI_LOOKUP_COMPLETE)
		return -1;

	return 0;
}

/*
 * UCI INIT EXIT
 */

void cwmp_uci_init_by_config(int config)
{
	if(uci_save_conf_paths[config].uci_ctx != NULL)
		return;
	uci_save_conf_paths[config].uci_ctx = uci_alloc_context();
	if ( uci_save_conf_paths[config].uci_ctx == NULL)
		return;
	uci_add_delta_path(uci_save_conf_paths[config].uci_ctx, uci_save_conf_paths[config].uci_ctx->savedir);
	uci_set_savedir(uci_save_conf_paths[config].uci_ctx, uci_save_conf_paths[config].save_dir);
	uci_set_confdir(uci_save_conf_paths[config].uci_ctx, uci_save_conf_paths[config].conf_dir);
}

void cwmp_uci_exit_by_config(int config)
{
	if (uci_save_conf_paths[config].uci_ctx) {
		uci_free_context(uci_save_conf_paths[config].uci_ctx);
		uci_save_conf_paths[config].uci_ctx = NULL;
	}
}

int cwmp_uci_init()
{
	int i;
	int uci_nbre_conf_paths = (int)(sizeof(uci_save_conf_paths) / sizeof(struct uci_paths));
	for (i = 0; i < uci_nbre_conf_paths; i++) {
		cwmp_uci_init_by_config(i);
	}
	return 0;
}

void cwmp_uci_exit(void)
{
	int i;
	int uci_nbre_conf_paths = (int)(sizeof(uci_save_conf_paths) / sizeof(struct uci_paths));
	for (i = 0; i < uci_nbre_conf_paths; i++) {
		cwmp_uci_exit_by_config(i);
	}
}

void cwmp_uci_reinit(void)
{
	cwmp_uci_exit();
	cwmp_uci_init();
}

/*
 * UCI GET option value
 */
int cwmp_uci_get_option_value_string(char *package, char *section, char *option, uci_config_paths uci_type, char **value)
{
	struct uci_ptr ptr = { 0 };

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, option, NULL) != UCI_OK) {
		*value = NULL;
		return UCI_ERR_PARSE;
	}
	if (ptr.o && ptr.o->type == UCI_TYPE_LIST) {
		*value = cwmp_uci_list_to_string(&ptr.o->v.list, " ");
	} else if (ptr.o && ptr.o->v.string) {
		*value = icwmp_strdup(ptr.o->v.string);
	} else {
		*value = NULL;
		return UCI_ERR_NOTFOUND;
	}
	return UCI_OK;
}

int cwmp_uci_get_value_by_path(char *path, uci_config_paths uci_type, char **value)
{
	struct uci_ptr ptr;
	char *s;

	*value = NULL;

	s = strdup(path);
	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, s, true) != UCI_OK) {
		CWMP_LOG(ERROR, "Error occurred in uci get %s", path);
		free(s);
		return UCI_ERR_PARSE;
	}
	free(s);
	if (ptr.flags & UCI_LOOKUP_COMPLETE) {
		if (ptr.o == NULL || ptr.o->v.string == NULL) {
			CWMP_LOG(INFO, "%s not found or empty value", path);
			return UCI_OK;
		}
		*value = strdup(ptr.o->v.string);
	}
	return UCI_OK;
}

int uci_get_state_value(char *path, char **value)
{
	int error;
	error = cwmp_uci_get_value_by_path(path, UCI_VARSTATE_CONFIG, value);
	return error;
}

int uci_get_value(char *path, char **value)
{
	int error;
	error = cwmp_uci_get_value_by_path(path, UCI_STANDARD_CONFIG, value);
	return error;
}

int cwmp_uci_get_value_by_section_string(struct uci_section *s, char *option, char **value)
{
	struct uci_element *e;
	struct uci_option *o;

	*value = NULL;
	if (s == NULL || option == NULL)
		goto not_found;

	uci_foreach_element(&s->options, e)
	{
		o = (uci_to_option(e));
		if (!strcmp(o->e.name, option)) {
			if (o->type == UCI_TYPE_LIST) {
				*value = cwmp_uci_list_to_string(&o->v.list, " ");
			} else {
				*value = o->v.string ? icwmp_strdup(o->v.string) : NULL;
			}
			return UCI_OK;
		}
	}

not_found:
	*value = NULL;
	return UCI_ERR_NOTFOUND;
}

int cwmp_uci_get_value_by_section_list(struct uci_section *s, char *option, struct uci_list **value)
{
	struct uci_element *e;
	struct uci_option *o;
	struct uci_list *list;
	char *pch = NULL, *spch = NULL;
	char dup[256];

	*value = NULL;

	if (s == NULL || option == NULL)
		return -1;

	uci_foreach_element(&s->options, e)
	{
		o = (uci_to_option(e));
		if (strcmp(o->e.name, option) == 0) {
			switch (o->type) {
			case UCI_TYPE_LIST:
				*value = &o->v.list;
				return 0;
			case UCI_TYPE_STRING:
				if (!o->v.string || (o->v.string)[0] == '\0')
					return 0;
				list = icwmp_calloc(1, sizeof(struct uci_list));
				cwmp_uci_list_init(list);
				snprintf(dup, sizeof(dup), "%s", o->v.string);
				pch = strtok_r(dup, " ", &spch);
				while (pch != NULL) {
					e = icwmp_calloc(1, sizeof(struct uci_element));
					e->name = pch;
					cwmp_uci_list_add(list, &e->list);
					pch = strtok_r(NULL, " ", &spch);
				}
				*value = list;
				return 0;
			default:
				return -1;
			}
		}
	}
	return -1;
}

/*
 * UCI Set option value
 */
int cwmp_uci_set_value_string(char *package, char *section, char *option, char *value, uci_config_paths uci_type)
{
	struct uci_ptr ptr = {0};

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, option, value))
		return UCI_ERR_PARSE;
	if (uci_set(uci_save_conf_paths[uci_type].uci_ctx, &ptr) != UCI_OK)
		return UCI_ERR_NOTFOUND;
	if (ptr.o)
		return UCI_OK;
	return UCI_ERR_NOTFOUND;
}

int cwmp_uci_set_value(char *package, char *section, char *option, char *value)
{
	return cwmp_uci_set_value_string(package, section, option, value, UCI_STANDARD_CONFIG);
}

int cwmp_uci_set_varstate_value(char *package, char*section, char *option, char *value)
{
	return cwmp_uci_set_value_string(package, section, option, value, UCI_VARSTATE_CONFIG);
}

int uci_set_value_by_path(char *path, char *value, uci_config_paths uci_type)
{
	struct uci_ptr ptr;
	int ret = UCI_OK;

	char cmd[256];
	snprintf(cmd, sizeof(cmd), "%s=%s", path, value);
	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, cmd, true) != UCI_OK) {
		return UCI_ERR_PARSE;
	}

	ret = uci_set(uci_save_conf_paths[uci_type].uci_ctx, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(uci_save_conf_paths[uci_type].uci_ctx, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", path);
		return UCI_ERR_NOTFOUND;
	}
	return UCI_OK;
}

int cwmp_uci_set_value_by_path(char *path, char *value)
{
	return uci_set_value_by_path(path, value, UCI_STANDARD_CONFIG);
}

int cwmp_uci_set_varstate_value_by_path(char *path, char *value)
{
	return uci_set_value_by_path(path, value, UCI_VARSTATE_CONFIG);
}

/*
 * UCI Lists
 */
void cwmp_uci_list_init(struct uci_list *ptr)
{
	ptr->prev = ptr;
	ptr->next = ptr;
}

void cwmp_uci_list_insert(struct uci_list *list, struct uci_list *ptr)
{
	list->next->prev = ptr;
	ptr->prev = list;
	ptr->next = list->next;
	list->next = ptr;
}

void cwmp_uci_list_add(struct uci_list *head, struct uci_list *ptr)
{
	cwmp_uci_list_insert(head->prev, ptr);
}

void cwmp_uci_list_del(struct uci_element *e)
{
	struct uci_list *ptr = e->list.prev;
	ptr->next = e->list.next;
}

void cwmp_delete_uci_element_from_list(struct uci_element *e)
{
	cwmp_uci_list_del(e);
	free(e->name);
	free(e);
}

void cwmp_free_uci_list(struct uci_list *list)
{
	struct uci_element *e = NULL, *tmp = NULL;

	uci_foreach_element_safe(list, e, tmp) {
		cwmp_delete_uci_element_from_list(e);
	}
}

char *cwmp_uci_list_to_string(struct uci_list *list, char *delimitor)
{
	if (list && !uci_list_empty(list)) {
		struct uci_element *e = NULL;
		char list_val[512] = { 0 };
		unsigned pos = 0;

		list_val[0] = 0;
		uci_foreach_element(list, e)
		{
			if (e->name)
				pos += snprintf(&list_val[pos], sizeof(list_val) - pos, "%s%s", e->name, delimitor);
		}

		if (pos)
			list_val[pos - 1] = 0;

		return icwmp_strdup(list_val);
	}
	return NULL;
}

int cwmp_uci_get_option_value_list(char *package, char *section, char *option, uci_config_paths uci_type, struct uci_list **value)
{
	struct uci_element *e = NULL;
	struct uci_ptr ptr = {0};
	struct uci_list *list;
	char *pch = NULL, *spch = NULL, *dup = NULL;
	int option_type;
	*value = NULL;

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, option, NULL))
		return UCI_ERR_PARSE;

	if (ptr.o) {
		switch(ptr.o->type) {
			case UCI_TYPE_LIST:
				*value = &ptr.o->v.list;
				option_type = UCI_TYPE_LIST;
				break;
			case UCI_TYPE_STRING:
				if (!ptr.o->v.string || (ptr.o->v.string)[0] == '\0') {
					return UCI_TYPE_STRING;
				}
				list = calloc(1, sizeof(struct uci_list));
				cwmp_uci_list_init(list);
				dup = strdup(ptr.o->v.string);
				pch = strtok_r(dup, " ", &spch);
				while (pch != NULL) {
					e = calloc(1, sizeof(struct uci_element));
					e->name = pch;
					cwmp_uci_list_add(list, &e->list);
					pch = strtok_r(NULL, " ", &spch);
				}
				*value = list;
				option_type = UCI_TYPE_STRING;
				break;
			default:
				return UCI_ERR_NOTFOUND;
		}
	} else {
		return UCI_ERR_NOTFOUND;
	}
	return option_type;
}

int cwmp_uci_get_cwmp_standard_option_value_list(char *package, char *section, char *option, struct uci_list **value)
{
	return cwmp_uci_get_option_value_list(package, section, option, UCI_STANDARD_CONFIG, value);
}

int cwmp_uci_get_cwmp_varstate_option_value_list(char *package, char *section, char *option, struct uci_list **value)
{
	return cwmp_uci_get_option_value_list(package, section, option, UCI_VARSTATE_CONFIG, value);
}

int cwmp_uci_add_list_value(char *package, char *section, char *option, char *value, uci_config_paths uci_type)
{
	struct uci_ptr ptr = {0};
	int error = UCI_OK;

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, option, value))
		return UCI_ERR_PARSE;

	error = uci_add_list(uci_save_conf_paths[uci_type].uci_ctx, &ptr);
	if (error != UCI_OK)
		return error;

	return UCI_OK;
}

int cwmp_uci_del_list_value(char *package, char *section, char *option, char *value, uci_config_paths uci_type)
{
	struct uci_ptr ptr = {0};

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, option, value))
		return -1;

	if (uci_del_list(uci_save_conf_paths[uci_type].uci_ctx, &ptr) != UCI_OK)
		return -1;

	return 0;
}

int uci_add_list_value(char *cmd, uci_config_paths uci_type)
{
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, cmd, true) != UCI_OK) {
		return UCI_ERR_PARSE;
	}
	ret = uci_add_list(uci_save_conf_paths[uci_type].uci_ctx, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(uci_save_conf_paths[uci_type].uci_ctx, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", cmd);
		return UCI_ERR_NOTFOUND;
	}
	return UCI_OK;
}

/*
 * UCI ADD Section
 */

int cwmp_uci_add_section(char *package, char *stype, uci_config_paths uci_type , struct uci_section **s)
{
	struct uci_ptr ptr = {0};
	char fname[128];

	*s = NULL;

	snprintf(fname, sizeof(fname), "%s/%s", uci_save_conf_paths[uci_type].conf_dir, package);

	if (!file_exists(fname)) {
		FILE *fptr = fopen(fname, "w");
		if (fptr)
			fclose(fptr);
		else
			return UCI_ERR_UNKNOWN;
	}

	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, NULL, NULL, NULL) == 0
		&& uci_add_section(uci_save_conf_paths[uci_type].uci_ctx, ptr.p, stype, s) == UCI_OK) {
		CWMP_LOG(INFO, "New uci section %s added successfully", stype);
	}
	else
		return UCI_ERR_NOTFOUND;

	return UCI_OK;
}

struct uci_section* get_section_by_section_name(char *package, char *stype, char* sname, uci_config_paths uci_type)
{
	struct uci_section *s;
	cwmp_uci_foreach_sections(package, stype, uci_type, s) {
		if (strcmp(section_name(s), sname) == 0)
			return s;
	}
 	return NULL;

}

int cwmp_uci_rename_section_by_section(struct uci_section *s, char *value, uci_config_paths uci_type)
{
	struct uci_ptr up = {0};

	if (cwmp_uci_lookup_ptr_by_section(uci_save_conf_paths[uci_type].uci_ctx, &up, s, NULL, value) == -1)
		return UCI_ERR_PARSE;
	if (uci_rename(uci_save_conf_paths[uci_type].uci_ctx, &up) != UCI_OK)
		return UCI_ERR_NOTFOUND;

	return UCI_OK;
}

int cwmp_uci_add_section_with_specific_name(char *package, char *stype, char *section_name, uci_config_paths uci_type)
{
	struct uci_section *s = NULL;

	if (get_section_by_section_name(package, stype, section_name, uci_type) != NULL)
		return UCI_ERR_DUPLICATE;
	if (cwmp_uci_add_section(package, stype, uci_type, &s) != UCI_OK)
		return UCI_ERR_UNKNOWN;

	return cwmp_uci_rename_section_by_section(s, section_name, uci_type);
}

/*
 * UCI Delete Value
 */
int uci_delete_value(char *path, int uci_type)
{
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, path, true) != UCI_OK)
		return CWMP_GEN_ERR;

	ret = uci_delete(uci_save_conf_paths[uci_type].uci_ctx, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(uci_save_conf_paths[uci_type].uci_ctx, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", path);
		return CWMP_GEN_ERR;
	}
	return CWMP_OK;
}

int cwmp_uci_get_section_type(char *package, char *section, uci_config_paths uci_type, char **value)
{
	struct uci_ptr ptr = {0};
	if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, section, NULL, NULL)) {
		*value = "";
		return -1;
	}
	if (ptr.s) {
		*value = icwmp_strdup(ptr.s->type);
	} else {
		*value = "";
	}
	return UCI_OK;
}

struct uci_section *cwmp_uci_walk_section(char *package, char *stype, void *arg1, void *arg2, int cmp, int (*filter)(struct uci_section *s, void *value), struct uci_section *prev_section, uci_config_paths uci_type, int walk)
{
	struct uci_section *s = NULL;
	struct uci_element *e, *m;
	char *value = NULL, *pch = NULL, *spch = NULL;
	char dup[256];
	struct uci_list *list_value, *list_section;
	struct uci_ptr ptr = { 0 };

	if (walk == CWMP_GET_FIRST_SECTION) {
		if (cwmp_uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, NULL, NULL, NULL) != UCI_OK)
			goto end;

		list_section = &(ptr.p)->sections;
		e = list_to_element(list_section->next);
	} else {
		list_section = &prev_section->package->sections;
		e = list_to_element(prev_section->e.list.next);
	}

	while (&e->list != list_section) {
		s = uci_to_section(e);
		if (s && s->type && strcmp(s->type, stype) == 0) {
			switch (cmp) {
			case CWMP_CMP_SECTION:
				goto end;
			case CWMP_CMP_OPTION_EQUAL:
				cwmp_uci_get_value_by_section_string(s, (char *)arg1, &value);
				if (strcmp(value, (char *)arg2) == 0)
					goto end;
				break;
			case CWMP_CMP_OPTION_CONTAINING:
				cwmp_uci_get_value_by_section_string(s, (char *)arg1, &value);
				if (strstr(value, (char *)arg2))
					goto end;
				break;
			case CWMP_CMP_OPTION_CONT_WORD:
				cwmp_uci_get_value_by_section_string(s, (char *)arg1, &value);
				snprintf(dup, sizeof(dup), "%s", value);
				pch = strtok_r(dup, " ", &spch);
				while (pch != NULL) {
					if (strcmp((char *)arg2, pch) == 0)
						goto end;

					pch = strtok_r(NULL, " ", &spch);
				}
				break;
			case CWMP_CMP_LIST_CONTAINING:
				cwmp_uci_get_value_by_section_list(s, (char *)arg1, &list_value);
				if (list_value != NULL) {
					uci_foreach_element(list_value, m)
					{
						if (strcmp(m->name, (char *)arg2) == 0)
							goto end;
					}
				}
				break;
			case CWMP_CMP_FILTER_FUNC:
				if (filter(s, arg1) == 0)
					goto end;
				break;
			default:
				break;
			}
		}
		e = list_to_element(e->list.next);
		s = NULL;
	}
end:
	return s;
}

int cwmp_commit_package(char *package, uci_config_paths uci_type)
{
	struct uci_ptr ptr = { 0 };
	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, true) != UCI_OK) {
		return -1;
	}

	if (uci_commit(uci_save_conf_paths[uci_type].uci_ctx, &ptr.p, false) != UCI_OK) {
		return -1;
	}
	return 0;
}

int cwmp_uci_import(char *package_name, const char *input_path, uci_config_paths uci_type)
{
	struct uci_package *package = NULL;
	struct uci_element *e = NULL;
	int ret = CWMP_OK;
	FILE *input = fopen(input_path, "r");
	if (!input)
		return -1;

	if (uci_import(uci_save_conf_paths[uci_type].uci_ctx, input, package_name, &package, (package_name != NULL)) != UCI_OK) {
		ret = -1;
		goto end;
	}

	uci_foreach_element(&uci_save_conf_paths[uci_type].uci_ctx->root, e)
	{
		struct uci_package *p = uci_to_package(e);
		if (uci_commit(uci_save_conf_paths[uci_type].uci_ctx, &p, true) != UCI_OK)
			ret = CWMP_GEN_ERR;
	}

end:
	fclose(input);
	return ret;
}

int cwmp_uci_export_package(char *package, const char *output_path, uci_config_paths uci_type)
{
	struct uci_ptr ptr = { 0 };
	int ret = 0;
	FILE *out = fopen(output_path, "a");
	if (!out)
		return -1;

	if (uci_lookup_ptr(uci_save_conf_paths[uci_type].uci_ctx, &ptr, package, true) != UCI_OK) {
		ret = -1;
		goto end;
	}

	if (uci_export(uci_save_conf_paths[uci_type].uci_ctx, out, ptr.p, true) != UCI_OK)
		ret = -1;

end:
	fclose(out);
	return ret;
}

int cwmp_uci_export(const char *output_path, uci_config_paths uci_type)
{
	char **configs = NULL;
	char **p;

	if ((uci_list_configs(uci_save_conf_paths[uci_type].uci_ctx, &configs) != UCI_OK))
		return -1;

	for (p = configs; *p; p++)
		cwmp_uci_export_package(*p, output_path, uci_type);

	free(configs);
	return 0;
}
