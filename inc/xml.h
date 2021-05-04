/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 *
 */

#ifndef _FREECWMP_XML_H__
#define _FREECWMP_XML_H__

#include <microxml.h>
#include "common.h"
#include "session.h"

#define CWMP_MXML_TAB_SPACE "  "
#define MAX_SCHEDULE_INFORM_QUEUE 10

#define MXML_DELETE(X)                 \
	do {                           \
		if (X) {               \
			mxmlDelete(X); \
			X = NULL;      \
		}                      \
	} while (0)

extern const struct rpc_cpe_method rpc_cpe_methods[__RPC_CPE_MAX];
extern const struct rpc_acs_method rpc_acs_methods[__RPC_ACS_MAX];

void xml_exit(void);

int cwmp_handle_rpc_cpe_get_rpc_methods(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_set_parameter_values(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_values(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_names(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_set_parameter_attributes(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_attributes(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_add_object(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_delete_object(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_reboot(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_download(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_upload(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_factory_reset(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_x_factory_reset_soft(struct session *session, struct rpc *rpc);
int cancel_transfer(char *key);
int cwmp_handle_rpc_cpe_cancel_transfer(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_schedule_inform(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_schedule_download(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_change_du_state(struct session *session, struct rpc *rpc);
int cwmp_handle_rpc_cpe_fault(struct session *session, struct rpc *rpc);

int cwmp_rpc_acs_prepare_message_inform(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_parse_response_inform(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_prepare_get_rpc_methods(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_prepare_transfer_complete(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_prepare_du_state_change_complete(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_destroy_data_inform(struct session *session, struct rpc *rpc);

int xml_handle_message(struct session *session);
int xml_prepare_msg_out(struct session *session);
int xml_prepare_lwnotification_message(char **msg_out);
int xml_set_cwmp_id_rpc_cpe(struct session *session);
int cwmp_create_fault_message(struct session *session, struct rpc *rpc_cpe, int fault_code);

const char *whitespace_cb(mxml_node_t *node, int where);
int xml_set_cwmp_id(struct session *session);
int xml_send_message(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
char *calculate_lwnotification_cnonce();
#endif
