/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *	  Author MOHAMED Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *	  Author Feten Besbes <feten.besbes@pivasoftware.com>
 *
 */
#include "log.h"
#include "dmcwmp.h"
#include "dmubus.h"
#include "dmuci.h"
#include "dmentry.h"
#include "cwmp.h"
#include "wepkey.h"

LIST_HEAD(head_package_change);

int dm_global_init(void)
{
	dm_entry_set_prefix_methods_enable();
	return 0;
}

static int dm_ctx_init_custom(struct dmctx *ctx, int custom)
{
	struct cwmp   *cwmp = &cwmp_main;
	if (custom == CTX_INIT_ALL) {
		memset(&dmubus_ctx, 0, sizeof(struct dmubus_ctx));
		INIT_LIST_HEAD(&dmubus_ctx.obj_head);
		uci_ctx = uci_alloc_context();
		uci_varstate_ctx = uci_alloc_context();
		DMUCI_INIT(icwmpd);
	}
	INIT_LIST_HEAD(&ctx->list_parameter);
	INIT_LIST_HEAD(&ctx->set_list_tmp);
	INIT_LIST_HEAD(&ctx->list_fault_param);
	if(cwmp->conf.supported_amd_version == 0)
		get_amd_version_config();
	get_instance_mode_config();
	ctx->amd_version = cwmp->conf.amd_version;
	ctx->instance_mode = cwmp->conf.instance_mode;
	return 0;
}

static int dm_ctx_clean_custom(struct dmctx *ctx, int custom)
{
	free_all_list_parameter(ctx);
	free_all_set_list_tmp(ctx);
	free_all_list_fault_param(ctx);
	DMFREE(ctx->addobj_instance);
	if (custom == CTX_INIT_ALL) {
		if (uci_ctx) uci_free_context(uci_ctx);
		uci_ctx = NULL;
		if (uci_varstate_ctx) uci_free_context(uci_varstate_ctx);
		uci_varstate_ctx = NULL;
		DMUCI_EXIT(icwmpd);
		dmubus_ctx_free(&dmubus_ctx);
		dmcleanmem();
	}
	return 0;
}

int dm_ctx_init(struct dmctx *ctx)
{
	dm_ctx_init_custom(ctx, CTX_INIT_ALL);
	return 0;
}

int dm_ctx_clean(struct dmctx *ctx)
{
	dm_ctx_clean_custom(ctx, CTX_INIT_ALL);
	return 0;
}

int dm_ctx_init_sub(struct dmctx *ctx)
{
	dm_ctx_init_custom(ctx, CTX_INIT_SUB);
	return 0;
}

int dm_ctx_clean_sub(struct dmctx *ctx)
{
	dm_ctx_clean_custom(ctx, CTX_INIT_SUB);
	return 0;
}

void dmentry_instance_lookup_inparam(struct dmctx *ctx)
{
	char *pch, *spch, *in_param;
	in_param = dmstrdup(ctx->in_param);
	int i = 0;
	for (pch = strtok_r(in_param, ".", &spch); pch != NULL; pch = strtok_r(NULL, ".", &spch)) {
		if (pch[0]== '[') {
			ctx->alias_register |= (1 << i);
			i++;
		} else if (isdigit(pch[0])) {
			i++;
		}
	}
	dmfree(in_param);
	ctx->nbrof_instance = i;
}

int dm_entry_param_method(struct dmctx *ctx, int cmd, char *inparam, char *arg1, char *arg2)
{
	int fault = 0;
	bool setnotif = true;
	
	if (!inparam) inparam = "";
	ctx->in_param = inparam;
	dmentry_instance_lookup_inparam(ctx);
	if (ctx->in_param[0] == '\0' || strcmp(ctx->in_param, DMROOT) == 0) {
		ctx->tree = true;
	} else {
		ctx->tree = false;
	}
	ctx->stop = false;
	switch(cmd) {
		case CMD_GET_VALUE:
			CWMP_LOG(DEBUG,"Get Value Param : %s\n",ctx->in_param);
			if (ctx->in_param[0] == '.' && strlen(ctx->in_param) == 1)
				fault = FAULT_9005;
			else
				fault = dm_entry_get_value(ctx);
			break;
		case CMD_GET_NAME:
			CWMP_LOG(DEBUG,"Get Name Param : %s Level %s\n",ctx->in_param, arg1);
			if (ctx->in_param[0] == '.' && strlen(ctx->in_param) == 1)
				fault = FAULT_9005;
			else if (arg1 && string_to_bool(arg1, &ctx->nextlevel) == 0){
				fault = dm_entry_get_name(ctx);
			} else {
				fault = FAULT_9003;
			}
			break;
		case CMD_GET_NOTIFICATION:
			CWMP_LOG(DEBUG,"Get Notification Param : %s \n",ctx->in_param);
			if (ctx->in_param[0] == '.' && strlen(ctx->in_param) == 1)
				fault = FAULT_9005;
			else
				fault = dm_entry_get_notification(ctx);
			break;
		case CMD_SET_VALUE:
			ctx->in_value = arg1 ? arg1 : "";
			ctx->setaction = VALUECHECK;
			CWMP_LOG(DEBUG,"Set Value Param: %s Value: %s\n",ctx->in_param, ctx->in_value);
			fault = dm_entry_set_value(ctx);
			if (fault)
				add_list_fault_param(ctx, ctx->in_param, fault);
			break;
		case CMD_SET_NOTIFICATION:
			if (arg2)
				string_to_bool(arg2, &setnotif);
			if (setnotif && arg1 &&
				(strcmp(arg1, "0") == 0 ||
				strcmp(arg1, "1") == 0  ||
				strcmp(arg1, "2") == 0 ||
				strcmp(arg1, "3") == 0 ||
				strcmp(arg1, "4") == 0 ||
				strcmp(arg1, "5") == 0 ||
				strcmp(arg1, "6") == 0)) {
				ctx->in_notification = arg1;
				ctx->setaction = VALUECHECK;
				fault = dm_entry_set_notification(ctx);
			} else {
				fault = FAULT_9003;
			}
			break;
		case CMD_INFORM:
			dm_entry_inform(ctx);
			break;
		case CMD_ADD_OBJECT:
			CWMP_LOG(DEBUG,"Object To Add: %s \n",ctx->in_param);
			fault = dm_entry_add_object(ctx);
			if (!fault) {
				dmuci_set_value("cwmp", "acs", "ParameterKey", arg1 ? arg1 : "");
				dmuci_change_packages(&head_package_change);
			}
			break;
		case CMD_DEL_OBJECT:
			CWMP_LOG(DEBUG,"Object To Del: %s \n",ctx->in_param);
			fault = dm_entry_delete_object(ctx);
			if (!fault) {
				dmuci_set_value("cwmp", "acs", "ParameterKey", arg1 ? arg1 : "");
				dmuci_change_packages(&head_package_change);
			}
			break;
	}
	dmuci_commit();
	return fault;
}

int dm_entry_apply(struct dmctx *ctx, int cmd, char *arg1, char *arg2)
{
	int fault = 0;
	struct set_tmp *n, *p;
	
	switch(cmd) {
		case CMD_SET_VALUE:
			ctx->setaction = VALUESET;
			ctx->tree = false;
			list_for_each_entry_safe(n, p, &ctx->set_list_tmp, list) {
				ctx->in_param = n->name;
				ctx->in_value = n->value ? n->value : "";
				ctx->stop = false;
				fault = dm_entry_set_value(ctx);
				if (fault) break;
			}
			if (fault) {
				//Should not happen
				dmuci_revert();
				add_list_fault_param(ctx, ctx->in_param, fault);
			} else {
				dmuci_set_value("cwmp", "acs", "ParameterKey", arg1 ? arg1 : "");
				dmuci_change_packages(&head_package_change);
				dmuci_commit();
			}
			free_all_set_list_tmp(ctx);
			break;
		case CMD_SET_NOTIFICATION:
			ctx->setaction = VALUESET;
			ctx->tree = false;
			list_for_each_entry_safe(n, p, &ctx->set_list_tmp, list) {
				ctx->in_param = n->name;
				ctx->in_notification = n->value ? n->value : "0";
				ctx->stop = false;
				fault = dm_entry_set_notification(ctx);
				if (fault) break;
			}
			if (fault) {
				//Should not happen
				dmuci_revert();
			} else {
				dmuci_commit();
			}
			free_all_set_list_tmp(ctx);
			break;
	}
	return fault;
}

int dm_entry_load_enabled_notify()
{
	struct dmctx dmctx = {0};
	dm_ctx_init(&dmctx);
	dmctx.in_param = "";
	dmctx.tree = true;
	free_all_list_enabled_lwnotify();
	free_all_list_enabled_notify();
	dm_entry_enabled_notify(&dmctx);
	dm_ctx_clean(&dmctx);
	return 0;
}

int adm_entry_get_linker_param(char *param, char *linker, char **value)
{
	struct dmctx dmctx = {0};

	dm_ctx_init_sub(&dmctx);
	dmctx.in_param = param ? param : "";
	dmctx.linker = linker;


	if (dmctx.in_param[0] == '\0') {
		dmctx.tree = true;
	} else {
		dmctx.tree = false;
	}

	dm_entry_get_linker(&dmctx);
	*value = dmctx.linker_param;

	dm_ctx_clean_sub(&dmctx);
	return 0;
}

int adm_entry_get_linker_value(char *param, char **value)
{
	struct dmctx dmctx = {0};
	*value = NULL;

	if (!param || param[0] == '\0') {
		return 0;
	}

	dm_ctx_init_sub(&dmctx);
	dmctx.in_param = param;
	dmctx.tree = false;

	dm_entry_get_linker_value(&dmctx);
	*value = dmctx.linker;

	dm_ctx_clean_sub(&dmctx);
	return 0;
}

int dm_entry_restart_services()
{
	struct package_change *pc;

	list_for_each_entry(pc, &head_package_change, list) {
		if(strcmp(pc->package, "cwmp") == 0)
			continue;
		dmubus_call_set("uci", "commit", UBUS_ARGS{{"config", pc->package, String}}, 1);
	}
	free_all_list_package_change(&head_package_change);

	return 0;
}

int cli_output_dm_result(struct dmctx *dmctx, int fault, int cmd, int out)
{
	if (!out) return 0;

	if (dmctx->list_fault_param.next != &dmctx->list_fault_param) {
		struct param_fault *p;
		list_for_each_entry(p, &dmctx->list_fault_param, list) {
			fprintf (stdout, "{ \"parameter\": \"%s\", \"fault\": \"%d\" }\n", p->name, p->fault);
		}
		goto end;
	}
	if (fault) {
		fprintf (stdout, "{ \"fault\": \"%d\" }\n", fault);
		goto end;
	}

	if (cmd == CMD_ADD_OBJECT) {
		if (dmctx->addobj_instance) {
			fprintf (stdout, "{ \"status\": \"1\", \"instance\": \"%s\" }\n", dmctx->addobj_instance);
			goto end;
		} else {
			fprintf (stdout, "{ \"fault\": \"%d\" }\n", FAULT_9002);
			goto end;
		}
	}

	if (cmd == CMD_DEL_OBJECT || cmd == CMD_SET_VALUE) {
		fprintf (stdout, "{ \"status\": \"1\" }\n");
		goto end;
	}

	if (cmd == CMD_SET_NOTIFICATION) {
		fprintf (stdout, "{ \"status\": \"0\" }\n");
		goto end;
	}

	struct dm_parameter *n;
	if (cmd == CMD_GET_NAME) {
		list_for_each_entry(n, &dmctx->list_parameter, list) {
			fprintf (stdout, "{ \"parameter\": \"%s\", \"writable\": \"%s\" }\n", n->name, n->data);
		}
	}
	else if (cmd == CMD_GET_NOTIFICATION) {
		list_for_each_entry(n, &dmctx->list_parameter, list) {
			fprintf (stdout, "{ \"parameter\": \"%s\", \"notification\": \"%s\" }\n", n->name, n->data);
		}
	}
	else if (cmd == CMD_GET_VALUE || cmd == CMD_INFORM) {
		list_for_each_entry(n, &dmctx->list_parameter, list) {
			fprintf (stdout, "{ \"parameter\": \"%s\", \"value\": \"%s\", \"type\": \"%s\" }\n", n->name, n->data, n->type);
		}
	}
end:
	return 0;
}

void dm_entry_cli(int argc, char** argv)
{
	struct dmctx cli_dmctx = {0};
	int output = 1;
	char *param, *next_level, *parameter_key, *value, *cmd;
	int fault = 0, status = -1;
	bool set_fault = false;

	dm_global_init();
	dm_ctx_init(&cli_dmctx);

	if (argc < 4) goto invalid_arguments;

	output = atoi(argv[2]);
	cmd = argv[3];

	/* GET NAME */
	if (strcmp(cmd, "get_name") == 0) {
		if (argc < 6) goto invalid_arguments;
		param =argv[4];
		next_level =argv[5];
		fault = dm_entry_param_method(&cli_dmctx, CMD_GET_NAME, param, next_level, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_GET_NAME, output);
	}
	/* GET VALUE */
	else if (strcmp(cmd, "get_value") == 0) {
		if (argc < 5) goto invalid_arguments;
		param =argv[4];
		fault = dm_entry_param_method(&cli_dmctx, CMD_GET_VALUE, param, NULL, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_GET_VALUE, output);
	}
	/* GET NOTIFICATION */
	else if (strcmp(cmd, "get_notification") == 0) {
		if (argc < 5) goto invalid_arguments;
		param =argv[4];
		fault = dm_entry_param_method(&cli_dmctx, CMD_GET_NOTIFICATION, param, NULL, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_GET_NOTIFICATION, output);
	}
	/* SET VALUE */
	else if (strcmp(cmd, "set_value") == 0) {
		if (argc < 7 || (argc % 2) == 0) goto invalid_arguments;
		int i;
		for (i = 5; i < argc; i+=2) {
			param = argv[i];
			value = argv[i+1];
			fault = dm_entry_param_method(&cli_dmctx, CMD_SET_VALUE, param, value, NULL);
			if (fault) set_fault = true;
		}
		parameter_key = argv[4];
		if (!set_fault) {
			fault = dm_entry_apply(&cli_dmctx, CMD_SET_VALUE, parameter_key, NULL);
		}
		cli_output_dm_result(&cli_dmctx, fault, CMD_SET_VALUE, output);
	}
	/* SET NOTIFICATION */
	else if (strcmp(cmd, "set_notification") == 0) {
		if (argc < 6 || (argc % 2) != 0) goto invalid_arguments;
		int i;
		for (i = 4; i < argc; i+=2) {
			param = argv[i];
			value = argv[i+1];
			fault = dm_entry_param_method(&cli_dmctx, CMD_SET_NOTIFICATION, param, value, "1");
			if (fault) set_fault = true;
		}
		if(!set_fault) {
			fault = dm_entry_apply(&cli_dmctx, CMD_SET_NOTIFICATION, NULL, NULL);
		}
		cli_output_dm_result(&cli_dmctx, fault, CMD_SET_NOTIFICATION, output);
	}
	/* ADD OBJECT */
	else if (strcmp(cmd, "add_object") == 0) {
		if (argc < 6) goto invalid_arguments;
		param =argv[5];
		parameter_key =argv[4];
		fault = dm_entry_param_method(&cli_dmctx, CMD_ADD_OBJECT, param, parameter_key, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_ADD_OBJECT, output);
	}
	/* DEL OBJECT */
	else if (strcmp(cmd, "delete_object") == 0) {
		if (argc < 6) goto invalid_arguments;
		param =argv[5];
		parameter_key =argv[4];
		fault = dm_entry_param_method(&cli_dmctx, CMD_DEL_OBJECT, param, parameter_key, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_DEL_OBJECT, output);
	}
	/* INFORM */
	else if (strcmp(cmd, "inform") == 0) {
		fault = dm_entry_param_method(&cli_dmctx, CMD_INFORM, "", NULL, NULL);
		cli_output_dm_result(&cli_dmctx, fault, CMD_INFORM, output);
	}
	else {
		goto invalid_arguments;
	}
	dm_ctx_clean(&cli_dmctx);
	return;

invalid_arguments:
	dm_ctx_clean(&cli_dmctx);
	fprintf(stdout, "Invalid arguments!\n");;
}

int cli_output_wepkey64(char strk64[4][11])
{
	fprintf(stdout, "%s\n%s\n%s\n%s\n", strk64[0], strk64[1], strk64[2], strk64[3]);
	return 0;
}

int cli_output_wepkey128(char strk128[27])
{
	fprintf(stdout, "%s\n", strk128);
	return 0;
}

void wepkey_cli(int argc, char** argv)
{
	if (argc < 4) goto invalid_arguments;

	char *strength = argv[2];
	char *passphrase =  argv[3];

	if (!strength || !passphrase || passphrase[0] == '\0')
		goto invalid_arguments;

	if (strcmp(strength, "64") == 0) {
		char strk64[4][11];
		wepkey64(passphrase, strk64);
		cli_output_wepkey64(strk64);
	}
	else if (strcmp(strength, "128") == 0) {
		char strk128[27];
		wepkey128(passphrase, strk128);
		cli_output_wepkey128(strk128);
	}
	else {
		goto invalid_arguments;
	}
	return;

invalid_arguments:
	fprintf(stdout, "Invalid arguments!\n");;
}
