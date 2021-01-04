/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _FREECWMP_HTTP_H__
#define _FREECWMP_HTTP_H__

#include "common.h"
extern char *fc_cookies;

#define HTTP_TIMEOUT 30

struct http_client
{
	struct curl_slist *header_list;
	char *url;
};

int http_client_init(struct cwmp *cwmp);
void http_client_exit(void);
int http_send_message(struct cwmp *cwmp, char *msg_out, int msg_out_len,char **msg_in);

void http_server_init(void);
void http_server_listen(void);
void http_success_cr();

#endif

