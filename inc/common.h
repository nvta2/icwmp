/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#ifndef __CCOMMON_H
#define __CCOMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <libubox/list.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

#ifndef CWMP_VERSION
#define CWMP_VERSION "3.0.0"
#endif

#define __offsetof__(x) (x)

#define ARRAYSIZEOF(a) (sizeof(a) / sizeof((a)[0]))

#define MAX_EVENTS 64
#define MAX_INT32 2147483646
#define MAX_INT_ID MAX_INT32
#define MIN_INT_ID 836464
#define PERIOD_INFORM_MIN 60
#define PERIOD_INFORM_DEFAULT 86400
#define CONNECTION_REQUEST_RESTRICT_PERIOD 5
#define CONNECTION_REQUEST_RESTRICT_REQUEST 50
#define DEFAULT_CONNECTION_REQUEST_PORT 7547
#define DEFAULT_NOTIFY_PERIOD 10
#define DEFAULT_LWN_PORT 7547
#define DEFAULT_RETRY_MINIMUM_WAIT_INTERVAL 5
#define DEFAULT_RETRY_INITIAL_INTERVAL 60
#define DEFAULT_RETRY_INTERVAL_MULTIPLIER 2000
#define DEFAULT_RETRY_MAX_INTERVAL 60
#define DEFAULT_AMD_VERSION 5
#define DEFAULT_INSTANCE_MODE 0
#define DEFAULT_SESSION_TIMEOUT 60
#define DEFAULT_ACSURL "http://192.168.1.1:8080/openacs/acs"

extern char *commandKey;
#define FIREWALL_CWMP "/etc/firewall.cwmp"
#define ICWMP_DOWNLOAD_FILE "/tmp/icwmp_download"
#define FIRMWARE_UPGRADE_IMAGE "/tmp/firmware.bin"

typedef struct env {
	unsigned short boot;
	unsigned short periodic;
	long int max_firmware_size;
} env;

typedef struct config {
	char *acsurl;
	char *acs_userid;
	char *acs_passwd;
	char *acs_ssl_capath;
	char *acs_ssl_version;
	char *https_ssl_capath;
	char *cpe_userid;
	char *cpe_passwd;
	char *ip;
	char *ipv6;
	char *interface;
	char *ubus_socket;
	char *default_wan_iface;
	char *connection_request_path;
	int connection_request_port;
	int period;
	int periodic_notify_interval;
	int compression;
	time_t time;
	bool periodic_enable;
	bool periodic_notify_enable;
	bool insecure_enable;
	bool ipv6_enable;
	int retry_min_wait_interval;
	int retry_interval_multiplier;
	bool lw_notification_enable;
	char *lw_notification_hostname;
	int lw_notification_port;
	int amd_version;
	int supported_amd_version;
	unsigned int instance_mode;
	unsigned int session_timeout;
	bool http_disable_100continue;
} config;

struct deviceid {
	char *manufacturer;
	char *oui;
	char *serialnumber;
	char *productclass;
	char *softwareversion;
};

typedef struct session_status {
	time_t last_start_time;
	time_t last_end_time;
	int last_status;
	time_t next_periodic;
	time_t next_retry;
	unsigned int success_session;
	unsigned int failure_session;
} session_status;

typedef struct cwmp {
	struct env env;
	struct config conf;
	struct deviceid deviceid;
	struct list_head head_session_queue;
	pthread_mutex_t mutex_session_queue;
	struct session *session_send;
	bool cwmp_cr_event;
	pthread_mutex_t mutex_session_send;
	pthread_cond_t threshold_session_send;
	pthread_mutex_t mutex_periodic;
	pthread_mutex_t mutex_notify_periodic;
	pthread_cond_t threshold_periodic;
	pthread_cond_t threshold_notify_periodic;
	pthread_cond_t threshold_handle_notify;
	int count_handle_notify;
	int retry_count_session;
	struct list_head *head_event_container;
	FILE *pid_file;
	time_t start_time;
	struct session_status session_status;
	unsigned int cwmp_id;
	int cr_socket_desc;
} cwmp;

enum action
{
	NONE = 0,
	START,
	STOP,
	RESTART,
};

enum cwmp_start
{
	CWMP_START_BOOT = 1,
	CWMP_START_PERIODIC = 2
};

enum cwmp_ret_err
{
	CWMP_OK, /* No Error */
	CWMP_GEN_ERR, /* General Error */
	CWMP_MEM_ERR, /* Memory Error */
	CWMP_MUTEX_ERR,
	CWMP_RETRY_SESSION
};

enum http_compression
{
	COMP_NONE,
	COMP_GZIP,
	COMP_DEFLATE
};

enum enum_ip_version
{
	IPv4 = 4,
	IPv6 = 6
};

typedef struct rpc {
	struct list_head list;
	int type;
	void *extra_data;
	struct list_head *list_set_value_fault;
} rpc;

struct cwmp_param_fault {
	struct list_head list;
	char *name;
	int fault;
};

struct cwmp_dm_parameter {
	struct list_head list;
	char *name;
	char *value;
	char *type;
	int notification;
	bool writable;
};

enum amd_version_enum
{
	AMD_1 = 1,
	AMD_2,
	AMD_3,
	AMD_4,
	AMD_5,
};

enum instance_mode
{
	INSTANCE_MODE_NUMBER,
	INSTANCE_MODE_ALIAS
};

struct cwmp_namespaces {
	char *soap_env;
	char *soap_enc;
	char *xsd;
	char *xsi;
	char *cwmp;
} ns;

enum rpc_cpe_methods_idx
{
	RPC_CPE_GET_RPC_METHODS = 1,
	RPC_CPE_SET_PARAMETER_VALUES,
	RPC_CPE_GET_PARAMETER_VALUES,
	RPC_CPE_GET_PARAMETER_NAMES,
	RPC_CPE_SET_PARAMETER_ATTRIBUTES,
	RPC_CPE_GET_PARAMETER_ATTRIBUTES,
	RPC_CPE_ADD_OBJECT,
	RPC_CPE_DELETE_OBJECT,
	RPC_CPE_REBOOT,
	RPC_CPE_DOWNLOAD,
	RPC_CPE_UPLOAD,
	RPC_CPE_FACTORY_RESET,
	RPC_CPE_SCHEDULE_INFORM,
	RPC_CPE_SCHEDULE_DOWNLOAD,
	RPC_CPE_CHANGE_DU_STATE,
	RPC_CPE_CANCEL_TRANSFER,
	RPC_CPE_X_FACTORY_RESET_SOFT,
	RPC_CPE_FAULT,
	__RPC_CPE_MAX
};

enum rpc_acs_methods_idx
{
	RPC_ACS_INFORM = 1,
	RPC_ACS_GET_RPC_METHODS,
	RPC_ACS_TRANSFER_COMPLETE,
	RPC_ACS_DU_STATE_CHANGE_COMPLETE,
	__RPC_ACS_MAX
};

enum load_type
{
	TYPE_DOWNLOAD = 0,
	TYPE_SCHEDULE_DOWNLOAD,
	TYPE_UPLOAD
};

enum dustate_type
{
	DU_INSTALL = 1,
	DU_UPDATE,
	DU_UNINSTALL
};

enum fault_cpe_idx
{
	FAULT_CPE_NO_FAULT,
	FAULT_CPE_METHOD_NOT_SUPPORTED,
	FAULT_CPE_REQUEST_DENIED,
	FAULT_CPE_INTERNAL_ERROR,
	FAULT_CPE_INVALID_ARGUMENTS,
	FAULT_CPE_RESOURCES_EXCEEDED,
	FAULT_CPE_INVALID_PARAMETER_NAME,
	FAULT_CPE_INVALID_PARAMETER_TYPE,
	FAULT_CPE_INVALID_PARAMETER_VALUE,
	FAULT_CPE_NON_WRITABLE_PARAMETER,
	FAULT_CPE_NOTIFICATION_REJECTED,
	FAULT_CPE_DOWNLOAD_FAILURE,
	FAULT_CPE_UPLOAD_FAILURE,
	FAULT_CPE_FILE_TRANSFER_AUTHENTICATION_FAILURE,
	FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL,
	FAULT_CPE_DOWNLOAD_FAIL_MULTICAST_GROUP,
	FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER,
	FAULT_CPE_DOWNLOAD_FAIL_ACCESS_FILE,
	FAULT_CPE_DOWNLOAD_FAIL_COMPLETE_DOWNLOAD,
	FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED,
	FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION,
	FAULT_CPE_DOWNLOAD_FAIL_WITHIN_TIME_WINDOW,
	FAULT_CPE_DUPLICATE_DEPLOYMENT_UNIT,
	FAULT_CPE_SYSTEM_RESOURCES_EXCEEDED,
	FAULT_CPE_UNKNOWN_DEPLOYMENT_UNIT,
	FAULT_CPE_INVALID_DEPLOYMENT_UNIT_STATE,
	FAULT_CPE_INVALID_DOWNGRADE_REJECTED,
	FAULT_CPE_INVALID_UPDATE_VERSION_UNSPECIFIED,
	FAULT_CPE_INVALID_UPDATE_VERSION_EXIST,
	__FAULT_CPE_MAX
};

enum
{
	FAULT_CPE_TYPE_CLIENT,
	FAULT_CPE_TYPE_SERVER
};

struct rpc_cpe_method {
	const char *name;
	int (*handler)(struct session *session, struct rpc *rpc);
	int amd;
};

struct rpc_acs_method {
	const char *name;
	int (*prepare_message)(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
	int (*parse_response)(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
	int (*extra_clean)(struct session *session, struct rpc *rpc);
};

typedef struct FAULT_CPE {
	char *CODE;
	int ICODE;
	int TYPE;
	char *DESCRIPTION;
} FAULT_CPE;

typedef struct schedule_inform {
	struct list_head list;
	time_t scheduled_time;
	char *commandKey;
} schedule_inform;

typedef struct download {
	struct list_head list;
	time_t scheduled_time;
	int file_size;
	char *command_key;
	char *file_type;
	char *url;
	char *username;
	char *password;
} download;

typedef struct timewindow {
	time_t windowstart;
	time_t windowend;
	char *windowmode;
	char *usermessage;
	int maxretries;
} timewindow;

typedef struct timeinterval {
	time_t windowstart;
	time_t windowend;
	int maxretries;
} timeinterval;

typedef struct schedule_download {
	struct list_head list;
	//time_t                              scheduled_time;
	int file_size;
	char *command_key;
	char *file_type;
	char *url;
	char *username;
	char *password;
	struct timewindow timewindowstruct[2];
} schedule_download;

typedef struct apply_schedule_download {
	struct list_head list;
	char *start_time;
	char *command_key;
	char *file_type;
	struct timeinterval timeintervals[2];
} apply_schedule_download;

typedef struct change_du_state {
	struct list_head list;
	time_t timeout;
	char *command_key;
	struct list_head list_operation;
} change_du_state;

typedef struct operations {
	struct list_head list;
	int type;
	char *url;
	char *uuid;
	char *version;
	char *username;
	char *password;
	char *executionenvref;
} operations;

typedef struct upload {
	struct list_head list;
	time_t scheduled_time;
	char *command_key;
	char *file_type;
	char *url;
	char *username;
	char *password;
	char *f_instance;
} upload;

typedef struct transfer_complete {
	int fault_code;
	char *command_key;
	char *start_time;
	char *complete_time;
	char *old_software_version;
	int type;
} transfer_complete;

typedef struct du_state_change_complete {
	char *command_key;
	time_t timeout;
	struct list_head list_opresult;
} du_state_change_complete;

typedef struct opresult {
	struct list_head list;
	char *uuid;
	char *du_ref;
	char *version;
	char *current_state;
	bool resolved;
	char *execution_unit_ref;
	char *start_time;
	char *complete_time;
	int fault;
} opresult;

typedef struct opfault {
	int fault_code;
	char *fault_string;
} opfault;

extern struct cwmp cwmp_main;
extern long int flashsize;

int cwmp_exit(void);
void add_dm_parameter_to_list(struct list_head *head, char *param_name, char *param_data, char *param_type, int notification, bool writable);
void delete_dm_parameter_from_list(struct cwmp_dm_parameter *dm_parameter);
void cwmp_free_all_dm_parameter_list(struct list_head *list);
int global_env_init(int argc, char **argv, struct env *env);
void cwmp_add_list_fault_param(char *param, int fault, struct list_head *list_set_value_fault);
void cwmp_del_list_fault_param(struct cwmp_param_fault *param_fault);
void cwmp_free_all_list_param_fault(struct list_head *list_param_fault);
int cwmp_asprintf(char **s, const char *format, ...);
bool folder_exists(const char *path);
bool file_exists(const char *path);
void cwmp_reboot(char *command_key);
void cwmp_factory_reset();
void get_firewall_zone_name_by_wan_iface(char *if_wan, char **zone_name);
int update_firewall_cwmp_file(int port, char *zone_name, char *ip_addr, int ip_type);
int download_file(const char *file_path, const char *url, const char *username, const char *password);
int upload_file(const char *file_path, const char *url, const char *username, const char *password);
long int get_file_size(char *file_name);
int cwmp_check_image();
void cwmp_apply_firmware();
int opkg_install_package(char *package_path);
int copy(const char *from, const char *to);
int icwmp_rand(void);
void icwmp_srand(unsigned int seed);
#ifndef FREE
#define FREE(x)                   \
	do {                      \
		if (x) {          \
			free(x);  \
			x = NULL; \
		}                 \
	} while (0)
#endif

#endif
