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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <curl/curl.h>

#include "http.h"
#include "log.h"
#include "times.h"
#include "common.h"

static struct http_client http_c;
static CURL *curl;

int http_client_init(struct profile *profile)
{
	char *url = create_request_url(profile);
	if(url) {
		asprintf(&http_c.url, "%s", url);
		free(url);
	}
	bulkdata_log(SINFO, "ACS url: %s", http_c.url);

	curl_global_init(CURL_GLOBAL_SSL);
	curl = curl_easy_init();
	if (!curl) return -1;

	return 0;
}

void http_client_exit(void)
{
	FREE(http_c.url);

	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

}

static size_t http_get_response(void *buffer, size_t size, size_t rxed, char **msg_in)
{
	char *c;

	if (asprintf(&c, "%s%.*s", *msg_in, size * rxed, (char *)buffer) == -1) {
		FREE(*msg_in);
		return -1;
	}

	free(*msg_in);
	*msg_in = c;

	return size * rxed;
}

int http_send_message(struct profile *profile, char *msg_out, int msg_out_len, char **msg_in)
{
	CURLcode res;
	long http_code = 0;
	char errbuf[CURL_ERROR_SIZE];

	http_c.header_list = NULL;
	http_c.header_list = curl_slist_append(http_c.header_list, "User-Agent: iopsys-bulkdata");
	if (!http_c.header_list) return -1;

	if (profile->http_use_date_header) {
		if(bulkdata_get_time() != NULL) {
			http_c.header_list = curl_slist_append(http_c.header_list, bulkdata_get_time());
			if (!http_c.header_list) return -1;
		}
	}

	if (strcmp(profile->encoding_type, "json") == 0) {
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Type: application/json; charset=\"utf-8\"");
		if (!http_c.header_list) return -1;

		if(strcmp (profile->json_encoding_report_format, "objecthierarchy") == 0) {
			http_c.header_list = curl_slist_append(http_c.header_list, "BBF-Report-Format: \"ObjectHierarchy\"");
			if (!http_c.header_list) return -1;
		} else if(strcmp(profile->json_encoding_report_format, "namevaluepair") == 0) {
			http_c.header_list = curl_slist_append(http_c.header_list, "BBF-Report-Format: \"NameValuePair\"");
			if (!http_c.header_list) return -1;
		}
	} else if(strcmp(profile->encoding_type, "csv") == 0) {
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Type: text/csv; charset=\"utf-8\"");
		if (!http_c.header_list) return -1;

		if(strcmp (profile->csv_encoding_report_format, "row") == 0) {
			http_c.header_list = curl_slist_append(http_c.header_list, "BBF-Report-Format: \"ParameterPerRow\"");
			if (!http_c.header_list) return -1;
		} else if(strcmp (profile->csv_encoding_report_format, "column") == 0) {
			http_c.header_list = curl_slist_append(http_c.header_list, "BBF-Report-Format: \"ParameterPerColumn\"");
			if (!http_c.header_list) return -1;
		}
	}

	curl_easy_setopt(curl, CURLOPT_URL, http_c.url);
	curl_easy_setopt(curl, CURLOPT_USERNAME, profile->http_username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, profile->http_password);
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC|CURLAUTH_DIGEST);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 0);

	if(strcasecmp(profile->http_compression, "gzip") == 0) {
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Encoding: gzip");
	} else if(strcasecmp(profile->http_compression, "compress") == 0) {
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "compress");
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Encoding: compress");
	} else if(strcasecmp(profile->http_compression, "deflate") == 0) {
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Encoding: deflate");	
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_c.header_list);
	if(strcasecmp(profile->http_method, "put") == 0)
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg_out);
	if (msg_out)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) msg_out_len);
	else
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, msg_in);

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
	
	if (profile->http_ssl_capath)
		curl_easy_setopt(curl, CURLOPT_CAPATH, profile->http_ssl_capath);
	if (profile->http_insecure_enable) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);		
	}
	*msg_in = (char *) calloc (1, sizeof(char));

	res = curl_easy_perform(curl);

	if(res != CURLE_OK) {
		size_t len = strlen(errbuf);
		if(len) {
			if (errbuf[len - 1] == '\n') errbuf[len - 1] = '\0';
			bulkdata_log(SCRIT, "libcurl: (%d) %s", res, errbuf);
		} else {
			bulkdata_log(SCRIT, "libcurl: (%d) %s", res, curl_easy_strerror(res));
		}
	}

	if (!strlen(*msg_in))
		FREE(*msg_in);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	if(http_code == 200)
		bulkdata_log(SINFO, "Receive HTTP 200 OK from Bulkdata Collector");
	else if(http_code == 401)
		bulkdata_log(SINFO, "Receive HTTP 401 Unauthorized from Bulkdata Collector");
	else if(http_code == 204)
		bulkdata_log(SINFO, "Receive HTTP 204 No Content from Bulkdata Collector");
	else
		bulkdata_log(SINFO, "Receive HTTP %d from Bulkdata Collector", http_code);

	if(http_code == 415)
	{
		strcpy(profile->http_compression, "None");
		goto error;
	}
	if (http_code != 200 && http_code != 204)
		goto error;

	curl_easy_reset_no_auth(curl);
	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}

	if (res) goto error;

	return http_code;

error:
	FREE(*msg_in);
	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}
	return -1;
}
