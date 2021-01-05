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
#define DOWNLOAD_PROTOCOL_HTTP "http://"
#define DOWNLOAD_PROTOCOL_HTTPS "https://"
#define DOWNLOAD_PROTOCOL_FTP "ftp://"
#define MAX_DOWNLOAD_QUEUE 10
#define MAX_SCHEDULE_INFORM_QUEUE 10

#define MXML_DELETE(X)                 \
	do {                           \
		if (X) {               \
			mxmlDelete(X); \
			X = NULL;      \
		}                      \
	} while (0)

extern struct list_head list_schedule_inform;
extern struct list_head list_download;
extern struct list_head list_upload;
extern struct list_head list_schedule_download;
extern struct list_head list_apply_schedule_download;
extern struct list_head list_change_du_state;
extern int count_download_queue;
extern const struct rpc_cpe_method rpc_cpe_methods[__RPC_CPE_MAX];
extern const struct rpc_acs_method rpc_acs_methods[__RPC_ACS_MAX];

enum fault_code_enum
{
	FAULT_9000 = 9000, // Method not supported
	FAULT_9001, // Request denied
	FAULT_9002, // Internal error
	FAULT_9003, // Invalid arguments
	FAULT_9004, // Resources exceeded
	FAULT_9005, // Invalid parameter name
	FAULT_9006, // Invalid parameter type
	FAULT_9007, // Invalid parameter value
	FAULT_9008, // Attempt to set a non-writable parameter
	FAULT_9009, // Notification request rejected
	FAULT_9010, // Download failure
	FAULT_9011, // Upload failure
	FAULT_9012, // File transfer server authentication failure
	FAULT_9013, // Unsupported protocol for file transfer
	FAULT_9014, // Download failure: unable to join multicast group
	FAULT_9015, // Download failure: unable to contact file server
	FAULT_9016, // Download failure: unable to access file
	FAULT_9017, // Download failure: unable to complete download
	FAULT_9018, // Download failure: file corrupted
	FAULT_9019, // Download failure: file authentication failure
	FAULT_9020, // Download failure: unable to complete download
	FAULT_9021, // Cancelation of file transfer not permitted
	FAULT_9022, // Invalid UUID format
	FAULT_9023, // Unknown Execution Environment
	FAULT_9024, // Disabled Execution Environment
	FAULT_9025, // Diployment Unit to Execution environment mismatch
	FAULT_9026, // Duplicate Deployment Unit
	FAULT_9027, // System Ressources Exceeded
	FAULT_9028, // Unknown Deployment Unit
	FAULT_9029, // Invalid Deployment Unit State
	FAULT_9030, // Invalid Deployment Unit Update: Downgrade not permitted
	FAULT_9031, // Invalid Deployment Unit Update: Version not specified
	FAULT_9032, // Invalid Deployment Unit Update: Version already exist
	__FAULT_MAX
};

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
int cwmp_rpc_acs_destroy_data_transfer_complete(struct session *session, struct rpc *rpc);
int cwmp_rpc_acs_destroy_data_du_state_change_complete(struct session *session, struct rpc *rpc);

int xml_handle_message(struct session *session);
int xml_prepare_msg_out(struct session *session);
int xml_prepare_lwnotification_message(char **msg_out);
int xml_set_cwmp_id_rpc_cpe(struct session *session);
int cwmp_create_fault_message(struct session *session, struct rpc *rpc_cpe, int fault_code);
int cwmp_get_fault_code(int fault_code);
int cwmp_get_fault_code_by_string(char *fault_code);
int cwmp_scheduleInform_remove_all();
int cwmp_scheduledDownload_remove_all();
int cwmp_scheduledUpload_remove_all();
int cwmp_scheduled_Download_remove_all();
int cwmp_apply_scheduled_Download_remove_all();
struct transfer_complete *cwmp_set_data_rpc_acs_transferComplete();
void *thread_cwmp_rpc_cpe_scheduleInform(void *v);
void *thread_cwmp_rpc_cpe_download(void *v);
void *thread_cwmp_rpc_cpe_upload(void *v);
void *thread_cwmp_rpc_cpe_schedule_download(void *v);
void *thread_cwmp_rpc_cpe_apply_schedule_download(void *v);
void *thread_cwmp_rpc_cpe_change_du_state(void *v);

const char *whitespace_cb(mxml_node_t *node, int where);

int cwmp_root_cause_changedustate_complete(struct cwmp *cwmp, struct du_state_change_complete *p);
void cwmp_root_cause_event_ipdiagnostic(void);
int xml_set_cwmp_id(struct session *session);
int xml_send_message(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
int cwmp_free_change_du_state_request(struct change_du_state *change_du_state);
int cwmp_free_download_request(struct download *download);
int cwmp_free_upload_request(struct upload *upload);
int cwmp_free_schedule_download_request(struct schedule_download *schedule_download);
int cwmp_add_apply_schedule_download(struct schedule_download *schedule_download, char *start_time);
int cwmp_free_apply_schedule_download_request(struct apply_schedule_download *apply_schedule_download);
char *calculate_lwnotification_cnonce();
#endif
