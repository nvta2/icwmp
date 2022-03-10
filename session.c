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
#include "cwmp_uci.h"
#include "log.h"
#include "http.h"
#include "notifications.h"
#include "diagnostic.h"
#include "soap.h"
#include "ubus.h"
#include "download.h"
#include "upload.h"
#include "cwmp_event.h"
#include "ssl_utils.h"

pthread_mutex_t start_session_mutext = PTHREAD_MUTEX_INITIALIZER;
static void cwmp_priodic_session_timer(struct uloop_timeout *timeout);
struct uloop_timeout session_timer = { .cb = cwmp_schedule_session };
struct uloop_timeout priodic_session_timer = { .cb = cwmp_priodic_session_timer };
struct uloop_timeout retry_session_timer = { .cb = cwmp_schedule_session };

unsigned int end_session_flag = 0;

int create_cwmp_session_structure()
{
	cwmp_main->session = calloc(1, sizeof(struct session));
	if (cwmp_main->session == NULL)
		return CWMP_GEN_ERR;
	INIT_LIST_HEAD(&(cwmp_main->session->events));
	INIT_LIST_HEAD(&(cwmp_main->session->head_rpc_acs));
	INIT_LIST_HEAD(&(cwmp_main->session->head_rpc_cpe));
	return CWMP_OK;
}

int cwmp_session_init()
{
	struct rpc *rpc_acs;

	cwmp_main->cwmp_cr_event = 0;

	cwmp_uci_init();
	rpc_acs = cwmp_add_session_rpc_acs_head(RPC_ACS_INFORM);
	if (rpc_acs == NULL)
		return CWMP_GEN_ERR;

	set_cwmp_session_status(SESSION_RUNNING, 0);
	if (file_exists(fc_cookies))
		remove(fc_cookies);
	return CWMP_OK;
}

int clean_cwmp_session_structure()
{
	FREE(cwmp_main->session);
	return 0;
}

int cwmp_session_rpc_destructor(struct rpc *rpc)
{
	list_del(&(rpc->list));
	free(rpc);
	return CWMP_OK;
}

int cwmp_session_exit()
{
	cwmp_uci_exit();
	icwmp_cleanmem();
	return CWMP_OK;
}

static int cwmp_rpc_cpe_handle_message(struct rpc *rpc_cpe)
{
	if (xml_prepare_msg_out())
		return -1;
	if (rpc_cpe_methods[rpc_cpe->type].handler(rpc_cpe))
		return -1;
	if (xml_set_cwmp_id_rpc_cpe())
		return -1;

	return 0;
}

static int cwmp_schedule_rpc()
{
	struct list_head *ilist;
	struct rpc *rpc_acs, *rpc_cpe;

	if (http_client_init() || cwmp_stop) {
		CWMP_LOG(INFO, "Initializing http client failed");
		goto retry;
	}

	while (1) {
		list_for_each (ilist, &(cwmp_main->session->head_rpc_acs)) {

			rpc_acs = list_entry(ilist, struct rpc, list);
			if (!rpc_acs->type || cwmp_stop)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s RPC message to send to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].prepare_message(rpc_acs) || cwmp_stop)
				goto retry;

			if (xml_set_cwmp_id() || cwmp_stop)
				goto retry;

			CWMP_LOG(INFO, "Send the %s RPC message to the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (xml_send_message(rpc_acs) || cwmp_stop)
				goto retry;

			CWMP_LOG(INFO, "Get the %sResponse message from the ACS", rpc_acs_methods[rpc_acs->type].name);
			if (rpc_acs_methods[rpc_acs->type].parse_response || cwmp_stop)
				if (rpc_acs_methods[rpc_acs->type].parse_response(rpc_acs))
					goto retry;

			ilist = ilist->prev;
			if (rpc_acs_methods[rpc_acs->type].extra_clean != NULL)
				rpc_acs_methods[rpc_acs->type].extra_clean(rpc_acs);
			cwmp_session_rpc_destructor(rpc_acs);
			MXML_DELETE(cwmp_main->session->tree_in);
			MXML_DELETE(cwmp_main->session->tree_out);
			if (cwmp_main->session->hold_request || cwmp_stop)
				break;
		}

		// If restart service caused firewall restart, wait for firewall restart to complete
		if (g_firewall_restart == true)
			check_firewall_restart_state();

		CWMP_LOG(INFO, "Send empty message to the ACS");
		if (xml_send_message(NULL) || cwmp_stop)
			goto retry;
		if (!cwmp_main->session->tree_in || cwmp_stop)
			goto next;

		CWMP_LOG(INFO, "Receive request from the ACS");
		if (xml_handle_message() || cwmp_stop)
			goto retry;

		while (cwmp_main->session->head_rpc_cpe.next != &(cwmp_main->session->head_rpc_cpe)) {

			rpc_cpe = list_entry(cwmp_main->session->head_rpc_cpe.next, struct rpc, list);
			if (!rpc_cpe->type || cwmp_stop)
				goto retry;

			CWMP_LOG(INFO, "Preparing the %s%s message", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (cwmp_rpc_cpe_handle_message(rpc_cpe) || cwmp_stop)
				goto retry;
			MXML_DELETE(cwmp_main->session->tree_in);

			CWMP_LOG(INFO, "Send the %s%s message to the ACS", rpc_cpe_methods[rpc_cpe->type].name, (rpc_cpe->type != RPC_CPE_FAULT) ? "Response" : "");
			if (xml_send_message(rpc_cpe) || cwmp_stop)
				goto retry;
			MXML_DELETE(cwmp_main->session->tree_out);

			cwmp_session_rpc_destructor(rpc_cpe);
			if (!cwmp_main->session->tree_in || cwmp_stop)
				break;

			CWMP_LOG(INFO, "Receive request from the ACS");
			if (xml_handle_message() || cwmp_stop)
				goto retry;
		}

	next:
		if (cwmp_main->session->head_rpc_acs.next == &(cwmp_main->session->head_rpc_acs))
			break;
		MXML_DELETE(cwmp_main->session->tree_in);
		MXML_DELETE(cwmp_main->session->tree_out);
	}

	cwmp_main->session->error = CWMP_OK;
	goto end;

retry:
	CWMP_LOG(INFO, "Failed");
	cwmp_main->session->error = CWMP_RETRY_SESSION;
	event_remove_noretry_event_container();

end:
	MXML_DELETE(cwmp_main->session->tree_in);
	MXML_DELETE(cwmp_main->session->tree_out);
	http_client_exit();
	xml_exit();
	return cwmp_main->session->error;
}

void set_cwmp_session_status(int status, int retry_time)
{
	cwmp_main->session->session_status.last_status = status;
	if (status == SESSION_SUCCESS) {
		cwmp_main->session->session_status.last_end_time = time(NULL);
		cwmp_main->session->session_status.next_retry = 0;
		cwmp_main->session->session_status.success_session++;
	} else if (status == SESSION_RUNNING) {
		cwmp_main->session->session_status.last_end_time = 0;
		cwmp_main->session->session_status.next_retry = 0;
		cwmp_main->session->session_status.last_start_time = time(NULL);
	} else {
		cwmp_main->session->session_status.last_end_time = time(NULL);
		cwmp_main->session->session_status.next_retry = time(NULL) + retry_time;
		cwmp_main->session->session_status.failure_session++;
	}
}

void rpc_exit()
{
	struct rpc *rpc;
	while (cwmp_main->session->head_rpc_acs.next != &(cwmp_main->session->head_rpc_acs)) {
		rpc = list_entry(cwmp_main->session->head_rpc_acs.next, struct rpc, list);
		if (!rpc)
			break;
		if (rpc_acs_methods[rpc->type].extra_clean != NULL)
			rpc_acs_methods[rpc->type].extra_clean(rpc);
		cwmp_session_rpc_destructor(rpc);
	}

	while (cwmp_main->session->head_rpc_cpe.next != &(cwmp_main->session->head_rpc_cpe)) {
		rpc = list_entry(cwmp_main->session->head_rpc_cpe.next, struct rpc, list);
		if (!rpc)
			break;
		cwmp_session_rpc_destructor(rpc);
	}
}

void start_cwmp_session()
{
	int t, error;
	char *exec_download = NULL;
	int is_notify = 0;

	uloop_timeout_cancel(&check_notify_timer);
	if (cwmp_session_init() != CWMP_OK) {
		CWMP_LOG(ERROR, "Not able to init a CWMP session");
		t = cwmp_get_session_retry_interval();
		CWMP_LOG(INFO, "Retry session, retry count = %d, retry in %ds", cwmp_main->retry_count_session, t);
		set_cwmp_session_status(SESSION_FAILURE, t);
		trigger_periodic_notify_check();
		return;
	}

	if (cwmp_main->session->session_status.last_status == SESSION_FAILURE)
		reload_networking_config();
	/*
	 * Value changes
	 */
	if (file_exists(DM_ENABLED_NOTIFY)) {
		if (!event_exist_in_list(EVENT_IDX_4VALUE_CHANGE))
			is_notify = check_value_change();
	}
	if (is_notify > 0 || !file_exists(DM_ENABLED_NOTIFY) || cwmp_main->custom_notify_active) {
		cwmp_main->custom_notify_active = false;
		cwmp_update_enabled_notify_file();
	}
	cwmp_prepare_value_change(cwmp_main);
	clean_list_value_change();

	/*
	 * Start session
	 */

	CWMP_LOG(INFO, "Start session");

	uci_get_value(UCI_CPE_EXEC_DOWNLOAD, &exec_download);
	if (exec_download && strcmp(exec_download, "1") == 0) {
		CWMP_LOG(INFO, "Firmware downloaded and applied successfully");
		cwmp_uci_set_value("cwmp", "cpe", "exec_download", "0");
		cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	}
	FREE(exec_download);
	error = cwmp_schedule_rpc();
	if (error != CWMP_OK)
		CWMP_LOG(ERROR, "CWMP session error: %d", error);
	/*
	 * End session
	 */
	CWMP_LOG(INFO, "End session");

	if (cwmp_stop) {
		event_remove_all_event_container(RPC_SEND);
		event_remove_all_event_container(RPC_QUEUE);
		run_session_end_func();
		cwmp_session_exit();
		rpc_exit();
		return;
	}

	if (cwmp_main->session->error == CWMP_RETRY_SESSION && (!list_empty(&(cwmp_main->session->events)) || (list_empty(&(cwmp_main->session->events)) && cwmp_main->cwmp_cr_event == 0))) { //CWMP Retry session
		reload_networking_config();
		cwmp_main->retry_count_session++;
		t = cwmp_get_session_retry_interval();
		CWMP_LOG(INFO, "Retry session, retry count = %d, retry in %ds", cwmp_main->retry_count_session, t);
		set_cwmp_session_status(SESSION_FAILURE, t);
		uloop_timeout_set(&retry_session_timer, 1000 * t);
	} else {
		event_remove_all_event_container(RPC_SEND);
		//event_remove_all_event_container(RPC_QUEUE);
		cwmp_main->retry_count_session = 0;
		set_cwmp_session_status(SESSION_SUCCESS, 0);
		rpc_exit();
	}
	run_session_end_func();
	cwmp_session_exit();
	CWMP_LOG(INFO, "Waiting the next session");
	trigger_periodic_notify_check();
}

void trigger_cwmp_session_timer()
{
	uloop_timeout_cancel(&retry_session_timer);
	uloop_timeout_set(&session_timer, 10);
}

void cwmp_schedule_session(struct uloop_timeout *timeout  __attribute__((unused)))
{
	pthread_mutex_lock(&start_session_mutext);
	start_cwmp_session();
	pthread_mutex_unlock(&start_session_mutext);
	if (cwmp_main->start_diagnostics) {
		cwmp_main->start_diagnostics = false;
		trigger_cwmp_session_timer();
	}
}

static void cwmp_priodic_session_timer(struct uloop_timeout *timeout  __attribute__((unused)))
{
	if (cwmp_main->conf.periodic_enable && cwmp_main->conf.period > 0) {
		cwmp_main->session->session_status.next_periodic = time(NULL) + cwmp_main->conf.period;
		uloop_timeout_set(&priodic_session_timer, cwmp_main->conf.period * 1000);
	}
	if (cwmp_main->conf.periodic_enable) {
		cwmp_add_event_container(EVENT_IDX_2PERIODIC, "");
		trigger_cwmp_session_timer();
	}
}

long int cwmp_periodic_session_time(void)
{
	long int delta_time;
	long int periodic_time;

	delta_time = time(NULL) - cwmp_main->conf.time;
	if(delta_time > 0)
		periodic_time = cwmp_main->conf.period - (delta_time % cwmp_main->conf.period);
	else
		periodic_time = (-delta_time) % cwmp_main->conf.period;

	cwmp_main->session->session_status.next_periodic = time(NULL) + periodic_time;
	return  periodic_time;
}

void initiate_cwmp_periodic_session_feature()
{
	uloop_timeout_cancel(&priodic_session_timer);
	if (cwmp_main->conf.periodic_enable && cwmp_main->conf.period > 0) {
		if (cwmp_main->conf.time >= 0){
			CWMP_LOG(INFO, "Init periodic inform: periodic_inform time = %ld, interval = %d\n", cwmp_main->conf.time, cwmp_main->conf.period);
			uloop_timeout_set(&priodic_session_timer, cwmp_periodic_session_time() * 1000);
		}
		else {
			CWMP_LOG(INFO, "Init periodic inform: interval = %d\n", cwmp_main->conf.period);
			cwmp_main->session->session_status.next_periodic = time(NULL) + cwmp_main->conf.period;
			uloop_timeout_set(&priodic_session_timer, cwmp_main->conf.period * 1000);
		}
	}
}

struct rpc *cwmp_add_session_rpc_cpe(int type)
{
	struct rpc *rpc_cpe;

	rpc_cpe = calloc(1, sizeof(struct rpc));
	if (rpc_cpe == NULL) {
		return NULL;
	}
	rpc_cpe->type = type;
	list_add_tail(&(rpc_cpe->list), &(cwmp_main->session->head_rpc_cpe));
	return rpc_cpe;
}

struct rpc *cwmp_add_session_rpc_acs(int type)
{
	struct rpc *rpc_acs;

	rpc_acs = calloc(1, sizeof(struct rpc));
	if (rpc_acs == NULL) {
		return NULL;
	}
	rpc_acs->type = type;
	list_add_tail(&(rpc_acs->list), &(cwmp_main->session->head_rpc_acs));
	return rpc_acs;
}

int cwmp_get_session_retry_interval()
{
	unsigned int retry_count = 0;
	double min = 0;
	double max = 0;
	int m = cwmp_main->conf.retry_min_wait_interval;
	int k = cwmp_main->conf.retry_interval_multiplier;
	int exp = cwmp_main->retry_count_session;
	if (exp == 0)
		return MAX_INT32;
	if (exp > 10)
		exp = 10;
	min = pow(((double)k / 1000), (double)(exp - 1)) * m;
	max = pow(((double)k / 1000), (double)exp) * m;
	char *rand = generate_random_string(4);
	if (rand) {
		unsigned int dividend = (unsigned int)strtoul(rand, NULL, 16);
		retry_count = dividend % ((unsigned int)max + 1 - (unsigned int)min) + (unsigned int)min;
		free(rand);
	}
	return (retry_count);
}

struct rpc *cwmp_add_session_rpc_acs_head(int type)
{
	struct rpc *rpc_acs;

	rpc_acs = calloc(1, sizeof(struct rpc));
	if (rpc_acs == NULL) {
		return NULL;
	}
	rpc_acs->type = type;
	list_add(&(rpc_acs->list), &(cwmp_main->session->head_rpc_acs));
	return rpc_acs;
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
		initiate_cwmp_periodic_session_feature();
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

	if (end_session_flag & END_SESSION_DOWNLOAD) {
		CWMP_LOG(INFO, "Trigger Uloop Downaload Calls");
		struct list_head *ilist;
		list_for_each (ilist, &(list_download)) {
			struct download *download = list_entry(ilist, struct download, list);
			int download_delay = 0;
			if (download->scheduled_time > time(NULL)) {
				download_delay = download->scheduled_time - time(NULL);
			}
			uloop_timeout_set(&download->handler_timer, 1000 * download_delay);
		}
	}

	if (end_session_flag & END_SESSION_SCHEDULE_DOWNLOAD) {
		CWMP_LOG(INFO, "Trigger Uloop ScheduleDownaload Calls");
		struct list_head *ilist;
		list_for_each (ilist, &(list_schedule_download)) {
			struct download *sched_download = list_entry(ilist, struct download, list);
			time_t now = time(NULL);
			int download_delay;
			if (sched_download->timewindowstruct[0].windowstart > now)
				download_delay = sched_download->timewindowstruct[0].windowstart - now;
			else if (sched_download->timewindowstruct[0].windowend >= now)
				download_delay = 1;
			else if (now < sched_download->timewindowstruct[1].windowstart)
				download_delay = sched_download->timewindowstruct[1].windowstart - now;
			else if (sched_download->timewindowstruct[1].windowend >= now)
				download_delay = 1;
			else
				download_delay = 1;

			uloop_timeout_set(&sched_download->handler_timer, 1000 * download_delay);
		}
	}

	if (end_session_flag & END_SESSION_UPLOAD) {
		CWMP_LOG(INFO, "Trigger Uloop Upload Calls");
		struct list_head *ilist;
		list_for_each (ilist, &(list_upload)) {
			struct download *upload = list_entry(ilist, struct download, list);
			int upload_delay = 0;
			if (upload->scheduled_time > time(NULL)) {
				upload_delay = upload->scheduled_time - time(NULL);
			}
			uloop_timeout_set(&upload->handler_timer, 1000 * upload_delay);
		}
	}

	end_session_flag = 0;
	return CWMP_OK;
}
