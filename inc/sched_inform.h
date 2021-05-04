/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef CWMP_SCHED_INFORM_H
#define CWMP_SCHED_INFORM_H

extern struct list_head list_schedule_inform;
extern pthread_mutex_t mutex_schedule_inform;
extern pthread_cond_t threshold_schedule_inform;
extern int count_schedule_inform_queue;

extern pthread_mutex_t mutex_schedule_inform;
extern pthread_cond_t threshold_schedule_inform;
void *thread_cwmp_rpc_cpe_scheduleInform(void *v);
int cwmp_scheduleInform_remove_all();
#endif
