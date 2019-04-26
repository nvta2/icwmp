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

#include <inttypes.h>
#include <sys/time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "twamp.h"
#include "twamplog.h"

void timeval_to_timestamp(const struct timeval *tv, TWAMPTimestamp * ts)
{
    if (!tv || !ts)
        return;

    /* Unix time to NTP */
    ts->integer = tv->tv_sec + 2208988800uL;
    ts->fractional = (uint32_t) ((double)tv->tv_usec * ((double)(1uLL << 32)
                                                        / (double)1e6));

    ts->integer = htonl(ts->integer);
    ts->fractional = htonl(ts->fractional);
}

void timestamp_to_timeval(const TWAMPTimestamp * ts, struct timeval *tv)
{
    if (!tv || !ts)
        return;

    TWAMPTimestamp ts_host_ord;

    ts_host_ord.integer = ntohl(ts->integer);
    ts_host_ord.fractional = ntohl(ts->fractional);

    /* NTP to Unix time */
    tv->tv_sec = ts_host_ord.integer - 2208988800uL;
    tv->tv_usec = (uint32_t) (double)ts_host_ord.fractional * (double)1e6 / (double)(1uLL << 32);
}

TWAMPTimestamp get_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    TWAMPTimestamp ts;
    timeval_to_timestamp(&tv, &ts);
    return ts;
}

uint64_t get_usec(const TWAMPTimestamp * ts)
{
    struct timeval tv;
    timestamp_to_timeval(ts, &tv);

    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int get_actual_shutdown(const struct timeval *tv, const struct timeval *ts,
                        const TWAMPTimestamp * t)
{
    /* If ts is 0 then no StopSessions message was received */
    if ((ts->tv_sec * 1000000 + ts->tv_usec) == 0)
        return 1;
    /* Else compute time difference */
    uint64_t current = tv->tv_sec * 1000000 + tv->tv_usec;
    uint64_t shutdown = ts->tv_sec * 1000000 + ts->tv_usec;
    uint64_t timeout = get_usec(t);

    /* This should be ok, as no difference is computed */
    if (current > shutdown + timeout)
        return 1;
    return 0;
}

void print_metrics_server(char *addr_cl, uint16_t snd_port, uint16_t rcv_port,
                          uint8_t snd_tos, uint8_t fw_tos,
                          const ReflectorUPacket * pack)
{

    /* Compute timestamps in usec */
    uint64_t t_sender_usec1 = get_usec(&pack->sender_time);
    uint64_t t_receive_usec1 = get_usec(&pack->receive_time);
    uint64_t t_reflsender_usec1 = get_usec(&pack->time);

    /* Compute delays */
    int64_t fwd1 = t_receive_usec1 - t_sender_usec1;
    int64_t intd1 = t_reflsender_usec1 - t_receive_usec1;
    char sync1 = 'Y';
    if (fwd1 < 0) {
        sync1 = 'N';
    }

    /* Sequence number */
    uint32_t snd_nb = ntohl(pack->sender_seq_number);
    uint32_t rcv_nb = ntohl(pack->seq_number);

    /* Sender TOS with ECN from FW TOS */
    snd_tos = snd_tos + (fw_tos & 0x3) - (((fw_tos & 0x2) >> 1) & (fw_tos & 0x1));

    /* Print different metrics */
    twamp_log(SINFO,"Snd@: %s", addr_cl);
    twamp_log(SINFO,"Time: %.0f", (double)t_sender_usec1 * 1e-3);
    twamp_log(SINFO,"Snd#: %d", snd_nb);
    twamp_log(SINFO,"Rcv#: %d", rcv_nb);
    twamp_log(SINFO,"SndPt: %d", snd_port);
    twamp_log(SINFO,"RcvPt: %d", rcv_port);
    twamp_log(SINFO,"Sync: %c", sync1);
    twamp_log(SINFO,"TTL: %d", pack->sender_ttl);
    twamp_log(SINFO,"SndTOS: %d", snd_tos);
    twamp_log(SINFO,"FW_TOS: %d", fw_tos);
    twamp_log(SINFO,"Int D: %.3f", (double)intd1 * 1e-3);
}

void set_socket_option(int socket, uint8_t ip_ttl)
{
    /* Set socket options : timeout, IPTTL, IP_RECVTTL, IP_RECVTOS */
    uint8_t One = 1;
    int result;

    /* Set Timeout */
    struct timeval timeout = { LOSTTIME, 0 };   //set timeout for 2 seconds

    /* Set receive UDP message timeout value */
#ifdef SO_RCVTIMEO
    result = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,
                        (char *)&timeout, sizeof(struct timeval));
    if (result != 0) {
    	twamp_log(SDEBUG, "[PROBLEM] Cannot set the timeout value for reception.\n");
    }
#else
    twamp_log(SDEBUG, "No way to set the timeout value for incoming packets on that platform.\n");
#endif

    /* Set IPTTL value to twamp standard: 255 */
#ifdef IP_TTL
    result = setsockopt(socket, IPPROTO_IP, IP_TTL, &ip_ttl, sizeof(ip_ttl));
    if (result != 0) {
    	twamp_log(SDEBUG, "[PROBLEM] Cannot set the TTL value for emission.\n");
    }
#else
    twamp_log(SDEBUG, "No way to set the TTL value for leaving packets on that platform.\n");
#endif

    /* Set receive IP_TTL option */
#ifdef IP_RECVTTL
    result = setsockopt(socket, IPPROTO_IP, IP_RECVTTL, &One, sizeof(One));
    if (result != 0) {
    	twamp_log(SDEBUG, "[PROBLEM] Cannot set the socket option for TTL reception.\n");
    }
#else
    twamp_log(SDEBUG, "No way to ask for the TTL of incoming packets on that platform.\n");
#endif

    /* Set receive IP_TOS option */
#ifdef IP_RECVTOS
    result = setsockopt(socket, IPPROTO_IP, IP_RECVTOS, &One, sizeof(One));
    if (result != 0) {
    	twamp_log(SDEBUG, "[PROBLEM] Cannot set the socket option for TOS reception.\n");
    }
#else
    twamp_log(SDEBUG, "No way to ask for the TOS of incoming packets on that platform.\n");
#endif

}

void set_socket_tos(int socket, uint8_t ip_tos)
{
    /* Set socket options : IP_TOS */
    int result;

    /* Set IP TOS value */
#ifdef IP_TOS
    result = setsockopt(socket, IPPROTO_IP, IP_TOS, &ip_tos, sizeof(ip_tos));
    if (result != 0) {
    	twamp_log(SDEBUG, "[PROBLEM] Cannot set the TOS value for emission.\n");
    }
#else
    twamp_log(SDEBUG, "No way to set the TOS value for leaving packets on that platform.\n");
#endif

}
