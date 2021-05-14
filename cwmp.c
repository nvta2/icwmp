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

#include "cwmp.h"

struct cwmp cwmp_main = { 0 };

int cwmp_session_rpc_destructor(struct rpc *rpc)
{
	list_del(&(rpc->list));
	free(rpc);
	return CWMP_OK;
}

int cwmp_get_retry_interval(struct cwmp *cwmp)
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

int cwmp_session_destructor(struct session *session)
{
	struct rpc *rpc;

	while (session->head_rpc_acs.next != &(session->head_rpc_acs)) {
		rpc = list_entry(session->head_rpc_acs.next, struct rpc, list);
		if (rpc_acs_methods[rpc->type].extra_clean != NULL)
			rpc_acs_methods[rpc->type].extra_clean(session, rpc);
		cwmp_session_rpc_destructor(rpc);
	}

	while (session->head_rpc_cpe.next != &(session->head_rpc_cpe)) {
		rpc = list_entry(session->head_rpc_cpe.next, struct rpc, list);
		cwmp_session_rpc_destructor(rpc);
	}

	if (session->list.next != NULL && session->list.prev != NULL)
		list_del(&(session->list));

	free(session);

	return CWMP_OK;
}
int cwmp_move_session_to_session_queue(struct cwmp *cwmp, struct session *session)
{
	struct list_head *ilist, *jlist;
	struct rpc *rpc_acs, *queue_rpc_acs, *rpc_cpe;
	struct event_container *event_container_old, *event_container_new;
	struct session *session_queue;
	bool dup;

	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	cwmp->retry_count_session++;
	cwmp->session_send = NULL;
	if (cwmp->head_session_queue.next == &(cwmp->head_session_queue)) {
		list_add_tail(&(session->list), &(cwmp->head_session_queue));
		session->hold_request = 0;
		session->digest_auth = 0;
		cwmp->head_event_container = &(session->head_event_container);
		if (session->head_rpc_acs.next != &(session->head_rpc_acs)) {
			rpc_acs = list_entry(session->head_rpc_acs.next, struct rpc, list);
			if (rpc_acs->type != RPC_ACS_INFORM) {
				if ((rpc_acs = cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM)) == NULL) {
					pthread_mutex_unlock(&(cwmp->mutex_session_queue));
					return CWMP_MEM_ERR;
				}
			}
		} else {
			if ((rpc_acs = cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM)) == NULL) {
				pthread_mutex_unlock(&(cwmp->mutex_session_queue));
				return CWMP_MEM_ERR;
			}
		}
		while (session->head_rpc_cpe.next != &(session->head_rpc_cpe)) {
			rpc_cpe = list_entry(session->head_rpc_cpe.next, struct rpc, list);
			cwmp_session_rpc_destructor(rpc_cpe);
		}
		bkp_session_move_inform_to_inform_queue();
		bkp_session_save();
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_OK;
	}
	list_for_each (ilist, &(session->head_event_container)) {
		event_container_old = list_entry(ilist, struct event_container, list);
		event_container_new = cwmp_add_event_container(cwmp, event_container_old->code, event_container_old->command_key);
		if (event_container_new == NULL) {
			pthread_mutex_unlock(&(cwmp->mutex_session_queue));
			return CWMP_MEM_ERR;
		}
		list_splice_init(&(event_container_old->head_dm_parameter), &(event_container_new->head_dm_parameter));
		cwmp_save_event_container(event_container_new);
	}
	session_queue = list_entry(cwmp->head_event_container, struct session, head_event_container);
	list_for_each (ilist, &(session->head_rpc_acs)) {
		rpc_acs = list_entry(ilist, struct rpc, list);
		dup = false;
		list_for_each (jlist, &(session_queue->head_rpc_acs)) {
			queue_rpc_acs = list_entry(jlist, struct rpc, list);
			if (queue_rpc_acs->type == rpc_acs->type && (rpc_acs->type == RPC_ACS_INFORM || rpc_acs->type == RPC_ACS_GET_RPC_METHODS)) {
				dup = true;
				break;
			}
		}
		if (dup) {
			continue;
		}
		ilist = ilist->prev;
		list_del(&(rpc_acs->list));
		list_add_tail(&(rpc_acs->list), &(session_queue->head_rpc_acs));
	}
	cwmp_session_destructor(session);
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	return CWMP_OK;
}

int cwmp_schedule_rpc(struct cwmp *cwmp, struct session *session)
{
	struct list_head *ilist;
	struct rpc *rpc_acs, *rpc_cpe;

	if (http_client_init(cwmp)) {
		CWMP_LOG(INFO, "Initializing http client failed");
		goto retry;
	}

	while (1) {
		list_for_each (ilist, &(session->head_rpc_acs)) {
			rpc_acs = list_entry(ilist, struct rpc, list);
			if (!rpc_acs->type)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s RPC message to send to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].prepare_message(cwmp, session, rpc_acs))
				goto retry;

			if (xml_set_cwmp_id(session))
				goto retry;

			CWMP_LOG(INFO, "Send the %s RPC message to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (xml_send_message(cwmp, session, rpc_acs))
				goto retry;

			CWMP_LOG(INFO, "Get the %sResponse message from the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].parse_response)
				if (rpc_acs_methods[rpc_acs->type].parse_response(cwmp, session, rpc_acs))
					goto retry;

			ilist = ilist->prev;
			if (rpc_acs_methods[rpc_acs->type].extra_clean != NULL)
				rpc_acs_methods[rpc_acs->type].extra_clean(session, rpc_acs);
			cwmp_session_rpc_destructor(rpc_acs);
			MXML_DELETE(session->tree_in);
			MXML_DELETE(session->tree_out);
			if (session->hold_request)
				break;
		}
		CWMP_LOG(INFO, "Send empty message to the ACS");
		if (xml_send_message(cwmp, session, NULL))
			goto retry;
		if (!session->tree_in)
			goto next;

		CWMP_LOG(INFO, "Receive request from the ACS");
		if (xml_handle_message(session))
			goto retry;

		while (session->head_rpc_cpe.next != &(session->head_rpc_cpe)) {
			rpc_cpe = list_entry(session->head_rpc_cpe.next, struct rpc, list);
			if (!rpc_cpe->type)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s%s message", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (cwmp_rpc_cpe_handle_message(session, rpc_cpe))
				goto retry;
			MXML_DELETE(session->tree_in);

			CWMP_LOG(INFO, "Send the %s%s message to the ACS", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (xml_send_message(cwmp, session, rpc_cpe))
				goto retry;
			MXML_DELETE(session->tree_out);

			cwmp_session_rpc_destructor(rpc_cpe);
			if (!session->tree_in)
				break;

			CWMP_LOG(INFO, "Receive request from the ACS");
			if (xml_handle_message(session))
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

int run_session_end_func()
{
	if (end_session_flag & END_SESSION_RELOAD) {
		CWMP_LOG(INFO, "Config reload: end session request");
		cwmp_apply_acs_changes();
	}

	if (end_session_flag & END_SESSION_SET_NOTIFICATION_UPDATE) {
		CWMP_LOG(INFO, "SetParameterAttributes end session: update enabled notify file");
		cwmp_update_enabled_notify_file();
	}
	if (end_session_flag & END_SESSION_TRANSACTION_COMMIT) {
		cwmp_transaction_commit();
		transaction_started = false;
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

	if (end_session_flag & END_SESSION_REBOOT) {
		CWMP_LOG(INFO, "Executing Reboot: end session request");
		cwmp_reboot(commandKey);
		FREE(commandKey);
		exit(EXIT_SUCCESS);
	}

	if (end_session_flag & END_SESSION_X_FACTORY_RESET_SOFT) {
		CWMP_LOG(INFO, "Executing factory reset soft: end session request");
		cwmp_factory_reset();
		exit(EXIT_SUCCESS);
	}

	if (end_session_flag & END_SESSION_FACTORY_RESET) {
		CWMP_LOG(INFO, "Executing factory reset: end session request");
		cwmp_factory_reset();
		exit(EXIT_SUCCESS);
	}

	end_session_flag = 0;
	return CWMP_OK;
}

void cwmp_schedule_session(struct cwmp *cwmp)
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
			pthread_cond_timedwait(&(cwmp->threshold_session_send), &(cwmp->mutex_session_send), &time_to_wait);
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
		error = cwmp_schedule_rpc(cwmp, session);
		CWMP_LOG(INFO, "End session");
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

int cwmp_rpc_cpe_handle_message(struct session *session, struct rpc *rpc_cpe)
{
	if (xml_prepare_msg_out(session))
		return -1;

	if (rpc_cpe_methods[rpc_cpe->type].handler(session, rpc_cpe))
		return -1;

	if (xml_set_cwmp_id_rpc_cpe(session))
		return -1;

	return 0;
}

void *thread_uloop_run(void *v __attribute__((unused)))
{
	ubus_init(&cwmp_main);
	return NULL;
}

void *thread_http_cr_server_listen(void *v __attribute__((unused)))
{
	http_server_listen();
	return NULL;
}

void signal_handler(int signal_num __attribute__((unused)))
{
	close(cwmp_main.cr_socket_desc);
	_exit(EXIT_SUCCESS);
}

int cwmp_init(int argc, char **argv, struct cwmp *cwmp)
{
	int error;
	struct env env;

	memset(&env, 0, sizeof(struct env));

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

	if ((error = global_env_init(argc, argv, &env, &cwmp->conf)))
		return error;


	pthread_mutex_init(&cwmp->mutex_periodic, NULL);
	pthread_mutex_init(&cwmp->mutex_session_queue, NULL);
	pthread_mutex_init(&cwmp->mutex_session_send, NULL);
	memcpy(&(cwmp->env), &env, sizeof(struct env));
	INIT_LIST_HEAD(&(cwmp->head_session_queue));

	if ((error = global_conf_init(&(cwmp->conf))))
		return error;

	cwmp_get_deviceid(cwmp);
	return CWMP_OK;
}

int cwmp_exit(void)
{
	struct cwmp *cwmp = &cwmp_main;

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
	bkp_tree_clean();
	ubus_exit();
	uloop_done();
	return 0;
}

int main(int argc, char **argv)
{
	struct cwmp *cwmp = &cwmp_main;
	int error;
	pthread_t periodic_event_thread;
	pthread_t scheduleInform_thread;
	pthread_t change_du_state_thread;
	pthread_t download_thread;
	pthread_t schedule_download_thread;
	pthread_t apply_schedule_download_thread;
	pthread_t upload_thread;
	pthread_t ubus_thread;
	pthread_t http_cr_server_thread;
	pthread_t periodic_check_notify;

	struct sigaction act = { 0 };

	memset(&cwmp_main, 0, sizeof(struct cwmp));

	if ((error = cwmp_init(argc, argv, cwmp)))
		return error;

	CWMP_LOG(INFO, "STARTING ICWMP with PID :%d", getpid());
	cwmp->start_time = time(NULL);

	if ((error = cwmp_init_backup_session(cwmp, NULL, ALL)))
		return error;

	if ((error = cwmp_root_cause_events(cwmp)))
		return error;

	http_server_init();

	act.sa_handler = signal_handler;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGTERM, &act, 0);

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
		CWMP_LOG(ERROR, "Error when creating the download thread!");
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

	cwmp_schedule_session(cwmp);

	pthread_join(ubus_thread, NULL);
	pthread_join(periodic_event_thread, NULL);
	pthread_join(scheduleInform_thread, NULL);
	pthread_join(download_thread, NULL);
	pthread_join(upload_thread, NULL);
	pthread_join(schedule_download_thread, NULL);
	pthread_join(apply_schedule_download_thread, NULL);
	pthread_join(change_du_state_thread, NULL);
	pthread_join(http_cr_server_thread, NULL);

	CWMP_LOG(INFO, "EXIT ICWMP");
	cwmp_exit();
	return CWMP_OK;
}
