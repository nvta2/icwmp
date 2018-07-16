/*
 * stun.c -- the main file of stun application
 *
 *  Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 *  Author: Omar Kallel <omar.kallel@pivasoftware.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 *  02110-1301 USA
 *
 * TR-069 STUN  client software
 * Copyright (C) 2018 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 * Author: Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *         Omar Kallel    <omar.kallel@pivasoftware.com>
 *         Anis Ellouze   <anis.ellouze@pivasoftware.com>
 */

#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <stdlib.h>
#include <libubox/uloop.h>
#include <openssl/hmac.h>
#include <string.h>
#include <ifaddrs.h>

#include "stun.h"
#include "config.h"
#include "ubus.h"

struct env_var env = {0};
int keepalive_timeout = DEFAULT_MINKEEPALIVE;
int retry_timeout = DEFAULT_RETRYTIME;
int bindcrreusaddr = 1;
int bindcrreusport = 1;
const char *BBFCR = "dslforum.org/TR-111 ";

static void tr069_notify_cb(struct uloop_timeout *timeout);
static void tr069_inform_cb(struct uloop_timeout *timeout);
static void listening_crport_cb(struct uloop_timeout *timeout);
static void binding_request_crport_cb(struct uloop_timeout *timeout);


static struct udp_listen listen_crport = {
	.fd = -1,
	.utimer = {.cb = listening_crport_cb}
};
static struct binding_request br_crport = {
	.binding_cr = 1,
	.udp_listen = &listen_crport,
	.utimer = {.cb = binding_request_crport_cb}
};
static struct binding_request br_crport_keepalive = {
	.is_keealive = 1,
	.binding_cr = 1,
	.udp_listen = &listen_crport,
	.utimer = {.cb = binding_request_crport_cb}
};

struct uloop_timeout tr069_notify_timer = {.cb = tr069_notify_cb};
struct uloop_timeout tr069_inform_timer = {.cb = tr069_inform_cb};

void tr069_notify(int afterms)
{
	uloop_timeout_set(&tr069_notify_timer, afterms);
}

void tr069_inform(int afterms)
{
	uloop_timeout_set(&tr069_inform_timer, afterms);
}


static void tr069_notify_cb(struct uloop_timeout *timeout)
{
	stun_log(SINFO, "ubus call tr069 notify");
	if (subus_call("tr069", "notify", 0, UBUS_ARGS{}) < 0) {
		stun_log(SINFO, "ubus call tr069 notify failed! retry after 1s");
		tr069_notify(1000);
	}
}

static void tr069_inform_cb(struct uloop_timeout *timeout)
{
	stun_log(SINFO, "ubus call tr069 inform '{\"event\": \"6 connection request\"}'");
	if (subus_call("tr069", "inform", 1, UBUS_ARGS{{"event", "6 connection request"}}) < 0) {
		stun_log(SINFO, "ubus call tr069 inform '{\"event\": \"6 connection request\"}' failed! retry after 1s");
		tr069_inform(1000);
	}
}

static int stunid_cmp(stunid *left, stunid *right)
{
	return memcmp(left, right, sizeof(*left));
}

static void *stunid_cpy(stunid *left, stunid *right)
{
	return memcpy(left, right, sizeof(*left));
}

static void stunid_rand(stunid *id)
{
	int i;
	srand(time(NULL));
	for (i = 0; i < 4; i++)
	{
		id->id[i] = rand();
	}
}

ssize_t timeout_recvfrom(int sock, char *buf, int length, struct sockaddr_in *connection, int timeoutinseconds)
{
	fd_set socks;
	ssize_t r = 0;
	struct timeval t = {0};
	int clen = sizeof(*connection);

	stun_log(SDEBUG, "udp revcfrom, timeout: %ds", timeoutinseconds);

	FD_ZERO(&socks);
	FD_SET(sock, &socks);
	t.tv_sec = timeoutinseconds;
	if (select(sock + 1, &socks, NULL, NULL, &t) &&
		(r = recvfrom(sock, buf, length, 0, (struct sockaddr *)connection, &clen)) != -1) {
		return r;
	}
	else {
		return -1;
	}
}

static int stun_send(int s, char *buf)
{
	struct stun_header *sh;
	struct hostent *he;
	struct sockaddr_in dst = {0};

	sh = (struct stun_header *)buf;
	if ((he = gethostbyname(conf.server_address)) == NULL) {
		return -1;
	}


	memcpy(&(dst.sin_addr), he->h_addr_list[0], he->h_length);
	dst.sin_port = htons(conf.server_port);
	dst.sin_family = AF_INET;

	stun_log(SINFO, "send STUN message to %s:%u (%s:%u)", conf.server_address, conf.server_port, inet_ntoa(dst.sin_addr), ntohs(dst.sin_port));

	return sendto(s, buf, ntohs(sh->len) + sizeof(*sh), 0, (struct sockaddr *)&dst, sizeof(dst));
}

static int net_socket(int srcport)
{
	int sock = -1;
	stun_log(SINFO, "Open UDP socket");
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock >= 0) {
		if (srcport > 0) {
			struct sockaddr_in bindcraddr = {0};
			int i = 0;
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &bindcrreusaddr, sizeof(int)) < 0) {
				stun_log(SWARNING, "setsockopt(SO_REUSEADDR) failed");
			}
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &bindcrreusport, sizeof(int)) < 0) {
				stun_log(SWARNING, "setsockopt(SO_REUSEPORT) failed");
			}
			bindcraddr.sin_family = AF_INET;
			bindcraddr.sin_addr.s_addr = htonl(INADDR_ANY);
			bindcraddr.sin_port = htons((unsigned short)srcport);
			for(;i<9;i++)
			{
				if (bind(sock, (struct sockaddr *)&bindcraddr, sizeof(bindcraddr)) < 0) {
					continue;
				}
				stun_log(SINFO, "binding socket source port to %u", srcport);
				break;
			}
		}
		fcntl(sock, F_SETFD, fcntl(sock, F_GETFD) | FD_CLOEXEC);
		fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
	}
	return sock;
}

static void stun_close_socket(struct udp_listen *udp_listen)
{
	uloop_timeout_cancel(&udp_listen->utimer);
	if (udp_listen->fd > 0) {
		stun_log(SINFO, "STUN close socket %d", udp_listen->fd);
		close(udp_listen->fd);
	}
	udp_listen->fd = -1;
}

static void stun_socket(struct udp_listen *udp_listen)
{

	if (udp_listen->fd > 0) {
		stun_close_socket(udp_listen);
	}
	udp_listen->fd = net_socket(conf.client_port);
	stun_log(SINFO, "STUN new socket %d", udp_listen->fd);
	uloop_timeout_set(&udp_listen->utimer, 0);
}

static int trailing_buffer_alloc(int mp, char *buf, int len, char trail, char **bufm, int *mlen)
{
	*bufm = NULL;
	*mlen = len % mp;
	*mlen = len + (*mlen ? (mp - *mlen) : 0);
	*bufm = calloc(1, *mlen);
	if (*bufm == NULL)
		return -1;
	memcpy(*bufm, buf, len);
	if (trail) {
		while(len < *mlen) {
			(*bufm)[len++] = trail;
		}
	}
	return 0;
}

static int append_attribute_buffer(unsigned char **start, char *buf, int len, unsigned short attr_type, int free)
{
	if ((sizeof(struct stun_attribute) + len) > free)
		return -1;
	struct stun_attribute *sa = (struct stun_attribute *)*start;
	sa->len = htons((unsigned short)len);
	sa->type = htons(attr_type);
	memcpy(sa->value, buf, len);
	*start += sizeof(struct stun_attribute) + len;
	return 0;
}

static int stun_hmac(int trailing_mp, unsigned char *data, int len, char *password, unsigned char *hmac)
{
	char *bufmp = NULL;
	int lenmp;
	unsigned char* digest;

	if (trailing_mp) {
		if (trailing_buffer_alloc(trailing_mp, data, len, 0, &bufmp, &lenmp))
			return -1;
		digest = HMAC(EVP_sha1(), password, strlen(password), bufmp, lenmp, NULL, NULL);
		free(bufmp);
	}
	else {
		digest = HMAC(EVP_sha1(), password, strlen(password), data, len, NULL, NULL);
	}

	memcpy(hmac, digest, 20);
	return 0;
}

static void hex_to_str(char *hex, int len, char *str)
{
	while (len--) {
		sprintf(str, "%02X", *hex++);
		str += 2;
	}
}

static int generate_stun_packet(struct binding_request *br, char *req_buf, int maxlen)
{
	struct stun_header *req;
	unsigned char *stunmsg;

	req = (struct stun_header *) req_buf;
	stunmsg = req->stunmsg;

	stun_log(SINFO, "STUN generate BINDING-REQUEST");
	req->type = htons(BINDING_REQUSET);
	stunid_rand(&(req->id));
	stunid_cpy(&(br->id), &(req->id));
	stun_log(SINFO, "STUN request id: %d%d%d%d", req->id.id[0], req->id.id[1], req->id.id[2], req->id.id[3]);
	if (conf.username) {
		char *buf4;
		int len4;
		if (trailing_buffer_alloc(4, conf.username, strlen(conf.username), ' ', &buf4, &len4))
			return -1;
		append_attribute_buffer(&stunmsg, buf4, len4, ATTR_USERNAME, (maxlen - (stunmsg - (unsigned char *)req)));
		stun_log(SINFO, "STUN append USERNAME: **%.*s**", len4, buf4);
		free(buf4);
	}
	if (br->binding_cr) {
		stun_log(SINFO, "STUN append CONNECTION-REQUEST-BINDING: **%s**", BBFCR);
		append_attribute_buffer(&stunmsg, (char *)BBFCR, strlen(BBFCR), ATTR_CONNECTION_REQUEST_BINDING, (maxlen - (stunmsg - (unsigned char *)req)));
	}
	if (br->binding_change) {
		stun_log(SINFO, "STUN append BINDING-CHANGE");
		append_attribute_buffer(&stunmsg, "", 0, ATTR_BINDING_CHANGE, (maxlen - (stunmsg - (unsigned char *)req)));
	}
	if (br->msg_integrity) {
		if (conf.username) {
			char *password;
			unsigned char hmac[20] = {0};
			char hmacstr[64];
			req->len = htons((stunmsg - (unsigned char *)req) - sizeof(struct stun_header) + sizeof(struct stun_attribute) + 20);
			password = conf.password ? conf.password : "";
			stun_hmac(64, (unsigned char *)req, stunmsg - (unsigned char *)req, password, hmac);
			append_attribute_buffer(&stunmsg, hmac, sizeof(hmac), ATTR_MESSAGE_INTEGRITY, (maxlen - (stunmsg - (unsigned char *)req)));
			hex_to_str(hmac, 20, hmacstr);
			stun_log(SINFO, "STUN append MESSAGE-INTEGRITY: ***%s***", hmacstr);
		}
		else {
			req->len = htons((stunmsg - (unsigned char *)req) - sizeof(struct stun_header));
			br->msg_integrity = 0;
			return -1;
		}
	}
	else {
		req->len = htons((stunmsg - (unsigned char *)req) - sizeof(struct stun_header));
	}
	stun_log(SINFO, "STUN request length: %d", ntohs(req->len));
	return 0;
}

static int stun_get_mapped_address(char *buf, unsigned int *ip, unsigned short *port)
{
	struct stun_header *sh = (struct stun_header *)buf;
	struct stun_attribute *sa = (struct stun_attribute *)sh->stunmsg;
	char *p;
	while (((char *)sa - (char *)sh - sizeof(*sh)) < ntohs(sh->len)) {
		if(ntohs(sa->type) == ATTR_MAPPED_ADDRESS) {
			struct stun_address *ma = (struct stun_address *)sa->value;
			*port = ma->port;
			*ip = ma->address;
			return 0;
		}
		p = (char *)sa;
		p += sizeof(struct stun_attribute) + ntohs(sa->len);
		sa = (struct stun_attribute *)p;
	}
	return -1;
}

static int stun_get_error_code(char *buf)
{
	struct stun_header *sh = (struct stun_header *)buf;
	struct stun_attribute *sa = (struct stun_attribute *)sh->stunmsg;
	char *p;
	unsigned char class, number;
	while (((char *)sa - (char *)sh - sizeof(*sh)) < ntohs(sh->len)) {
		if(ntohs(sa->type) == ATTR_ERROR_CODE) {
			unsigned int class, number;
			unsigned int ui = ntohl(*((unsigned int *)sa->value));
			class = (ui >> 8) & 0x7;
			number = ui & 0xff;
			return (int)(class * 100 + number);
		}
		p = (char *)sa;
		p += sizeof(struct stun_attribute) + ntohs(sa->len);
		sa = (struct stun_attribute *)p;
	}
	return 0;
}

static void handle_udp_cr(char *resp_buf)
{
	char *str;
	char un[64], cn[64], sig[64], buf[256];
	char *crusername;
	char *crpassword;
	unsigned int crid = 0, ts = 0;
	int valid = 1;

	stun_log(SINFO, "Handle UDP Connection Request");

	if ((str = strstr(resp_buf, "ts="))) {
		sscanf(str, "ts=%u", &ts);
		stun_log(SINFO, "UDP CR ts = %u", ts);
	}
	else {
		stun_log(SWARNING, "UDP CR ts not found");
		return;
	}
	if ((str = strstr(resp_buf, "id="))) {
		sscanf(str, "id=%u", &crid);
		stun_log(SINFO, "UDP CR id = %u", crid);
	}
	else {
		return;
		stun_log(SWARNING, "UDP CR id not found");
	}
	if (crid && ts && crid != env.last_crid && ts > env.last_ts) {
		stun_log(SINFO, "NEW UDP CR");
		env.last_crid = crid;
		env.last_ts = ts;
		if ((str = strstr(resp_buf, "un="))) {
			sscanf(str, "un=%[^?& \t\n\r]", un);
			stun_log(SINFO, "UDP CR un = %s", un);
		}
		else {
			stun_log(SWARNING, "UDP CR un not found");
			return;
		}
		if ((str = strstr(resp_buf, "cn="))) {
			sscanf(str, "cn=%[^?& \t\n\r]", cn);
			stun_log(SINFO, "UDP CR cn = %s", cn);
		}
		else {
			stun_log(SWARNING, "UDP CR cn not found");
			return;
		}
		if ((str = strstr(resp_buf, "sig="))) {
			sscanf(str, "sig=%[^?& \t\n\r]",sig);
			stun_log(SINFO, "UDP CR sig = %s", sig);
		}
		else {
			stun_log(SWARNING, "UDP CR sig not found");
			return;
		}
		suci_init();
		crusername = suci_get_value("easycwmp", "local", "username");
		crpassword = suci_get_value("easycwmp", "local", "password");
		if (*crusername && *crpassword) {
			if (strcmp(crusername, un) != 0) {
				stun_log(SINFO, "UDP CR username mismatch!");
				valid = 0;
			}
			else {
				char hmac[20], hmacstr[64];
				sprintf(buf, "%u%u%s%s", ts, crid, un, cn);
				stun_hmac(0, (unsigned char *)buf, strlen(buf), crpassword, hmac);
				hex_to_str(hmac, 20, hmacstr);
				if (strcasecmp(hmacstr, sig) != 0) {
					stun_log(SINFO, "UDP CR sig mismatch!");
					valid = 0;
				}
			}
		}
		suci_fini();
		if (valid) {
			tr069_inform(0);
		}
	} else {
		if (!ts || !crid)
			stun_log(SINFO, "UDP CR ts or id not found");
		else
			stun_log(SINFO, "UDP CR ts is old or id is already received");
	}
}

static void save_udpcr_var_state(unsigned int ip, unsigned short port)
{
	struct in_addr ip_addr;
	char buf[64];

	ip_addr.s_addr = env.address;
	sprintf(buf, "%s:%d", inet_ntoa(ip_addr), ntohs(env.port));
	stun_log(SINFO, "Save New UDPConnectionRequestAddress to /var/state %s", buf);
	suci_init();
	suci_set_value_state("icwmp_stun", "stun", "crudp_address", buf);
	suci_fini();
}

static int is_udpcr_changed(unsigned int ip, unsigned short port)
{
	struct in_addr ip_addr;
	char buf[64];
	char *v;
	int changed = 0;

	ip_addr.s_addr = ip;
	sprintf(buf, "%s:%d", inet_ntoa(ip_addr), ntohs(port));
	suci_init();
	v = suci_get_value_state("icwmp_stun", "stun", "crudp_address");
	if (strcmp(buf, v) != 0)
		changed = 1;
	suci_fini();
	return changed;
}

static void save_natdetected_var_state(unsigned int ip)
{
	struct ifaddrs * ifaddrlist = NULL;
	struct ifaddrs * ifa = NULL;
	int islocal = 0;
	char *nd;

	getifaddrs(&ifaddrlist);

	for (ifa = ifaddrlist; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa ->ifa_addr && ifa ->ifa_addr->sa_family == AF_INET) {
			if (((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr == ip) {
				islocal = 1;
				break;
			}
		}
	}

	if (ifaddrlist != NULL)
		freeifaddrs(ifaddrlist);

	suci_init();
	nd = suci_get_value_state("icwmp_stun", "stun", "nat_detected");

	if (islocal && *nd != '\0') {
		stun_log(SINFO, "Device is not behind NAT, set NATDetected to false");
		suci_set_value_state("icwmp_stun", "stun", "nat_detected", "");
	}
	else if (!islocal && *nd == '\0') {
		stun_log(SINFO, "Device is behind NAT, set NATDetected to true");
		suci_set_value_state("icwmp_stun", "stun", "nat_detected", "1");
	}
	suci_fini();
}

static void binding_request_crport_cb(struct uloop_timeout *timeout)
{
	struct binding_request *br;
	struct udp_listen *udp_listen;
	unsigned char req_buf[2048] = {0};
	int r;


	br = binding_request_entry(timeout);
	udp_listen = br->udp_listen;

	udp_listen->br = br;

	stun_log(SINFO, "Binding Request cb start %s", br->is_keealive ? "(KeepAlive)" : "");

	if (udp_listen->fd <= 0) {
		stun_socket(udp_listen);
	}

	if (br->resp_success > 0)
		br->resp_success = 0;

	if (generate_stun_packet(br, req_buf, sizeof(req_buf) - 1)) {
		br->retry_interval = (br->retry_interval) ? 2 * br->retry_interval : retry_timeout;
		br->retry_interval = (br->retry_interval > 1500) ? 1500 : br->retry_interval;
		uloop_timeout_set(&br->utimer, br->retry_interval * 1000);
		return;
	}
	r = stun_send(udp_listen->fd, req_buf);
	if (r < 0) {
		stun_close_socket(udp_listen);
		udp_listen->br = NULL;
		br->retry_interval = (br->retry_interval) ? 2 * br->retry_interval : retry_timeout;
		br->retry_interval = (br->retry_interval > 1500) ? 1500 : br->retry_interval;
		stun_log(SINFO, "Failed send of Binding Request! retry in %ds", br->retry_interval);
		uloop_timeout_set(timeout, br->retry_interval * 1000);
	}
	else {
		br->retry_interval = 0;
		stun_log(SINFO, "Success send of Binding Request.");
		stun_log(SINFO, "Start KeepAlive Binding Request in %ds.", keepalive_timeout);
		uloop_timeout_set(&br_crport_keepalive.utimer, keepalive_timeout * 1000);
	}
}

static void listening_crport_cb(struct uloop_timeout *timeout)
{
	struct udp_listen *udp_listen;
	struct sockaddr_in src = {0};
	unsigned char resp_buf[2048] = {0};
	unsigned int ip = 0;
	unsigned short port = 0;
	int r;

	stun_log(SDEBUG, "Binding listening CR cb start");
	udp_listen = udp_listen_entry(timeout);
	if (udp_listen->fd < 0) {
		stun_log(SINFO, "Binding listening CR: Socket = -1");
		uloop_timeout_set(timeout, 19);
		return;
	}
	r = timeout_recvfrom(udp_listen->fd, resp_buf, sizeof(resp_buf) - 1, &src, 1);
	if (r > 0) {
		stun_log(SINFO, "Binding listening CR: get UDP packet");
		struct stun_header *sh = (struct stun_header *)resp_buf;
		if (ntohs(sh->type) == BINDING_ERROR) {
			int code = stun_get_error_code(resp_buf);
			stun_log(SINFO, "get BINDING-ERROR: code is %d", code);
			if (udp_listen->br != NULL && stunid_cmp(&(sh->id), &(udp_listen->br->id)) == 0 && code == 401) {
				udp_listen->br->msg_integrity = 1;
				udp_listen->br->auth_fail++;
				stun_log(SINFO, "Cancel scheduled Keepalive Binding Request");
				uloop_timeout_cancel(&br_crport_keepalive.utimer);
				stun_log(SINFO, "Trying new Binding Request in %ds",
						(udp_listen->br->auth_fail < 3) ? 0 : (udp_listen->br->auth_fail - 2)*3);
				uloop_timeout_set(&udp_listen->br->utimer,
								(udp_listen->br->auth_fail < 3) ? 0 : ((udp_listen->br->auth_fail - 2) * 3000));
				udp_listen->br = NULL;
			}
			else if (code != 401) {
				stun_log(SINFO, "Unsupported error code");
			}
			goto end;
		}
		else if (ntohs(sh->type) == BINDING_RESPONSE) {
			struct in_addr ip_addr;
			stun_log(SINFO, "get BINDING-RESPONSE");
			if (udp_listen->br != NULL && stunid_cmp(&(sh->id), &(udp_listen->br->id)) == 0) {
				udp_listen->br->resp_success = 1;
				udp_listen->br->msg_integrity = 0;
				udp_listen->br->auth_fail = 0;
				stun_get_mapped_address(resp_buf, &ip, &port);
				ip_addr.s_addr = ip;
				stun_log(SINFO, "Mapped Address is: %s:%u", inet_ntoa(ip_addr), ntohs(port));
				save_natdetected_var_state(ip);
				if (is_udpcr_changed(ip, port)) {
					env.address = ip;
					env.port = port;
					udp_listen->br->resp_success = 0;
					udp_listen->br->binding_change = 1;
					uloop_timeout_set(&udp_listen->br->utimer, 0);
					save_udpcr_var_state(ip, port);
					tr069_notify(0);
				}
				else {
					udp_listen->br = NULL;
				}
			}
			goto end;
		}
		else if (strstr(resp_buf, "http") || strstr(resp_buf, "HTTP")) {
			stun_log(SINFO, "get UDP Connection Request");
			handle_udp_cr(resp_buf);
		}
		else {
			stun_log(SINFO, "get non supported STUN/UDP message");
		}
	}
	else {
		/* timed out */
		if (!udp_listen->br || udp_listen->br->resp_success == 1)
			goto end;
		stun_log(SINFO, "Timed OUT!");
		udp_listen->br->resp_success--;
		if (udp_listen->br->resp_success < -2) {
			int rs = -udp_listen->br->resp_success;
			if ((rs % 9) == 0) {
				stun_log(SINFO, "Retry sending in a new socket");
				stun_close_socket(udp_listen);
				uloop_timeout_set(&udp_listen->br->utimer, 0);
				udp_listen->br = NULL;
			}
			else if ((rs % 3) == 0) {
				stun_log(SINFO, "Retry sending.");
				uloop_timeout_set(&udp_listen->br->utimer, 0);
			}
		}
	}
end:
	uloop_timeout_set(timeout, 1);
}

int main()
{
	stun_log(SINFO, "Start icwmp_stund daemon");
	config_init();
	keepalive_timeout = conf.min_keepalive;

	uloop_init();
	uloop_timeout_set(&br_crport.utimer, 100);
	uloop_run();
	uloop_done();
	config_fini();
	stun_log(SINFO, "Stop icwmp_stund daemon");
	return 0;
}
