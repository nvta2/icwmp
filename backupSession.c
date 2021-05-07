/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *
 */

#include <unistd.h>
#include <fcntl.h>

#include "backupSession.h"
#include "xml.h"
#include "log.h"
#include "notifications.h"
#include "event.h"
#include "download.h"
#include "cwmp_du_state.h"
#include "upload.h"
#include "sched_inform.h"

static mxml_node_t *bkp_tree = NULL;
pthread_mutex_t mutex_backup_session = PTHREAD_MUTEX_INITIALIZER;

enum backup_attributes_types
{
	BKP_STRING,
	BKP_INTEGER,
	BKP_BOOL,
	BKP_TIME,
};

struct backup_attributes_name_type {
	char *name;
	enum backup_attributes_types bkp_type;
};

struct backup_attributes_name_type bkp_attrs_names[] = { { "command_key", BKP_STRING },
							 { "url", BKP_STRING },
							 { "file_type", BKP_STRING },
							 { "username", BKP_STRING },
							 { "password", BKP_STRING },
							 { "windowmode1", BKP_STRING },
							 { "usermessage1", BKP_STRING },
							 { "windowmode2", BKP_STRING },
							 { "usermessage2", BKP_STRING },
							 { "start_time", BKP_STRING },
							 { "complete_time", BKP_STRING },
							 { "uuid", BKP_STRING },
							 { "version", BKP_STRING },
							 { "du_ref", BKP_STRING },
							 { "current_state", BKP_STRING },
							 { "execution_unit_ref", BKP_STRING },
							 { "old_software_version", BKP_STRING },
							 { "executionenvref", BKP_STRING },
							 { "index", BKP_INTEGER },
							 { "id", BKP_INTEGER },
							 { "file_size", BKP_INTEGER },
							 { "maxretrie1", BKP_INTEGER },
							 { "maxretrie2", BKP_INTEGER },
							 { "type", BKP_INTEGER },
							 { "fault", BKP_INTEGER },
							 { "fault_code", BKP_INTEGER },
							 { "resolved", BKP_BOOL },
							 { "time", BKP_TIME },
							 { "windowstart1", BKP_TIME },
							 { "windowend1", BKP_TIME },
							 { "windowstart2", BKP_TIME },
							 { "windowend2", BKP_TIME } };
struct backup_attributes {
	char **command_key;
	char **url;
	char **file_type;
	char **username;
	char **password;
	char **windowmode1;
	char **usermessage1;
	char **windowmode2;
	char **usermessage2;
	char **start_time;
	char **complete_time;
	char **uuid;
	char **version;
	char **du_ref;
	char **current_state;
	char **execution_unit_ref;
	char **old_software_version;
	char **executionenvref;
	int *index;
	int *id;
	int *file_size;
	int *maxretrie1;
	int *maxretrie2;
	int *type;
	int *fault;
	int *fault_code;
	bool *resolved;
	time_t *time;
	time_t *windowstart1;
	time_t *windowend1;
	time_t *windowstart2;
	time_t *windowend2;
};

int get_bkp_attribute_index_type(char *name)
{
	unsigned int i;
	if (name == NULL)
		return -1;
	size_t total_size = sizeof(bkp_attrs_names) / sizeof(struct backup_attributes_name_type);
	for (i = 0; i < total_size; i++) {
		if (strcmp(name, bkp_attrs_names[i].name) == 0)
			return i;
	}
	return -1;
}

void load_specific_backup_attributes(mxml_node_t *tree, struct backup_attributes *bkp_attrs)
{
	mxml_node_t *b = tree, *c;
	int idx = -1;
	void **ptr;
	char **str;
	int *intgr;
	bool *bol;
	time_t *time;

	b = mxmlWalkNext(b, tree, MXML_DESCEND);
	while (b) {
		if (b && b->type == MXML_ELEMENT) {
			idx = get_bkp_attribute_index_type(b->value.element.name);
			c = mxmlWalkNext(b, b, MXML_DESCEND);
			if (c && c->type == MXML_OPAQUE) {
				if (c->value.opaque != NULL) {
					ptr = (void **)((char *)bkp_attrs + idx * sizeof(char *));
					switch (bkp_attrs_names[idx].bkp_type) {
					case BKP_STRING:
						str = (char **)(*ptr);
						*str = strdup(c->value.opaque);
						break;
					case BKP_INTEGER:
						intgr = (int *)(*ptr);
						*intgr = atoi(c->value.opaque);
						break;
					case BKP_BOOL:
						bol = (bool *)(*ptr);
						*bol = c->value.opaque;
						break;
					case BKP_TIME:
						time = (time_t *)(*ptr);
						*time = atol(c->value.opaque);
						break;
					}
				}
			}
		}
		b = mxmlWalkNext(b, tree, MXML_NO_DESCEND);
	}
}

void bkp_tree_clean(void)
{
	if (bkp_tree != NULL)
		MXML_DELETE(bkp_tree);
	return;
}

void bkp_session_save()
{
	FILE *fp;

	pthread_mutex_lock(&mutex_backup_session);
	fp = fopen(CWMP_BKP_FILE, "w");
	mxmlSaveFile(bkp_tree, fp, MXML_NO_CALLBACK);
	fclose(fp);
	sync();
	pthread_mutex_unlock(&mutex_backup_session);
}

mxml_node_t *bkp_session_insert(mxml_node_t *tree, char *name, char *value)
{
	mxml_node_t *b;

	b = mxmlNewElement(tree, name);

	if (value != NULL) {
		mxmlNewOpaque(b, value);
	}
	return b;
}
/*
 * The order of key array filling should be the same of insertion function
 */
mxml_node_t *bkp_session_node_found(mxml_node_t *tree, char *name, struct search_keywords *keys, int size)
{
	mxml_node_t *b = tree, *c, *d;
	struct search_keywords;
	int i = 0;

	b = mxmlFindElement(b, b, name, NULL, NULL, MXML_DESCEND_FIRST);
	while (b) {
		if (b && b->child) {
			c = b->child;
			i = 0;
			while (c && i < size) {
				if (c->type == MXML_ELEMENT && strcmp(keys[i].name, c->value.element.name) == 0) {
					d = c;
					d = mxmlWalkNext(d, c, MXML_DESCEND);
					if ((keys[i].value == NULL) || (d && d->type == MXML_OPAQUE && keys[i].value != NULL && strcmp(keys[i].value, d->value.opaque) == 0))
						i++;
				}
				c = mxmlWalkNext(c, b, MXML_NO_DESCEND);
			}
		}
		if (i == size) {
			break;
		}
		b = mxmlWalkNext(b, tree, MXML_NO_DESCEND);
	}
	return b;
}

mxml_node_t *bkp_session_insert_event(int index, char *command_key, int id, char *status)
{
	struct search_keywords keys[1];
	char parent_name[32];
	char event_id[32];
	char event_idx[32];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	snprintf(parent_name, sizeof(parent_name), "%s_event", status);
	snprintf(event_id, sizeof(event_id), "%d", id);
	snprintf(event_idx, sizeof(event_idx), "%d", index);
	keys[0].name = "id";
	keys[0].value = event_id;
	b = bkp_session_node_found(bkp_tree, parent_name, keys, 1);
	if (!b) {
		b = bkp_session_insert(bkp_tree, parent_name, NULL);
		bkp_session_insert(b, "index", event_idx);
		bkp_session_insert(b, "id", event_id);
		bkp_session_insert(b, "command_key", command_key);
	}
	pthread_mutex_unlock(&mutex_backup_session);
	return b;
}

void bkp_session_delete_event(int id, char *status)
{
	struct search_keywords keys[1];
	char parent_name[32];
	char event_id[32];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	snprintf(parent_name, sizeof(parent_name), "%s_event", status);
	snprintf(event_id, sizeof(event_id), "%d", id);
	keys[0].name = "id";
	keys[0].value = event_id;
	b = bkp_session_node_found(bkp_tree, parent_name, keys, 1);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_parameter(mxml_node_t *b, char *name)
{
	pthread_mutex_lock(&mutex_backup_session);
	bkp_session_insert(b, "parameter", name);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_simple_insert(char *parent, char *child, char *value)
{
	mxml_node_t *b = bkp_tree;

	pthread_mutex_lock(&mutex_backup_session);
	b = mxmlFindElement(b, b, parent, NULL, NULL, MXML_DESCEND);
	if (b)
		mxmlDelete(b);
	b = bkp_session_insert(bkp_tree, parent, NULL);
	bkp_session_insert(b, child, value);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_simple_insert_in_parent(char *parent, char *child, char *value)
{
	mxml_node_t *n, *b = bkp_tree;

	pthread_mutex_lock(&mutex_backup_session);
	n = mxmlFindElement(b, b, parent, NULL, NULL, MXML_DESCEND);
	if (!n)
		n = bkp_session_insert(bkp_tree, parent, NULL);
	b = mxmlFindElement(n, n, child, NULL, NULL, MXML_DESCEND);
	if (b)
		mxmlDelete(b);
	bkp_session_insert(n, child, value);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_move_inform_to_inform_send()
{
	mxml_node_t *b = bkp_tree;

	pthread_mutex_lock(&mutex_backup_session);
	while (b) {
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "queue_event") && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "cwmp")) {
			FREE(b->value.element.name);
			b->value.element.name = strdup("send_event");
		}
		b = mxmlWalkNext(b, bkp_tree, MXML_DESCEND);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_move_inform_to_inform_queue()
{
	mxml_node_t *b = bkp_tree;

	pthread_mutex_lock(&mutex_backup_session);
	while (b) {
		if (b && b->type == MXML_ELEMENT && !strcmp(b->value.element.name, "send_event") && b->parent->type == MXML_ELEMENT && !strcmp(b->parent->value.element.name, "cwmp")) {
			FREE(b->value.element.name);
			b->value.element.name = strdup("queue_event");
		}
		b = mxmlWalkNext(b, bkp_tree, MXML_DESCEND);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_schedule_inform(time_t time, char *command_key)
{
	char schedule_time[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", time);
	struct search_keywords sched_inf_insert_keys[2] = { { "command_key", command_key }, { "time", schedule_time } };
	b = bkp_session_node_found(bkp_tree, "schedule_inform", sched_inf_insert_keys, 2);
	if (!b) {
		b = bkp_session_insert(bkp_tree, "schedule_inform", NULL);
		bkp_session_insert(b, "command_key", command_key);
		bkp_session_insert(b, "time", schedule_time);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_schedule_inform(time_t time, char *command_key)
{
	char schedule_time[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", time);
	struct search_keywords sched_inf_del_keys[2] = { { "command_key", command_key }, { "time", schedule_time } };
	b = bkp_session_node_found(bkp_tree, "schedule_inform", sched_inf_del_keys, 2);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_download(struct download *pdownload)
{
	char schedule_time[128];
	char file_size[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pdownload->scheduled_time);
	sprintf(file_size, "%d", pdownload->file_size);
	struct search_keywords download_insert_keys[7] = { { "url", pdownload->url }, { "command_key", pdownload->command_key }, { "file_type", pdownload->file_type }, { "username", pdownload->username }, { "password", pdownload->password }, { "file_size", file_size }, { "time", schedule_time } };

	b = bkp_session_node_found(bkp_tree, "download", download_insert_keys, 7);
	if (!b) {
		b = bkp_session_insert(bkp_tree, "download", NULL);
		bkp_session_insert(b, "url", pdownload->url);
		bkp_session_insert(b, "command_key", pdownload->command_key);
		bkp_session_insert(b, "file_type", pdownload->file_type);
		bkp_session_insert(b, "username", pdownload->username);
		bkp_session_insert(b, "password", pdownload->password);
		bkp_session_insert(b, "file_size", file_size);
		bkp_session_insert(b, "time", schedule_time);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_schedule_download(struct download *pschedule_download)
{
	char delay[4][128];
	int i;
	char file_size[128];
	char maxretrie[2][128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(file_size, "%d", pschedule_download->file_size);
	for (i = 0; i < 2; i++) {
		sprintf(delay[2 * i], "%ld", pschedule_download->timewindowstruct[i].windowstart);
		sprintf(delay[2 * i + 1], "%ld", pschedule_download->timewindowstruct[i].windowend);
		sprintf(maxretrie[i], "%d", pschedule_download->timewindowstruct[i].maxretries);
	}
	struct search_keywords sched_download_insert_keys[16] = { { "url", pschedule_download->url },
								  { "command_key", pschedule_download->command_key },
								  { "file_type", pschedule_download->file_type },
								  { "username", pschedule_download->username },
								  { "password", pschedule_download->password },
								  { "file_size", file_size },
								  { "windowstart1", delay[0] },
								  { "windowend1", delay[1] },
								  { "windowmode1", pschedule_download->timewindowstruct[0].windowmode },
								  { "usermessage1", pschedule_download->timewindowstruct[0].usermessage },
								  { "maxretrie1", maxretrie[0] },
								  { "windowstart2", delay[2] },
								  { "windowend2", delay[3] },
								  { "windowmode2", pschedule_download->timewindowstruct[1].windowmode },
								  { "usermessage2", pschedule_download->timewindowstruct[1].usermessage },
								  { "maxretrie2", maxretrie[1] } };

	b = bkp_session_node_found(bkp_tree, "schedule_download", sched_download_insert_keys, 16);
	if (!b) {
		b = bkp_session_insert(bkp_tree, "schedule_download", NULL);
		bkp_session_insert(b, "url", pschedule_download->url);
		bkp_session_insert(b, "command_key", pschedule_download->command_key);
		bkp_session_insert(b, "file_type", pschedule_download->file_type);
		bkp_session_insert(b, "username", pschedule_download->username);
		bkp_session_insert(b, "password", pschedule_download->password);
		bkp_session_insert(b, "file_size", file_size);
		bkp_session_insert(b, "windowstart1", delay[0]);
		bkp_session_insert(b, "windowend1", delay[1]);
		bkp_session_insert(b, "windowmode1", pschedule_download->timewindowstruct[0].windowmode);
		bkp_session_insert(b, "usermessage1", pschedule_download->timewindowstruct[0].usermessage);
		bkp_session_insert(b, "maxretrie1", maxretrie[0]);
		bkp_session_insert(b, "windowstart2", delay[2]);
		bkp_session_insert(b, "windowend2", delay[3]);
		bkp_session_insert(b, "windowmode2", pschedule_download->timewindowstruct[1].windowmode);
		bkp_session_insert(b, "usermessage2", pschedule_download->timewindowstruct[1].usermessage);
		bkp_session_insert(b, "maxretrie2", maxretrie[1]);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_apply_schedule_download(struct apply_schedule_download *papply_schedule_download)
{
	char delay[4][128];
	int i;
	char maxretrie[2][128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);

	for (i = 0; i < 2; i++) {
		sprintf(delay[2 * i], "%ld", papply_schedule_download->timeintervals[i].windowstart);
		sprintf(delay[2 * i + 1], "%ld", papply_schedule_download->timeintervals[i].windowend);
		sprintf(maxretrie[i], "%d", papply_schedule_download->timeintervals[i].maxretries);
	}

	struct search_keywords sched_download_insert_app_keys[9] = { { "command_key", papply_schedule_download->command_key },
								     { "file_type", papply_schedule_download->file_type },
								     { "start_time", papply_schedule_download->start_time },
								     { "windowstart1", delay[0] },
								     { "windowend1", delay[1] },
								     { "maxretrie1", maxretrie[0] },
								     { "windowstart2", delay[2] },
								     { "windowend2", delay[3] },
								     { "maxretrie2", maxretrie[1] } };

	b = bkp_session_node_found(bkp_tree, "apply_schedule_download", sched_download_insert_app_keys, 9);
	if (!b) {
		CWMP_LOG(INFO, "New schedule download key %s file", papply_schedule_download->command_key);
		b = bkp_session_insert(bkp_tree, "apply_schedule_download", NULL);
		bkp_session_insert(b, "start_time", papply_schedule_download->start_time);
		bkp_session_insert(b, "command_key", papply_schedule_download->command_key);
		bkp_session_insert(b, "file_type", papply_schedule_download->file_type);
		bkp_session_insert(b, "windowstart1", delay[0]);
		bkp_session_insert(b, "windowend1", delay[1]);
		bkp_session_insert(b, "maxretrie1", maxretrie[0]);

		bkp_session_insert(b, "windowstart2", delay[2]);
		bkp_session_insert(b, "windowend2", delay[3]);
		bkp_session_insert(b, "maxretrie2", maxretrie[1]);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_apply_schedule_download(struct apply_schedule_download *papply_schedule_download) //TODO
{
	char delay[4][128];
	char maxretrie[2][128];
	int i;
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	for (i = 0; i < 2; i++) {
		sprintf(delay[2 * i], "%ld", papply_schedule_download->timeintervals[i].windowstart);
		sprintf(delay[2 * i + 1], "%ld", papply_schedule_download->timeintervals[i].windowend);
		sprintf(maxretrie[i], "%d", papply_schedule_download->timeintervals[i].maxretries);
	}
	struct search_keywords sched_download_del_app_keys[9] = { { "start_time", papply_schedule_download->start_time },
								  { "command_key", papply_schedule_download->command_key },
								  { "file_type", papply_schedule_download->file_type },
								  { "windowstart1", delay[0] },
								  { "windowend1", delay[1] },
								  { "maxretrie1", maxretrie[0] },
								  { "windowstart2", delay[2] },
								  { "windowend2", delay[3] },
								  { "maxretrie2", maxretrie[1] } };

	b = bkp_session_node_found(bkp_tree, "apply_schedule_download", sched_download_del_app_keys, 9);

	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_change_du_state(struct change_du_state *pchange_du_state)
{
	struct operations *p;
	char schedule_time[128];
	mxml_node_t *b, *n;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pchange_du_state->timeout);
	b = bkp_session_insert(bkp_tree, "change_du_state", NULL);
	bkp_session_insert(b, "command_key", pchange_du_state->command_key);
	bkp_session_insert(b, "time", schedule_time);

	list_for_each_entry (p, &(pchange_du_state->list_operation), list) {
		if (p->type == DU_INSTALL) {
			n = bkp_session_insert(b, "install", NULL);
			bkp_session_insert(n, "url", p->url);
			bkp_session_insert(n, "uuid", p->uuid);
			bkp_session_insert(n, "username", p->username);
			bkp_session_insert(n, "password", p->password);
			bkp_session_insert(n, "executionenvref", p->executionenvref);
		} else if (p->type == DU_UPDATE) {
			n = bkp_session_insert(b, "update", NULL);
			bkp_session_insert(n, "uuid", p->uuid);
			bkp_session_insert(n, "version", p->version);
			bkp_session_insert(n, "url", p->url);
			bkp_session_insert(n, "username", p->username);
			bkp_session_insert(n, "password", p->password);
		} else if (p->type == DU_UNINSTALL) {
			n = bkp_session_insert(b, "uninstall", NULL);
			bkp_session_insert(n, "uuid", p->uuid);
			bkp_session_insert(n, "version", p->version);
			bkp_session_insert(n, "executionenvref", p->executionenvref);
		}
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_change_du_state(struct change_du_state *pchange_du_state)
{
	char schedule_time[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pchange_du_state->timeout);
	struct search_keywords cds_del_keys[2] = { { "command_key", pchange_du_state->command_key }, { "time", schedule_time } };
	b = bkp_session_node_found(bkp_tree, "change_du_state", cds_del_keys, 2);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_upload(struct upload *pupload)
{
	char schedule_time[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pupload->scheduled_time);
	struct search_keywords upload_insert_keys[6] = { { "url", pupload->url }, { "command_key", pupload->command_key }, { "username", pupload->username }, { "password", pupload->password }, { "time", schedule_time }, { "file_type", pupload->file_type } };

	b = bkp_session_node_found(bkp_tree, "upload", upload_insert_keys, 6);
	if (!b) {
		b = bkp_session_insert(bkp_tree, "upload", NULL);
		bkp_session_insert(b, "url", pupload->url);
		bkp_session_insert(b, "command_key", pupload->command_key);
		bkp_session_insert(b, "file_type", pupload->file_type);
		bkp_session_insert(b, "username", pupload->username);
		bkp_session_insert(b, "password", pupload->password);
		bkp_session_insert(b, "time", schedule_time);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}
void bkp_session_delete_download(struct download *pdownload)
{
	char schedule_time[128];
	char file_size[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pdownload->scheduled_time);
	sprintf(file_size, "%d", pdownload->file_size);
	struct search_keywords download_del_keys[7] = { { "url", pdownload->url }, { "command_key", pdownload->command_key }, { "file_type", pdownload->file_type }, { "username", pdownload->username }, { "password", pdownload->password }, { "file_size", file_size }, { "time", schedule_time } };

	b = bkp_session_node_found(bkp_tree, "download", download_del_keys, 7);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_schedule_download(struct download *pschedule_download_delete)
{
	char delay[4][128];
	char file_size[128];
	char maxretrie[2][128];
	int i;
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(file_size, "%d", pschedule_download_delete->file_size);
	for (i = 0; i < 2; i++) {
		sprintf(delay[2 * i], "%ld", pschedule_download_delete->timewindowstruct[i].windowstart);
		sprintf(delay[2 * i + 1], "%ld", pschedule_download_delete->timewindowstruct[i].windowend);
		sprintf(maxretrie[i], "%d", pschedule_download_delete->timewindowstruct[i].maxretries);
	}
	struct search_keywords sched_download_del_keys[16] = { { "url", pschedule_download_delete->url },
							       { "command_key", pschedule_download_delete->command_key },
							       { "file_type", pschedule_download_delete->file_type },
							       { "username", pschedule_download_delete->username },
							       { "password", pschedule_download_delete->password },
							       { "file_size", file_size },
							       { "windowstart1", delay[0] },
							       { "windowend1", delay[1] },
							       { "windowmode1", pschedule_download_delete->timewindowstruct[0].windowmode },
							       { "usermessage1", pschedule_download_delete->timewindowstruct[0].usermessage },
							       { "maxretrie1", maxretrie[0] },
							       { "windowstart2", delay[2] },
							       { "windowend2", delay[3] },
							       { "windowmode2", pschedule_download_delete->timewindowstruct[1].windowmode },
							       { "usermessage2", pschedule_download_delete->timewindowstruct[1].usermessage },
							       { "maxretrie2", maxretrie[1] } };

	b = bkp_session_node_found(bkp_tree, "schedule_download", sched_download_del_keys, 16);

	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}
void bkp_session_delete_upload(struct upload *pupload)
{
	char schedule_time[128];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pupload->scheduled_time);
	struct search_keywords upload_del_keys[6] = { { "url", pupload->url }, { "command_key", pupload->command_key }, { "file_type", pupload->file_type }, { "username", pupload->username }, { "password", pupload->password }, { "time", schedule_time } };
	b = bkp_session_node_found(bkp_tree, "upload", upload_del_keys, 6);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_insert_du_state_change_complete(struct du_state_change_complete *pdu_state_change_complete)
{
	char schedule_time[128], resolved[8], fault_code[8];
	struct opresult *p;
	mxml_node_t *b, *n;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pdu_state_change_complete->timeout);
	b = bkp_session_insert(bkp_tree, "du_state_change_complete", NULL);
	bkp_session_insert(b, "command_key", pdu_state_change_complete->command_key);
	bkp_session_insert(b, "time", schedule_time);
	list_for_each_entry (p, &(pdu_state_change_complete->list_opresult), list) {
		n = bkp_session_insert(b, "opresult", NULL);
		sprintf(resolved, "%d", p->resolved);
		sprintf(fault_code, "%d", p->fault);
		bkp_session_insert(n, "uuid", p->uuid);
		bkp_session_insert(n, "du_ref", p->du_ref);
		bkp_session_insert(n, "version", p->version);
		bkp_session_insert(n, "current_state", p->current_state);
		bkp_session_insert(n, "resolved", resolved);
		bkp_session_insert(n, "execution_unit_ref", p->execution_unit_ref);
		bkp_session_insert(n, "start_time", p->start_time);
		bkp_session_insert(n, "complete_time", p->complete_time);
		bkp_session_insert(n, "fault", fault_code);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_du_state_change_complete(struct du_state_change_complete *pdu_state_change_complete)
{
	mxml_node_t *b;
	char schedule_time[128];

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(schedule_time, "%ld", pdu_state_change_complete->timeout);
	struct search_keywords cds_complete_keys[2] = { { "command_key", pdu_state_change_complete->command_key }, { "time", schedule_time } };

	b = bkp_session_node_found(bkp_tree, "du_state_change_complete", cds_complete_keys, 2);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}
void bkp_session_insert_transfer_complete(struct transfer_complete *ptransfer_complete)
{
	struct search_keywords keys[5];
	char fault_code[16];
	char type[16];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(fault_code, "%d", ptransfer_complete->fault_code);
	keys[0].name = "command_key";
	keys[0].value = ptransfer_complete->command_key;
	keys[1].name = "start_time";
	keys[1].value = ptransfer_complete->start_time;
	keys[2].name = "complete_time";
	keys[2].value = ptransfer_complete->complete_time;
	keys[3].name = "fault_code";
	keys[3].value = fault_code;
	keys[4].name = "type";
	sprintf(type, "%d", ptransfer_complete->type);
	keys[4].value = type;
	b = bkp_session_node_found(bkp_tree, "transfer_complete", keys, 5);
	if (!b) {
		b = bkp_session_insert(bkp_tree, "transfer_complete", NULL);
		bkp_session_insert(b, "command_key", ptransfer_complete->command_key);
		bkp_session_insert(b, "start_time", ptransfer_complete->start_time);
		bkp_session_insert(b, "complete_time", ptransfer_complete->complete_time);
		bkp_session_insert(b, "old_software_version", ptransfer_complete->old_software_version);
		bkp_session_insert(b, "fault_code", fault_code);
		bkp_session_insert(b, "type", type);
	}
	pthread_mutex_unlock(&mutex_backup_session);
}

void bkp_session_delete_transfer_complete(struct transfer_complete *ptransfer_complete)
{
	char fault_code[16];
	char type[16];
	mxml_node_t *b;

	pthread_mutex_lock(&mutex_backup_session);
	sprintf(fault_code, "%d", ptransfer_complete->fault_code);
	sprintf(type, "%d", ptransfer_complete->type);
	struct search_keywords trans_comp_del_keys[5] = { { "command_key", ptransfer_complete->command_key }, { "start_time", ptransfer_complete->start_time }, { "complete_time", ptransfer_complete->complete_time }, { "fault_code", fault_code }, { "type", type } };

	b = bkp_session_node_found(bkp_tree, "transfer_complete", trans_comp_del_keys, 5);
	if (b)
		mxmlDelete(b);
	pthread_mutex_unlock(&mutex_backup_session);
}

int save_acs_bkp_config(struct cwmp *cwmp)
{
	struct config *conf;

	conf = &(cwmp->conf);
	bkp_session_simple_insert("acs", "url", conf->acsurl);
	bkp_session_save();
	return CWMP_OK;
}

/*
 * Load backup session
 */
char *load_child_value(mxml_node_t *tree, char *sub_name)
{
	char *value = NULL;
	mxml_node_t *b = tree;

	if (b) {
		b = mxmlFindElement(b, b, sub_name, NULL, NULL, MXML_DESCEND);
		if (b) {
			b = mxmlWalkNext(b, tree, MXML_DESCEND);
			if (b && b->type == MXML_OPAQUE) {
				if (b->value.opaque != NULL) {
					value = strdup(b->value.opaque);
				}
			}
		}
	}
	return value;
}

void load_queue_event(mxml_node_t *tree, struct cwmp *cwmp)
{
	char *command_key = NULL;
	mxml_node_t *b = tree, *c;
	int idx = -1, id = -1;
	struct event_container *event_container_save = NULL;

	struct backup_attributes bkp_attrs = {.index = &idx, .id = &id, .command_key = &command_key };
	load_specific_backup_attributes(tree, &bkp_attrs);

	b = mxmlWalkNext(b, tree, MXML_DESCEND);

	while (b) {
		if (b && b->type == MXML_ELEMENT) {
			if (strcmp(b->value.element.name, "command_key") == 0) {
				if (idx != -1) {
					if (EVENT_CONST[idx].RETRY & EVENT_RETRY_AFTER_REBOOT) {
						event_container_save = cwmp_add_event_container(cwmp, idx, ((command_key != NULL) ? command_key : ""));
						if (event_container_save != NULL) {
							event_container_save->id = id;
						}
					}
				}
				FREE(command_key);
			} else if (strcmp(b->value.element.name, "parameter") == 0) {
				c = mxmlWalkNext(b, b, MXML_DESCEND);
				if (c && c->type == MXML_OPAQUE) {
					if (c->value.opaque != NULL) {
						if (event_container_save != NULL) {
							add_dm_parameter_to_list(&(event_container_save->head_dm_parameter), c->value.opaque, NULL, NULL, 0, false);
						}
					}
				}
			}
		}
		b = mxmlWalkNext(b, tree, MXML_NO_DESCEND);
	}
}

void load_schedule_inform(mxml_node_t *tree)
{
	char *command_key = NULL;
	time_t scheduled_time = 0;
	struct schedule_inform *schedule_inform = NULL;
	struct list_head *ilist = NULL;

	struct backup_attributes bkp_attrs = {.command_key = &command_key, .time = &scheduled_time };
	load_specific_backup_attributes(tree, &bkp_attrs);

	list_for_each (ilist, &(list_schedule_inform)) {
		schedule_inform = list_entry(ilist, struct schedule_inform, list);
		if (schedule_inform->scheduled_time > scheduled_time) {
			break;
		}
	}
	schedule_inform = calloc(1, sizeof(struct schedule_inform));
	if (schedule_inform != NULL) {
		schedule_inform->commandKey = command_key;
		schedule_inform->scheduled_time = scheduled_time;
		list_add(&(schedule_inform->list), ilist->prev);
	}
}

void load_download(mxml_node_t *tree)
{
	struct download *download_request = NULL;
	struct list_head *ilist = NULL;
	struct download *idownload_request = NULL;

	download_request = calloc(1, sizeof(struct download));

	struct backup_attributes bkp_attrs = {.url = &download_request->url,
					      .command_key = &download_request->command_key,
					      .file_type = &download_request->file_type,
					      .username = &download_request->username,
					      .password = &download_request->password,
					      .file_size = &download_request->file_size,
					      .time = &download_request->scheduled_time };
	load_specific_backup_attributes(tree, &bkp_attrs);

	list_for_each (ilist, &(list_download)) {
		idownload_request = list_entry(ilist, struct download, list);
		if (idownload_request->scheduled_time > download_request->scheduled_time) {
			break;
		}
	}
	list_add(&(download_request->list), ilist->prev);
	if (download_request->scheduled_time != 0)
		count_download_queue++;
}

void load_schedule_download(mxml_node_t *tree)
{
	struct download *download_request = NULL;
	struct list_head *ilist = NULL;
	struct download *idownload_request = NULL;

	download_request = calloc(1, sizeof(struct download));

	struct backup_attributes bkp_attrs = {
		.url = &download_request->url,
		.command_key = &download_request->command_key,
		.file_type = &download_request->file_type,
		.username = &download_request->username,
		.password = &download_request->password,
		.file_size = &download_request->file_size,
		.windowstart1 = &download_request->timewindowstruct[0].windowstart,
		.windowend1 = &download_request->timewindowstruct[0].windowend,
		.windowmode1 = &download_request->timewindowstruct[0].windowmode,
		.usermessage1 = &download_request->timewindowstruct[0].usermessage,
		.maxretrie1 = &download_request->timewindowstruct[0].maxretries,
		.windowstart2 = &download_request->timewindowstruct[1].windowstart,
		.windowend2 = &download_request->timewindowstruct[1].windowend,
		.windowmode2 = &download_request->timewindowstruct[1].windowmode,
		.usermessage2 = &download_request->timewindowstruct[1].usermessage,
		.maxretrie2 = &download_request->timewindowstruct[1].maxretries,
	};
	load_specific_backup_attributes(tree, &bkp_attrs);

	list_for_each (ilist, &(list_schedule_download)) {
		idownload_request = list_entry(ilist, struct download, list);
		if (idownload_request->timewindowstruct[0].windowstart > download_request->timewindowstruct[0].windowstart) {
			break;
		}
	}
	list_add(&(download_request->list), ilist->prev);
	if (download_request->timewindowstruct[0].windowstart != 0)
		count_download_queue++;
}

void load_apply_schedule_download(mxml_node_t *tree)
{
	struct apply_schedule_download *download_request = NULL;

	download_request = calloc(1, sizeof(struct apply_schedule_download));

	struct backup_attributes bkp_attrs = {
		.command_key = &download_request->command_key,
		.file_type = &download_request->file_type,
		.start_time = &download_request->start_time,
		.windowstart1 = &download_request->timeintervals[0].windowstart,
		.windowend1 = &download_request->timeintervals[0].windowend,
		.maxretrie1 = &download_request->timeintervals[0].maxretries,
		.windowstart2 = &download_request->timeintervals[1].windowstart,
		.windowend2 = &download_request->timeintervals[1].windowend,
		.maxretrie2 = &download_request->timeintervals[1].maxretries,
	};
	load_specific_backup_attributes(tree, &bkp_attrs);

	list_add_tail(&(download_request->list), &(list_apply_schedule_download));
	if (download_request->timeintervals[0].windowstart != 0)
		count_download_queue++;
}

void load_upload(mxml_node_t *tree)
{
	struct upload *upload_request = NULL;
	struct list_head *ilist = NULL;
	struct upload *iupload_request = NULL;

	upload_request = calloc(1, sizeof(struct upload));

	struct backup_attributes bkp_attrs = {.url = &upload_request->url, .command_key = &upload_request->command_key, .file_type = &upload_request->file_type, .username = &upload_request->username, .password = &upload_request->password, .time = &upload_request->scheduled_time };
	load_specific_backup_attributes(tree, &bkp_attrs);

	list_for_each (ilist, &(list_upload)) {
		iupload_request = list_entry(ilist, struct upload, list);
		if (iupload_request->scheduled_time > upload_request->scheduled_time) {
			break;
		}
	}
	list_add(&(upload_request->list), ilist->prev);
	if (upload_request->scheduled_time != 0)
		count_download_queue++;
}

void load_change_du_state(mxml_node_t *tree)
{
	mxml_node_t *b = tree;
	struct change_du_state *change_du_state_request = NULL;
	struct operations *elem;

	change_du_state_request = calloc(1, sizeof(struct change_du_state));
	INIT_LIST_HEAD(&(change_du_state_request->list_operation));

	struct backup_attributes bkp_attrs = {.command_key = &change_du_state_request->command_key, .time = &change_du_state_request->timeout };
	load_specific_backup_attributes(tree, &bkp_attrs);

	b = mxmlWalkNext(b, tree, MXML_DESCEND);

	while (b) {
		if (b && b->type == MXML_ELEMENT) {
			if (strcmp(b->value.element.name, "update") == 0) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_UPDATE;
				list_add_tail(&(elem->list), &(change_du_state_request->list_operation));
				struct backup_attributes update_bkp_attrs = {.uuid = &elem->uuid, .version = &elem->version, .url = &elem->url, .username = &elem->username, .password = &elem->password };
				load_specific_backup_attributes(b, &update_bkp_attrs);
			} else if (strcmp(b->value.element.name, "install") == 0) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_INSTALL;
				list_add_tail(&(elem->list), &(change_du_state_request->list_operation));

				struct backup_attributes install_bkp_attrs = {.uuid = &elem->uuid, .executionenvref = &elem->executionenvref, .url = &elem->url, .username = &elem->username, .password = &elem->password };
				load_specific_backup_attributes(b, &install_bkp_attrs);
			} else if (strcmp(b->value.element.name, "uninstall") == 0) {
				elem = (operations *)calloc(1, sizeof(operations));
				elem->type = DU_UNINSTALL;
				list_add_tail(&(elem->list), &(change_du_state_request->list_operation));
				struct backup_attributes uninstall_bkp_attrs = {.uuid = &elem->uuid, .version = &elem->version, .executionenvref = &elem->executionenvref };
				load_specific_backup_attributes(b, &uninstall_bkp_attrs);
			}
		}
		b = mxmlWalkNext(b, tree, MXML_NO_DESCEND);
	}
	list_add_tail(&(change_du_state_request->list_operation), &(list_change_du_state));
}

void load_du_state_change_complete(mxml_node_t *tree, struct cwmp *cwmp)
{
	mxml_node_t *b = tree;
	struct du_state_change_complete *du_state_change_complete_request = NULL;
	struct opresult *elem;

	du_state_change_complete_request = calloc(1, sizeof(struct du_state_change_complete));
	INIT_LIST_HEAD(&(du_state_change_complete_request->list_opresult));

	struct backup_attributes bkp_attrs = {.command_key = &du_state_change_complete_request->command_key, .time = &du_state_change_complete_request->timeout };
	load_specific_backup_attributes(tree, &bkp_attrs);

	b = mxmlWalkNext(b, tree, MXML_DESCEND);

	while (b) {
		if (b && b->type == MXML_ELEMENT) {
			if (strcmp(b->value.element.name, "opresult") == 0) {
				elem = (opresult *)calloc(1, sizeof(opresult));
				list_add_tail(&(elem->list), &(du_state_change_complete_request->list_opresult));

				struct backup_attributes opresult_bkp_attrs = {.uuid = &elem->uuid,
									       .version = &elem->version,
									       .du_ref = &elem->du_ref,
									       .current_state = &elem->current_state,
									       .resolved = &elem->resolved,
									       .start_time = &elem->start_time,
									       .complete_time = &elem->complete_time,
									       .fault = &elem->fault,
									       .execution_unit_ref = &elem->execution_unit_ref };
				load_specific_backup_attributes(b, &opresult_bkp_attrs);
			}
		}
		b = mxmlWalkNext(b, tree, MXML_NO_DESCEND);
	}
	cwmp_root_cause_changedustate_complete(cwmp, du_state_change_complete_request);
}

void load_transfer_complete(mxml_node_t *tree, struct cwmp *cwmp)
{
	struct transfer_complete *ptransfer_complete;

	ptransfer_complete = calloc(1, sizeof(struct transfer_complete));

	struct backup_attributes bkp_attrs = {.command_key = &ptransfer_complete->command_key,
					      .start_time = &ptransfer_complete->start_time,
					      .complete_time = &ptransfer_complete->complete_time,
					      .old_software_version = &ptransfer_complete->old_software_version,
					      .fault_code = &ptransfer_complete->fault_code,
					      .type = &ptransfer_complete->type };
	load_specific_backup_attributes(tree, &bkp_attrs);

	cwmp_root_cause_transfer_complete(cwmp, ptransfer_complete);
	sotfware_version_value_change(cwmp, ptransfer_complete);
}

void bkp_session_create_file()
{
	FILE *pFile;

	pthread_mutex_lock(&mutex_backup_session);
	pFile = fopen(CWMP_BKP_FILE, "w");
	if (pFile == NULL) {
		CWMP_LOG(ERROR, "Unable to create %s file", CWMP_BKP_FILE);
		pthread_mutex_unlock(&mutex_backup_session);
		return;
	}
	fprintf(pFile, "%s", CWMP_BACKUP_SESSION);
	if (bkp_tree != NULL)
		MXML_DELETE(bkp_tree);
	bkp_tree = mxmlLoadString(NULL, CWMP_BACKUP_SESSION, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	pthread_mutex_unlock(&mutex_backup_session);
}

int bkp_session_check_file()
{
	FILE *pFile;

	if (!file_exists(CWMP_BKP_FILE)) {
		bkp_session_create_file();
		return -1;
	}

	if (bkp_tree == NULL) {
		pFile = fopen(CWMP_BKP_FILE, "r");
		bkp_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
		fclose(pFile);
	}

	if (bkp_tree == NULL) {
		bkp_session_create_file();
		return -1;
	}
	bkp_session_move_inform_to_inform_queue();
	bkp_session_save();
	return 0;
}

int cwmp_init_backup_session(struct cwmp *cwmp, char **ret, enum backup_loading load)
{
	int error;

	if (bkp_session_check_file())
		return 0;

	error = cwmp_load_saved_session(cwmp, ret, load);
	return error;
}

int cwmp_load_saved_session(struct cwmp *cwmp, char **ret, enum backup_loading load)
{
	mxml_node_t *b;

	b = bkp_tree;
	b = mxmlWalkNext(b, bkp_tree, MXML_DESCEND);
	while (b) {
		if (load == ACS) {
			if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "acs") == 0) {
				*ret = load_child_value(b, "url");
				break;
			}
		}
		if (load == CR_IP) {
			if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "connection_request") == 0) {
				*ret = load_child_value(b, "ip");
				break;
			}
		}
		if (load == CR_IPv6) {
			if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "connection_request") == 0) {
				*ret = load_child_value(b, "ipv6");
				break;
			}
		}
		if (load == CR_PORT) {
			if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "connection_request") == 0) {
				*ret = load_child_value(b, "port");
				break;
			}
		}
		if (load == ALL) {
			if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "queue_event") == 0) {
				load_queue_event(b, cwmp);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "download") == 0) {
				load_download(b);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "upload") == 0) {
				load_upload(b);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "transfer_complete") == 0) {
				load_transfer_complete(b, cwmp);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "schedule_inform") == 0) {
				load_schedule_inform(b);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "change_du_state") == 0) {
				load_change_du_state(b);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "du_state_change_complete") == 0) {
				load_du_state_change_complete(b, cwmp);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "schedule_download") == 0) {
				load_schedule_download(b);
			} else if (b->type == MXML_ELEMENT && strcmp(b->value.element.name, "apply_schedule_download") == 0) {
				load_apply_schedule_download(b);
			}
		}
		b = mxmlWalkNext(b, bkp_tree, MXML_NO_DESCEND);
	}

	return CWMP_OK;
}
