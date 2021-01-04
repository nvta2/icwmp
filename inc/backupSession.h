/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *
 */

#ifndef _BACKUPSESSION_H__
#define _BACKUPSESSION_H__

#include <microxml.h>
#include "common.h"

#define RPC_NO_STATUS   -1
#define RPC_QUEUE       0
#define RPC_SEND        1

# define CWMP_BACKUP_SESSION "<cwmp></cwmp>"
#define CWMP_BKP_FILE "/etc/icwmpd/.icwmpd_backup_session.xml"
typedef enum backup_loading {
    ALL,
    ACS,
    CR_IP,
	CR_IPv6,
    CR_PORT
} backup_loading;

struct search_keywords {
	char		*name;
	char		*value;
};

extern pthread_mutex_t mutex_backup_session;

int cwmp_init_backup_session(struct cwmp *cwmp, char **ret, enum backup_loading load);
void bkp_session_save();
int cwmp_load_saved_session(struct cwmp *cwmp, char **acsurl, enum backup_loading load);
mxml_node_t *bkp_session_insert_event(int index, char *command_key, int id, char *status);
void bkp_session_delete_event(int id, char *status);
void bkp_session_simple_insert_in_parent(char *parent, char *child, char *value);
void bkp_session_insert_parameter(mxml_node_t *b, char *name);
void bkp_session_simple_insert(char *parent, char *child, char *value);
void bkp_session_move_inform_to_inform_send ();
void bkp_session_move_inform_to_inform_queue ();
void bkp_session_insert_schedule_inform(time_t schedule_time,char *command_key);
void bkp_session_delete_schedule_inform(time_t schedule_time,char *command_key);
void bkp_session_insert_download(struct download *pdownload);
void bkp_session_delete_download(struct download *pdownload);
void bkp_session_insert_upload(struct upload *pupload);
void bkp_session_delete_upload(struct upload *pupload);
void bkp_session_insert_change_du_state(struct change_du_state *pchange_du_state);
void bkp_session_delete_change_du_state(struct change_du_state *pchange_du_state);
void bkp_session_insert_transfer_complete(struct transfer_complete *ptransfer_complete);
void bkp_session_delete_transfer_complete(struct transfer_complete *ptransfer_complete);
int save_acs_bkp_config(struct cwmp *cwmp);

void bkp_session_insert_schedule_download(struct schedule_download *pschedule_download);
void bkp_session_insert_apply_schedule_download(struct apply_schedule_download *papply_schedule_download);
void bkp_session_delete_apply_schedule_download(struct apply_schedule_download *papply_schedule_download);
void bkp_session_delete_du_state_change_complete(struct du_state_change_complete *pdu_state_change_complete);
void bkp_session_delete_schedule_download(struct schedule_download *pschedule_download);
void bkp_session_insert_du_state_change_complete(struct du_state_change_complete *pdu_state_change_complete);
void bkp_tree_clean(void);
#endif /* _BACKUPSESSION_H__ */
