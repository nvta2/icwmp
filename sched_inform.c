/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include "common.h"
#include "sched_inform.h"
#include "log.h"
#include "backupSession.h"
#include "event.h"

LIST_HEAD(list_schedule_inform);
pthread_mutex_t mutex_schedule_inform = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t threshold_schedule_inform;

int count_schedule_inform_queue = 0;

void *thread_cwmp_rpc_cpe_scheduleInform(void *v)
{
	struct cwmp *cwmp = (struct cwmp *)v;
	struct event_container *event_container;
	struct schedule_inform *schedule_inform;
	struct timespec si_timeout = { 0, 0 };
	time_t current_time, stime;
	bool add_event_same_time = false;

	for (;;) {
		if (list_schedule_inform.next != &(list_schedule_inform)) {
			schedule_inform = list_entry(list_schedule_inform.next, struct schedule_inform, list);
			stime = schedule_inform->scheduled_time;
			current_time = time(NULL);
			if (current_time >= schedule_inform->scheduled_time) {
				if (add_event_same_time) {
					pthread_mutex_lock(&mutex_schedule_inform);
					list_del(&(schedule_inform->list));
					if (schedule_inform->commandKey != NULL) {
						bkp_session_delete_schedule_inform(schedule_inform->scheduled_time, schedule_inform->commandKey);
						free(schedule_inform->commandKey);
					}
					free(schedule_inform);
					pthread_mutex_unlock(&mutex_schedule_inform);
					continue;
				}
				pthread_mutex_lock(&(cwmp->mutex_session_queue));
				CWMP_LOG(INFO, "Schedule Inform thread: add ScheduleInform event in the queue");
				event_container = cwmp_add_event_container(cwmp, EVENT_IDX_3SCHEDULED, "");
				if (event_container != NULL) {
					cwmp_save_event_container(event_container);
				}
				event_container = cwmp_add_event_container(cwmp, EVENT_IDX_M_ScheduleInform, schedule_inform->commandKey);
				if (event_container != NULL) {
					cwmp_save_event_container(event_container);
				}
				pthread_mutex_unlock(&(cwmp->mutex_session_queue));
				pthread_cond_signal(&(cwmp->threshold_session_send));
				pthread_mutex_lock(&mutex_schedule_inform);
				list_del(&(schedule_inform->list));
				if (schedule_inform->commandKey != NULL) {
					bkp_session_delete_schedule_inform(schedule_inform->scheduled_time, schedule_inform->commandKey);
					free(schedule_inform->commandKey);
				}
				free(schedule_inform);
				count_schedule_inform_queue--;
				pthread_mutex_unlock(&mutex_schedule_inform);
				add_event_same_time = true;
				continue;
			}
			bkp_session_save();
			add_event_same_time = false;
			pthread_mutex_lock(&mutex_schedule_inform);
			si_timeout.tv_sec = stime;
			pthread_cond_timedwait(&threshold_schedule_inform, &mutex_schedule_inform, &si_timeout);
			pthread_mutex_unlock(&mutex_schedule_inform);
		} else {
			bkp_session_save();
			add_event_same_time = false;
			pthread_mutex_lock(&mutex_schedule_inform);
			pthread_cond_wait(&threshold_schedule_inform, &mutex_schedule_inform);
			pthread_mutex_unlock(&mutex_schedule_inform);
		}
	}

	return NULL;
}

int cwmp_scheduleInform_remove_all()
{
	struct schedule_inform *schedule_inform;

	pthread_mutex_lock(&mutex_schedule_inform);
	while (list_schedule_inform.next != &(list_schedule_inform)) {
		schedule_inform = list_entry(list_schedule_inform.next, struct schedule_inform, list);

		list_del(&(schedule_inform->list));
		if (schedule_inform->commandKey != NULL) {
			bkp_session_delete_schedule_inform(schedule_inform->scheduled_time, schedule_inform->commandKey);
			free(schedule_inform->commandKey);
		}
		free(schedule_inform);
	}
	bkp_session_save();
	pthread_mutex_unlock(&mutex_schedule_inform);

	return CWMP_OK;
}
