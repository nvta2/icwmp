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

/*
 * The following logical-model will be implemented:
 *
 * +----------------+                   +-------------------+
 * | Control-Client |<--TWAMP-Control-->| Server            |
 * |                |                   |                   |
 * | Session-Sender |<--TWAMP-Test----->| Session-Reflector |
 * +----------------+                   +-------------------+
 *
 */

#ifndef _TWAMP_H__
#define _TWAMP_H__

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>

#define CHECK_TIMES 100
#define LOSTTIME	2           /* SECONDS - Timeout for TWAMP test packet */
#define MAX_CLIENTS 10
#define MAX_SESSIONS_PER_CLIENT 10

enum CommandNumber {
    kReserved,
    kForbidden,
    kStartSessions,
    kStopSessions,
    kReserved4,
    kRequestTWSession,
    kExperimentation
};

enum Mode {
    kModeReserved = 0,
    kModeUnauthenticated = 1,
    kModeAuthenticated = 2,
    kModeEncrypted = 4,
    kModeHybrid = 8,            /* Unauthenticated test, encrypted control */
    kModeIndividual = 16,
    kModeReflectOctets = 32,
    KModeSymmetrical = 64,
    KModeIKEv2Derived = 128,
    kModeDSCPECN = 256
};

enum AcceptCode {
    kOK,
    kFailure,
    kInternalError,
    kAspectNotSupported,
    kPermanentResourceLimitation,
    kTemporaryResourceLimitation
};

typedef enum {
    kOffline = 0,
    kConnected,
    kConfigured,
    kTesting
} ClientStatus;

typedef struct twamp_timestamp {
    uint32_t integer;
    uint32_t fractional;
} TWAMPTimestamp;

/*****************************************/
/*                                       */
/* TWAMP-Control specific messages       */
/*                                       */
/*****************************************/

/* First messsage sent by the Server to the Control-Client to establish a connection */
typedef struct server_greeting {
    uint8_t Unused[12];
    /* Modes = bit-wise OR between Mode values
     * First 23 bits MUST be zero in TWAMP (29 in first version)*/
    uint32_t Modes;
    uint8_t Challenge[16];      /* Random sequence of bytes generated by the server */
    uint8_t Salt[16];
    uint32_t Count;             /* MUST be a power of 2. Minimum 1024 */
    uint8_t MBZ[12];
} ServerGreeting;

/* The Control-Client's response to the Server's greeting */
typedef struct control_client_greeting_response {
    uint32_t Mode;              /* if 0 -> the Server does not wish to communicate */
    uint8_t KeyID[80];
    uint8_t Token[64];
    uint8_t ClientIV[16];
} SetUpResponse;

/* The Server sends a start message to conclude the TWAMP-Control session */
typedef struct server_start {
    uint8_t MBZ1[15];
    uint8_t Accept;             /* 0 means Continue. See 3.3 of RFC 4656 */
    uint8_t ServerIV[16];
    TWAMPTimestamp StartTime;   /* TWAMPTimestamp; 0 if Accept is NonZero. */
    uint8_t MBZ2[8];
} ServerStart;

/* The Control-Client sends a RequestSession packet for each TWAMP-Test session */
typedef struct request_session {
    uint8_t Type;               /* 5 / CommandNumber */
    uint8_t IPVN;               /* MBZ | IPVN */
    uint8_t ConfSender;         /* 0 */
    uint8_t ConfReceiver;       /* 0 */
    uint32_t SlotsNo;           /* 0 */
    uint32_t PacketsNo;         /* 0 */
    uint16_t SenderPort;
    uint16_t ReceiverPort;
    uint32_t SenderAddress;
    uint8_t MBZ1[12];           /* Sender Address Cont */
    uint32_t ReceiverAddress;
    uint8_t MBZ2[12];           /* Receiver Address Cont */
    uint8_t SID[16];            /* 0 */
    uint32_t PaddingLength;
    TWAMPTimestamp StartTime;
    TWAMPTimestamp Timeout;
    uint32_t TypePDescriptor;
    uint16_t OctetsToBeReflected;
    uint16_t PadLenghtToReflect;
    uint8_t MBZ3[4];
    uint8_t HMAC[16];
} RequestSession;

/* The Server's response to the RequestSession packet */
typedef struct accept_session_packet {
    uint8_t Accept;             /* 3 if not supported */
    uint8_t MBZ1;
    uint16_t Port;
    uint8_t SID[16];            /* Generated by server */
    //uint16_t ReflectedOctets;
    //uint16_t ServerOctets;
    uint8_t MBZ2[8];
    uint8_t HMAC[16];
} AcceptSession;

/* The Control-Client sends a StartSessions message to start all accepted TWAMP-Test sessions */
typedef struct start_message1 {
    uint8_t Type;               /* 2 */
    uint8_t MBZ[15];
    uint8_t HMAC[16];
} StartSessions;

/* When it receives a StartSessions, the Server responds with a StartACK */
typedef struct start_ack {
    uint8_t Accept;
    uint8_t MBZ[15];
    uint8_t HMAC[16];
} StartACK;

/* The Control-Client sends a StopSessions message to stop all active TWAMP-Test sessions */
typedef struct twamp_stop {
    uint8_t Type;               /* 3 */
    uint8_t Accept;
    uint8_t MBZ1[2];
    uint32_t SessionsNo;
    uint8_t MBZ2[8];
    uint8_t HMAC[16];
} StopSessions;

/*****************************************/
/*                                       */
/* TWAMP-Test specific messages          */
/*                                       */
/*****************************************/
#define TST_PKT_SIZE 1472       //1472 (MTU 1514)

/* Session-Sender TWAMP-Test packet for Unauthenticated mode */
typedef struct test_packet {
    uint32_t seq_number;
    TWAMPTimestamp time;
    uint16_t error_estimate;
    uint8_t padding[TST_PKT_SIZE - 14];
} SenderUPacket;

/* Session-Reflector TWAMP-Test packet for Unauthenticated mode */
typedef struct reflector_unauth_packet {
    uint32_t seq_number;
    TWAMPTimestamp time;
    uint16_t error_estimate;
    uint8_t mbz1[2];
    TWAMPTimestamp receive_time;
    uint32_t sender_seq_number;
    TWAMPTimestamp sender_time;
    uint16_t sender_error_estimate;
    uint8_t mbz2[2];
    uint8_t sender_ttl;
    uint8_t sender_tos;
    uint8_t padding[TST_PKT_SIZE - 42];
} ReflectorUPacket;

struct twamp_config
{
	bool enable;
	char *interface;
	char *device;
	int ip_version;
	int port;
	int max_ttl;
	char *ip_list;
	char *port_list;
	int loglevel;
};

extern struct twamp_config cur_twamp_conf;

void timeval_to_timestamp(const struct timeval *tv, TWAMPTimestamp * ts);
void timestamp_to_timeval(const TWAMPTimestamp * ts, struct timeval *tv);
uint64_t get_usec(const TWAMPTimestamp * ts);
TWAMPTimestamp get_timestamp();
int get_actual_shutdown(const struct timeval *tv, const struct timeval *ts, const TWAMPTimestamp * t);
void print_metrics_server(char *addr_cl, uint16_t snd_port, uint16_t rcv_port, uint8_t snd_tos, uint8_t fw_tos, const ReflectorUPacket * pack);
void set_socket_option(int socket, uint8_t ip_ttl);
void set_socket_tos(int socket, uint8_t ip_tos);

#endif /* _TWAMP_H__ */
