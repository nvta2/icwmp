#ifndef _STUN_H
#define _STUN_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libubox/uloop.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
	({ \
		const typeof(((type *) NULL)->member) *__mptr = (ptr); \
		(type *) ((char *) __mptr - offsetof(type, member)); \
	})
#endif

#define binding_request_entry(X) container_of(X, struct binding_request, utimer)
#define udp_listen_entry(X) container_of(X, struct udp_listen, utimer)

#define BINDING_REQUSET				0x0001
#define BINDING_RESPONSE			0x0101
#define BINDING_ERROR				0x0111

#define ATTR_MAPPED_ADDRESS				0x0001
#define ATTR_RESPONSE_ADDRESS			0x0002
#define ATTR_CHANGE_REQUEST				0x0003
#define ATTR_SOURCE_ADDRESS				0x0004
#define ATTR_CHANGED_ADDRESS			0x0005
#define ATTR_USERNAME					0x0006
#define ATTR_PASSWORD					0x0007
#define ATTR_MESSAGE_INTEGRITY			0x0008
#define ATTR_ERROR_CODE					0x0009
#define ATTR_UNKNOWN_ATTRIBUTES			0x000a
#define ATTR_REFLECTED_FROM				0x000b
#define ATTR_CONNECTION_REQUEST_BINDING	0xC001
#define ATTR_BINDING_CHANGE				0xC002

#define STUN_HEADER_LEN 20

struct binding_request;

typedef struct { unsigned int id[4]; } __attribute__((packed)) stunid;

struct udp_listen {
	stunid expected_id;
	int fd;
	struct uloop_timeout utimer;
	struct binding_request *br;
};

struct binding_request {
	stunid id;
	unsigned char is_keealive;
	unsigned char msg_integrity; /* if true ==> MESSAGE-INTEGRITY should be append in the STUN msg */
	unsigned char binding_change; /* if true ==> BINDING-CHANGE should be append in the STUN msg */
	unsigned char binding_cr; /* if true ==> CONNECTION-REQUEST-BINDING should be append in the STUN msg */
	int resp_success;
	int auth_fail;
	int retry_interval;
	struct udp_listen *udp_listen;
	struct uloop_timeout utimer;
};


struct stun_header {
	unsigned short type;
	unsigned short len;
	stunid id;
	unsigned char stunmsg[0];
} __attribute__((packed));

struct stun_attribute {
	unsigned short type;
	unsigned short len;
	unsigned char value[0];
} __attribute__((packed));

struct stun_address {
	unsigned char na;
	unsigned char family;
	unsigned short port;
	unsigned int address;
} __attribute__((packed));

struct env_var {
	unsigned short port;
	unsigned int address;
	unsigned int last_ts;
	unsigned int last_crid;
};

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _STUN_H */
