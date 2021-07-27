/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <pthread.h>
#include <libubox/uloop.h>
#include <sys/file.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "session.h"
#include "backupSession.h"
#include "http.h"
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
#include "rpc_soap.h"

static pthread_t periodic_event_thread;
static pthread_t scheduleInform_thread;
static pthread_t change_du_state_thread;
static pthread_t download_thread;
static pthread_t schedule_download_thread;
static pthread_t apply_schedule_download_thread;
static pthread_t upload_thread;
static pthread_t ubus_thread;
static pthread_t http_cr_server_thread;
static pthread_t periodic_check_notify;
static pthread_t signal_handler_thread;

static int cwmp_get_retry_interval(struct cwmp *cwmp)
{
	int retry_count = 0;
	double min = 0;
	double max = 0;
	int m = cwmp->conf.retry_min_wait_interval;
	int k = cwmp->conf.retry_interval_multiplier;
	int exp = cwmp->retry_count_session;
	if (exp == 0)
		return MAX_INT32;
	if (exp > 10)
		exp = 10;
	min = pow(((double)k / 1000), (double)(exp - 1)) * m;
	max = pow(((double)k / 1000), (double)exp) * m;
	icwmp_srand(time(NULL));
	retry_count = icwmp_rand() % ((int)max + 1 - (int)min) + (int)min;
	return (retry_count);
}

static int cwmp_rpc_cpe_handle_message(struct session *session, struct rpc *rpc_cpe)
{
	if (xml_prepare_msg_out(session))
		return -1;

	if (rpc_cpe_methods[rpc_cpe->type].handler(session, rpc_cpe))
		return -1;

	if (xml_set_cwmp_id_rpc_cpe(session))
		return -1;

	return 0;
}

static void cwmp_prepare_value_change(struct cwmp *cwmp)
{
	struct event_container *event_container;
	if (list_value_change.next == &(list_value_change))
		return;
	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container(cwmp, EVENT_IDX_4VALUE_CHANGE, "");
	if (!event_container)
		goto end;
	pthread_mutex_lock(&(mutex_value_change));
	list_splice_init(&(list_value_change), &(event_container->head_dm_parameter));
	pthread_mutex_unlock(&(mutex_value_change));
	cwmp_save_event_container(event_container);

end:
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
}

static int cwmp_schedule_rpc(struct cwmp *cwmp, struct session *session)
{
	struct list_head *ilist;
	struct rpc *rpc_acs, *rpc_cpe;

	if (http_client_init(cwmp) || thread_end) {
		CWMP_LOG(INFO, "Initializing http client failed");
		goto retry;
	}

	while (1) {
		list_for_each (ilist, &(session->head_rpc_acs)) {

			rpc_acs = list_entry(ilist, struct rpc, list);
			if (!rpc_acs->type || thread_end)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s RPC message to send to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].prepare_message(cwmp, session, rpc_acs) || thread_end)
				goto retry;

			if (xml_set_cwmp_id(session) || thread_end)
				goto retry;

			CWMP_LOG(INFO, "Send the %s RPC message to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (xml_send_message(cwmp, session, rpc_acs) || thread_end)
				goto retry;

			CWMP_LOG(INFO, "Get the %sResponse message from the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].parse_response || thread_end)
				if (rpc_acs_methods[rpc_acs->type].parse_response(cwmp, session, rpc_acs))
					goto retry;

			ilist = ilist->prev;
			if (rpc_acs_methods[rpc_acs->type].extra_clean != NULL)
				rpc_acs_methods[rpc_acs->type].extra_clean(session, rpc_acs);
			cwmp_session_rpc_destructor(rpc_acs);
			MXML_DELETE(session->tree_in);
			MXML_DELETE(session->tree_out);
			if (session->hold_request || thread_end)
				break;
		}
		CWMP_LOG(INFO, "Send empty message to the ACS");
		if (xml_send_message(cwmp, session, NULL) || thread_end)
			goto retry;
		if (!session->tree_in || thread_end)
			goto next;

		CWMP_LOG(INFO, "Receive request from the ACS");
		if (xml_handle_message(session) || thread_end)
			goto retry;

		while (session->head_rpc_cpe.next != &(session->head_rpc_cpe)) {

			rpc_cpe = list_entry(session->head_rpc_cpe.next, struct rpc, list);
			if (!rpc_cpe->type || thread_end)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s%s message", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (cwmp_rpc_cpe_handle_message(session, rpc_cpe) || thread_end)
				goto retry;
			MXML_DELETE(session->tree_in);

			CWMP_LOG(INFO, "Send the %s%s message to the ACS", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (xml_send_message(cwmp, session, rpc_cpe) || thread_end)
				goto retry;
			MXML_DELETE(session->tree_out);

			cwmp_session_rpc_destructor(rpc_cpe);
			if (!session->tree_in || thread_end)
				break;

			CWMP_LOG(INFO, "Receive request from the ACS");
			if (xml_handle_message(session) || thread_end)
				goto retry;
		}

	next:
		if (session->head_rpc_acs.next == &(session->head_rpc_acs))
			break;
		MXML_DELETE(session->tree_in);
		MXML_DELETE(session->tree_out);
	}

	session->error = CWMP_OK;
	goto end;

retry:
	CWMP_LOG(INFO, "Failed");
	session->error = CWMP_RETRY_SESSION;
	event_remove_noretry_event_container(session, cwmp);

end:
	MXML_DELETE(session->tree_in);
	MXML_DELETE(session->tree_out);
	http_client_exit();
	xml_exit();
	return session->error;
}

int run_session_end_func(void)
{
	if (end_session_flag & END_SESSION_RELOAD) {
		CWMP_LOG(INFO, "Config reload: end session request");
		cwmp_apply_acs_changes();
	}

	if (end_session_flag & END_SESSION_SET_NOTIFICATION_UPDATE) {
		CWMP_LOG(INFO, "SetParameterAttributes/Values end session: update enabled notify file");
		cwmp_update_enabled_notify_file();
	}

	if (end_session_flag & END_SESSION_NSLOOKUP_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing nslookupdiagnostic: end session request");
		cwmp_nslookup_diagnostics();
	}

	if (end_session_flag & END_SESSION_TRACEROUTE_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing traceroutediagnostic: end session request");
		cwmp_traceroute_diagnostics();
	}

	if (end_session_flag & END_SESSION_UDPECHO_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing udpechodiagnostic: end session request");
		cwmp_udp_echo_diagnostics();
	}

	if (end_session_flag & END_SESSION_SERVERSELECTION_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing serverselectiondiagnostic: end session request");
		cwmp_serverselection_diagnostics();
	}

	if (end_session_flag & END_SESSION_IPPING_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing ippingdiagnostic: end session request");
		cwmp_ip_ping_diagnostics();
	}

	if (end_session_flag & END_SESSION_DOWNLOAD_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing download diagnostic: end session request");
		cwmp_download_diagnostics();
	}

	if (end_session_flag & END_SESSION_UPLOAD_DIAGNOSTIC) {
		CWMP_LOG(INFO, "Executing upload diagnostic: end session request");
		cwmp_upload_diagnostics();
	}

	if (end_session_flag & END_SESSION_RESTART_SERVICES) {
		CWMP_LOG(INFO, "Restart modified services");
		icwmp_restart_services();
	}

	if (end_session_flag & END_SESSION_REBOOT) {
		CWMP_LOG(INFO, "Executing Reboot: end session request");
		cwmp_reboot(commandKey);
		exit(EXIT_SUCCESS);
	}

	if (end_session_flag & END_SESSION_FACTORY_RESET) {
		CWMP_LOG(INFO, "Executing factory reset: end session request");
		cwmp_factory_reset();
		exit(EXIT_SUCCESS);
	}

	if (end_session_flag & END_SESSION_X_FACTORY_RESET_SOFT) {
		CWMP_LOG(INFO, "Executing factory reset soft: end session request");
		cwmp_factory_reset();
		exit(EXIT_SUCCESS);
	}

	icwmp_cleanmem();
	end_session_flag = 0;
	return CWMP_OK;
}

static void cwmp_schedule_session(struct cwmp *cwmp)
{
	struct list_head *ilist;
	struct session *session;
	int t, error = CWMP_OK;
	static struct timespec time_to_wait = { 0, 0 };
	bool retry = false;
	char *exec_download = NULL;
	int is_notify = 0;

	cwmp->cwmp_cr_event = 0;
	while (1) {
		pthread_mutex_lock(&(cwmp->mutex_session_send));
		ilist = (&(cwmp->head_session_queue))->next;
		while ((ilist == &(cwmp->head_session_queue)) || retry) {
			t = cwmp_get_retry_interval(cwmp);
			time_to_wait.tv_sec = time(NULL) + t;
			CWMP_LOG(INFO, "Waiting the next session");

			if (thread_end) {
				pthread_mutex_unlock(&(cwmp->mutex_session_send));
				return;
			}

			pthread_cond_timedwait(&(cwmp->threshold_session_send), &(cwmp->mutex_session_send), &time_to_wait);
	
			if (thread_end) {
				pthread_mutex_unlock(&(cwmp->mutex_session_send));
				return;
			}

			ilist = (&(cwmp->head_session_queue))->next;
			retry = false;
		}

		session = list_entry(ilist, struct session, list);
		if (file_exists(DM_ENABLED_NOTIFY)) {
			if (!event_exist_in_list(cwmp, EVENT_IDX_4VALUE_CHANGE))
				is_notify = check_value_change();
		}
		if (is_notify > 0 || !file_exists(DM_ENABLED_NOTIFY))
			cwmp_update_enabled_notify_file();
		cwmp_prepare_value_change(cwmp);
		clean_list_value_change();
		if ((error = cwmp_move_session_to_session_send(cwmp, session))) {
			CWMP_LOG(EMERG, "FATAL error in the mutex process in the session scheduler!");
			exit(EXIT_FAILURE);
		}

		cwmp->session_status.last_end_time = 0;
		cwmp->session_status.last_start_time = time(NULL);
		cwmp->session_status.last_status = SESSION_RUNNING;
		cwmp->session_status.next_retry = 0;

		if (file_exists(fc_cookies))
			remove(fc_cookies);
		CWMP_LOG(INFO, "Start session");

		uci_get_value(UCI_CPE_EXEC_DOWNLOAD, &exec_download);
		if (strcmp(exec_download, "1") == 0) {
			CWMP_LOG(INFO, "Firmware downloaded and applied successfully");
			uci_set_value(UCI_CPE_EXEC_DOWNLOAD, "0", CWMP_CMD_SET);
		}
		FREE(exec_download);
		error = cwmp_schedule_rpc(cwmp, session);
		CWMP_LOG(INFO, "End session");

		if (thread_end) {
			event_remove_all_event_container(session, RPC_SEND);
			run_session_end_func();
			cwmp_session_destructor(session);
			pthread_mutex_unlock(&(cwmp->mutex_session_send));
			return;
		}

		if (session->error == CWMP_RETRY_SESSION && (!list_empty(&(session->head_event_container)) || (list_empty(&(session->head_event_container)) && cwmp->cwmp_cr_event == 0))) {
			run_session_end_func();
			error = cwmp_move_session_to_session_queue(cwmp, session);
			CWMP_LOG(INFO, "Retry session, retry count = %d, retry in %ds", cwmp->retry_count_session, cwmp_get_retry_interval(cwmp));
			retry = true;
			cwmp->session_status.last_end_time = time(NULL);
			cwmp->session_status.last_status = SESSION_FAILURE;
			cwmp->session_status.next_retry = time(NULL) + cwmp_get_retry_interval(cwmp);
			cwmp->session_status.failure_session++;
			pthread_mutex_unlock(&(cwmp->mutex_session_send));
			continue;
		}
		event_remove_all_event_container(session, RPC_SEND);
		run_session_end_func();
		cwmp_session_destructor(session);
		cwmp->session_send = NULL;
		cwmp->retry_count_session = 0;
		cwmp->session_status.last_end_time = time(NULL);
		cwmp->session_status.last_status = SESSION_SUCCESS;
		cwmp->session_status.next_retry = 0;
		cwmp->session_status.success_session++;
		pthread_mutex_unlock(&(cwmp->mutex_session_send));
	}
}

static void *thread_uloop_run(void *v __attribute__((unused)))
{
	cwmp_ubus_init(&cwmp_main);
	return NULL;
}

static void *thread_http_cr_server_listen(void *v __attribute__((unused)))
{
	http_server_listen();
	return NULL;
}

void load_forced_inform_json_file(struct cwmp *cwmp)
{
	struct blob_buf bbuf;
	struct blob_attr *cur;
	struct blob_attr *forced_inform_list = NULL;
	int rem;
	struct cwmp_dm_parameter cwmp_dm_param = { 0 };

	if (cwmp->conf.forced_inform_json_file == NULL || !file_exists(cwmp->conf.forced_inform_json_file))
		return;

	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);

	if (blobmsg_add_json_from_file(&bbuf, cwmp->conf.forced_inform_json_file) == false) {
		CWMP_LOG(WARNING, "The file %s is not a valid JSON file", cwmp->conf.forced_inform_json_file);
		return;
	}
	blobmsg_for_each_attr(cur, bbuf.head, rem)
	{
		if (blobmsg_type(cur) == BLOBMSG_TYPE_ARRAY) {
			forced_inform_list = cur;
			break;
		}
	}
	if (forced_inform_list == NULL) {
		CWMP_LOG(WARNING, "The JSON file %s doesn't contain a parameters list", cwmp->conf.forced_inform_json_file);
		return;
	}
	blobmsg_for_each_attr(cur, forced_inform_list, rem)
	{
		char parameter_path[128];
		snprintf(parameter_path, sizeof(parameter_path), "%s", blobmsg_get_string(cur));
		if (parameter_path[strlen(parameter_path)-1] == '.') {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Only leaf parameters are allowed.", parameter_path);
			continue;
		}
		char *fault = cwmp_get_single_parameter_value(parameter_path, &cwmp_dm_param);
		if (fault != NULL) {
			CWMP_LOG(WARNING, "%s is rejected as inform parameter. Wrong parameter path.", parameter_path);
			continue;
		}
		custom_forced_inform_parameters[nbre_custom_inform++] = strdup(parameter_path);
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
}

static int cwmp_init(int argc, char **argv, struct cwmp *cwmp)
{
	int error;
	struct env env;

	memset(&env, 0, sizeof(struct env));
	if ((error = global_env_init(argc, argv, &env)))
		return error;

	icwmp_init_list_services();
	/* Only One instance should run*/
	cwmp->pid_file = fopen("/var/run/icwmpd.pid", "w+");
	fcntl(fileno(cwmp->pid_file), F_SETFD, fcntl(fileno(cwmp->pid_file), F_GETFD) | FD_CLOEXEC);
	int rc = flock(fileno(cwmp->pid_file), LOCK_EX | LOCK_NB);
	if (rc) {
		if (EWOULDBLOCK != errno) {
			char *piderr = "PID file creation failed: Quit the daemon!";
			fprintf(stderr, "%s\n", piderr);
			CWMP_LOG(ERROR, "%s", piderr);
			exit(EXIT_FAILURE);
		} else
			exit(EXIT_SUCCESS);
	}
	if (cwmp->pid_file)
		fclose(cwmp->pid_file);

	pthread_mutex_init(&cwmp->mutex_periodic, NULL);
	pthread_mutex_init(&cwmp->mutex_session_queue, NULL);
	pthread_mutex_init(&cwmp->mutex_session_send, NULL);
	memcpy(&(cwmp->env), &env, sizeof(struct env));
	INIT_LIST_HEAD(&(cwmp->head_session_queue));
	if ((error = global_conf_init(cwmp)))
		return error;

	cwmp_get_deviceid(cwmp);
	load_forced_inform_json_file(cwmp);
	return CWMP_OK;
}

static void cwmp_free(struct cwmp *cwmp)
{
	FREE(cwmp->deviceid.manufacturer);
	FREE(cwmp->deviceid.serialnumber);
	FREE(cwmp->deviceid.productclass);
	FREE(cwmp->deviceid.oui);
	FREE(cwmp->deviceid.softwareversion);
	FREE(cwmp->conf.lw_notification_hostname);
	FREE(cwmp->conf.ip);
	FREE(cwmp->conf.ipv6);
	FREE(cwmp->conf.acsurl);
	FREE(cwmp->conf.acs_userid);
	FREE(cwmp->conf.acs_passwd);
	FREE(cwmp->conf.interface);
	FREE(cwmp->conf.cpe_userid);
	FREE(cwmp->conf.cpe_passwd);
	FREE(cwmp->conf.ubus_socket);
	FREE(cwmp->conf.connection_request_path);
	FREE(cwmp->conf.default_wan_iface);
	FREE(cwmp->conf.forced_inform_json_file);
	bkp_tree_clean();
	cwmp_ubus_exit();
	clean_custom_inform_parameters();
}

static void *thread_cwmp_signal_handler_thread(void *arg)
{
	sigset_t *set = (sigset_t *)arg;
	int s, signal_num;

	for (;;) {
		s = sigwait(set, &signal_num);
		if (s == -1) {
			CWMP_LOG(ERROR, "Error in sigwait");
		} else {
			CWMP_LOG(INFO, "Catch of Signal(%d)", signal_num);

			if (signal_num == SIGINT || signal_num == SIGTERM) {

				signal_exit = true;

				if (!ubus_exit)
					cwmp_ubus_call("tr069", "command", CWMP_UBUS_ARGS{ { "command", { .str_val = "exit" }, UBUS_String } }, 1, NULL, NULL);

				break;
			}
		}
	}

	return NULL;
}

int main(int argc, char **argv)
{
	struct cwmp *cwmp = &cwmp_main;
	sigset_t set;
	int error;

	if ((error = cwmp_init(argc, argv, cwmp)))
		return error;

	CWMP_LOG(INFO, "STARTING ICWMP with PID :%d", getpid());
	cwmp->start_time = time(NULL);

	if ((error = cwmp_init_backup_session(cwmp, NULL, ALL)))
		return error;

	if ((error = cwmp_root_cause_events(cwmp)))
		return error;

	http_server_init();

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigprocmask(SIG_BLOCK, &set, NULL);

	error = pthread_create(&http_cr_server_thread, NULL, &thread_http_cr_server_listen, NULL);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the http connection request server thread!");
	}

	error = pthread_create(&ubus_thread, NULL, &thread_uloop_run, NULL);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the ubus thread!");
	}

	error = pthread_create(&periodic_event_thread, NULL, &thread_event_periodic, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the periodic event thread!");
	}

	error = pthread_create(&periodic_check_notify, NULL, &thread_periodic_check_notify, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the periodic check notify thread!");
	}
	error = pthread_create(&scheduleInform_thread, NULL, &thread_cwmp_rpc_cpe_scheduleInform, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the scheduled inform thread!");
	}

	error = pthread_create(&download_thread, NULL, &thread_cwmp_rpc_cpe_download, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the download thread!");
	}

	error = pthread_create(&change_du_state_thread, NULL, &thread_cwmp_rpc_cpe_change_du_state, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the state change thread!");
	}

	error = pthread_create(&schedule_download_thread, NULL, &thread_cwmp_rpc_cpe_schedule_download, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the schedule download thread!");
	}

	error = pthread_create(&apply_schedule_download_thread, NULL, &thread_cwmp_rpc_cpe_apply_schedule_download, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the schedule download thread!");
	}

	error = pthread_create(&upload_thread, NULL, &thread_cwmp_rpc_cpe_upload, (void *)cwmp);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the download thread!");
	}

	error = pthread_create(&signal_handler_thread, NULL, &thread_cwmp_signal_handler_thread, (void *)&set);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the signal handler thread!");
	}

	cwmp_schedule_session(cwmp);

	/* Join all threads */
	pthread_join(periodic_event_thread, NULL);
	pthread_join(periodic_check_notify, NULL);
	pthread_join(scheduleInform_thread, NULL);
	pthread_join(download_thread, NULL);
	pthread_join(upload_thread, NULL);
	pthread_join(schedule_download_thread, NULL);
	pthread_join(apply_schedule_download_thread, NULL);
	pthread_join(change_du_state_thread, NULL);
	pthread_join(http_cr_server_thread, NULL);
	pthread_join(ubus_thread, NULL);
	pthread_join(signal_handler_thread, NULL);

	/* Free all memory allocation */
	cwmp_free(cwmp);

	CWMP_LOG(INFO, "EXIT ICWMP");
	return CWMP_OK;
}
