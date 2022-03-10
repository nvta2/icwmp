/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <pthread.h>
#include <libubox/uloop.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <libubox/uloop.h>

#include "common.h"
#include "session.h"
#include "backupSession.h"
#include "http.h"
#include "cwmp_http.h"
#include "diagnostic.h"
#include "config.h"
#include "ubus.h"
#include "log.h"
#include "notifications.h"
#include "cwmp_uci.h"
#include "cwmp_du_state.h"
#include "download.h"
#include "upload.h"
#include "sched_inform.h"
#include "digestauth.h"
#include "rpc_soap.h"
#include "netlink.h"
#include "cwmp_http.h"
#include "cwmp_config.h"
#include "cwmp_http.h"

void load_forced_inform_json_file()
{
	struct blob_buf bbuf;
	struct blob_attr *cur;
	struct blob_attr *custom_forced_inform_list = NULL;
	int rem;

	if (cwmp_main->conf.forced_inform_json_file == NULL || !file_exists(cwmp_main->conf.forced_inform_json_file))
		return;

	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);

	if (blobmsg_add_json_from_file(&bbuf, cwmp_main->conf.forced_inform_json_file) == false) {
		CWMP_LOG(WARNING, "The file %s is not a valid JSON file", cwmp_main->conf.forced_inform_json_file);
		blob_buf_free(&bbuf);
		return;
	}
	const struct blobmsg_policy p[1] = { { "forced_inform", BLOBMSG_TYPE_ARRAY } };
	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
	if (tb[0] == NULL) {
		CWMP_LOG(WARNING, "The JSON file %s doesn't contain a forced inform parameters list", cwmp_main->conf.custom_notify_json);
		blob_buf_free(&bbuf);
		return;
	}
	custom_forced_inform_list = tb[0];


	blobmsg_for_each_attr(cur, custom_forced_inform_list, rem)
	{
		char parameter_path[128];
		char *val = NULL;
		snprintf(parameter_path, sizeof(parameter_path), "%s", blobmsg_get_string(cur));
		if (parameter_path[strlen(parameter_path)-1] == '.') {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Only leaf parameters are allowed.", parameter_path);
			continue;
		}
		int fault = cwmp_get_leaf_value(parameter_path, &val);
		if (fault != 0) {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Wrong parameter path.", parameter_path);
			continue;
		}
		custom_forced_inform_parameters[nbre_custom_inform++] = strdup(parameter_path);
		FREE(val);
	}
	blob_buf_free(&bbuf);

}

void clean_custom_inform_parameters()
{
	int i;
	for (i=0; i < nbre_custom_inform; i++) {
		free(custom_forced_inform_parameters[i]);
		custom_forced_inform_parameters[i] = NULL;
	}
	nbre_custom_inform = 0;
	for (i=0; i < nbre_boot_inform; i++) {
		free(boot_inform_parameters[i]);
		boot_inform_parameters[i] = NULL;
	}
	nbre_boot_inform = 0;
}

void load_boot_inform_json_file()
{
	struct blob_buf bbuf;
	struct blob_attr *cur;
	struct blob_attr *custom_boot_inform_list = NULL;
	int rem;

	if (cwmp_main->conf.boot_inform_json_file == NULL || !file_exists(cwmp_main->conf.boot_inform_json_file))
		return;

	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);

	if (blobmsg_add_json_from_file(&bbuf, cwmp_main->conf.boot_inform_json_file) == false) {
		CWMP_LOG(WARNING, "The file %s is not a valid JSON file", cwmp_main->conf.boot_inform_json_file);
		blob_buf_free(&bbuf);
		return;
	}
	const struct blobmsg_policy p[1] = { { "boot_inform", BLOBMSG_TYPE_ARRAY } };
	struct blob_attr *tb[1] = { NULL };
	blobmsg_parse(p, 1, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
	if (tb[0] == NULL) {
		CWMP_LOG(WARNING, "The JSON file %s doesn't contain a boot inform parameters list", cwmp_main->conf.custom_notify_json);
		blob_buf_free(&bbuf);
		return;
	}
	custom_boot_inform_list = tb[0];



	blobmsg_for_each_attr(cur, custom_boot_inform_list, rem)
	{
		char parameter_path[128];
		char *val = NULL;

		snprintf(parameter_path, sizeof(parameter_path), "%s", blobmsg_get_string(cur));
		if (parameter_path[strlen(parameter_path)-1] == '.') {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Only leaf parameters are allowed.", parameter_path);
			continue;
		}
		int fault = cwmp_get_leaf_value(parameter_path, &val);
		if (fault != 0) {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Wrong parameter path.", parameter_path);
			continue;
		}
		boot_inform_parameters[nbre_boot_inform++] = strdup(parameter_path);
		FREE(val);
	}
	blob_buf_free(&bbuf);
}

int create_cwmp_var_state_files()
{
	/*
	 * Create Notifications empty uci package
	 */
	if (!file_exists(CWMP_VARSTATE_UCI_PACKAGE)) {
		FILE *fptr = fopen(CWMP_VARSTATE_UCI_PACKAGE, "w+");
		if (fptr)
			fclose(fptr);
		else
			return CWMP_GEN_ERR;
	}
	if (!folder_exists("/var/run/icwmpd")) {
		if (mkdir("/var/run/icwmpd", S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
			CWMP_LOG(INFO, "Not able to create the folder /var/run/icwmpd");
			return CWMP_GEN_ERR;
		}
	}
	return CWMP_OK;
}

static bool g_usp_object_available = false;

static void lookup_event_cb(struct ubus_context *ctx __attribute__((unused)),
		struct ubus_event_handler *ev __attribute__((unused)),
		const char *type, struct blob_attr *msg)
{
	const struct blobmsg_policy policy = {
		"path", BLOBMSG_TYPE_STRING
	};
	struct blob_attr *attr;
	const char *path;

	if (strcmp(type, "ubus.object.add") != 0)
		return;

	blobmsg_parse(&policy, 1, &attr, blob_data(msg), blob_len(msg));
	if (!attr)
		return;

	path = blobmsg_data(attr);
	if (strcmp(path, USP_OBJECT_NAME) == 0) {
		g_usp_object_available = true;
		uloop_end();
	}
}

static void lookup_timeout_cb(struct uloop_timeout *timeout __attribute__((unused)))
{
	uloop_end();
}

static int wait_for_usp_raw_object()
{
#define USP_RAW_WAIT_TIMEOUT 60

	struct ubus_context *uctx;
	int ret;
	uint32_t ubus_id;
	struct ubus_event_handler add_event;
	struct uloop_timeout u_timeout;

	g_usp_object_available = false;
	uctx = ubus_connect(NULL);
	if (uctx == NULL) {
		CWMP_LOG(ERROR, "Can't create ubus context");
		return FAULT_CPE_INTERNAL_ERROR;
	}

	uloop_init();
	ubus_add_uloop(uctx);

	// register for add event
	memset(&add_event, 0, sizeof(struct ubus_event_handler));
	add_event.cb = lookup_event_cb;
	ubus_register_event_handler(uctx, &add_event, "ubus.object.add");

	// check if object already present
	ret = ubus_lookup_id(uctx, USP_OBJECT_NAME, &ubus_id);
	if (ret == 0) {
		g_usp_object_available = true;
		goto end;
	}

	// Set timeout to expire lookup
	memset(&u_timeout, 0, sizeof(struct uloop_timeout));
	u_timeout.cb = lookup_timeout_cb;
	uloop_timeout_set(&u_timeout, USP_RAW_WAIT_TIMEOUT * 1000);

	uloop_run();
	uloop_done();

end:
	ubus_free(uctx);

	if (g_usp_object_available == false) {
		CWMP_LOG(ERROR, "%s object not found", USP_OBJECT_NAME);
		return FAULT_CPE_INTERNAL_ERROR;
	}

	return 0;
}

int cwmp_apply_acs_changes(void)
{
	int error;

	if ((error = cwmp_config_reload()))
		return error;

	if ((error = cwmp_root_cause_events()))
		return error;

	return CWMP_OK;
}

static void configure_var_state()
{
	char *zone_name = NULL;

	cwmp_uci_init();
	if (!file_exists(VARSTATE_CONFIG"/cwmp"))
		creat(VARSTATE_CONFIG"/cwmp", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	cwmp_uci_add_section_with_specific_name("cwmp", "acs", "acs", UCI_VARSTATE_CONFIG);
	cwmp_uci_add_section_with_specific_name("cwmp", "cpe", "cpe", UCI_VARSTATE_CONFIG);

	get_firewall_zone_name_by_wan_iface(cwmp_main->conf.default_wan_iface, &zone_name);
	cwmp_uci_set_varstate_value("cwmp", "acs", "zonename", zone_name ? zone_name : "wan");

	cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
	cwmp_uci_exit();
}

static int cwmp_init(int argc, char **argv)
{
	int error;
	struct env env;

	openlog("cwmp", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	CWMP_LOG(INFO, "STARTING ICWMP with PID :%d", getpid());

	cwmp_main = (struct cwmp*)calloc(1, sizeof(struct cwmp));

	memset(&env, 0, sizeof(struct env));
	if ((error = global_env_init(argc, argv, &env)))
		return error;

	error = wait_for_usp_raw_object();
	if (error)
		return error;
	icwmp_init_list_services();

	/* Only One instance should run*/
	cwmp_main->pid_file = fopen("/var/run/icwmpd.pid", "w+");
	fcntl(fileno(cwmp_main->pid_file), F_SETFD, fcntl(fileno(cwmp_main->pid_file), F_GETFD) | FD_CLOEXEC);
	int rc = flock(fileno(cwmp_main->pid_file), LOCK_EX | LOCK_NB);
	if (rc) {
		if (EWOULDBLOCK != errno) {
			char *piderr = "PID file creation failed: Quit the daemon!";
			fprintf(stderr, "%s\n", piderr);
			CWMP_LOG(ERROR, "%s", piderr);
			exit(EXIT_FAILURE);
		} else
			exit(EXIT_SUCCESS);
	}
	if (cwmp_main->pid_file)
		fclose(cwmp_main->pid_file);

	memcpy(&(cwmp_main->env), &env, sizeof(struct env));

	if ((error = create_cwmp_var_state_files()))
		return error;

	cwmp_uci_init();
	if ((error = global_conf_init()))
		return error;

	cwmp_get_deviceid();
	load_forced_inform_json_file();
	load_boot_inform_json_file();
	load_custom_notify_json();
	init_list_param_notify();
	//http_server_init();
	create_cwmp_session_structure();
	cwmp_uci_exit();
	generate_nonce_priv_key();
	cwmp_main->start_time = time(NULL);
	cwmp_main->start_diagnostics = false;
	cwmp_main->event_id = 0;
	cwmp_main->cwmp_period = 0;
	cwmp_main->cwmp_periodic_time = 0;
	cwmp_main->cwmp_periodic_enable = false;
	return CWMP_OK;
}

static void cwmp_exit()
{
	pthread_join(http_cr_server_thread, NULL);

	FREE(cwmp_main->deviceid.manufacturer);
	FREE(cwmp_main->deviceid.serialnumber);
	FREE(cwmp_main->deviceid.productclass);
	FREE(cwmp_main->deviceid.oui);
	FREE(cwmp_main->deviceid.softwareversion);
	FREE(cwmp_main->conf.lw_notification_hostname);
	FREE(cwmp_main->conf.ip);
	FREE(cwmp_main->conf.ipv6);
	FREE(cwmp_main->conf.acsurl);
	FREE(cwmp_main->conf.acs_userid);
	FREE(cwmp_main->conf.acs_passwd);
	FREE(cwmp_main->conf.interface);
	FREE(cwmp_main->conf.cpe_userid);
	FREE(cwmp_main->conf.cpe_passwd);
	FREE(cwmp_main->conf.ubus_socket);
	FREE(cwmp_main->conf.connection_request_path);
	FREE(cwmp_main->conf.default_wan_iface);
	FREE(cwmp_main->conf.forced_inform_json_file);
	FREE(cwmp_main->conf.custom_notify_json);
	FREE(cwmp_main->conf.boot_inform_json_file);
	FREE(nonce_privacy_key);

	clean_list_param_notify();
	bkp_tree_clean();
	cwmp_ubus_exit();
	clean_custom_inform_parameters();
	icwmp_cleanmem();
	cwmp_uci_exit();
	CWMP_LOG(INFO, "EXIT ICWMP");
	closelog();
	clean_cwmp_session_structure();
	FREE(cwmp_main);
}

void cwmp_end_handler(int signal_num __attribute__((unused)))
{
	cwmp_ubus_call("tr069", "exit", CWMP_UBUS_ARGS{ {} }, 0, NULL, NULL);
}

int main(int argc, char **argv)
{
	int error;

	if ((error = cwmp_init(argc, argv)))
		return error;

	if ((error = cwmp_init_backup_session(NULL, ALL)))
		return error;

	if ((error = cwmp_root_cause_events()))
		return error;

	signal(SIGINT, cwmp_end_handler);
	signal(SIGTERM, cwmp_end_handler);

	configure_var_state();
	http_server_init();

	uloop_init();

	cwmp_netlink_init();

	cwmp_ubus_init();

	trigger_cwmp_session_timer();

	initiate_cwmp_periodic_session_feature();

	http_server_start();

	uloop_run();
	uloop_done();

	cwmp_exit();

	return CWMP_OK;
}
