/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 *
 */

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>
#include <limits.h>
#include <locale.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <libubox/uloop.h>
#include <libubox/usock.h>
#include "http.h"
#include "log.h"
#include "config.h"
#include "event.h"
#include "cwmp_uci.h"

static int itfcmp(char *itf1, char *itf2);
static void netlink_new_msg(struct uloop_fd *ufd, unsigned events);
static struct uloop_fd netlink_event = {.cb = netlink_new_msg };
static struct uloop_fd netlink_event_v6 = {.cb = netlink_new_msg };

static int itfcmp(char *itf1, char *itf2)
{
	int index = 0;
	int status = 1;
	char *str = NULL;
	char *buf1 = NULL;
	char *buf2 = NULL;

	if (itf1[0] == '\0')
		goto end;
	str = strchr(itf1, '.');
	if (str == NULL)
		goto end;
	index = (int)(str - itf1);
	if (!index)
		goto end;

	buf1 = malloc(index);
	if (!buf1)
		goto end;

	strncpy(buf1, itf1, index);
	buf1[index] = '\0';
	if (itf2[0] == '\0')
		goto end;

	str = strchr(itf2, '.');
	if (str == NULL)
		goto end;

	index = (int)(str - itf2);
	if (!index)
		goto end;

	buf2 = malloc(index);
	if (!buf2)
		goto end;

	strncpy(buf2, itf1, index);
	buf2[index] = '\0';

	if (strcmp(buf1, buf2) == 0)
		status = 0;
end:
	if (buf1)
		free(buf1);
	if (buf2)
		free(buf2);
	return status;
}

static void freecwmp_netlink_interface(struct nlmsghdr *nlh)
{
	struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA(nlh);
	struct rtattr *rth = IFA_RTA(ifa);
	int rtl = IFA_PAYLOAD(nlh);
	char if_name[IFNAMSIZ], if_addr[INET_ADDRSTRLEN];

	memset(&if_name, 0, sizeof(if_name));
	memset(&if_addr, 0, sizeof(if_addr));

	if (ifa->ifa_family == AF_INET) { //CASE IPv4
		while (rtl && RTA_OK(rth, rtl)) {
			if (rth->rta_type != IFA_LOCAL) {
				rth = RTA_NEXT(rth, rtl);
				continue;
			}

			uint32_t addr = htonl(*(uint32_t *)RTA_DATA(rth));
			if (htonl(13) == 13) {
				// running on big endian system
			} else {
				// running on little endian system
				addr = __builtin_bswap32(addr);
			}

			if_indextoname(ifa->ifa_index, if_name);
			if (itfcmp(cwmp_main->conf.interface, if_name)) {
				rth = RTA_NEXT(rth, rtl);
				continue;
			}

			inet_ntop(AF_INET, &(addr), if_addr, INET_ADDRSTRLEN);

			FREE(cwmp_main->conf.ip);
			cwmp_main->conf.ip = strdup(if_addr);
			cwmp_uci_set_varstate_value("cwmp", "cpe", "ip", cwmp_main->conf.ip);
			cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
			connection_request_ip_value_change(IPv4);
			break;
		}
	} else { //CASE IPv6
		while (rtl && RTA_OK(rth, rtl)) {
			char pradd_v6[128];
			if (rth->rta_type != IFA_ADDRESS || ifa->ifa_scope == RT_SCOPE_LINK) {
				rth = RTA_NEXT(rth, rtl);
				continue;
			}
			inet_ntop(AF_INET6, RTA_DATA(rth), pradd_v6, sizeof(pradd_v6));
			if_indextoname(ifa->ifa_index, if_name);
			if (strncmp(cwmp_main->conf.interface, if_name, IFNAMSIZ)) {
				rth = RTA_NEXT(rth, rtl);
				continue;
			}

			FREE(cwmp_main->conf.ipv6);
			cwmp_main->conf.ipv6 = strdup(pradd_v6);
			cwmp_uci_set_varstate_value("cwmp", "cpe", "ipv6", cwmp_main->conf.ip);
			cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
			connection_request_ip_value_change(IPv6);
			break;
		}
	}
}

static void netlink_new_msg(struct uloop_fd *ufd, unsigned events __attribute__((unused)))
{
	struct nlmsghdr *nlh;
	char buffer[BUFSIZ];
	size_t msg_size;

	memset(&buffer, 0, sizeof(buffer));

	nlh = (struct nlmsghdr *)buffer;
	if ((int)(msg_size = recv(ufd->fd, nlh, BUFSIZ, 0)) == -1) {
		CWMP_LOG(ERROR, "error receiving netlink message");
		return;
	}

	while ((size_t)msg_size > sizeof(*nlh)) {
		int len = nlh->nlmsg_len;
		int req_len = len - sizeof(*nlh);

		if (req_len < 0 || (size_t)len > msg_size) {
			CWMP_LOG(ERROR, "error reading netlink message");
			return;
		}

		if (!NLMSG_OK(nlh, msg_size)) {
			CWMP_LOG(ERROR, "netlink message is not NLMSG_OK");
			return;
		}

		if (nlh->nlmsg_type == RTM_NEWADDR)
			freecwmp_netlink_interface(nlh);

		msg_size -= NLMSG_ALIGN(len);
		nlh = (struct nlmsghdr *)((char *)nlh + NLMSG_ALIGN(len));
	}
}

int netlink_init_v6(void)
{
	struct {
		struct nlmsghdr hdr;
		struct ifaddrmsg msg;
	} req;
	struct sockaddr_nl addr;
	int sock[2];

	memset(&addr, 0, sizeof(addr));
	memset(&req, 0, sizeof(req));

	if ((sock[0] = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1) {
		CWMP_LOG(ERROR, "couldn't open NETLINK_ROUTE socket");
		return -1;
	}

	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_IPV6_IFADDR;
	if ((bind(sock[0], (struct sockaddr *)&addr, sizeof(addr))) == -1) {
		CWMP_LOG(ERROR, "couldn't bind netlink socket");
		return -1;
	}

	netlink_event_v6.fd = sock[0];
	uloop_fd_add(&netlink_event_v6, ULOOP_READ | ULOOP_EDGE_TRIGGER);

	if ((sock[1] = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) == -1) {
		CWMP_LOG(ERROR, "couldn't open NETLINK_ROUTE socket");
		return -1;
	}

	req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
	req.hdr.nlmsg_type = RTM_GETADDR;
	req.msg.ifa_family = AF_INET6;

	if ((send(sock[1], &req, req.hdr.nlmsg_len, 0)) == -1) {
		CWMP_LOG(ERROR, "couldn't send netlink socket");
		return -1;
	}

	struct uloop_fd dummy_event = {.fd = sock[1] };
	netlink_new_msg(&dummy_event, 0);

	return 0;
}

int netlink_init(void)
{
	struct {
		struct nlmsghdr hdr;
		struct ifaddrmsg msg;
	} req;
	struct sockaddr_nl addr;
	int sock[2];

	memset(&addr, 0, sizeof(addr));
	memset(&req, 0, sizeof(req));

	if ((sock[0] = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1) {
		CWMP_LOG(ERROR, "couldn't open NETLINK_ROUTE socket");
		return -1;
	}

	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_IPV4_IFADDR;
	if ((bind(sock[0], (struct sockaddr *)&addr, sizeof(addr))) == -1) {
		CWMP_LOG(ERROR, "couldn't bind netlink socket");
		return -1;
	}

	netlink_event.fd = sock[0];
	uloop_fd_add(&netlink_event, ULOOP_READ | ULOOP_EDGE_TRIGGER);

	if ((sock[1] = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) == -1) {
		CWMP_LOG(ERROR, "couldn't open NETLINK_ROUTE socket");
		return -1;
	}

	req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
	req.hdr.nlmsg_type = RTM_GETADDR;
	req.msg.ifa_family = AF_INET;

	if ((send(sock[1], &req, req.hdr.nlmsg_len, 0)) == -1) {
		CWMP_LOG(ERROR, "couldn't send netlink socket");
		return -1;
	}

	struct uloop_fd dummy_event = {.fd = sock[1] };
	netlink_new_msg(&dummy_event, 0);

	return 0;
}

void cwmp_netlink_init(void)
{
	if (netlink_init()) {
		CWMP_LOG(ERROR, "netlink initialization failed");
	}

	if (cwmp_main->conf.ipv6_enable) {
		if (netlink_init_v6()) {
			CWMP_LOG(ERROR, "netlink initialization failed");
		}
	}
}
