/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *	  Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#include <curl/curl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "log.h"
#include "config.h"
#include "event.h"
#include "http.h"
#include "digestauth.h"
#include "cwmp_uci.h"
#include "ubus.h"

#define REALM "authenticate@cwmp"
#define OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

static struct http_client http_c;

static CURL *curl = NULL;

#ifdef DUMMY_MODE
char *fc_cookies = "./ext/tmp/icwmp_cookies";
#else
char *fc_cookies = "/tmp/icwmp_cookies";
#endif

void http_set_timeout(void)
{
	if (curl)
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
}

int http_client_init(struct cwmp *cwmp)
{
	char *dhcp_dis = NULL;
	char *acs_var_stat = NULL;
	unsigned char buf[sizeof(struct in6_addr)];
	uci_get_value(UCI_DHCP_DISCOVERY_PATH, &dhcp_dis);

	if (dhcp_dis && cwmp->retry_count_session > 0 && strcmp(dhcp_dis, "enable") == 0) {
		uci_get_state_value(UCI_DHCP_ACS_URL, &acs_var_stat);
		if (acs_var_stat) {
			if (icwmp_asprintf(&http_c.url, "%s", acs_var_stat) == -1) {
				free(acs_var_stat);
				FREE(dhcp_dis);
				return -1;
			}
		} else {
			if (icwmp_asprintf(&http_c.url, "%s", cwmp->conf.acsurl) == -1) {
				FREE(dhcp_dis);
				return -1;
			}
		}
	} else {
		if (icwmp_asprintf(&http_c.url, "%s", cwmp->conf.acsurl) == -1) {
			FREE(dhcp_dis);
			return -1;
		}
	}

	if (dhcp_dis)
		free(dhcp_dis);

	CWMP_LOG(INFO, "ACS url: %s", http_c.url);

	/* TODO debug ssl config from freecwmp*/

	curl_global_init(CURL_GLOBAL_SSL);
	curl = curl_easy_init();
	if (!curl)
		return -1;

	if (cwmp->conf.ipv6_enable) {
		char *ip = NULL;
		curl_easy_setopt(curl, CURLOPT_URL, http_c.url);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
		curl_easy_perform(curl);
		int tmp = inet_pton(AF_INET, ip, buf);

		cwmp_uci_set_value("cwmp", "acs", "ip_version", (tmp == 1) ? "4" : "6");
		cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	}
	return 0;
}

void http_client_exit(void)
{
	icwmp_free(http_c.url);

	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}
	if (file_exists(fc_cookies))
		remove(fc_cookies);
	if (curl) {
		curl_easy_cleanup(curl);
		curl = NULL;
	}
	curl_global_cleanup();
}

static size_t http_get_response(void *buffer, size_t size, size_t rxed, char **msg_in)
{
	char *c;

	if (cwmp_asprintf(&c, "%s%.*s", *msg_in, (int)(size * rxed), (char *)buffer) == -1) {
		FREE(*msg_in);
		return -1;
	}

	free(*msg_in);
	*msg_in = c;

	return size * rxed;
}

int http_send_message(struct cwmp *cwmp, char *msg_out, int msg_out_len, char **msg_in)
{
	unsigned char buf[sizeof(struct in6_addr)];
	int tmp = 0;
	CURLcode res;
	long http_code = 0;
	static char ip_acs[128] = { 0 };
	char *ip = NULL;
	char errbuf[CURL_ERROR_SIZE];

	http_c.header_list = NULL;
	http_c.header_list = curl_slist_append(http_c.header_list, "User-Agent: iopsys-cwmp");
	if (!http_c.header_list)
		return -1;
	http_c.header_list = curl_slist_append(http_c.header_list, "Content-Type: text/xml");
	if (!http_c.header_list)
		return -1;
#ifdef ACS_FUSION
	char *expect_header = "Expect:";
	http_c.header_list = curl_slist_append(http_c.header_list, expect_header);
	if (!http_c.header_list)
		return -1;
#endif /* ACS_FUSION */
	if (cwmp->conf.http_disable_100continue) {
		char *expect_header = "Expect:";
		http_c.header_list = curl_slist_append(http_c.header_list, expect_header);
		if (!http_c.header_list)
			return -1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, http_c.url);
	curl_easy_setopt(curl, CURLOPT_USERNAME, cwmp->conf.acs_userid);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, cwmp->conf.acs_passwd);
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC | CURLAUTH_DIGEST);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
	switch (cwmp->conf.compression) {
	case COMP_NONE:
		break;
	case COMP_GZIP:
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Encoding: gzip");
		break;
	case COMP_DEFLATE:
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
		http_c.header_list = curl_slist_append(http_c.header_list, "Content-Encoding: deflate");
		break;
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_c.header_list);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg_out);
	if (msg_out)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)msg_out_len);
	else
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, msg_in);

#ifdef DEVEL
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, fc_cookies);
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, fc_cookies);

	if (cwmp->conf.acs_ssl_capath)
		curl_easy_setopt(curl, CURLOPT_CAPATH, cwmp->conf.acs_ssl_capath);
	if (cwmp->conf.insecure_enable) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	}

	curl_easy_setopt(curl, CURLOPT_INTERFACE, cwmp->conf.interface);
	*msg_in = (char *)calloc(1, sizeof(char));

	res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		size_t len = strlen(errbuf);
		if (len) {
			if (errbuf[len - 1] == '\n')
				errbuf[len - 1] = '\0';
			CWMP_LOG(ERROR, "libcurl: (%d) %s", res, errbuf);
		} else {
			CWMP_LOG(ERROR, "libcurl: (%d) %s", res, curl_easy_strerror(res));
		}
	}

	if (!strlen(*msg_in))
		FREE(*msg_in);

	curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
	if (ip && ip[0] != '\0') {
		if (ip_acs[0] == '\0' || strcmp(ip_acs, ip) != 0) {
			CWMP_STRNCPY(ip_acs, ip, sizeof(ip_acs));
			tmp = inet_pton(AF_INET, ip, buf);
			if (tmp == 1)
				tmp = 0;
			else
				tmp = inet_pton(AF_INET6, ip, buf);
			cwmp_uci_add_section_with_specific_name("cwmp", "acs", "acs", UCI_VARSTATE_CONFIG);
			char *zone_name = NULL;
			get_firewall_zone_name_by_wan_iface(cwmp->conf.default_wan_iface, &zone_name);
			cwmp_uci_set_varstate_value("cwmp", "acs", tmp ? "ip6" : "ip", ip_acs);
			char connection_requset_port_str[10];
			snprintf(connection_requset_port_str, sizeof(connection_requset_port_str), "%d", cwmp->conf.connection_request_port);
			cwmp_uci_set_varstate_value("cwmp", "acs", "port", connection_requset_port_str);
			cwmp_uci_set_varstate_value("cwmp", "acs", "zonename", zone_name ? zone_name : "wan");
			cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);

			/*
			 * Restart firewall service
			 */
			cwmp_ubus_call("uci", "commit", CWMP_UBUS_ARGS{ { "config", { .str_val = "firewall" }, UBUS_String } }, 1, NULL, NULL);
		}
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code == 204) {
		CWMP_LOG(INFO, "Receive HTTP 204 No Content");
	}

	if (http_code == 415) {
		cwmp->conf.compression = COMP_NONE;
		goto error;
	}
	if (http_code != 200 && http_code != 204)
		goto error;

	/* TODO add check for 301, 302 and 307 HTTP Redirect*/

	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}

	if (res)
		goto error;

	return 0;

error:
	FREE(*msg_in);
	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}
	return -1;
}

void http_success_cr(void)
{
	CWMP_LOG(INFO, "Connection Request thread: add connection request event in the queue");
	pthread_mutex_lock(&(cwmp_main.mutex_session_queue));
	cwmp_add_event_container(&cwmp_main, EVENT_IDX_6CONNECTION_REQUEST, "");
	pthread_mutex_unlock(&(cwmp_main.mutex_session_queue));
	pthread_cond_signal(&(cwmp_main.threshold_session_send));
}

static void http_cr_new_client(int client, bool service_available)
{
	FILE *fp;
	char buffer[BUFSIZ];
	char auth_digest_buffer[BUFSIZ];
	int8_t auth_status = 0;
	bool auth_digest_checked = false;
	bool method_is_get = false;
	char cr_http_get_head[512];

	pthread_mutex_lock(&mutex_config_load);
	fp = fdopen(client, "r+");
	char *username = cwmp_main.conf.cpe_userid;
	char *password = cwmp_main.conf.cpe_passwd;

	if (!username || !password) {
		// if we dont have username or password configured proceed with connecting to ACS
		service_available = false;
		goto http_end;
	}
	snprintf(cr_http_get_head, sizeof(cr_http_get_head), "GET %s HTTP/1.1", cwmp_main.conf.connection_request_path);
	while (fgets(buffer, sizeof(buffer), fp)) {
		if (!strncasecmp(buffer, cr_http_get_head, strlen(cr_http_get_head)))
			method_is_get = true;
		if (!strncasecmp(buffer, "Authorization: Digest ", strlen("Authorization: Digest "))) {
			auth_digest_checked = true;
			CWMP_STRNCPY(auth_digest_buffer, buffer, BUFSIZ);
		}

		if (buffer[0] == '\r' || buffer[0] == '\n') {
			/* end of http request (empty line) */
			break;
		}
	}
	if (!service_available || !method_is_get) {
		goto http_end;
	}
	if (auth_digest_checked && http_digest_auth_check("GET", "/", auth_digest_buffer + strlen("Authorization: Digest "), REALM, username, password, 300) == MHD_YES)
		auth_status = 1;
	else
		auth_status = 0;
http_end:
	if (!service_available || !method_is_get) {
		CWMP_LOG(INFO, "Receive Connection Request: Return 503 Service Unavailable");
		fputs("HTTP/1.1 503 Service Unavailable\r\n", fp);
		fputs("Connection: close\r\n", fp);
		fputs("Content-Length: 0\r\n", fp);
	} else if (auth_status) {
		CWMP_LOG(INFO, "Receive Connection Request: success authentication");
		fputs("HTTP/1.1 200 OK\r\n", fp);
		fputs("Connection: close\r\n", fp);
		fputs("Content-Length: 0\r\n", fp);
		http_success_cr();
	} else {
		CWMP_LOG(INFO, "Receive Connection Request: Return 401 Unauthorized");
		fputs("HTTP/1.1 401 Unauthorized\r\n", fp);
		fputs("Connection: close\r\n", fp);
		http_digest_auth_fail_response(fp, "GET", "/", REALM, OPAQUE);
		fputs("\r\n", fp);
	}
	fputs("\r\n", fp);

	fclose(fp);
	pthread_mutex_unlock(&mutex_config_load);
}

void http_server_init(void)
{
	struct sockaddr_in6 server = { 0 };
	unsigned short cr_port;

	for (;;) {
		cr_port = (unsigned short)(cwmp_main.conf.connection_request_port);
		unsigned short i = (DEFAULT_CONNECTION_REQUEST_PORT == cr_port) ? 1 : 0;
		//Create socket
		cwmp_main.cr_socket_desc = socket(AF_INET6, SOCK_STREAM, 0);
		if (cwmp_main.cr_socket_desc == -1) {
			CWMP_LOG(ERROR, "Could not open server socket for Connection Requests, Error no is : %d, Error description is : %s", errno, strerror(errno));
			sleep(1);
			continue;
		}

		fcntl(cwmp_main.cr_socket_desc, F_SETFD, fcntl(cwmp_main.cr_socket_desc, F_GETFD) | FD_CLOEXEC);

		int reusaddr = 1;
		if (setsockopt(cwmp_main.cr_socket_desc, SOL_SOCKET, SO_REUSEADDR, &reusaddr, sizeof(int)) < 0) {
			CWMP_LOG(WARNING, "setsockopt(SO_REUSEADDR) failed");
		}

		//Prepare the sockaddr_in structure
		server.sin6_family = AF_INET6;
		server.sin6_addr = in6addr_any;

		for (;; i++) {
			server.sin6_port = htons(cr_port);
			//Bind
			if (bind(cwmp_main.cr_socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
				//print the error message
				CWMP_LOG(ERROR, "Could not bind server socket on the port %d, Error no is : %d, Error description is : %s", cr_port, errno, strerror(errno));
				cr_port = DEFAULT_CONNECTION_REQUEST_PORT + i;
				CWMP_LOG(INFO, "Trying to use another connection request port: %d", cr_port);
				continue;
			}
			break;
		}
		break;
	}
	char cr_port_str[6];
	snprintf(cr_port_str, 6, "%hu", cr_port);
	cr_port_str[5] = '\0';
	cwmp_uci_set_value("cwmp", "cpe", "port", cr_port_str);
	cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
	connection_request_port_value_change(&cwmp_main, cr_port);
	CWMP_LOG(INFO, "Connection Request server initiated with the port: %d", cr_port);
}

void http_server_listen(void)
{
	int client_sock, c;
	static int cr_request = 0;
	static time_t restrict_start_time = 0;
	time_t current_time;
	bool service_available;
	struct sockaddr_in6 client;

	//Listen
	listen(cwmp_main.cr_socket_desc, 3);

	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);
	while ((client_sock = accept(cwmp_main.cr_socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
		
		if (thread_end)
			return;

		current_time = time(NULL);
		service_available = true;
		if ((restrict_start_time == 0) || ((current_time - restrict_start_time) > CONNECTION_REQUEST_RESTRICT_PERIOD)) {
			restrict_start_time = current_time;
			cr_request = 1;
		} else {
			cr_request++;
			if (cr_request > CONNECTION_REQUEST_RESTRICT_REQUEST) {
				restrict_start_time = current_time;
				service_available = false;
			}
		}
		http_cr_new_client(client_sock, service_available);
		close(client_sock);
	}

	if (client_sock < 0) {
		CWMP_LOG(ERROR, "Could not accept connections for Connection Requests!");
		return;
	}
}
