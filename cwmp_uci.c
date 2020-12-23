/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include "common.h"
#include "log.h"
#include "cwmp_uci.h"

struct uci_paths uci_save_conf_paths[] = {
		[UCI_STANDARD_CONFIG] ={"/etc/config","/tmp/.uci"},
		[UCI_DB_CONFIG] = {"/lib/db/config", NULL},
		[UCI_BOARD_DB_CONFIG] = {"/etc/board-db/config", NULL},
		[UCI_VARSTATE_CONFIG] = {"/var/state", NULL},
		[UCI_BBFDM_CONFIG] = {"/etc/bbfdm", "/tmp/.bbfdm"}
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
	if (cwmp_uci_ctx)
		uci_free_context(cwmp_uci_ctx);
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
	if (ptr->section &&  !cwmp_check_section_name(ptr->section , true))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	/*option*/
	if (!option || !option[0]) {
		ptr->target = UCI_TYPE_SECTION;
		goto lookup;
	}
	ptr->target = UCI_TYPE_OPTION;
	ptr->option = option;

lookup:
	if (uci_lookup_ptr(ctx, ptr, NULL, true) != UCI_OK || !UCI_LOOKUP_COMPLETE) {
		return -1;
	}
	return 0;
}

char* cwmp_db_get_value_string(char *package, char *section, char *option)
{
	cwmp_uci_ctx = uci_alloc_context();

	struct uci_option *o = NULL;
	struct uci_element *e;
	struct uci_ptr ptr = {0};

	cwmp_uci_ctx->confdir = LIB_DB_CONFIG;

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, NULL))
		return "";

	e = ptr.last;
	switch(e->type) {
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

int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct list_head *list)
{
	struct uci_element *e;
	struct uci_ptr ptr = {0};
	cwmp_uci_ctx = uci_alloc_context();
	struct config_uci_list      *uci_list_elem;
	int size = 0;
	//*value = NULL;

	if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, section, option, NULL)) {
		uci_free_context(cwmp_uci_ctx);
		return -1;
	}

	if (ptr.o == NULL) {
		uci_free_context(cwmp_uci_ctx);
		return -1;
	}

    if (ptr.o->type == UCI_TYPE_LIST) {
        uci_foreach_element(&ptr.o->v.list, e)
        {
            if((e != NULL)&&(e->name))
            {
                uci_list_elem = calloc(1,sizeof(struct config_uci_list));
                if(uci_list_elem == NULL)
                {
                    uci_free_context(cwmp_uci_ctx);
                    return -1;
                }
                uci_list_elem->value = strdup(e->name);
                list_add_tail (&(uci_list_elem->list), list);
                size++;
            }
            else
            {
                uci_free_context(cwmp_uci_ctx);
                return size;
            }
        }
	}

    uci_free_context(cwmp_uci_ctx);
	return size;
}

int uci_get_list_value(char *cmd, struct list_head *list)
{
    struct  uci_ptr             ptr;
    struct  uci_context         *c = uci_alloc_context();
    struct uci_element          *e;
    struct config_uci_list      *uci_list_elem;
    char                        *s,*t;
    int                         size = 0;

    if (!c)
    {
        CWMP_LOG(ERROR, "Out of memory");
        return size;
    }

    s = strdup(cmd);
    t = s;
    if (uci_lookup_ptr(c, &ptr, s, true) != UCI_OK)
    {
        CWMP_LOG(ERROR, "Invalid uci command path: %s",cmd);
        free(t);
        uci_free_context(c);
        return size;
    }

    if(ptr.o == NULL)
    {
        free(t);
        uci_free_context(c);
        return size;
    }

    if(ptr.o->type == UCI_TYPE_LIST)
    {
        uci_foreach_element(&ptr.o->v.list, e)
        {
            if((e != NULL)&&(e->name))
            {
                uci_list_elem = calloc(1,sizeof(struct config_uci_list));
                if(uci_list_elem == NULL)
                {
                    free(t);
                    uci_free_context(c);
                    return CWMP_GEN_ERR;
                }
                uci_list_elem->value = strdup(e->name);
                list_add_tail (&(uci_list_elem->list), list);
                size++;
            }
            else
            {
                free(t);
                uci_free_context(c);
                return size;
            }
        }
    }
    free(t);
    uci_free_context(c);
    return size;
}

int uci_get_value_common(char *cmd,char **value,bool state)
{
    struct  uci_ptr             ptr;
    struct  uci_context         *c = uci_alloc_context();
    char                        *s,*t;
    char                        state_path[32];

    *value = NULL;
    if (!c)
    {
        CWMP_LOG(ERROR, "Out of memory");
        return CWMP_GEN_ERR;
    }
    if (state)
    {
        strcpy(state_path,VARSTATE_CONFIG);
        uci_add_delta_path(c, c->savedir);
        uci_set_savedir(c, state_path);
    }
    s = strdup(cmd);
    t = s;
    if (uci_lookup_ptr(c, &ptr, s, true) != UCI_OK)
    {
        CWMP_LOG(ERROR, "Error occurred in uci %s get %s",state?"state":"config",cmd);
        free(t);
        uci_free_context(c);
        return CWMP_GEN_ERR;
    }
    free(t);
    if(ptr.flags & UCI_LOOKUP_COMPLETE)
    {
        if (ptr.o==NULL || ptr.o->v.string==NULL)
        {
            CWMP_LOG(INFO, "%s not found or empty value",cmd);
            uci_free_context(c);
            return CWMP_OK;
        }
        *value = strdup(ptr.o->v.string);
    }
    uci_free_context(c);
    return CWMP_OK;
}

int uci_get_state_value(char *cmd,char **value)
{
    int error;
    error = uci_get_value_common (cmd,value,true);
    return error;
}

int uci_get_value(char *cmd,char **value)
{
    int error;
    error = uci_get_value_common (cmd,value,false);
    return error;
}

static int uci_action_value_common(char *cmd, uci_config_action action)
{
    int                         ret = UCI_OK;
    char                        *s,*t;
    struct uci_context          *c = uci_alloc_context();
    struct uci_ptr              ptr;
    char                        state_path[32];

    if (!c)
    {
        CWMP_LOG(ERROR, "Out of memory");
        return CWMP_GEN_ERR;
    }

    if (action == CWMP_CMD_SET_STATE)
    {
        strcpy(state_path,VARSTATE_CONFIG);
        uci_add_delta_path(c, c->savedir);
        uci_set_savedir(c, state_path);
    }

    s = strdup(cmd);
    t = s;

    if (uci_lookup_ptr(c, &ptr, s, true) != UCI_OK)
    {
        free(t);
        uci_free_context(c);
        return CWMP_GEN_ERR;
    }
    switch (action)
    {
        case CWMP_CMD_SET:
        case CWMP_CMD_SET_STATE:
            ret = uci_set(c, &ptr);
            break;
        case CWMP_CMD_DEL:
            ret = uci_delete(c, &ptr);
            break;
        case CWMP_CMD_ADD_LIST:
            ret = uci_add_list(c, &ptr);
            break;
    }
    if (ret == UCI_OK)
    {
        ret = uci_save(c, ptr.p);
    }
    else
    {
        CWMP_LOG(ERROR, "UCI %s %s not succeed %s",action==CWMP_CMD_SET_STATE?"state":"config",action==CWMP_CMD_DEL?"delete":"set",cmd);
    }
    free(t);
    uci_free_context(c);
    return CWMP_OK;
}

int uci_delete_value(char *cmd)
{
    int error;
    error = uci_action_value_common (cmd,CWMP_CMD_DEL);
    return error;
}

int uci_set_value(char *cmd)
{
    int error;
    error = uci_action_value_common (cmd,CWMP_CMD_SET);
    return error;
}

int uci_set_state_value(char *cmd)
{
    int error;
    error = uci_action_value_common (cmd,CWMP_CMD_SET_STATE);
    return error;
}

int uci_add_list_value(char *cmd)
{
    int error;
    error = uci_action_value_common (cmd,CWMP_CMD_ADD_LIST);
    return error;
}

static inline void cwmp_uci_list_insert(struct uci_list *list, struct uci_list *ptr)
{
	list->next->prev = ptr;
	ptr->prev = list;
	ptr->next = list->next;
	list->next = ptr;
}

static inline void cwmp_uci_list_add(struct uci_list *head, struct uci_list *ptr)
{
	cwmp_uci_list_insert(head->prev, ptr);
}

char *cwmp_uci_list_to_string(struct uci_list *list, char *delimitor)
{
	struct uci_element *e = NULL;
	char val[512] = {0};
	int del_len = strlen(delimitor);

	if (list) {
		uci_foreach_element(list, e) {
			int len = strlen(val);
			if (len != 0) {
				memcpy(val + len, delimitor, del_len);
				strcpy(val + len + del_len, e->name);
			} else
				strcpy(val, e->name);
		}
		return (strdup(val));
	} else {
		return "";
	}
}

int cwmp_uci_get_value_by_section_string(struct uci_section *s, char *option, char **value)
{
	struct uci_element *e;
	struct uci_option *o;

	if (s == NULL || option == NULL)
		goto not_found;

	uci_foreach_element(&s->options, e) {
		o = (uci_to_option(e));
		if (!strcmp(o->e.name, option)) {
			if (o->type == UCI_TYPE_LIST) {
				*value = cwmp_uci_list_to_string(&o->v.list, " ");
			} else {
				*value = o->v.string ? strdup(o->v.string) : "";
			}
			return 0;
		}
	}

	not_found:
		*value = "";
		return -1;
}

static inline void cwmp_uci_list_init(struct uci_list *ptr)
{
	ptr->prev = ptr;
	ptr->next = ptr;
}

int cwmp_uci_get_value_by_section_list(struct uci_section *s, char *option, struct uci_list **value)
{
	struct uci_element *e;
	struct uci_option *o;
	struct uci_list *list;
	char *pch = NULL, *spch = NULL, *dup;

	*value = NULL;

	if (s == NULL || option == NULL)
		return -1;

	uci_foreach_element(&s->options, e) {
		o = (uci_to_option(e));
		if (strcmp(o->e.name, option) == 0) {
			switch(o->type) {
				case UCI_TYPE_LIST:
					*value = &o->v.list;
					return 0;
				case UCI_TYPE_STRING:
					if (!o->v.string || (o->v.string)[0] == '\0')
						return 0;
					list = calloc(1, sizeof(struct uci_list));
					cwmp_uci_list_init(list);
					dup = strdup(o->v.string);
					pch = strtok_r(dup, " ", &spch);
					while (pch != NULL) {
						e = calloc(1, sizeof(struct uci_element));
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
struct uci_section *cwmp_uci_walk_section (char *package, char *stype, void *arg1, void *arg2, int cmp , int (*filter)(struct uci_section *s, void *value), struct uci_section *prev_section, int walk)
{
	struct uci_section *s = NULL;
	struct uci_element *e, *m;
	char *value, *dup, *pch, *spch;
	struct uci_list *list_value, *list_section;
	struct uci_ptr ptr = {0};

	if (walk == CWMP_GET_FIRST_SECTION) {
		if (cwmp_uci_lookup_ptr(cwmp_uci_ctx, &ptr, package, NULL, NULL, NULL) != UCI_OK)
			goto end;

		list_section = &(ptr.p)->sections;
		e = list_to_element(list_section->next);
	} else {
		list_section = &prev_section->package->sections;
		e = list_to_element(prev_section->e.list.next);
	}

	while(&e->list != list_section) {
		s = uci_to_section(e);
		if (strcmp(s->type, stype) == 0) {
			switch(cmp) {
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
					dup = strdup(value);
					pch = strtok_r(dup, " ", &spch);
					while (pch != NULL) {
						if (strcmp((char *)arg2, pch) == 0) {
							FREE(dup);
							goto end;
						}
						pch = strtok_r(NULL, " ", &spch);
					}
					FREE(dup);
					break;
				case CWMP_CMP_LIST_CONTAINING:
					cwmp_uci_get_value_by_section_list(s, (char *)arg1, &list_value);
					if (list_value != NULL) {
						uci_foreach_element(list_value, m) {
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
	FREE(value);
	return s;
}
