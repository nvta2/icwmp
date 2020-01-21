/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*				Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include "common.h"

static pathnode *head = NULL;
static pathnode *temphead = NULL;

void add_list_value_change(char *param_name, char *param_data, char *param_type)
{
	bulkdata_log(SDEBUG,"Dummy funtion");
	if(param_name && param_data && param_type){}
}

void send_active_value_change(void)
{
	bulkdata_log(SDEBUG,"Dummy funtion");
}

int bulkdata_dm_ctx_init(struct dmctx *ctx)
{
	struct bulkdata *bulkdata = &bulkdata_main;
	dm_ctx_init(ctx, DM_CWMP, bulkdata->amd_version, bulkdata->instance_mode);
	return 0;
}

int bulkdata_dm_ctx_clean(struct dmctx *ctx)
{
	dm_ctx_clean(ctx);
	return 0;
}

static char **str_split(const char* str, const char* delim, size_t* numtokens)
{
	char *s = strdup(str);
	size_t tokens_alloc = 1;
	size_t tokens_used = 0;
	char **tokens = calloc(tokens_alloc, sizeof(char*));
	char *token, *strtok_ctx;
	for (token = strtok_r(s, delim, &strtok_ctx); token != NULL; token = strtok_r(NULL, delim, &strtok_ctx)) {
		if (tokens_used == tokens_alloc) {
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(char*));
		}
		tokens[tokens_used++] = strdup(token);
	}
	// cleanup
	if (tokens_used == 0) {
		FREE(tokens);
	} else {
		tokens = realloc(tokens, tokens_used * sizeof(char*));
	}
	*numtokens = tokens_used;
	FREE(s);
	return tokens;
}

static bool match(const char *string, const char *pattern)
{
	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) return 0;
	int status = regexec(&re, string, 0, NULL, 0);
	regfree(&re);
	if (status != 0) return false;
	return true;
}

static bool is_res_required(char *str, int *start, int *len)
{
	char temp_char[NAME_MAX] = {'\0'};

	if (match(str, GLOB_CHAR)) {
		int s_len = strlen(str);
		int b_len = s_len, p_len = s_len;

		char *star = strchr(str, '*');
		if(star)
			s_len = star - str;
		*start = MIN(MIN(s_len, p_len), b_len);
		if (*start == s_len)
			*len = 1;

		strncpy(temp_char, str+*start, *len);

		if(match(temp_char, "[*+]+"))
			return true;
	}
	*start = strlen(str);
	return false;
}

static void insert(char *data, bool active)
{
	pathnode *link = (pathnode*) calloc(1, sizeof(pathnode));
	if(!link) {
		return;
	}

	link->ref_path = data;

	if(active) {
		link->next = head;
		head = link;
	} else {
		link->next = temphead;
		temphead = link;
	}
}

static void swap_heads()
{
	pathnode *temp = head;
	head = temphead;
	temphead = temp;
}

static void deleteList()
{
	pathnode *ptr = head, *temp;
	while(ptr != NULL) {
		temp = ptr;
		free(ptr->ref_path);
		if(ptr->next != NULL) {
			ptr = ptr->next;
		} else {
			ptr = NULL;
		}
		free(temp);
	}
	head = NULL;
	swap_heads();
}

void bulkdata_add_data_to_list(struct list_head *dup_list, char *name, char *value, char *type)
{
	struct resultsnode *link;
	link = calloc(1, sizeof(struct resultsnode));
	list_add_tail(&link->list, dup_list);
	link->name = strdup(name);
	link->data = strdup(value);
	link->type = strdup(type);
}

void bulkdata_delete_data_from_list(struct resultsnode *link)
{
	list_del(&link->list);
	FREE(link->name);
	FREE(link->data);
	FREE(link->type);
	FREE(link);
}

void bulkdata_free_data_from_list(struct list_head *dup_list)
{
	struct resultsnode *link;
	while (dup_list->next != dup_list) {
		link = list_entry(dup_list->next, struct resultsnode, list);
		bulkdata_delete_data_from_list(link);
	}
}

static bool bulkdata_get(int operation, char *path, struct dmctx *dm_ctx)
{
	int fault = 0;

	switch(operation) {
		case CMD_GET_NAME:
			fault = dm_entry_param_method(dm_ctx, CMD_GET_NAME, path, "true", NULL);
			break;
		case CMD_GET_VALUE:
			fault = dm_entry_param_method(dm_ctx, CMD_GET_VALUE, path, NULL, NULL);
			break;
		default:
			return false;
	}

	if (dm_ctx->list_fault_param.next != &dm_ctx->list_fault_param) {
		return false;
	}
	if (fault) {
		return false;
	}
	return true;
}

char *bulkdata_get_value_param(char *path)
{
	struct dmctx ctx = {0};
	struct dm_parameter *n;
	char *value = NULL;

	bulkdata_dm_ctx_init(&ctx);
	if(bulkdata_get(CMD_GET_VALUE, path, &ctx)) {
		list_for_each_entry(n, &ctx.list_parameter, list) {
			value = strdup(n->data);
			break;
		}
	}
	bulkdata_dm_ctx_clean(&ctx);
	return value;
}

void bulkdata_get_value(char *path, struct list_head *list)
{
	struct dmctx ctx = {0};
	struct dm_parameter *n;

	bulkdata_dm_ctx_init(&ctx);
	if(bulkdata_get(CMD_GET_VALUE, path, &ctx)) {
		list_for_each_entry(n, &ctx.list_parameter, list) {
			bulkdata_add_data_to_list(list, n->name, n->data, n->type);
		}
	}
	bulkdata_dm_ctx_clean(&ctx);
}

bool bulkdata_get_name(char *path)
{
	struct dmctx ctx = {0};
	struct dm_parameter *n;
	bool ret = false;

	bulkdata_dm_ctx_init(&ctx);
	if(bulkdata_get(CMD_GET_NAME, path, &ctx)) {
		list_for_each_entry(n, &ctx.list_parameter, list) {
			insert(strdup(n->name), false);
		}
		ret = true;
	}
	bulkdata_dm_ctx_clean(&ctx);
	return ret;
}

static void fill_node_path()
{
	pathnode *p=head;
	while(p!=NULL) {
		bulkdata_get_name(p->ref_path);
		p=p->next;
	}
	deleteList();
}

static void bulkdata_filter_results(char *path, int start, int end)
{
	int startpos = start, m_index = 0, m_len = 0;
	char *pp = path + startpos;
	char exp[NAME_MAX] = {'\0'};

	if(start >= end) {
		return;
	}

	if(!is_res_required(pp, &m_index, &m_len)) {
		//append rest of the path to the final list
		if(pp == path ) {
			insert(strdup(pp), true);
			return;
		}

		pathnode *p = head;
		while(p != NULL) {
			char name[NAME_MAX] = {'\0'};
			strcpy(name, p->ref_path);
			strcat(name, pp);
			insert(strdup(name), false);
			p = p->next;
		}
		deleteList();
		return;
	}

	// Get the string before the match
	char name[NAME_MAX]={'\0'};
	strncpy(name, pp, m_index);

	pathnode *p = head;
	if(p == NULL) {
		insert(strdup(name), false);
	}

	while(p != NULL) {
		char ref_name[NAME_MAX] = {'\0'};
		sprintf(ref_name, "%s%s", p->ref_path, name);
		insert(strdup(ref_name), false);
		p = p->next;
	}
	deleteList();

	startpos += m_index;
	strncpy(exp, pp+m_index, m_len);
	pp = path + startpos;
	fill_node_path();
	startpos += 2;
	bulkdata_filter_results(path, startpos, end);
}

static void bulkdata_parse_results(struct list_head *list)
{
	pathnode *p = head;
	while(p != NULL) {
		bulkdata_get_value(p->ref_path, list);
		p = p->next;
	}
	deleteList();
}

void bulkdata_get_value_results(char *path, struct list_head *list)
{
	bulkdata_filter_results(path, 0, strlen(path));
	bulkdata_parse_results(list);
}

char *create_request_url(struct profile *profile)
{
	int i = 0, http_uri_number = profile->profile_http_request_uri_parameter_number;
	char *value, *uri_param = NULL, *uri_tmp = NULL, *http_url = NULL; 

	for (i = 0; i < http_uri_number; i++)
	{
		if((profile->profile_http_uri_parameter[i].reference == NULL) || (profile->profile_http_uri_parameter[i].name == NULL))
			continue;
		value = bulkdata_get_value_param(profile->profile_http_uri_parameter[i].reference);
		if(!uri_param) {
			asprintf(&uri_param, "&%s=%s", profile->profile_http_uri_parameter[i].name, value);
			free(value);
		}
		else {
			uri_tmp = strdup(uri_param);
			free(uri_param);
			asprintf(&uri_param, "%s&%s=%s", uri_tmp, profile->profile_http_uri_parameter[i].name, value);
			free(value);
			free(uri_tmp);
		}
	}
	if(uri_param) {
		asprintf(&http_url, "%s%s", profile->http_url, uri_param);
		free(uri_param);
	} else {
		asprintf(&http_url, "%s", profile->http_url);
	}
	return http_url;
}

char *get_bulkdata_profile_parameter_name(char *paramref, char *paramname, char *param)
{
	char **paramarr, *idx1 = NULL, *idx2 = NULL, *res = NULL, *instance = NULL, *tmp = NULL, *retparam = NULL, *s = NULL;
	int i, j = 0;
	size_t length;

	if(paramname == NULL || strlen(paramname) <= 0)
		return strdup(param);
	paramarr = str_split(paramref, "*", &length);
	res = strdup(paramname);
	for(i = 0; i < length; i++) {
		if(i == length - 1)
			break;
		j++;
		idx1 = strstr(param, paramarr[i]);
		idx2 = strstr(param, paramarr[i+1]);
		instance = (char*)calloc(idx2 - idx1 - strlen(paramarr[i]) + 1, sizeof(char));
		memcpy(instance, idx1 + strlen(paramarr[i]), idx2 - idx1 - strlen(paramarr[i]));
		tmp = strdup(res);
		FREE(res);
		asprintf(&res, "%s.%s", tmp, instance);
		FREE(tmp);
		FREE(instance);
	}
	if ((s = strstr(param,paramarr[j]) ) != NULL && strlen(s) == strlen(paramarr[j]))
		asprintf(&retparam, "%s", res);
	else
		asprintf(&retparam, "%s.%s", res, strstr(param, paramarr[j]) + strlen(paramarr[j]));

	FREE(res);
	for(int k = 0; k < length; k++)
		FREE(paramarr[k]);
	FREE(paramarr);

	return retparam;
}

void append_string_to_string(char *strappend, char **target)
{
	char *tmp = NULL, *str = NULL;

	if(strappend == NULL || strlen(strappend) <= 0)
		return;
	if(*target == NULL || strlen(*target) <= 0) {
		*target = strdup(strappend);
		return;
	} else {
		tmp = strdup(*target);
		FREE(*target);
	}
	asprintf(target, "%s%s", tmp, strappend);
	FREE(tmp);
}

void bulkdata_add_failed_report(struct profile *profile, char *freport)
{
	struct failed_reports *report, *retreport, *rtmp;

	if(profile->nbre_failed_reports < profile->nbre_of_retained_failed_reports || profile->nbre_of_retained_failed_reports < 0) {
		profile->nbre_failed_reports++;
	} else {
		list_for_each_entry_safe(retreport, rtmp, profile->failed_reports, list) {
			bulkdata_delete_failed_report(retreport);
			break;
		}
	}
	report = calloc(1, sizeof(struct failed_reports));
	list_add_tail(&report->list, profile->failed_reports);
	report->freport= strdup(freport);
}

void bulkdata_delete_failed_report(struct failed_reports *report)
{
	if(report != NULL) {
		list_del(&report->list);
		FREE(report->freport);
		FREE(report);
	}
}

struct failed_reports* empty_failed_reports_list(struct profile *profile)
{
	struct failed_reports *report, *rtmp;

	if(list_empty(profile->failed_reports))
		return NULL;
	list_for_each_entry_safe(report, rtmp, profile->failed_reports, list) {
		list_del(&report->list);
		FREE(report->freport);
		FREE(report);
	}
	return NULL;
}

void add_failed_reports_to_report_csv(struct profile *profile, char **report, int isnext)
{
	struct failed_reports *retreport = NULL;
	int j = 0;

	if(list_empty(profile->failed_reports))
		return;
	list_for_each_entry(retreport, profile->failed_reports, list) {
		if(!j && isnext) {
			j = 1;
			continue;
		}
		append_string_to_string(retreport->freport, report);
	}
}
