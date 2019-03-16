/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "udpechoserver.h"
#include "udpechoserverlog.h"
#include "udpechoserveruci.h"

struct udpechoserver_config cur_udpechoserver_conf = {0};
struct udpechoserver_result cur_udpechoserver_result={0};

static void set_udpechoserver_stats(void)
{
	char PacketsReceived[8], PacketsResponded[8], BytesReceived[8], BytesResponded[8];

	sprintf(PacketsReceived, "%d", cur_udpechoserver_result.PacketsReceived);
	sprintf(PacketsResponded, "%d", cur_udpechoserver_result.PacketsResponded);
	sprintf(BytesReceived, "%d", cur_udpechoserver_result.BytesReceived);
	sprintf(BytesResponded, "%d", cur_udpechoserver_result.BytesResponded);

	dmuci_init();
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "PacketsReceived", PacketsReceived);
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "PacketsResponded", PacketsResponded);
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "BytesReceived", BytesReceived);
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "BytesResponded", BytesResponded);
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "TimeFirstPacketReceived", cur_udpechoserver_result.TimeFirstPacket);
	dmuci_set_value_state("cwmp_udpechoserver", "udpechoserver", "TimeLastPacketReceived", cur_udpechoserver_result.TimeLastPacket);
	dmuci_fini();
}

int udpechoserver_connect(void)
{
	int sock, fromlen, n, ret;
	struct sockaddr_in server, from;
	struct timeval tv_recv, tv_reply;
	struct tm lt;
	char buf[1024];
	char str[INET_ADDRSTRLEN];
	char s_now[default_date_size];
	struct udpechoserver_plus *plus;

	memset(&cur_udpechoserver_result, 0, sizeof(cur_udpechoserver_result));

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		udpechoserver_log(SCRIT,"Socket error = %d", sock);
		return -1;
	}

	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(cur_udpechoserver_conf.server_port);

	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) > 0) {
		udpechoserver_log(SCRIT,"Error in bind function");
		return -1;
	}

	udpechoserver_log(SINFO,"CONNECT UDPECHOSERVER");

	while(true)
	{
		fromlen = sizeof(struct sockaddr_in);
		n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
		if (n < 0)
			udpechoserver_log(SCRIT,"Error in receive message");

		inet_ntop(AF_INET, &(from.sin_addr.s_addr), str, INET_ADDRSTRLEN);
		udpechoserver_log(SINFO,"UDPECHOSERVER receive massage from %s with data size is %d Bytes", str, n);

		if(cur_udpechoserver_conf.address && cur_udpechoserver_conf.address[0] != '\0') {
			if(strcmp(cur_udpechoserver_conf.address, str) != 0) {
				udpechoserver_log(SINFO,"UDPECHOSERVER can not respond to a UDP echo from this source IP address %s", str);
				continue;
			}
		}

		gettimeofday(&tv_recv, NULL);
		if(cur_udpechoserver_result.TimeFirstPacketReceived.tv_sec == 0 && cur_udpechoserver_result.TimeFirstPacketReceived.tv_usec == 0)
			cur_udpechoserver_result.TimeFirstPacketReceived = tv_recv;
		cur_udpechoserver_result.TimeLastPacketReceived = tv_recv;
		cur_udpechoserver_result.PacketsReceived++;
		cur_udpechoserver_result.BytesReceived+=n;
		(void) localtime_r(&(cur_udpechoserver_result.TimeFirstPacketReceived.tv_sec), &lt);
		strftime(s_now, sizeof s_now, "%Y-%m-%dT%H:%M:%S", &lt);
		sprintf(cur_udpechoserver_result.TimeFirstPacket,"%s.%06ld", s_now, (long) cur_udpechoserver_result.TimeFirstPacketReceived.tv_usec);
		(void) localtime_r(&(cur_udpechoserver_result.TimeLastPacketReceived.tv_sec), &lt);
		strftime(s_now, sizeof s_now, "%Y-%m-%dT%H:%M:%S", &lt);
		sprintf(cur_udpechoserver_result.TimeLastPacket,"%s.%06ld", s_now, (long) cur_udpechoserver_result.TimeLastPacketReceived.tv_usec);

		if(cur_udpechoserver_conf.plus) {
			if(n >= sizeof(struct udpechoserver_plus)) {
				plus = (struct udpechoserver_plus *)buf;
				plus->TestRespSN = htonl(cur_udpechoserver_result.TestRespSN);
				plus->TestRespRecvTimeStamp = htonl(tv_recv.tv_sec*1000000+tv_recv.tv_usec);
				plus->TestRespReplyFailureCount = htonl(cur_udpechoserver_result.TestRespReplyFailureCount);
				gettimeofday(&tv_reply, NULL);
				plus->TestRespReplyTimeStamp = htonl(tv_reply.tv_sec*1000000+tv_reply.tv_usec);
			}
		}

		ret = sendto(sock, buf, n, 0, (struct sockaddr *)&from, fromlen);
		if (n != ret) {
			cur_udpechoserver_result.TestRespReplyFailureCount++;
			udpechoserver_log(SCRIT,"Error in send massage");
		}
		else {
			cur_udpechoserver_result.TestRespSN++;
			cur_udpechoserver_result.PacketsResponded++;
			cur_udpechoserver_result.BytesResponded+=ret;
		}
		udpechoserver_log(SINFO,"UDPECHOSERVER sent massage to %s with data size is %d Bytes", str, ret);

		set_udpechoserver_stats();

		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : PacketsReceived = %d", cur_udpechoserver_result.PacketsReceived);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : PacketsResponded = %d", cur_udpechoserver_result.PacketsResponded);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : BytesReceived = %d", cur_udpechoserver_result.BytesReceived);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : BytesResponded = %d", cur_udpechoserver_result.BytesResponded);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : TestRespReplyFailureCount = %d", cur_udpechoserver_result.TestRespReplyFailureCount);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : TimeFirstPacketReceived = %s", cur_udpechoserver_result.TimeFirstPacket);
		udpechoserver_log(SDEBUG,"UDPECHOSERVER Stats : TimeLastPacketReceived = %s", cur_udpechoserver_result.TimeLastPacket);
	}
}

int udpechoserver_init(void)
{
	char *value = NULL;
	int a;

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "log_level");
	if(value != NULL && *value != '\0') {
		a = atoi(value);
		cur_udpechoserver_conf.loglevel = a;
	}
	else
		cur_udpechoserver_conf.loglevel = DEFAULT_LOGLEVEL;
	udpechoserver_log(SDEBUG,"Log Level of UDP ECHO SERVER is :%d", cur_udpechoserver_conf.loglevel);

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "enable");
	if(value != NULL && *value != '\0') {
		if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0)) {
			cur_udpechoserver_conf.enable = true;
			udpechoserver_log(SDEBUG,"UDP echo server is Enabled");
		}
	}

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "interface");
	if(value != NULL && *value != '\0') {
		if (cur_udpechoserver_conf.interface != NULL)
			free(cur_udpechoserver_conf.interface);
		cur_udpechoserver_conf.interface = strdup(value);
		udpechoserver_log(SDEBUG,"UDP echo server interface is :%s", cur_udpechoserver_conf.interface);
	}
	else {
		cur_udpechoserver_conf.interface = strdup("");
		udpechoserver_log(SDEBUG,"UDP echo server interface is empty");
	}

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "address");
	if(value != NULL && *value != '\0') {
		if (cur_udpechoserver_conf.address != NULL)
			free(cur_udpechoserver_conf.address);
		cur_udpechoserver_conf.address = strdup(value);
		udpechoserver_log(SDEBUG,"UDP echo server address is :%s", cur_udpechoserver_conf.address);
	}
	else {
		cur_udpechoserver_conf.address = strdup("");
		udpechoserver_log(SDEBUG,"UDP echo server address is empty");
	}

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "server_port");
	if(value != NULL && *value != '\0') {
		a = atoi(value);
		cur_udpechoserver_conf.server_port = a;
		udpechoserver_log(SDEBUG,"UDP echo server port is :%d", cur_udpechoserver_conf.server_port);
	}

	value = dmuci_get_value("cwmp_udpechoserver", "udpechoserver", "plus");
	if(value != NULL && *value != '\0') {
		if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0)) {
			cur_udpechoserver_conf.plus = true;
			udpechoserver_log(SDEBUG,"UDP echo server plus is Enabled");
		}
		else {
			cur_udpechoserver_conf.plus = false;
			udpechoserver_log(SDEBUG,"UDP echo server plus is Disabled");
		}
	}
	return 0;
}

void udpechoserver_exit(void)
{
	free(cur_udpechoserver_conf.address);
	free(cur_udpechoserver_conf.interface);
}

int main(void)
{
	dmuci_init();
	udpechoserver_init();
	dmuci_fini();
	udpechoserver_log(SINFO,"START UDPECHOSERVER");

	udpechoserver_connect();

	udpechoserver_exit();
	udpechoserver_log(SINFO,"EXIT UDPECHOSERVER");
	return 0;
}
