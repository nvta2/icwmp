/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include "session.h"
#include "event.h"
#include "backupSession.h"
#include "config.h"

unsigned int end_session_flag = 0;

void cwmp_set_end_session(unsigned int flag) { end_session_flag |= flag; }
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

int cwmp_apply_acs_changes(void)
{
	int error;

	if ((error = cwmp_config_reload(&cwmp_main)))
		return error;

	if ((error = cwmp_root_cause_events(&cwmp_main)))
		return error;

	return CWMP_OK;
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
