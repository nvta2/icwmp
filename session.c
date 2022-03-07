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

#include <stdlib.h>

#include "session.h"
#include "config.h"
#include "event.h"
#include "rpc_soap.h"
#include "backupSession.h"

unsigned int end_session_flag = 0;

void cwmp_set_end_session(unsigned int flag)
{
	end_session_flag |= flag;
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
	rpc_acs = cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM);
	if (rpc_acs == NULL) {
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
				if (cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM) == NULL) {
					pthread_mutex_unlock(&(cwmp->mutex_session_queue));
					return CWMP_MEM_ERR;
				}
			}
		} else {
			if (cwmp_add_session_rpc_acs_head(session, RPC_ACS_INFORM) == NULL) {
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
