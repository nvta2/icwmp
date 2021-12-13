/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include "session.h"
#include "event.h"
#include "backupSession.h"
#include "config.h"
#include "rpc_soap.h"
#include "cwmp_uci.h"
#include "log.h"
#include "http.h"
#include "notifications.h"
#include "diagnostic.h"

pthread_mutex_t start_session_mutext = PTHREAD_MUTEX_INITIALIZER;
static void cwmp_priodic_session_timer(struct uloop_timeout *timeout);
struct uloop_timeout session_timer = { .cb = cwmp_schedule_session };
struct uloop_timeout priodic_session_timer = { .cb = cwmp_priodic_session_timer };
struct uloop_timeout retry_session_timer = { .cb = cwmp_schedule_session };

unsigned int end_session_flag = 0;

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

		// If restart service caused firewall restart, wait for firewall restart to complete
		if (g_firewall_restart == true)
			check_firewall_restart_state();

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

void start_cwmp_session(struct cwmp *cwmp)
{
	int t, error = CWMP_OK;
	static struct timespec time_to_wait = { 0, 0 };
	char *exec_download = NULL;
	int is_notify = 0;

	printf("%s:%s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	cwmp->cwmp_cr_event = 0;
	struct list_head *ilist;
	struct session *session;

	/*
	 * Get the session to be executed
	 */
 	ilist = (&(cwmp->head_session_queue))->next;

	session = list_entry(ilist, struct session, list);

	/*
	 * Initiate UCI
	 */
	cwmp_uci_init();

	/*
	 * Value changes
	 */
	if (file_exists(DM_ENABLED_NOTIFY)) {
		if (!event_exist_in_list(cwmp, EVENT_IDX_4VALUE_CHANGE))
			is_notify = check_value_change();
	}
	if (is_notify > 0 || !file_exists(DM_ENABLED_NOTIFY) || cwmp->custom_notify_active) {
		cwmp->custom_notify_active = false;
		cwmp_update_enabled_notify_file();
	}
	cwmp_prepare_value_change(cwmp);
	clean_list_value_change();

	/*
	 * Start session
	 */
	if ((error = cwmp_move_session_to_session_send(cwmp, session))) {
		CWMP_LOG(EMERG, "FATAL error in the mutex process in the session scheduler!");
		exit(EXIT_FAILURE);
	}
	printf("%s:%s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	cwmp->session_status.last_end_time = 0;
	cwmp->session_status.last_start_time = time(NULL);
	cwmp->session_status.last_status = SESSION_RUNNING;
	cwmp->session_status.next_retry = 0;

	if (file_exists(fc_cookies))
		remove(fc_cookies);

	CWMP_LOG(INFO, "Start session");

	uci_get_value(UCI_CPE_EXEC_DOWNLOAD, &exec_download);
	if (exec_download && strcmp(exec_download, "1") == 0) {
		CWMP_LOG(INFO, "Firmware downloaded and applied successfully");
		cwmp_uci_set_value("cwmp", "cpe", "exec_download", "0");
		cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	}
	FREE(exec_download);
	error = cwmp_schedule_rpc(cwmp, session);

	/*
	 * End session
	 */
	CWMP_LOG(INFO, "End session");

	if (thread_end) {
		event_remove_all_event_container(session, RPC_SEND);
		run_session_end_func();
		cwmp_session_destructor(session);
		return;
	}

	run_session_end_func();
	if (session->error == CWMP_RETRY_SESSION && (!list_empty(&(session->head_event_container)) || (list_empty(&(session->head_event_container)) && cwmp->cwmp_cr_event == 0))) { //CWMP Retry session
		error = cwmp_move_session_to_session_queue(cwmp, session);
		CWMP_LOG(INFO, "Retry session, retry count = %d, retry in %ds", cwmp->retry_count_session, cwmp_get_session_retry_interval(cwmp));
		cwmp->session_status.last_end_time = time(NULL);
		cwmp->session_status.last_status = SESSION_FAILURE;
		cwmp->session_status.next_retry = time(NULL) + cwmp_get_session_retry_interval(cwmp);
		cwmp->session_status.failure_session++;
		t = cwmp_get_session_retry_interval(cwmp);
		time_to_wait.tv_sec = time(NULL) + t;
		uloop_timeout_set(&retry_session_timer, 1000 * t);
	} else {
		event_remove_all_event_container(session, RPC_SEND);
		cwmp_session_destructor(session);
		cwmp->session_send = NULL;
		cwmp->retry_count_session = 0;
		cwmp->session_status.last_end_time = time(NULL);
		cwmp->session_status.last_status = SESSION_SUCCESS;
		cwmp->session_status.next_retry = 0;
		cwmp->session_status.success_session++;
	}
	CWMP_LOG(INFO, "Waiting the next session");

}

void trigger_cwmp_session_timer()
{
	uloop_timeout_cancel(&retry_session_timer);
	uloop_timeout_set(&session_timer, 10);
}

void cwmp_schedule_session(struct uloop_timeout *timeout)
{
	pthread_mutex_lock(&start_session_mutext);
	start_cwmp_session(&cwmp_main);
	pthread_mutex_unlock(&start_session_mutext);
}

static void cwmp_priodic_session_timer(struct uloop_timeout *timeout)
{
	if (cwmp_main.conf.periodic_enable && cwmp_main.conf.period > 0) {
		cwmp_main.session_status.next_periodic = time(NULL) + cwmp_main.conf.period;
		uloop_timeout_set(&priodic_session_timer, cwmp_main.conf.period * 1000);
	}
	if (cwmp_main.conf.periodic_enable) {
		cwmp_add_event_container(&cwmp_main, EVENT_IDX_2PERIODIC, "");
		trigger_cwmp_session_timer();
	}
}

long int cwmp_periodic_session_time(void)
{
	long int delta_time;
	long int periodic_time;

	delta_time = time(NULL) - cwmp_main.conf.time;
	if(delta_time > 0)
		periodic_time = cwmp_main.conf.period - (delta_time % cwmp_main.conf.period);
	else
		periodic_time = (-delta_time) % cwmp_main.conf.period;

	cwmp_main.session_status.next_periodic = time(NULL) + periodic_time;
	return  periodic_time;
}

void initiate_cwmp_periodic_session_feature()
{
	uloop_timeout_cancel(&priodic_session_timer);
	if (cwmp_main.conf.periodic_enable && cwmp_main.conf.period > 0) {
		if (cwmp_main.conf.time >= 0){
			CWMP_LOG(INFO, "init periodic inform: reference time = %ld, interval = %d\n", cwmp_main.conf.time, cwmp_main.conf.period);
			uloop_timeout_set(&priodic_session_timer, cwmp_periodic_session_time() * 1000);
		}
		else {
			CWMP_LOG(INFO, "init periodic inform: reference time = n/a, interval = %d\n", cwmp_main.conf.period);
			cwmp_main.session_status.next_periodic = time(NULL) + cwmp_main.conf.period;
			uloop_timeout_set(&priodic_session_timer, cwmp_main.conf.period * 1000);
		}
	}
}

struct rpc *cwmp_add_session_rpc_cpe(struct session *session, int type)
{
	struct rpc *rpc_cpe;

	rpc_cpe = calloc(1, sizeof(struct rpc));
	if (rpc_cpe == NULL) {
		return NULL;
	}
	rpc_cpe->type = type;
	list_add_tail(&(rpc_cpe->list), &(session->head_rpc_cpe));
	return rpc_cpe;
}

struct rpc *cwmp_add_session_rpc_acs(struct session *session, int type)
{
	struct rpc *rpc_acs;

	rpc_acs = calloc(1, sizeof(struct rpc));
	if (rpc_acs == NULL) {
		return NULL;
	}
	rpc_acs->type = type;
	list_add_tail(&(rpc_acs->list), &(session->head_rpc_acs));
	return rpc_acs;
}

int cwmp_get_session_retry_interval(struct cwmp *cwmp)
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
	srand(time(NULL));
	retry_count = rand() % ((int)max + 1 - (int)min) + (int)min;
	return (retry_count);
}

int cwmp_move_session_to_session_send(struct cwmp *cwmp, struct session *session)
{
	pthread_mutex_lock(&(cwmp->mutex_session_queue));
	if (cwmp->session_send != NULL) {
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_MUTEX_ERR;
	}
	list_del(&(session->list));
	cwmp->session_send = session;
	cwmp->head_event_container = NULL;
	bkp_session_move_inform_to_inform_send();
	bkp_session_save();
	pthread_mutex_unlock(&(cwmp->mutex_session_queue));
	return CWMP_OK;
}

struct rpc *cwmp_add_session_rpc_acs_head(struct session *session, int type)
{
	struct rpc *rpc_acs;

	rpc_acs = calloc(1, sizeof(struct rpc));
	if (rpc_acs == NULL) {
		return NULL;
	}
	rpc_acs->type = type;
	list_add(&(rpc_acs->list), &(session->head_rpc_acs));
	return rpc_acs;
}

struct session *cwmp_add_queue_session(struct cwmp *cwmp)
{
	struct session *session = NULL;
	struct rpc *rpc_acs;

	session = calloc(1, sizeof(struct session));
	if (session == NULL)
		return NULL;

	list_add_tail(&(session->list), &(cwmp->head_session_queue));
	INIT_LIST_HEAD(&(session->head_event_container));
	INIT_LIST_HEAD(&(session->head_rpc_acs));
	INIT_LIST_HEAD(&(session->head_rpc_cpe));
	if ((rpc_acs = cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM)) == NULL) {
		FREE(session);
		return NULL;
	}

	return session;
}

int cwmp_session_rpc_destructor(struct rpc *rpc)
{
	list_del(&(rpc->list));
	free(rpc);
	return CWMP_OK;
}

int cwmp_session_destructor(struct session *session)
{
	struct rpc *rpc;
	while (session->head_rpc_acs.next != &(session->head_rpc_acs)) {
		rpc = list_entry(session->head_rpc_acs.next, struct rpc, list);
		if (!rpc)
			break;
		if (rpc_acs_methods[rpc->type].extra_clean != NULL)
			rpc_acs_methods[rpc->type].extra_clean(session, rpc);
		cwmp_session_rpc_destructor(rpc);
	}

	while (session->head_rpc_cpe.next != &(session->head_rpc_cpe)) {
		rpc = list_entry(session->head_rpc_cpe.next, struct rpc, list);
		if (!rpc)
			break;
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
	struct rpc *rpc_acs, *queue_rpc_acs;
	struct session *session_queue;

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
			struct rpc *rpc_cpe;
			rpc_cpe = list_entry(session->head_rpc_cpe.next, struct rpc, list);
			cwmp_session_rpc_destructor(rpc_cpe);
		}
		bkp_session_move_inform_to_inform_queue();
		bkp_session_save();
		pthread_mutex_unlock(&(cwmp->mutex_session_queue));
		return CWMP_OK;
	}
	list_for_each (ilist, &(session->head_event_container)) {
		struct event_container *event_container_new, *event_container_old;
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
		bool dup;
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

void cwmp_set_end_session(unsigned int flag)
{
	end_session_flag |= flag;
}

int run_session_end_func(void)
{
	if (end_session_flag & END_SESSION_RESTART_SERVICES) {
		CWMP_LOG(INFO, "Restart modified services");
		icwmp_restart_services();
	}

	if (end_session_flag & END_SESSION_RELOAD) {
		CWMP_LOG(INFO, "Config reload: end session request");
		cwmp_uci_reinit();
		cwmp_apply_acs_changes();
	}

	if (end_session_flag & END_SESSION_INIT_NOTIFY) {
		CWMP_LOG(INFO, "SetParameterAttributes end session: reinit list notify");
		reinit_list_param_notify();
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
	cwmp_uci_exit();
	icwmp_cleanmem();
	end_session_flag = 0;
	return CWMP_OK;
}
