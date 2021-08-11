/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include <ctype.h>
#include "common.h"
#include "log.h"
#include "cwmp_uci.h"

struct uci_paths uci_save_conf_paths[] = {
		[UCI_STANDARD_CONFIG] = { "/etc/config", "/tmp/.uci" }, [UCI_DB_CONFIG] = { "/lib/db/config", NULL }, [UCI_BOARD_DB_CONFIG] = { "/etc/board-db/config", NULL }, [UCI_VARSTATE_CONFIG] = { NULL, "/var/state" },
};

struct uci_context *cwmp_uci_ctx = ((void *)0);

int cwmp_uci_init(int uci_path_type)
{
	if (cwmp_uci_ctx == NULL) {
		cwmp_uci_ctx = uci_alloc_context();
		if (!cwmp_uci_ctx)
			return -1;
		if (uci_save_conf_paths[uci_path_type].save_dir) {
			uci_add_delta_path(cwmp_uci_ctx, cwmp_uci_ctx->savedir);
			uci_set_savedir(cwmp_uci_ctx, uci_save_conf_paths[uci_path_type].save_dir);
		}
		if (uci_save_conf_paths[uci_path_type].conf_dir)
			uci_set_confdir(cwmp_uci_ctx, uci_save_conf_paths[uci_path_type].conf_dir);
	}
	return 0;
}

void cwmp_uci_exit(void)
{
	if (cwmp_uci_ctx) {
		uci_free_context(cwmp_uci_ctx);
		cwmp_uci_ctx = NULL;
	}
	cwmp_uci_ctx = NULL;
}

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
		return -1;
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
		return -1;
	}
	return UCI_OK;
}

static inline void cwmp_uci_list_init(struct uci_list *ptr)
{
	ptr->prev = ptr;
	ptr->next = ptr;
}

static inline void cwmp_uci_list_insert(struct uci_list *list, struct uci_list *ptr)
{
	list->next->prev = ptr;
	ptr->prev = list;
	ptr->next = list->next;
	list->next = ptr;
}

static inline void cwmp_uci_list_add(struct uci_list *head, struct uci_list *ptr)
{ //
	cwmp_uci_list_insert(head->prev, ptr);
}

void cwmp_uci_list_del(struct uci_element *e)
{
	struct uci_list *ptr = e->list.prev;
	ptr->next = e->list.next;
}

static void cwmp_delete_uci_element_from_list(struct uci_element *e)
{
	cwmp_uci_list_del(e);
	free(e->name);
	free(e);
}

void cwmp_free_uci_list(struct uci_list *list)
{
	struct uci_element *e = NULL, *tmp = NULL;

	uci_foreach_element_safe(list, e, tmp)
		cwmp_delete_uci_element_from_list(e);
}

char *cwmp_uci_list_to_string(struct uci_list *list, char *delimitor)
{
	if (list) {
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
	} else {
		return icwmp_strdup("");
	}
}

int cwmp_uci_get_option_value_string(char *package, char *section, char *option, int uci_type, char **value)
{
	struct uci_ptr ptr = { 0 };
	cwmp_uci_init(uci_type);

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, NULL) != UCI_OK) {
		*value = "";
		cwmp_uci_ctx = NULL;
		return -1;
	}
	if (ptr.o && ptr.o->type == UCI_TYPE_LIST) {
		*value = cwmp_uci_list_to_string(&ptr.o->v.list, " ");
	} else if (ptr.o && ptr.o->v.string) {
		*value = icwmp_strdup(ptr.o->v.string);
	} else {
		*value = icwmp_strdup("");
		cwmp_uci_exit();
		return -1;
	}
	cwmp_uci_exit();
	return 0;
}

char *cwmp_db_get_value_string(char *package, char *section, char *option)
{
	struct uci_option *o = NULL;
	struct uci_element *e;
	struct uci_ptr ptr = { 0 };

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, NULL))
		return "";

	e = ptr.last;
	switch (e->type) {
	case UCI_TYPE_OPTION:
		o = ptr.o;
		break;
	default:
		break;
	}

	if (o)
		return o->v.string ? o->v.string : "";
	else {
		return "";
	}
}

int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct uci_list **value)
{
	struct uci_element *e = NULL;
	struct uci_ptr ptr = {0};
	struct uci_list *list;
	char *pch = NULL, *spch = NULL, *dup = NULL;
	int option_type;
	*value = NULL;

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, NULL))
		return -1;

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
				return -1;
		}
	} else {
		return -1;
	}
	return option_type;
}

int cwmp_uci_get_value_common(char *cmd, char **value, bool state)
{
	struct uci_ptr ptr;
	struct uci_context *c = uci_alloc_context();
	char *s, *t;
	char state_path[32];

	*value = NULL;
	if (!c) {
		CWMP_LOG(ERROR, "Out of memory");
		uci_free_context(c);
		return CWMP_GEN_ERR;
	}
	if (state) {
		CWMP_STRNCPY(state_path, VARSTATE_CONFIG, sizeof(state_path));
		uci_add_delta_path(c, c->savedir);
		uci_set_savedir(c, state_path);
	}
	s = strdup(cmd);
	t = s;
	if (uci_lookup_ptr(c, &ptr, s, true) != UCI_OK) {
		CWMP_LOG(ERROR, "Error occurred in uci %s get %s", state ? "state" : "config", cmd);
		free(t);
		uci_free_context(c);
		return CWMP_GEN_ERR;
	}
	free(t);
	if (ptr.flags & UCI_LOOKUP_COMPLETE) {
		if (ptr.o == NULL || ptr.o->v.string == NULL) {
			CWMP_LOG(INFO, "%s not found or empty value", cmd);
			uci_free_context(c);
			return CWMP_OK;
		}
		*value = strdup(ptr.o->v.string);
	}
	uci_free_context(c);
	return CWMP_OK;
}

int uci_get_state_value(char *cmd, char **value)
{
	int error;
	error = cwmp_uci_get_value_common(cmd, value, true);
	return error;
}

int uci_get_value(char *cmd, char **value)
{
	int error;
	error = cwmp_uci_get_value_common(cmd, value, false);
	return error;
}

int uci_delete_value(char *cmd)
{
	struct uci_context *c = uci_alloc_context();
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (!c) {
		CWMP_LOG(ERROR, "Out of memory");
		return CWMP_GEN_ERR;
	}

	if (uci_lookup_ptr(c, &ptr, cmd, true) != UCI_OK) {
		uci_free_context(c);
		return CWMP_GEN_ERR;
	}
	ret = uci_delete(c, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(c, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", cmd);
		return CWMP_GEN_ERR;
	}
	uci_free_context(c);
	return CWMP_OK;
}

int uci_set_value(char *path, char *value, uci_config_action action)
{
	struct uci_context *c = uci_alloc_context();
	struct uci_ptr ptr;
	int ret = UCI_OK;
	char state_path[32];

	if (!c) {
		CWMP_LOG(ERROR, "Out of memory");
		return CWMP_GEN_ERR;
	}

	if (action == CWMP_CMD_SET_STATE) {
		CWMP_STRNCPY(state_path, VARSTATE_CONFIG, sizeof(state_path));
		uci_add_delta_path(c, c->savedir);
		uci_set_savedir(c, state_path);
	}

	char cmd[256];
	snprintf(cmd, sizeof(cmd), "%s=%s", path, value);

	if (uci_lookup_ptr(c, &ptr, cmd, true) != UCI_OK) {
		uci_free_context(c);
		return CWMP_GEN_ERR;
	}

	ret = uci_set(c, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(c, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", cmd);
		return CWMP_GEN_ERR;
	}
	uci_free_context(c);
	return CWMP_OK;
}

int cwmp_uci_add_list_value(char *package, char *section, char *option, char *value)
{
	struct uci_ptr ptr = {0};

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, value))
		return CWMP_GEN_ERR;

	if (uci_add_list(cwmp_uci_ctx, &ptr) != UCI_OK)
		return CWMP_GEN_ERR;

	return CWMP_OK;
}

int cwmp_uci_del_list_value(char *package, char *section, char *option, char *value)
{
	struct uci_ptr ptr = {0};

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, value))
		return -1;

	if (uci_del_list(cwmp_uci_ctx, &ptr) != UCI_OK)
		return -1;

	return 0;
}

int uci_add_list_value(char *cmd)
{
	struct uci_context *c = uci_alloc_context();
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (!c) {
		CWMP_LOG(ERROR, "Out of memory");
		return CWMP_GEN_ERR;
	}

	if (uci_lookup_ptr(c, &ptr, cmd, true) != UCI_OK) {
		uci_free_context(c);
		return CWMP_GEN_ERR;
	}
	ret = uci_add_list(c, &ptr);

	if (ret == UCI_OK) {
		ret = uci_save(c, ptr.p);
	} else {
		CWMP_LOG(ERROR, "UCI delete not succeed %s", cmd);
		return CWMP_GEN_ERR;
	}
	uci_free_context(c);
	return CWMP_OK;
}

int cwmp_uci_get_section_type(char *package, char *section, char **value)
{
	struct uci_ptr ptr = {0};
	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, NULL, NULL)) {
		*value = "";
		return -1;
	}
	if (ptr.s) {
		*value = icwmp_strdup(ptr.s->type);
	} else {
		*value = "";
		return -1;
	}
	return 0;
}

char *cwmp_uci_add_section(char *package, char *stype, struct uci_section **s)
{
	struct uci_ptr ptr = {0};
	char fname[128], *val = "";
	*s = NULL;

	snprintf(fname, sizeof(fname), "/etc/config/%s", package);
	if (!file_exists(fname)) {
		FILE *fptr = fopen(fname, "w");
		if (fptr)
			fclose(fptr);
		else {
			return val;
		}
	}
	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, NULL, NULL, NULL) == 0
		&& uci_add_section(cwmp_uci_ctx, ptr.p, stype, s) == UCI_OK) {
		val = icwmp_strdup((*s)->e.name);
	}
	return val;
}

int cwmp_uci_get_value_by_section_string(struct uci_section *s, char *option, char **value)
{
	struct uci_element *e;
	struct uci_option *o;

	if (s == NULL || option == NULL)
		goto not_found;

	uci_foreach_element(&s->options, e)
	{
		o = (uci_to_option(e));
		if (!strcmp(o->e.name, option)) {
			if (o->type == UCI_TYPE_LIST) {
				*value = cwmp_uci_list_to_string(&o->v.list, " ");
			} else {
				*value = o->v.string ? icwmp_strdup(o->v.string) : icwmp_strdup("");
			}
			return 0;
		}
	}

not_found:
	*value = icwmp_strdup("");
	return -1;
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

struct uci_section *cwmp_uci_walk_section(char *package, char *stype, void *arg1, void *arg2, int cmp, int (*filter)(struct uci_section *s, void *value), struct uci_section *prev_section, int walk)
{
	struct uci_section *s = NULL;
	struct uci_element *e, *m;
	char *value = NULL, *pch = NULL, *spch = NULL;
	char dup[256];
	struct uci_list *list_value, *list_section;
	struct uci_ptr ptr = { 0 };

	if (walk == CWMP_GET_FIRST_SECTION) {
		if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, NULL, NULL, NULL) != UCI_OK)
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

int cwmp_commit_package(char *package)
{
	struct uci_ptr ptr = { 0 };
	if (uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, true) != UCI_OK) {
		return -1;
	}

	if (uci_commit(cwmp_uci_ctx, &ptr.p, false) != UCI_OK) {
		return -1;
	}
	return 0;
}

int cwmp_uci_import(char *package_name, const char *input_path)
{
	struct uci_package *package = NULL;
	struct uci_element *e = NULL;
	int ret = CWMP_OK;
	struct uci_context *c = uci_alloc_context();
	FILE *input = fopen(input_path, "r");
	if (!input) {
		uci_free_context(c);
		return -1;
	}

	if (uci_import(c, input, package_name, &package, (package_name != NULL)) != UCI_OK) {
		ret = -1;
		goto end;
	}

	uci_foreach_element(&c->root, e)
	{
		struct uci_package *p = uci_to_package(e);
		if (uci_commit(c, &p, true) != UCI_OK)
			ret = CWMP_GEN_ERR;
	}

end:
	fclose(input);
	uci_free_context(c);
	return ret;
}

int cwmp_uci_export_package(char *package, const char *output_path)
{
	struct uci_ptr ptr = { 0 };
	int ret = 0;
	struct uci_context *c = uci_alloc_context();
	FILE *out = fopen(output_path, "a");
	if (!out) {
		uci_free_context(c);
		return -1;
	}

	if (uci_lookup_ptr(c, &ptr, package, true) != UCI_OK) {
		ret = -1;
		goto end;
	}

	if (uci_export(c, out, ptr.p, true) != UCI_OK)
		ret = -1;

end:
	fclose(out);
	uci_free_context(c);
	return ret;
}

int cwmp_uci_export(const char *output_path)
{
	char **configs = NULL;
	char **p;

	struct uci_context *c = uci_alloc_context();
	if ((uci_list_configs(c, &configs) != UCI_OK) || !configs) {
		uci_free_context(c);
		return -1;
	}

	for (p = configs; *p; p++)
		cwmp_uci_export_package(*p, output_path);

	free(configs);
	uci_free_context(c);
	return 0;
}
