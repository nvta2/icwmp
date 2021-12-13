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

#ifndef EVENT_H_
#define EVENT_H_
#include "common.h"
#include "event.h"

typedef struct event_container {
	struct list_head list;
	int code; /* required element of type xsd:string */
	char *command_key;
	struct list_head head_dm_parameter;
	int id;
} event_container;

typedef struct EVENT_CONST_STRUCT {
	char *CODE;
	unsigned int TYPE;
	unsigned short RETRY;

} EVENT_CONST_STRUCT;

enum event_retry_after_enum
{
	EVENT_RETRY_AFTER_TRANSMIT_FAIL = 0x1,
	EVENT_RETRY_AFTER_REBOOT = 0x2,
	EVENT_RETRY_AFTER_BOOTSTRAP = 0x4
};

enum event_type_enum
{
	EVENT_TYPE_SINGLE = 0x0,
	EVENT_TYPE_MULTIPLE = 0x1
};

enum event_idx_enum
{
	EVENT_IDX_0BOOTSTRAP,
	EVENT_IDX_1BOOT,
	EVENT_IDX_2PERIODIC,
	EVENT_IDX_3SCHEDULED,
	EVENT_IDX_4VALUE_CHANGE,
	EVENT_IDX_5KICKED,
	EVENT_IDX_6CONNECTION_REQUEST,
	EVENT_IDX_7TRANSFER_COMPLETE,
	EVENT_IDX_8DIAGNOSTICS_COMPLETE,
	EVENT_IDX_9REQUEST_DOWNLOAD,
	EVENT_IDX_10AUTONOMOUS_TRANSFER_COMPLETE,
	EVENT_IDX_11DU_STATE_CHANGE_COMPLETE,
	EVENT_IDX_M_Reboot,
	EVENT_IDX_M_ScheduleInform,
	EVENT_IDX_M_Download,
	EVENT_IDX_M_Schedule_Download,
	EVENT_IDX_M_Upload,
	EVENT_IDX_M_ChangeDUState,
	__EVENT_IDX_MAX
};

extern const struct EVENT_CONST_STRUCT EVENT_CONST[__EVENT_IDX_MAX];
extern pthread_mutex_t add_event_mutext;

struct event_container *cwmp_add_event_container(struct cwmp *cwmp, int event_idx, char *command_key);
int event_remove_all_event_container(struct session *session, int rem_from);
int event_remove_noretry_event_container(struct session *session, struct cwmp *cwmp);
void cwmp_save_event_container(struct event_container *event_container);
void *thread_event_periodic(void *v);
void connection_request_ip_value_change(struct cwmp *cwmp, int version);
void connection_request_port_value_change(struct cwmp *cwmp, int port);
int cwmp_get_int_event_code(char *code);
bool event_exist_in_list(struct cwmp *cwmp, int event);
int cwmp_root_cause_events(struct cwmp *cwmp);
int cwmp_root_cause_transfer_complete(struct cwmp *cwmp, struct transfer_complete *p);
int cwmp_root_cause_changedustate_complete(struct cwmp *cwmp, struct du_state_change_complete *p);
void cwmp_root_cause_event_ipdiagnostic(void);
#endif /* SRC_INC_EVENT_H_ */
