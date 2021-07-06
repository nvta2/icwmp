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

#ifndef SESSION_H_
#define SESSION_H_

#include <microxml.h>
#include "common.h"

typedef struct session {
	struct list_head list;
	struct list_head head_event_container;
	struct list_head head_rpc_cpe;
	struct list_head head_rpc_acs;
	mxml_node_t *tree_in;
	mxml_node_t *tree_out;
	mxml_node_t *body_in;
	bool hold_request;
	bool digest_auth;
	int fault_code;
	int error;
} session;

enum end_session_enum
{
	END_SESSION_REBOOT = 1,
	END_SESSION_EXTERNAL_ACTION = 1 << 1,
	END_SESSION_RELOAD = 1 << 2,
	END_SESSION_FACTORY_RESET = 1 << 3,
	END_SESSION_IPPING_DIAGNOSTIC = 1 << 4,
	END_SESSION_DOWNLOAD_DIAGNOSTIC = 1 << 5,
	END_SESSION_UPLOAD_DIAGNOSTIC = 1 << 6,
	END_SESSION_X_FACTORY_RESET_SOFT = 1 << 7,
	END_SESSION_NSLOOKUP_DIAGNOSTIC = 1 << 8,
	END_SESSION_TRACEROUTE_DIAGNOSTIC = 1 << 9,
	END_SESSION_UDPECHO_DIAGNOSTIC = 1 << 10,
	END_SESSION_SERVERSELECTION_DIAGNOSTIC = 1 << 11,
	END_SESSION_SET_NOTIFICATION_UPDATE = 1 << 12,
	END_SESSION_RESTART_SERVICES = 1 << 13,
	END_SESSION_INIT_NOTIFY = 1 << 14
};

enum enum_session_status
{
	SESSION_WAITING,
	SESSION_RUNNING,
	SESSION_FAILURE,
	SESSION_SUCCESS
};

extern unsigned int end_session_flag;

void cwmp_set_end_session(unsigned int flag);
struct rpc *cwmp_add_session_rpc_cpe(struct session *session, int type);
struct session *cwmp_add_queue_session(struct cwmp *cwmp);
struct rpc *cwmp_add_session_rpc_acs(struct session *session, int type);
int cwmp_apply_acs_changes();
int cwmp_move_session_to_session_send(struct cwmp *cwmp, struct session *session);
struct rpc *cwmp_add_session_rpc_acs_head(struct session *session, int type);
int cwmp_session_rpc_destructor(struct rpc *rpc);
int cwmp_session_destructor(struct session *session);
int cwmp_move_session_to_session_queue(struct cwmp *cwmp, struct session *session);
#endif /* SRC_INC_SESSION_H_ */
