/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#ifndef __HTTP_H
#define __HTTP_H

#include <curl/curl.h>
#include "config.h"

#define HTTP_TIMEOUT 30

struct http_client
{
	struct curl_slist *header_list;
	char *url;
};

int http_client_init(struct profile *profile);
void http_client_exit(void);
int http_send_message(struct profile *profile, char *msg_out, int msg_out_len, char **msg_in);

#endif //__HTTP_H
