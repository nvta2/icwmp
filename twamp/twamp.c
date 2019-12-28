/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*       Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
* Name: Emma Mirică
* Project: TWAMP Protocol
* Class: OSS
* Email: emma.mirica@cti.pub.ro
* Contributions: stephanDB
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "twamp.h"
#include "twamplog.h"
#include "twampuci.h"

struct twamp_config cur_twamp_conf = {0};
TWAMPTimestamp ZeroT = { 0, 0 };

struct active_session {
    int socket;
    RequestSession req;
    uint16_t server_oct;
    uint32_t sid_addr;
    TWAMPTimestamp sid_time;
    uint32_t sid_rand;
    uint32_t seq_nb;
    uint32_t snd_nb;
    uint32_t fw_msg;
    uint32_t fw_lst_msg;
};

struct client_info {
    ClientStatus status;
    int socket;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int mode;
    int sess_no;
    struct timeval shutdown_time;
    struct active_session sessions[MAX_SESSIONS_PER_CLIENT];
};

static int fd_max = 0;
static enum Mode authmode = kModeUnauthenticated;
static int used_sockets = 0;
static fd_set read_fds;
static int socket_family = AF_INET;

static int check_ipv4_address(char *ip, char *maskstr, char *address)
{
	struct sockaddr_in sa = {0};
	unsigned long netaddress, maxaddress;
	unsigned long mask = ~((1 << (32 - atoi(maskstr))) - 1);
	inet_pton(AF_INET, address, &(sa.sin_addr));
	netaddress = (ntohl(sa.sin_addr.s_addr) & mask);
	sa.sin_addr.s_addr = 0;
	inet_pton(AF_INET, ip, &(sa.sin_addr));
	maxaddress = (ntohl(sa.sin_addr.s_addr) & mask);
	if (maxaddress == netaddress)
		return 1;
	return 0;
}

static char *check_ipv6_address_active(char *ip)
{
	unsigned char buf[sizeof(struct in6_addr)];
	char str[INET6_ADDRSTRLEN], *res;
	int s;

   s = inet_pton(AF_INET6, ip, buf);
   if (s <= 0) {
	   if (s == 0)
		   twamp_log(SCRIT, "Not in presentation format");
	   else
		   twamp_log(SCRIT, "inet_pton");
	   return "";
   }
   if (inet_ntop(AF_INET6, buf, str, INET6_ADDRSTRLEN) == NULL) {
	   twamp_log(SCRIT, "inet_ntop");
	   return "";
   }
   res = strdup(str);
	return res;
}

static int check_ipv6_address(char *ip, char *maskstr, char *address)
{
	struct sockaddr_in6 sa = {0};
	unsigned long netaddress, maxaddress;
	unsigned long mask = ~((1 << (128 - atoi(maskstr))) - 1);
	inet_pton(AF_INET6, address, &(sa.sin6_addr));
	netaddress = (ntohl(sa.sin6_addr.s6_addr) & mask);
	inet_pton(AF_INET6, ip, &(sa.sin6_addr));
	maxaddress = (ntohl(sa.sin6_addr.s6_addr) & mask);
	if (maxaddress == netaddress)
		return 1;
	return 0;
}

static int check_ip_address_allowed(char *address)
{
	char *pch, *spch, *ip, *mask, *str, *addr;

	for (pch = strtok_r(cur_twamp_conf.ip_list, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		if(strstr(pch, ".")) {
			if(strstr(pch, "/")) {
				ip = strtok_r(pch, "/", &str);
				mask = strtok_r(NULL, "", &str);
				if(check_ipv4_address(ip, mask, address))
					return 1;
				continue;
			}
			if (strcmp(pch, address) == 0)
				return 1;
		} else {
			addr = check_ipv6_address_active(address);
			if(strstr(pch, "/")) {
				ip = strtok_r(pch, "/", &str);
				mask = strtok_r(NULL, "", &str);
				ip = check_ipv6_address_active(ip);
				if(check_ipv6_address(ip, mask, addr))
					return 1;
				continue;
			}
			pch = check_ipv6_address_active(pch);
			if (strcmp(pch, addr) == 0)
				return 1;
		}
	}
	return 0;
}

static int check_port_allowed(int port)
{
	char *pch, *spch, *min, *max, *str;

	for (pch = strtok_r(cur_twamp_conf.port_list, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		if(strstr(pch, "-")) {
			min = strtok_r(pch, "-", &str);
			max = strtok_r(NULL, "", &str);
			if(port >= atoi(min) && port <= atoi(max))
				return 1;
			continue;
		}
		if (port == atoi(pch))
			return 1;
	}
	return 0;
}

/* The cleanup_client function will close every connection (TWAMP-Control ot TWAMP-Test that this server has with the client defined by the client_infor
 * structure received as a parameter.
 */
static void cleanup_client(struct client_info *client)
{
    char str_client[INET6_ADDRSTRLEN];
    inet_ntop(socket_family, (socket_family == AF_INET6) ? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));
    twamp_log(SINFO, "Cleanup client %s", str_client);
    FD_CLR(client->socket, &read_fds);
    close(client->socket);
    used_sockets--;
    int i;
    for (i = 0; i < client->sess_no; i++)
        /* If socket is -1 the session has already been closed */
        if (client->sessions[i].socket > 0) {
            FD_CLR(client->sessions[i].socket, &read_fds);
            close(client->sessions[i].socket);
            client->sessions[i].socket = -1;
            used_sockets--;
        }
    memset(client, 0, sizeof(struct client_info));
    client->status = kOffline;
}

/* The TWAMP server can only accept max_clients and it will recycle the positions for the available clients. */
static int find_empty_client(struct client_info *clients, int max_clients)
{
    int i;
    for (i = 0; i < max_clients; i++)
        if (clients[i].status == kOffline)
            return i;
    return -1;
}

/* Sends a ServerGreeting message to the Control-Client after the TCP connection has been established. */
static int send_greeting(uint16_t mode_mask, struct client_info *client)
{
    int socket = client->socket;

    char str_client[INET6_ADDRSTRLEN];   /* String for Client IP address */
    inet_ntop(socket_family, (socket_family == AF_INET6) ? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));

    int i;
    ServerGreeting greet;
    memset(&greet, 0, sizeof(greet));
    greet.Modes = htonl(client->mode & mode_mask);
    for (i = 0; i < 16; i++)
        greet.Challenge[i] = rand() % 16;
    for (i = 0; i < 16; i++)
        greet.Salt[i] = rand() % 16;
    greet.Count = htonl(1 << 10);

    int rv = send(socket, &greet, sizeof(greet), 0);
    if (rv < 0) {
        twamp_log(SCRIT,"Failed to send ServerGreeting message");
        cleanup_client(client);
    } else if ((authmode & 0x000F) == 0) {
        twamp_log(SCRIT,"Sent ServerGreeting message with Mode 0! Abort");
        cleanup_client(client);
    } else {
    	twamp_log(SINFO,"Sent ServerGreeting message to %s", str_client);
    }
    return rv;
}

/* After a ServerGreeting the Control-Client should respond with a SetUpResponse. This function treats this message */
static int receive_greet_response(struct client_info *client)
{
    int socket = client->socket;
    char str_client[INET6_ADDRSTRLEN];   /* String for Client IP address */
    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));

    SetUpResponse resp;
    memset(&resp, 0, sizeof(resp));
    int rv = recv(socket, &resp, sizeof(resp), 0);
    if (rv <= 32) {
        twamp_log(SCRIT,"Failed to receive SetUpResponse");
        cleanup_client(client);
    } else {
    	twamp_log(SINFO, "Received SetUpResponse message from %s with mode %d", str_client, ntohl(resp.Mode));
        if ((ntohl(resp.Mode) & client->mode & 0x000F) == 0) {
            twamp_log(SCRIT,"The client does not support any usable Mode");
            rv = 0;
        }
        client->mode = ntohl(resp.Mode);
    }
    return rv;
}

/* Sent a ServerStart message to the Control-Client to endthe TWAMP-Control session establishment phase */
static int send_start_serv(struct client_info *client, TWAMPTimestamp StartTime)
{
    int socket = client->socket;

    char str_client[INET6_ADDRSTRLEN];
    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));

    ServerStart msg;
    memset(&msg, 0, sizeof(msg));
    if ((StartTime.integer == 0) && (StartTime.fractional == 0)) {
        msg.Accept = kAspectNotSupported;
    } else {
        msg.Accept = kOK;
    }
    msg.StartTime = StartTime;
    int rv = send(socket, &msg, sizeof(msg), 0);
    if (rv <= 0) {
        twamp_log(SCRIT,"Failed to send ServerStart message");
        cleanup_client(client);
    } else {
        client->status = kConfigured;
        twamp_log(SINFO, "ServerStart message sent to %s", str_client);
        if (msg.Accept == kAspectNotSupported) {
            cleanup_client(client);
        }
    }
    return rv;
}

/* Sends a StartACK for the StartSessions message */
static int send_start_ack(struct client_info *client)
{
    char str_client[INET6_ADDRSTRLEN];
    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));
    StartACK ack;
    memset(&ack, 0, sizeof(ack));
    ack.Accept = kOK;
    int rv = send(client->socket, &ack, sizeof(ack), 0);
    if (rv <= 0) {
        twamp_log(SCRIT,"Failed to send StartACK message");
    } else
    	twamp_log(SINFO,"StartACK message sent to %s", str_client);
    return rv;
}

/* This function treats the case when a StartSessions is received from the Control-Client to start a number of TWAMP-Test sessions */
static int receive_start_sessions(struct client_info *client)
{
    int i;
    int rv = send_start_ack(client);
    if (rv <= 0)
        return rv;

    /* Now it can receive packets on the TWAMP-Test sockets */
    for (i = 0; i < client->sess_no; i++) {
        FD_SET(client->sessions[i].socket, &read_fds);
        if (fd_max < client->sessions[i].socket)
            fd_max = client->sessions[i].socket;
    }
    client->status = kTesting;
    return rv;
}

/* This functions treats the case when a StopSessions is received from the Control-Client to end all the Test sessions. */
static int receive_stop_sessions(struct client_info *client)
{
    /* If a StopSessions message was received, it can still receive Test packets until the timeout has expired */
    gettimeofday(&client->shutdown_time, NULL);
    return 0;
}

/* Computes the response to a RequestTWSession message */
static int send_accept_session(struct client_info *client, RequestSession * req)
{
    char str_client[INET6_ADDRSTRLEN];   /* String for Client IP address */
    AcceptSession acc;
    memset(&acc, 0, sizeof(acc));

    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));


    /* Check if there are any slots available */
    if ((used_sockets < 64) && (client->sess_no < MAX_SESSIONS_PER_CLIENT)) {
        int testfd = socket(socket_family, SOCK_DGRAM, 0);
        if (testfd < 0) {
            twamp_log(SCRIT,"Error opening socket");
            return -1;
        }

        int check_time = CHECK_TIMES;
        if(socket_family == AF_INET6) {
            struct sockaddr_in6 local_addr;
            memset(&local_addr, 0, sizeof(local_addr));
            local_addr.sin6_family = AF_INET6;
            local_addr.sin6_addr = in6addr_any;
            local_addr.sin6_port = req->ReceiverPort;

            while (check_time-- && bind(testfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
                local_addr.sin6_port = htons(20000 + rand() % 1000);

            if (check_time > 0) {
                req->ReceiverPort = local_addr.sin6_port;
            }
        } else {
            struct sockaddr_in local_addr;
            memset(&local_addr, 0, sizeof(local_addr));
            local_addr.sin_family = AF_INET;
            local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            local_addr.sin_port = req->ReceiverPort;

            while (check_time-- && bind(testfd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) < 0)
                local_addr.sin_port = htons(20000 + rand() % 1000);

            if (check_time > 0) {
                req->ReceiverPort = local_addr.sin_port;
            }
        }

        if (check_time > 0) {
            acc.Accept = kOK;
            acc.Port = req->ReceiverPort;
            client->sessions[client->sess_no].socket = testfd;
            client->sessions[client->sess_no].req = *req;
            /* SID construction */
            memcpy(acc.SID, &req->ReceiverAddress, 4);
            TWAMPTimestamp sidtime = get_timestamp();
            memcpy(&acc.SID[4], &sidtime, 8);
            int k;
            for (k = 0; k < 4; k++)
                acc.SID[12 + k] = rand() % 256;
            memcpy(&client->sessions[client->sess_no].sid_addr, &acc.SID, 4);
            client->sessions[client->sess_no].sid_time = sidtime;
            memcpy(&client->sessions[client->sess_no].sid_rand, &acc.SID[12], 4);

            twamp_log(SINFO, "SID: 0x%04X.%04X.%04X.%04X",
                    ntohl(client->sessions[client->sess_no].sid_addr),
                    ntohl(client->sessions[client->sess_no].sid_time.integer),
                    ntohl(client->sessions[client->sess_no].sid_time.fractional),
                    ntohl(client->sessions[client->sess_no].sid_rand));

            /* Set socket options */
            set_socket_option(testfd, cur_twamp_conf.max_ttl);
            set_socket_tos(testfd, (client->sessions[client->sess_no].req.TypePDescriptor << 2));

            client->sess_no++;

        } else {
        	twamp_log(SINFO, "kTemporaryResourceLimitation: check_time [%d]", check_time);
            acc.Accept = kTemporaryResourceLimitation;
            acc.Port = 0;
        }

    } else {
    	twamp_log(SINFO, "kTemporaryResourceLimitation: used_sockets [%d], sess_no [%d]", used_sockets, client->sess_no);
        acc.Accept = kTemporaryResourceLimitation;
        acc.Port = 0;
    }

    int rv = send(client->socket, &acc, sizeof(acc), 0);
    return rv;
}

/* This function treats the case when a RequestTWSession is received */
static int receive_request_session(struct client_info *client, RequestSession * req)
{
    char str_client[INET6_ADDRSTRLEN];   /* String for Client IP address */

    if(socket_family == AF_INET6) {
        inet_ntop(AF_INET6, &(client->addr6.sin6_addr), str_client, sizeof(str_client));
        twamp_log(SINFO, "Server received RequestTWSession message");
    } else {
        char str_server[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client->addr.sin_addr), str_client, INET_ADDRSTRLEN);
        struct in_addr se_addr;
        se_addr.s_addr = req->ReceiverAddress;
        inet_ntop(AF_INET, &(se_addr), str_server, INET_ADDRSTRLEN);
        twamp_log(SINFO, "Server %s received RequestTWSession message with port %d", str_server, ntohs(req->ReceiverPort));
    }
    /* Check port test packets if its allowed by PortAllowedList parameter */
    if(cur_twamp_conf.port_list[0] != '\0') {
		if(!check_port_allowed(ntohs(req->ReceiverPort))) {
			twamp_log(SINFO, "Port %d is not allowed", ntohs(req->ReceiverPort));
			return -1;
		}
    }

    int rv = send_accept_session(client, req);
    if (rv <= 0) {
        twamp_log(SCRIT,"Failed to send the Accept-Session message");
    }
    return rv;
}

/* This function will receive a TWAMP-Test packet and will send a response. In TWAMP the Session-Sender (in our case the Control-Client, meaning the
 * TWAMP-Client) is always sending TWAMP-Test packets and the Session-Reflector (Server) is receiving TWAMP-Test packets.
 */
static int receive_test_message(struct client_info *client, int session_index)
{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    socklen_t len = sizeof(addr);
    char str_client[INET6_ADDRSTRLEN];

    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(client->addr6.sin6_addr) : (void*) &(client->addr.sin_addr), str_client, sizeof(str_client));

    ReflectorUPacket pack_reflect;
    memset(&pack_reflect, 0, sizeof(pack_reflect));

    SenderUPacket pack;
    memset(&pack, 0, sizeof(pack));

    /* New for recvmsg */
    struct msghdr *message = malloc(sizeof(struct msghdr));
    struct cmsghdr *c_msg;
    char *control_buffer = malloc(TST_PKT_SIZE);
    uint16_t control_length = TST_PKT_SIZE;

    memset(message, 0, sizeof(*message));
    message->msg_name = (socket_family == AF_INET6)? (void*)&addr6: (void*)&addr;
    message->msg_namelen = len;
    message->msg_iov = malloc(sizeof(struct iovec));
    message->msg_iov->iov_base = &pack;
    message->msg_iov->iov_len = TST_PKT_SIZE;
    message->msg_iovlen = 1;
    message->msg_control = control_buffer;
    message->msg_controllen = control_length;

    int rv = recvmsg(client->sessions[session_index].socket, message, 0);
    pack_reflect.receive_time = get_timestamp();

    char str_server[INET6_ADDRSTRLEN];   /* String for Client IP address */

    inet_ntop(socket_family, (socket_family == AF_INET6)? (void*) &(addr6.sin6_addr) : (void*) &(addr.sin_addr), str_server, sizeof(str_server));
    if (rv <= 0) {
        twamp_log(SCRIT,"Failed to receive TWAMP-Test packet");
        return rv;
    } else if (rv < 14) {
        twamp_log(SCRIT,"Short TWAMP-Test packet");
        return rv;
    }

    /* Get TTL/TOS values from IP header */
    uint8_t fw_ttl = 0;
    uint8_t fw_tos = 0;

    for (c_msg = CMSG_FIRSTHDR(message); c_msg; c_msg = (CMSG_NXTHDR(message, c_msg))) {
        if ((c_msg->cmsg_level == IPPROTO_IP && c_msg->cmsg_type == IP_TTL) || (c_msg->cmsg_level == IPPROTO_IPV6 && c_msg->cmsg_type == IPV6_HOPLIMIT)) {
            fw_ttl = *(int *)CMSG_DATA(c_msg);
        } else if (c_msg->cmsg_level == IPPROTO_IP && c_msg->cmsg_type == IP_TOS) {
            fw_tos = *(int *)CMSG_DATA(c_msg);
        } else {
        	twamp_log(SINFO, "Warning, unexpected data of level %i and type %i", c_msg->cmsg_level, c_msg->cmsg_type);
        }
    }

    twamp_log(SINFO, "Received TWAMP-Test message from %s", inet_ntoa(addr.sin_addr));
    pack_reflect.seq_number = htonl(client->sessions[session_index].seq_nb++);
    pack_reflect.error_estimate = htons(0x8001);    // Sync = 1, Multiplier = 1
    pack_reflect.sender_seq_number = pack.seq_number;
    pack_reflect.sender_time = pack.time;
    pack_reflect.sender_error_estimate = pack.error_estimate;
    pack_reflect.sender_ttl = fw_ttl;   // Copy from the IP header packet from Sender
    if ((client->mode & kModeDSCPECN) == kModeDSCPECN) {
        pack_reflect.sender_tos = fw_tos;   // Copy from the IP header packet from Sender
    }

    if(socket_family == AF_INET6) {
        addr.sin_port = client->sessions[session_index].req.SenderPort;
    } else {
        addr6.sin6_port = client->sessions[session_index].req.SenderPort;
    }
    /* FW Loss Calculation */

    if (client->sessions[session_index].fw_msg == 0) {
        client->sessions[session_index].fw_msg = 1;
        /* Response packet for TOS with ECN */
        if ((fw_tos & 0x03) > 0) {
            uint8_t ecn_tos = (fw_tos & 0x03) - (((fw_tos & 0x2) >> 1) & (fw_tos & 0x1));
            set_socket_tos(client->sessions[session_index].socket, (client->sessions[session_index].req.TypePDescriptor << 2) + ecn_tos);
        }
    } else {
        client->sessions[session_index].fw_msg = client->sessions[session_index].fw_msg + ntohl(pack.seq_number) - client->sessions[session_index].snd_nb;
        client->sessions[session_index].fw_lst_msg = client->sessions[session_index].fw_lst_msg + ntohl(pack.seq_number) - client->sessions[session_index].snd_nb - 1;
    }
    client->sessions[session_index].snd_nb = ntohl(pack.seq_number);

    /* Response packet */
    pack_reflect.time = get_timestamp();

    if(socket_family == AF_INET6) {
        if (rv < 41) {
            rv = sendto(client->sessions[session_index].socket, &pack_reflect, 41, 0, (struct sockaddr *)&addr6, sizeof(addr6));
        } else {
            rv = sendto(client->sessions[session_index].socket, &pack_reflect, rv, 0, (struct sockaddr *)&addr6, sizeof(addr6));
        }
    } else {
        if (rv < 41) {
            rv = sendto(client->sessions[session_index].socket, &pack_reflect, 41, 0, (struct sockaddr *)&addr, sizeof(addr));
        } else {
            rv = sendto(client->sessions[session_index].socket, &pack_reflect, rv, 0, (struct sockaddr *)&addr, sizeof(addr));
        }
    }

    if (rv <= 0) {
        twamp_log(SCRIT,"Failed to send TWAMP-Test packet");
    }

    /* Print the FW metrics */
    print_metrics_server(str_client, socket_family == AF_INET6 ? ntohs(addr6.sin6_port): ntohs(addr.sin_port), ntohs(client->sessions[session_index].req.ReceiverPort), (client->sessions[session_index].req.TypePDescriptor << 2), fw_tos, &pack_reflect);

    if ((client->sessions[session_index].fw_msg % 10) == 0) {
    	twamp_log(SINFO,"FW Lost packets: %u/%u", client->sessions[session_index].fw_lst_msg, client->sessions[session_index].fw_msg);
    	twamp_log(SINFO,"FW Loss Ratio: %3.2f%%", (float)100 * client->sessions[session_index].fw_lst_msg / client->sessions[session_index].fw_msg);
        //printf("FW Lost packets: %u/%u, FW Loss Ratio: %3.2f%%\n", client->sessions[session_index].fw_lst_msg, client->sessions[session_index].fw_msg, (float)100 * client->sessions[session_index].fw_lst_msg / client->sessions[session_index].fw_msg);
    }
    return rv;
}

int twamp_connect(void)
{
	int listenfd, newsockfd;
	struct client_info clients[MAX_CLIENTS];
    struct sockaddr_in client_addr;
    struct sockaddr_in6 client_addr6;
    int rv;

    /* Obtain start server time in TWAMP format */
    TWAMPTimestamp StartTime = get_timestamp();

    if(cur_twamp_conf.ip_version == 4)
    	socket_family = AF_INET;
    else
    	socket_family = AF_INET6;

    listenfd = socket(socket_family, SOCK_STREAM, 0);
    if (listenfd < 0) {
        twamp_log(SCRIT,"Error opening socket");
        return -1;
    }

    int ret = setsockopt(listenfd, SOL_SOCKET, SO_BINDTODEVICE, cur_twamp_conf.device, strlen(cur_twamp_conf.device)+1);
    if(ret) {
		twamp_log(SCRIT,"Error on setsockopt with ret %d", ret);
		return -1;
	}

    if(socket_family == AF_INET6) {
        /* Set Server address and bind on the TWAMP port */
        struct sockaddr_in6 serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin6_family = AF_INET6;
        serv_addr.sin6_addr = in6addr_any;
        serv_addr.sin6_port = htons(cur_twamp_conf.port);

        if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            twamp_log(SCRIT,"Error on binding");
            return -1;
        }
    } else {
        /* Set Server address and bind on the TWAMP port */
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(cur_twamp_conf.port);

        if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) {
            twamp_log(SCRIT,"Error on binding");
            return -1;
        }
    }

    used_sockets++;

    /* Start listening on the TWAMP port for new TWAMP-Control connections */
    if (listen(listenfd, MAX_CLIENTS)) {
        twamp_log(SCRIT,"Error on listen");
        return -1;
    }

    FD_ZERO(&read_fds);
    FD_SET(listenfd, &read_fds);
    fd_max = listenfd;

    memset(clients, 0, MAX_CLIENTS * sizeof(struct client_info));
    fd_set tmp_fds;
    FD_ZERO(&tmp_fds);

    while (1) {
        tmp_fds = read_fds;
        if (select(fd_max + 1, &tmp_fds, NULL, NULL, NULL) < 0) {
            twamp_log(SCRIT,"Error in select");
            close(listenfd);
            return -1;
        }

        /* If an event happened on the listenfd, then a new TWAMP-Control connection is received */
        if (FD_ISSET(listenfd, &tmp_fds)) {
            uint32_t client_len = (socket_family == AF_INET6)? sizeof(client_addr6) : sizeof(client_addr);
            if ((newsockfd = accept(listenfd, (socket_family == AF_INET6) ? (struct sockaddr *)&client_addr6 : (struct sockaddr *)&client_addr, &client_len)) < 0) {
                twamp_log(SCRIT,"Error in accept");
            } else {
                /* Add a new client if there are any slots available */
                int pos = find_empty_client(clients, MAX_CLIENTS);
                uint16_t mode_mask = 0;
                if (pos != -1) {
                    clients[pos].status = kConnected;
                    clients[pos].socket = newsockfd;
                    clients[pos].addr = client_addr;
                    clients[pos].addr6 = client_addr6;
                    clients[pos].mode = authmode;
                    clients[pos].sess_no = 0;
                    used_sockets++;
                    FD_SET(newsockfd, &read_fds);
                    if (newsockfd > fd_max)
                        fd_max = newsockfd;
                    mode_mask = 0x01FF;
                }

            	char str_client[INET6_ADDRSTRLEN];   /* String for Client IP address */
            	inet_ntop(socket_family, (socket_family == AF_INET6) ? (void*) &(clients[pos].addr6.sin6_addr) : (void*) &(clients[pos].addr.sin_addr), str_client, sizeof(str_client));
            	twamp_log(SINFO,"Receive a TCP connection from %s", str_client);
            	/* Check ip test packets if its allowed by IPAllowedList parameter */
                if(cur_twamp_conf.ip_list[0] != '\0') {
            		if(!check_ip_address_allowed(str_client)) {
            			twamp_log(SINFO, "IP Address %d is not allowed", str_client);
            			close(listenfd);
            			return -1;
            		}
                }
                rv = send_greeting(mode_mask, &clients[pos]);
            }
        }

        /* Receives other packets from the established TWAMP-Control sessions */
        uint8_t buffer[4096];
        int i, j;
        for (i = 0; i < MAX_CLIENTS; i++)
            /* It can only receive TWAMP-Control messages from Online clients */
            if (clients[i].status != kOffline)
                if (FD_ISSET(clients[i].socket, &tmp_fds)) {
                    switch (clients[i].status) {
                    case kConnected:
                        /* If a TCP session has been established and a ServerGreeting has been sent, wait for the SetUpResponse and finish the TWAMP-Control setup */
                        rv = receive_greet_response(&clients[i]);
                        if (rv > 32) {
                            rv = send_start_serv(&clients[i], StartTime);
                        } else {
                            rv = send_start_serv(&clients[i], ZeroT);
                        }
                        break;
                    case kConfigured:
                        /* Reset the buffer to receive a new message */
                        memset(buffer, 0, 4096);
                        rv = recv(clients[i].socket, buffer, 4096, 0);
                        if (rv <= 0) {
                            cleanup_client(&clients[i]);
                            break;
                        }
                        /* Check the message received: It can only be StartSessions or RequestTWSession */
                        switch (buffer[0]) {
                        case kStartSessions:
                            rv = receive_start_sessions(&clients[i]);
                            break;
                        case kRequestTWSession:
                            rv = receive_request_session(&clients[i], (RequestSession *) buffer);
                            break;
                        default:
                            break;
                        }

                        if (rv <= 0)
                            cleanup_client(&clients[i]);
                        break;
                    case kTesting:
                        /* In this state can only receive a StopSessions msg */
                        memset(buffer, 0, 4096);
                        rv = recv(clients[i].socket, buffer, 4096, 0);
                        if (rv <= 0) {
                            cleanup_client(&clients[i]);
                            break;
                        }
                        if (buffer[0] == kStopSessions) {
                            rv = receive_stop_sessions(&clients[i]);
                        }
                        break;
                    default:
                        break;
                    }
                }

        /* Check for TWAMP-Test packets */
        for (i = 0; i < MAX_CLIENTS; i++) {
            struct timeval current;
            gettimeofday(&current, NULL);

            if (clients[i].status == kTesting) {
                uint8_t has_active_test_sessions = 0;
                for (j = 0; j < clients[i].sess_no; j++) {
                    rv = get_actual_shutdown(&current, &clients[i].shutdown_time, &clients[i].sessions[j].req.Timeout);
                    if (rv > 0) {
                        has_active_test_sessions = 1;
                        if (FD_ISSET(clients[i].sessions[j].socket, &tmp_fds)) {
                            rv = receive_test_message(&clients[i], j);
                        }
                    } else {
                        FD_CLR(clients[i].sessions[j].socket, &read_fds);
                        close(clients[i].sessions[j].socket);
                        used_sockets--;
                        clients[i].sessions[j].socket = -1;

                        /* print loss result */
                        twamp_log(SINFO, "Session: %u, FW Lost packets: %u/%u, FW Loss Ratio: %3.2f%%", j, clients[i].sessions[j].fw_lst_msg, clients[i].sessions[j].fw_msg, (float)100 * clients[i].sessions[j].fw_lst_msg / clients[i].sessions[j].fw_msg);

                    }
                }
                if (!has_active_test_sessions) {
                    memset(&clients[i].shutdown_time, 0, sizeof(clients[i].shutdown_time));
                    clients[i].sess_no = 0;
                    clients[i].status = kConfigured;
                }
            }
        }
    }
    return 0;
}

char *get_twamp_reflector_option(char *instance, char *option)
{
	struct uci_section *s;
	char *v, *twamp_id, *curr_interface;
	dmuci_foreach_section("cwmp_twamp", "twamp_reflector", s) {
		twamp_id = dmuci_get_value_bysection(s, "id");
		if(strcmp(twamp_id, instance) == 0)
		{
			v = dmuci_get_value_bysection(s, option);
			return v;
		}
	}
	v = "";
	return v;
}

int twamp_init(void)
{
	char *id, *interface, *value = NULL;
	int a;

	value = dmuci_get_value("cwmp_twamp", "twamp", "log_level");
	if(value != NULL && *value != '\0') {
		a = atoi(value);
		cur_twamp_conf.loglevel = a;
	}
	else
		cur_twamp_conf.loglevel = DEFAULT_LOGLEVEL;
	twamp_log(SDEBUG,"TWAMP Reflector Log Level:%d", cur_twamp_conf.loglevel);

	id = dmuci_get_value("cwmp_twamp", "twamp", "id");
	cur_twamp_conf.enable = atoi(get_twamp_reflector_option(id, "enable"));
	cur_twamp_conf.interface = strdup(get_twamp_reflector_option(id, "interface"));
	cur_twamp_conf.device = strdup(get_twamp_reflector_option(id, "device"));
	cur_twamp_conf.ip_version = atoi(get_twamp_reflector_option(id, "ip_version"));
	cur_twamp_conf.port = atoi(get_twamp_reflector_option(id, "port"));
	cur_twamp_conf.max_ttl = atoi(get_twamp_reflector_option(id, "max_ttl"));
	cur_twamp_conf.ip_list = strdup(get_twamp_reflector_option(id, "ip_list"));
	cur_twamp_conf.port_list = strdup(get_twamp_reflector_option(id, "port_list"));

	twamp_log(SDEBUG,"TWAMP Reflector Enable: %d", cur_twamp_conf.enable);
	twamp_log(SDEBUG,"TWAMP Reflector Interface: %s", cur_twamp_conf.interface);
	twamp_log(SDEBUG,"TWAMP Reflector Device: %s", cur_twamp_conf.device);
	twamp_log(SDEBUG,"TWAMP Reflector Port: %d", cur_twamp_conf.port);
	twamp_log(SDEBUG,"TWAMP Reflector MaximumTTL: %d", cur_twamp_conf.max_ttl);
	twamp_log(SDEBUG,"TWAMP Reflector IPAllowedList: %s", cur_twamp_conf.ip_list);
	twamp_log(SDEBUG,"TWAMP Reflector PortAllowedList: %s", cur_twamp_conf.port_list);

	return 0;
}

void twamp_exit(void)
{
	free(cur_twamp_conf.interface);
	free(cur_twamp_conf.device);
	free(cur_twamp_conf.ip_list);
	free(cur_twamp_conf.port_list);
}

int main(int argc, char *argv[])
{
	dmuci_init();
	twamp_init();
	dmuci_fini();
	twamp_log(SINFO,"START TWAMP Reflector");

	twamp_connect();

	twamp_exit();
	twamp_log(SINFO,"EXIT TWAMP Reflector");
	return 0;
}
