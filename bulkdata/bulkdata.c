/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*				Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include <stdio.h>
#include <string.h>
#include <libubox/uloop.h>

#include "http.h"
#include "config.h"
#include "log.h"
#include "common.h"
#include "report.h"
#include "times.h"
#include "bulkdata.h"

struct bulkdata bulkdata_main = {0};
int profiles_number = 0;

void bulkdata_profile_cb(struct uloop_timeout *timeout);

int get_retry_period(int min)
{
        srand(time(NULL));
        return rand()%min + min;
}

static void bulkdata_run_profiles(struct bulkdata *bulkdata)
{
	unsigned int next_period;
	int i = 0;

	for (i = 0; i < profiles_number; i++) {
		bulkdata->profile[i].utimer.cb = bulkdata_profile_cb;
		LIST_HEAD(failedreports);
		bulkdata->profile[i].failed_reports = &failedreports;
		next_period = get_next_period(bulkdata->profile[i].time_reference, bulkdata->profile[i].reporting_interval);
		bulkdata_log(SINFO, "The session of profile_id %d will be start in %d sec", bulkdata->profile[i].profile_id, next_period);
		uloop_timeout_set(&bulkdata->profile[i].utimer, next_period * 1000);
	}
}

int http_send_report(struct profile *profile, char *report)
{
	char *msg_in = NULL;
	int http_code;

	http_client_init(profile);
	bulkdata_log(SINFO, "Send the report of profile_id %d to Bulkdata Collector", profile->profile_id);
	http_code = http_send_message(profile, report, strlen(report), &msg_in);
	http_client_exit();
	return http_code;
}

void bulkdata_profile_cb(struct uloop_timeout *timeout)
{
	struct profile *profile;
	unsigned int http_code, retry_period;
	char *report = NULL;

	profile = container_of(timeout, struct profile, utimer);
	time_t now = time(NULL);

	bulkdata_log(SINFO, "New session of profile_id %d started", profile->profile_id);
	if(profile->retry_count == 0 || profile->next_retry > now || !profile->http_retry_enable) //Perdiodic execution
		create_encoding_bulkdata_report(profile, &report);
	else
		create_failed_report(profile, &report);

	bulkdata_log(SDEBUG, "The content of the profile_id report %d is :\n==========\n%s\n==========\n", profile->profile_id, report);
	http_code= http_send_report(profile, report);
	if(http_code != 200){
		if(profile->retry_count == 0 || profile->next_retry > now || !profile->http_retry_enable) {  //Perdiodic execution
			retry_period = get_retry_period(profile->http_retry_minimum_wait_interval);
			profile->next_period = now + profile->reporting_interval;
			profile->next_retry = now + retry_period;
			profile->retry_count = 1;
			profile->min_retry = profile->http_retry_minimum_wait_interval * 2;
			if((profile->next_retry < profile->next_period) && profile->http_retry_enable) {
				bulkdata_log(SINFO, "Retry session of profile_id %d in %d sec", profile->profile_id, retry_period);
				uloop_timeout_set(timeout, 1000 * retry_period);
			}
			else {
				bulkdata_log(SINFO, "Start New session of profile_id %d in %d sec", profile->profile_id, profile->reporting_interval);
				uloop_timeout_set(timeout, 1000 * profile->reporting_interval);
			}
		} else { //Retry execution
			retry_period= get_retry_period(profile->min_retry);
			profile->min_retry*=2;
			profile->next_retry+=retry_period;
			profile->retry_count++;
			if(profile->next_retry < profile->next_period) {
				bulkdata_log(SINFO, "Retry session of profile_id %d in %d sec", profile->profile_id, retry_period);
				uloop_timeout_set(timeout, 1000 * retry_period);
			}
			else {
				bulkdata_log(SINFO, "Retry session of profile_id %d in %d sec", profile->profile_id, (profile->next_period-profile->next_retry+retry_period));
				uloop_timeout_set(timeout, 1000 * (profile->next_period-profile->next_retry+retry_period));
			}
		}
		if(profile->new_report){
			bulkdata_add_failed_report(profile, profile->new_report);
			FREE(profile->new_report);
		}
		FREE(report);
	} else {
		if(profile->retry_count == 0 || profile->next_retry > now || !profile->http_retry_enable) {
			bulkdata_log(SINFO, "Start New session of profile_id %d in %d sec", profile->profile_id, profile->reporting_interval);
			uloop_timeout_set(timeout, 1000 * profile->reporting_interval);
		}
		else {
			bulkdata_log(SINFO, "Retry session of profile_id %d in %d sec", profile->profile_id, (profile->next_period-profile->next_retry));
			uloop_timeout_set(timeout, 1000 * (profile->next_period-profile->next_retry));
		}
		FREE(profile->new_report);
		FREE(report);
		empty_failed_reports_list(profile);
		profile->retry_count= 0;
	}
}

int main(void)
{
	struct bulkdata *bulkdata = &bulkdata_main;
	if(bulkdata_config_init(bulkdata) == -1)
		return -1;
	bulkdata_log(SINFO, "Start icwmp_bulkdatad daemon");
	
	uloop_init();
	bulkdata_run_profiles(bulkdata);
	uloop_run();
	uloop_done();

	bulkdata_config_fini(bulkdata);
	bulkdata_log(SINFO, "Stop icwmp_bulkdatad daemon");
	return 0;
}
