/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <pthread.h>
#include "common.h"
#include "cwmp_http.h"
#include "http.h"
#include "log.h"

struct uloop_fd http_event6;

pthread_t http_cr_server_thread;

void http_server_listen_uloop(struct uloop_fd *ufd __attribute__((unused)), unsigned events __attribute__((unused)))
{
	http_server_listen();
}

void http_server_start_uloop(void)
{
	http_server_init();
	http_event6.fd = cwmp_main->cr_socket_desc;
	http_event6.cb = http_server_listen_uloop;
	uloop_fd_add(&http_event6, ULOOP_READ | ULOOP_EDGE_TRIGGER);
}

static void *thread_http_cr_server_listen(void *v __attribute__((unused)))
{
	http_server_listen();
	return NULL;
}

void http_server_start(void)
{
	int error = pthread_create(&http_cr_server_thread, NULL, &thread_http_cr_server_listen, NULL);
	if (error < 0) {
		CWMP_LOG(ERROR, "Error when creating the http connection request server thread!");
	}
}
