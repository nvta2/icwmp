/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef _UDPECHOSERVER_H__
#define _UDPECHOSERVER_H__

#include <stdbool.h>
#include <sys/time.h>

#define default_date_format "AAAA-MM-JJTHH:MM:SS.000000Z"
#define default_date_size sizeof(default_date_format) + 1

struct udpechoserver_config
{
	bool enable;
	char *interface;
	char *address;
	int server_port;
	bool plus;
	int loglevel;
};

struct udpechoserver_plus
{
	unsigned int	TestGenSN;
	unsigned int	TestRespSN;
	unsigned int	TestRespRecvTimeStamp;
	unsigned int	TestRespReplyTimeStamp;
	unsigned int	TestRespReplyFailureCount;
};

struct udpechoserver_result
{
	unsigned int	TestRespSN;
	unsigned int	TestRespReplyFailureCount;
	unsigned int	PacketsReceived;
	unsigned int	PacketsResponded;
	unsigned int	BytesReceived;
	unsigned int	BytesResponded;
	struct timeval	TimeFirstPacketReceived;
	struct timeval	TimeLastPacketReceived;
	char TimeFirstPacket[default_date_size];
	char TimeLastPacket[default_date_size];
};

extern struct udpechoserver_config cur_udpechoserver_conf;
extern struct udpechoserver_result cur_udpechoserver_result;

#endif /* _UDPECHOSERVER_H__ */
