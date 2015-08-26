/*
 *	
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *	  Author MOHAMED Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author Feten Besbes <feten.besbes@pivasoftware.com>
 *
 */

#include <uci.h>
#include "dmuci.h"
#include "dmcwmp.h"
#include "root.h"
#include "times.h"
#include "upnp.h"
#include "landevice.h"
#include "wandevice.h"
#include "deviceinfo.h"
#include "managementserver.h"
#include "x_inteno_se_igmp.h"
#include "x_inteno_se_wifi.h"
#include "x_inteno_se_ice.h"
#include "x_inteno_se_power_mgmt.h"
#include "x_inteno_se_ipacccfg.h"

static char *get_parameter_notification (char *param);
static int remove_parameter_notification(char *param);
static int set_parameter_notification(char *param, char *value);
static int check_param_prefix (struct dmctx *ctx);
static int check_obj_is_nl1(char *refparam, char *inparam, int ndot);
static int get_value_obj(DMOBJECT_API_ARGS);
static int get_value_inparam_isobj_check_obj(DMOBJECT_API_ARGS);
static int get_value_inparam_isparam_check_obj(DMOBJECT_API_ARGS);
static int get_value_param(DMPARAM_API_ARGS);
static int get_value_inparam_isparam_check_param(DMPARAM_API_ARGS);
static int get_value_inparam_isobj_check_param(DMPARAM_API_ARGS);
static int get_name_obj(DMOBJECT_API_ARGS);
static int get_name_inparam_isparam_check_obj(DMOBJECT_API_ARGS);
static int get_name_inparam_isobj_check_obj(DMOBJECT_API_ARGS);
static int get_name_emptyin_nl1_obj(DMOBJECT_API_ARGS);
static int get_name_param(DMPARAM_API_ARGS);
static int get_name_inparam_isparam_check_param(DMPARAM_API_ARGS);
static int get_name_inparam_isobj_check_param(DMPARAM_API_ARGS);
static int get_name_emptyin_nl1_param(DMPARAM_API_ARGS);
static int get_notification_obj(DMOBJECT_API_ARGS);
static int get_notification_inparam_isparam_check_obj(DMOBJECT_API_ARGS);
static int get_notification_inparam_isobj_check_obj(DMOBJECT_API_ARGS);
static int get_notification_param(DMPARAM_API_ARGS);
static int get_notification_inparam_isparam_check_param(DMPARAM_API_ARGS);
static int get_notification_inparam_isobj_check_param(DMPARAM_API_ARGS);
static int inform_check_obj(DMOBJECT_API_ARGS);
static int inform_check_param(DMPARAM_API_ARGS);
static int add_object_obj(DMOBJECT_API_ARGS);
static int add_object_param(DMPARAM_API_ARGS);
static int delete_object_obj(DMOBJECT_API_ARGS);
static int delete_object_param(DMPARAM_API_ARGS);
static int set_value_check_obj(DMOBJECT_API_ARGS);
static int set_value_check_param(DMPARAM_API_ARGS);
static int set_notification_check_obj(DMOBJECT_API_ARGS);
static int set_notification_check_param(DMPARAM_API_ARGS);

struct notification notifications[] = {
	[0] = {"0", "disabled"},
	[1] = {"1", "passive"},
	[2] = {"2", "active"}
};

const struct prefix_method prefix_methods[] = {
	{ DMROOT, 1, &entry_method_root },
	{ DMROOT"DeviceInfo.", 1, &entry_method_root_DeviceInfo },
	{ DMROOT"ManagementServer.", 1, &entry_method_root_ManagementServer },
	{ DMROOT"X_INTENO_SE_PowerManagement.", 0, &entry_method_root_X_INTENO_SE_PowerManagement },
	{ DMROOT"LANDevice.", 0, &entry_method_root_LANDevice },
	{ DMROOT"WANDevice.", 1, &entry_method_root_WANDevice },
	{ DMROOT"X_INTENO_SE_IGMP.", 0, &entry_method_root_X_INTENO_SE_IGMP },
	{ DMROOT"X_INTENO_SE_Wifi.", 0, &entry_method_root_SE_Wifi },
	{ DMROOT"X_INTENO_SE_ICE.", 0, &entry_method_root_X_INTENO_SE_Ice },
	{ DMROOT"UPnP.", 0, &entry_method_root_upnp },
	{ DMROOT"Time.", 0, &entry_method_root_Time },
	{ DMROOT"X_INTENO_SE_IpAccCfg.", 0, &entry_method_root_X_INTENO_SE_IpAccCfg },
	//{ DMROOT"Layer2Bridging.", &entry_method_root_Layer2Bridging },
};

char *update_instance(struct uci_section *s, char *last_inst, char *inst_opt)
{
	char *instance;
	char buf[8] = {0};

	dmuci_get_value_by_section_string(s, inst_opt, &instance);
	if (instance[0] == '\0') {
		if (last_inst == NULL)
			sprintf(buf, "%d", 1);
		else
			sprintf(buf, "%d", atoi(last_inst)+1);
		instance = dmuci_set_value_by_section(s, inst_opt, buf);
	}
	return instance;
}

int get_empty(char *refparam, struct dmctx *args, char **value)
{
	*value = "";
	return 0;
}

void add_list_paramameter(struct dmctx *ctx, char *param_name, char *param_data, char *param_type)
{
	struct dm_parameter *dm_parameter;
	struct list_head *ilist;
	list_for_each(ilist, &ctx->list_parameter) {
		dm_parameter = list_entry(ilist, struct dm_parameter, list);
		int cmp = strcmp(dm_parameter->name, param_name);
		if (cmp == 0) {
			return;
		} else if (cmp>0) {
			break;
		}
	}
	dm_parameter = dmcalloc(1, sizeof(struct dm_parameter));
	_list_add(&dm_parameter->list, ilist->prev, ilist);
	dm_parameter->name = param_name;
	dm_parameter->data = param_data ? param_data : ""; //allocate memory in function
	dm_parameter->type = param_type;
}

void del_list_parameter(struct dm_parameter *dm_parameter)
{
	list_del(&dm_parameter->list);
	dmfree(dm_parameter->name);
	dmfree(dm_parameter);
}

void free_all_list_parameter(struct dmctx *ctx)
{
	struct dm_parameter *dm_parameter;
	while (ctx->list_parameter.next != &ctx->list_parameter) {
		dm_parameter = list_entry(ctx->list_parameter.next, struct dm_parameter, list);
		del_list_parameter(dm_parameter);
	}
}

void add_set_list_tmp(struct dmctx *ctx, char *param, char *value)
{
	struct set_tmp *set_tmp;
	set_tmp = dmcalloc(1, sizeof(struct set_tmp));
	list_add_tail(&set_tmp->list, &ctx->set_list_tmp);
	set_tmp->name = dmstrdup(param);
	set_tmp->value = dmstrdup(value);
}

void del_set_list_tmp(struct set_tmp *set_tmp)
{
	list_del(&set_tmp->list);
	dmfree(set_tmp->name);
	dmfree(set_tmp->value);
	dmfree(set_tmp);
}

void free_all_set_list_tmp(struct dmctx *ctx)
{
	struct set_tmp *set_tmp;
	while (ctx->set_list_tmp.next != &ctx->set_list_tmp) {
		set_tmp = list_entry(ctx->set_list_tmp.next, struct set_tmp, list);
		del_set_list_tmp(set_tmp);
	}
}

void add_list_fault_param(struct dmctx *ctx, char *param, int fault)
{
	struct param_fault *param_fault;
	if (param == NULL) param = "";

	param_fault = dmcalloc(1, sizeof(struct param_fault));
	list_add_tail(&param_fault->list, &ctx->list_fault_param);
	param_fault->name = dmstrdup(param);
	param_fault->fault = fault;
}

void del_list_fault_param(struct param_fault *param_fault)
{
	list_del(&param_fault->list);
	dmfree(param_fault->name);
	dmfree(param_fault);
}

void free_all_list_fault_param(struct dmctx *ctx)
{
	struct param_fault *param_fault;
	while (ctx->list_fault_param.next != &ctx->list_fault_param) {
		param_fault = list_entry(ctx->list_fault_param.next, struct param_fault, list);
		del_list_fault_param(param_fault);
	}
}

static char *get_parameter_notification (char *param)
{
	int i, maxlen = 0, len;
	struct uci_list *list_notif;
	char *pch;
	char *notification = "0";
	struct uci_element *e;

	for (i = (ARRAY_SIZE(notifications) - 1); i >= 0; i--) {
		dmuci_get_option_value_list("cwmp", "@notifications[0]", notifications[i].type, &list_notif);
		if (list_notif) {
			uci_foreach_element(list_notif, e) {
				pch = e->name;
				if (strcmp(pch, param) == 0) {
					notification = notifications[i].value;
					return notification;
				}
				len = strlen(pch);
				if (pch[len-1] == '.') {
					if (strstr(param, pch)) {
						if (len > maxlen )
						{
							notification = notifications[i].value;
							maxlen = len;
						}
					}
				}
			}
		}
	}
	return notification;
}


static int remove_parameter_notification(char *param)
{
	unsigned int i;
	struct uci_list *list_notif;
	struct uci_element *e;
	char *pch;
	for (i = (ARRAY_SIZE(notifications) - 1); i >= 0; i--) {
		if (param[strlen(param)-1] == '.') {
			dmuci_get_option_value_list("cwmp", "@notifications[0]", notifications[i].type, &list_notif);
			if (list_notif) {
				uci_foreach_element(list_notif, e) {
					pch = e->name;
					if (strstr(pch, param) == 0) {
						dmuci_del_list_value("cwmp", "@notifications[0]", notifications[i].type, pch);
					}
				}
			}
		} else {
			dmuci_del_list_value("cwmp", "@notifications[0]", notifications[i].type, param);
		}
	}
	return 0;
}

static int set_parameter_notification(char *param, char *value)
{
	char *tmp = NULL, *buf = NULL, *pch;
	char *notification = NULL;
	struct uci_section *s;
	dmuci_get_section_type("cwmp", "@notifications[0]", &tmp);
	if (!tmp || tmp[0] == '\0') {
		dmuci_add_section("cwmp", "notifications", &s, &buf);
	} else {
		remove_parameter_notification(param);
	}

	notification = get_parameter_notification(param);
	if (strcmp(notification, value) == 0)  {
		return 0;
	}
	if (strcmp(value, "1") == 0) {
		dmuci_add_list_value("cwmp", "@notifications[0]", "passive", param);
	} else if (strcmp(value, "2") == 0) {
		dmuci_add_list_value("cwmp", "@notifications[0]", "active", param);
	} else if (strcmp(value, "0") == 0) {
		struct uci_list *list_notif;
		struct uci_element *e;
		int i, len;
		for (i = (ARRAY_SIZE(notifications) - 1); i >= 1; i--) {
			dmuci_get_option_value_list("cwmp", "@notifications[0]", notifications[i].type, &list_notif);
			if (list_notif) {
				uci_foreach_element(list_notif, e) {
					pch = e->name;
					len = strlen(pch);
					if (pch[len-1] == '.' && strstr(param, pch)) {
						dmuci_add_list_value("cwmp", "@notifications[0]", "disabled", param);
						return 0;
					}
				}
			}
		}

	} else {
		return -1;
	}

	return 0;
}

static int check_param_prefix (struct dmctx *ctx)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		if (strcmp(ctx->in_param, prefix_methods[i].prefix_name) == 0) {
			return 0;
		}		
	}
	return -1;
}

static int check_obj_is_nl1(char *refparam, char *inparam, int ndot)
{
	unsigned int len, i;
	len = strlen(refparam);
	for (i = len - 1; i >= 0; i--) {
		if (refparam[i] == '.') {
			if (--ndot == 0)
				break;
		}
	}
	i++;
	if (strlen(inparam) == i)
		return 0;
	return -1;
}

int string_to_bool(char *v, bool *b)
{
	if (v[0] == '1' && v[1] == '\0') {
		*b = true;
		return 0;
	}
	if (v[0] == '0' && v[1] == '\0') {
		*b = false;
		return 0;
	}
	if (strcasecmp(v, "true") == 0) {
		*b = true;
		return 0;
	}
	if (strcasecmp(v, "false") == 0) {
		*b = false;
		return 0;
	}
	*b = false;
	return -1;
}

/* **********
 * get value 
 * **********/
int dm_entry_get_value(struct dmctx *ctx)
{
	int i;
	ctx->faultcode = FAULT_9005;

	if (ctx->in_param[0] == '\0' || check_param_prefix(ctx) == 0) {
		ctx->method_obj=&get_value_obj;
		ctx->method_param=&get_value_param;
		ctx->faultcode = 0;
	} else if (ctx->in_param[strlen(ctx->in_param)-1] == '.') {
		ctx->method_obj=&get_value_inparam_isobj_check_obj;
		ctx->method_param=&get_value_inparam_isobj_check_param;
	} else {
		ctx->method_obj=&get_value_inparam_isparam_check_obj;
		ctx->method_param=&get_value_inparam_isparam_check_param;
	}
	
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop)
			 return ret;
	}

	return ctx->faultcode;
}

static int get_value_obj(DMOBJECT_API_ARGS)
{
	return 0;
}

static int get_value_inparam_isobj_check_obj(DMOBJECT_API_ARGS)
{
	if (strstr(ctx->current_obj, ctx->in_param)) {
		ctx->faultcode = 0;
		return 0;
	}
	return FAULT_9005;
}

static int get_value_inparam_isparam_check_obj(DMOBJECT_API_ARGS)
{
	return FAULT_9005;
}

static int get_value_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *value = NULL;

	dmastrcat(&full_param, ctx->current_obj, lastname);
	(get_cmd)(full_param, ctx, &value);
	add_list_paramameter(ctx, full_param, value, type ? type : "xsd:string");
	return 0;
}

static int get_value_inparam_isparam_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *value = NULL;

	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strcmp(ctx->in_param, full_param) != 0) {
		dmfree(full_param);
		return FAULT_9005;
	}
	
	(get_cmd)(full_param, ctx, &value);
	add_list_paramameter(ctx, full_param, value, type ? type : "xsd:string");
	ctx->stop = true;
	return 0;
}

static int get_value_inparam_isobj_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *value = NULL;

	dmastrcat(&full_param, ctx->current_obj, lastname);			
	if (strstr(full_param, ctx->in_param)) {
		(get_cmd)(full_param, ctx, &value);
		add_list_paramameter(ctx, full_param, value, type ? type : "xsd:string");
		ctx->faultcode = 0;
		return 0;
	}
	dmfree(full_param);
	return FAULT_9005;
}

/* **********
 * get name 
 * **********/

int dm_entry_get_name(struct dmctx *ctx)
{
	int i;
	ctx->faultcode = FAULT_9005;
	if (ctx->in_param[0] == '\0' && ctx->nextlevel == 1) {
		ctx->method_obj=&get_name_emptyin_nl1_obj;
		ctx->method_param=&get_name_emptyin_nl1_param;
		entry_method_root(ctx);
		return 0;
	} 
	if ( ctx->in_param[0] == '\0' || check_param_prefix(ctx) == 0) {
		if (ctx->nextlevel == 0) {
			ctx->method_obj=&get_name_obj;
			ctx->method_param=&get_name_param;
			ctx->faultcode = 0;
		} else {
			ctx->method_obj=&get_name_inparam_isobj_check_obj;
			ctx->method_param=&get_name_inparam_isobj_check_param;
		}
	} else if (ctx->in_param[strlen(ctx->in_param)-1] == '.') {
		ctx->method_obj=&get_name_inparam_isobj_check_obj;
		ctx->method_param=&get_name_inparam_isobj_check_param;
	} else {
		ctx->method_obj=&get_name_inparam_isparam_check_obj;
		ctx->method_param=&get_name_inparam_isparam_check_param;
	}
	
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop == 1)
			 return ret;
	}
	return ctx->faultcode;
}

static int get_name_obj(DMOBJECT_API_ARGS)
{
	char *obj = dmstrdup(ctx->current_obj);
	char *p = permission;
	add_list_paramameter(ctx, obj, p, NULL);
	return 0;
}

static int get_name_inparam_isparam_check_obj(DMOBJECT_API_ARGS)
{
	return FAULT_9005;
}

static int get_name_inparam_isobj_check_obj(DMOBJECT_API_ARGS)
{
	if (strstr(ctx->current_obj, ctx->in_param)) {
		ctx->faultcode = 0;
		if (ctx->nextlevel == 0 || check_obj_is_nl1(ctx->current_obj, ctx->in_param, 2) == 0 ) {
			char *obj = dmstrdup(ctx->current_obj);
			char *p = permission;
			add_list_paramameter(ctx, obj, p, NULL);
			return 0;
		}
		return 0;
	}	
	return FAULT_9005;
}

static int get_name_emptyin_nl1_obj(DMOBJECT_API_ARGS)
{
	char *obj = dmstrdup(ctx->current_obj);
	char *p = permission;
	add_list_paramameter(ctx, obj, p, NULL);
	return 0;
}

static int get_name_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *p = permission;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	add_list_paramameter(ctx, full_param, p, NULL);
	return 0;
}

static int get_name_inparam_isparam_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strcmp(full_param, ctx->in_param) != 0) {
		dmfree(full_param);
		return FAULT_9005;
	}
	if (ctx->nextlevel == 1) {
		dmfree(full_param);
		ctx->stop = 1;
		return FAULT_9003;
	}
	char *p = permission;
	add_list_paramameter(ctx, full_param, p, NULL);
	ctx->stop = 1;
	return 0;
}

static int get_name_inparam_isobj_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strstr(full_param, ctx->in_param)) {
		ctx->faultcode = 0;
		if (ctx->nextlevel == 0 || check_obj_is_nl1(full_param, ctx->in_param, 1) == 0 ) {
			char *p = permission;
			add_list_paramameter(ctx, full_param, p, NULL);
			return 0;
		}
		dmfree(full_param);
		return 0; //TODO check the return value here!
	}
	dmfree(full_param);
	return FAULT_9005;
}

static int get_name_emptyin_nl1_param(DMPARAM_API_ARGS)
{
	return 0;
}

/* ********************
 * get notification
 * ********************/
int dm_entry_get_notification(struct dmctx *ctx)
{
	int i;
	ctx->faultcode = FAULT_9005;
	
	if (ctx->in_param[0] == '\0' || check_param_prefix(ctx) == 0) {
		ctx->method_obj=&get_notification_obj;
		ctx->method_param=&get_notification_param;
		ctx->faultcode = 0;
	} else if ( ctx->in_param[strlen(ctx->in_param)-1] == '.') {
		ctx->method_obj=&get_notification_inparam_isobj_check_obj;
		ctx->method_param=&get_notification_inparam_isobj_check_param;		
	} else {
		ctx->method_obj=&get_notification_inparam_isparam_check_obj;
		ctx->method_param=&get_notification_inparam_isparam_check_param;
	}	
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop == 1)
			 return ret;
	}
	return ctx->faultcode;
}

static int get_notification_obj(DMOBJECT_API_ARGS)
{
	return 0;
}

static int get_notification_inparam_isparam_check_obj(DMOBJECT_API_ARGS)
{
	return FAULT_9005; 
}

static int get_notification_inparam_isobj_check_obj(DMOBJECT_API_ARGS)
{
	if (strstr(ctx->current_obj, ctx->in_param)) {
		ctx->faultcode = 0;
		return 0;
	}
	return FAULT_9005;
}

static int get_notification_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *notification;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (forced_notify == UNDEF) {
		notification = get_parameter_notification(full_param);
	} else {
		notification = notifications[forced_notify].value;
	}
	add_list_paramameter(ctx, full_param, notification, NULL);
	return 0;
}

static int get_notification_inparam_isparam_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *notification;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strcmp(full_param, ctx->in_param) != 0) {
		dmfree(full_param);
		return FAULT_9005;
	}
	if (forced_notify == UNDEF) {
		notification = get_parameter_notification(full_param);
	} else {
		notification = notifications[forced_notify].value;
	}
	add_list_paramameter(ctx, full_param, notification, NULL);
	ctx->stop = true;
	return 0;
}

static int get_notification_inparam_isobj_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	char *notification;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strstr(full_param, ctx->in_param)) {		
		if (forced_notify == UNDEF) {
			notification = get_parameter_notification(full_param);
		} else {
			notification = notifications[forced_notify].value;
		}
		add_list_paramameter(ctx, full_param, notification, NULL);
		ctx->faultcode = 0;
		return 0;
	}
	dmfree(full_param);
	return FAULT_9005;
}

/***************
* inform
***************/
int dm_entry_inform(struct dmctx *ctx)
{
	int i;
	ctx->method_obj = &inform_check_obj;
	ctx->method_param = &inform_check_param;
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		if (prefix_methods[i].forced_inform)
			prefix_methods[i].method(ctx);
	}
	return 0;
}

static int inform_check_obj(DMOBJECT_API_ARGS)
{
	return FAULT_9005;
}

static int inform_check_param(DMPARAM_API_ARGS)
{
	if (!forced_inform) 
		return FAULT_9005;
	char *full_param;
	char *value = NULL;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	(get_cmd)(full_param, ctx, &value);
	add_list_paramameter(ctx, full_param, value, type ? type : "xsd:string");
	return 0;
}

/* **************
 * add object 
 * **************/
int dm_entry_add_object(struct dmctx *ctx)
{
	int i;
	if (ctx->tree)
		return FAULT_9005;
	ctx->method_obj=&add_object_obj;
	ctx->method_param=&add_object_param;
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop)
			 return ret;
	}	
	return FAULT_9005;
}

static int add_object_obj(DMOBJECT_API_ARGS)
{
	char *instance;
	if (strcmp(ctx->current_obj, ctx->in_param) != 0)
		return FAULT_9005;

	ctx->stop = true;
	if (addobj == NULL)
		return FAULT_9005; 

	int fault = (addobj)(ctx, &instance);
	if (fault)
		return fault;

	ctx->addobj_instance = instance;
	char *objinst;
	dmastrcat(&objinst, ctx->current_obj, instance);
	set_parameter_notification(objinst, "0");
	dmfree(objinst);
	return 0;
}

static int add_object_param(DMPARAM_API_ARGS)
{
	return FAULT_9005; 
}

 /* **************
 * del object 
 * **************/
int dm_entry_delete_object(struct dmctx *ctx)
{
	int i;
	if (ctx->tree == 1)
		return FAULT_9005;
	ctx->method_obj=&delete_object_obj;
	ctx->method_param=&delete_object_param;
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop)
			 return ret;
	}
	return FAULT_9005;
}

static int delete_object_obj(DMOBJECT_API_ARGS)
{
	if (strcmp(ctx->current_obj, ctx->in_param) != 0)
		return FAULT_9005;

	ctx->stop = true;
	if (delobj == NULL)
		return FAULT_9005;

	int fault = (delobj)(ctx);
	return fault;
}

static int delete_object_param(DMPARAM_API_ARGS)
{
	return FAULT_9005; 
}


 /* **************
 * set value  
 * **************/
int dm_entry_set_value(struct dmctx *ctx)
{
	int i;
	if (ctx->in_param[0] == '\0' || ctx->in_param[strlen(ctx->in_param)-1] == '.' ) {
		return FAULT_9005;
	} else {
		ctx->method_obj=&set_value_check_obj;
		ctx->method_param=&set_value_check_param; 
	}
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop)
			 return ret;
	}
	return FAULT_9005; 	
}
 
static int set_value_check_obj(DMOBJECT_API_ARGS)
{
	return FAULT_9005; 
}

static int set_value_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	dmastrcat(&full_param, ctx->current_obj, lastname);

	if (strcmp(ctx->in_param, full_param) != 0) {
		dmfree(full_param);
		return FAULT_9005;
	} 

	ctx->stop = true;

	if (ctx->setaction == VALUECHECK) {
		if (permission[0] != '1' || set_cmd == NULL) {
			dmfree(full_param);
			return FAULT_9008;
		}

		int fault = (set_cmd)(full_param, ctx, VALUECHECK, ctx->in_value);
		if (fault) {
			dmfree(full_param);
			return fault;
		}

		add_set_list_tmp(ctx, ctx->in_param, ctx->in_value);
	}
	else if (ctx->setaction == VALUESET) {
		(set_cmd)(full_param, ctx, VALUESET, ctx->in_value);
	}

	dmfree(full_param);
	return 0;
}

 /* ****************
 * set notification  
 * ****************/
int dm_entry_set_notification(struct dmctx *ctx)
{
	int i; 
	if (ctx->in_param[0] == '\0') {
		return FAULT_9009;
	} else {
		ctx->method_obj=&set_notification_check_obj;
		ctx->method_param=&set_notification_check_param; 
	}	
	for (i = 0; i < ARRAY_SIZE(prefix_methods); i++) {
		 int ret = prefix_methods[i].method(ctx);
		 if (ctx->stop)
			 return ret;
	}
	return FAULT_9005; 
}

static int set_notification_check_obj(DMOBJECT_API_ARGS)
{
	if (strcmp(ctx->in_param, ctx->current_obj) != 0)
		return FAULT_9005;

	ctx->stop = true;

	if (ctx->setaction == VALUECHECK) {
		if (!notif_permission)
			return FAULT_9009;

		add_set_list_tmp(ctx, ctx->in_param, ctx->in_notification);
	}
	else if (ctx->setaction == VALUESET) {
		set_parameter_notification(ctx->in_param, ctx->in_notification);
	}
	return 0;
}

static int set_notification_check_param(DMPARAM_API_ARGS)
{
	char *full_param;
	dmastrcat(&full_param, ctx->current_obj, lastname);
	if (strcmp(ctx->in_param, full_param) != 0) {
		dmfree(full_param);
		return FAULT_9005;
	}
	ctx->stop = true;

	if (ctx->setaction == VALUECHECK) {
		if (!notif_permission) {
			dmfree(full_param);
			return FAULT_9009;
		}
		add_set_list_tmp(ctx, ctx->in_param, ctx->in_notification);
	} else if (ctx->setaction == VALUESET) {
		set_parameter_notification(ctx->in_param, ctx->in_notification);
	}

	dmfree(full_param);
	return 0;
}