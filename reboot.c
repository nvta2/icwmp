/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <unistd.h>
#include "session.h"
#include "cwmp_uci.h"
#include "log.h"
#include "reboot.h"

void cwmp_schedule_reboot(struct uloop_timeout *timeout  __attribute__((unused)));
void cwmp_delay_reboot(struct uloop_timeout *timeout  __attribute__((unused)));

struct uloop_timeout schedule_reboot_timer = { .cb = cwmp_schedule_reboot };
struct uloop_timeout delay_reboot_timer = { .cb = cwmp_delay_reboot };

void cwmp_schedule_reboot(struct uloop_timeout *timeout  __attribute__((unused)))
{
	cwmp_uci_set_value("cwmp", "cpe", "schedule_reboot", "0001-01-01T00:00:00Z");
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	if (time(NULL) > cwmp_main->conf.schedule_reboot)
		return;
	cwmp_reboot("schedule_reboot");
}

void cwmp_delay_reboot(struct uloop_timeout *timeout  __attribute__((unused)))
{
	cwmp_uci_set_value("cwmp", "cpe", "delay_reboot", "-1");
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	if (cwmp_main->session->session_status.last_status == SESSION_RUNNING) {
		cwmp_set_end_session(END_SESSION_REBOOT);
	} else {
		cwmp_reboot("delay_reboot");
		exit(EXIT_SUCCESS);
	}
}

void launch_reboot_methods()
{
	static int curr_delay_reboot = -1;
	static time_t curr_schedule_redoot = 0;

	if (cwmp_main->conf.delay_reboot != curr_delay_reboot && cwmp_main->conf.delay_reboot > 0) {
		CWMP_LOG(INFO, "The device will reboot after %ld seconds", cwmp_main->conf.delay_reboot);
		curr_delay_reboot = cwmp_main->conf.delay_reboot;
		uloop_timeout_cancel(&delay_reboot_timer);
		uloop_timeout_set(&delay_reboot_timer, cwmp_main->conf.delay_reboot * 1000);
	}

	if (cwmp_main->conf.schedule_reboot != curr_schedule_redoot && (cwmp_main->conf.schedule_reboot - time(NULL)) > 0) {
		curr_schedule_redoot = cwmp_main->conf.schedule_reboot;
		time_t remaining_time = cwmp_main->conf.schedule_reboot - time(NULL);
		CWMP_LOG(INFO, "The device will reboot after %ld seconds", remaining_time);
		uloop_timeout_cancel(&schedule_reboot_timer);
		uloop_timeout_set(&schedule_reboot_timer, remaining_time * 1000);
	}
}
