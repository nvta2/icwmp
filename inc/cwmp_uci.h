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

#ifndef __CWMPUCI_H
#define __CWMPUCI_H

#include <uci.h>

//struct uci_context *cwmp_uci_ctx = ((void *)0);
#define UCI_DHCP_DISCOVERY_PATH "cwmp.acs.dhcp_discovery"
#define UCI_ACS_URL_PATH "cwmp.acs.url"
#define UCI_PERIODIC_INFORM_TIME_PATH "cwmp.acs.periodic_inform_time"
#define UCI_PERIODIC_INFORM_INTERVAL_PATH "cwmp.acs.periodic_inform_interval"
#define UCI_PERIODIC_INFORM_ENABLE_PATH "cwmp.acs.periodic_inform_enable"
#define UCI_ACS_USERID_PATH "cwmp.acs.userid"
#define UCI_ACS_PASSWD_PATH "cwmp.acs.passwd"
#define UCI_ACS_PARAMETERKEY_PATH "cwmp.acs.ParameterKey"
#define UCI_ACS_SSL_CAPATH "cwmp.acs.ssl_capath"
#define UCI_HTTPS_SSL_CAPATH "cwmp.acs.https_ssl_capath"
#define UCI_ACS_INSECURE_ENABLE "cwmp.acs.insecure_enable"
#define UCI_ACS_IPV6_ENABLE "cwmp.acs.ipv6_enable"
#define UCI_ACS_SSL_VERSION "cwmp.acs.ssl_version"
#define UCI_ACS_COMPRESSION "cwmp.acs.compression"
#define UCI_ACS_RETRY_MIN_WAIT_INTERVAL "cwmp.acs.retry_min_wait_interval"
#define HTTP_DISABLE_100CONTINUE "cwmp.acs.http_disable_100continue"
#define UCI_ACS_RETRY_INTERVAL_MULTIPLIER "cwmp.acs.retry_interval_multiplier"
#define UCI_LOG_SEVERITY_PATH "cwmp.cpe.log_severity"
#define UCI_CPE_USERID_PATH "cwmp.cpe.userid"
#define UCI_CPE_PASSWD_PATH "cwmp.cpe.passwd"
#define UCI_CPE_INTERFACE_PATH "cwmp.cpe.interface"
#define UCI_CPE_UBUS_SOCKET_PATH "cwmp.cpe.ubus_socket"
#define UCI_CPE_PORT_PATH "cwmp.cpe.port"
#define UCI_CPE_LOG_FILE_NAME "cwmp.cpe.log_file_name"
#define UCI_CPE_LOG_MAX_SIZE "cwmp.cpe.log_max_size"
#define UCI_CPE_ENABLE_STDOUT_LOG "cwmp.cpe.log_to_console"
#define UCI_CPE_ENABLE_FILE_LOG "cwmp.cpe.log_to_file"
#define UCI_CPE_ENABLE_SYSLOG "cwmp.cpe.log_to_syslog"
#define UCI_CPE_AMD_VERSION "cwmp.cpe.amd_version"
#define UCI_CPE_INSTANCE_MODE "cwmp.cpe.instance_mode"
#define UCI_CPE_SESSION_TIMEOUT "cwmp.cpe.session_timeout"
#define UCI_CPE_EXEC_DOWNLOAD "cwmp.cpe.exec_download"
#define UCI_CPE_NOTIFY_PERIODIC_ENABLE "cwmp.cpe.periodic_notify_enable"
#define UCI_CPE_NOTIFY_PERIOD "cwmp.cpe.periodic_notify_interval"
#define LW_NOTIFICATION_ENABLE "cwmp.lwn.enable"
#define LW_NOTIFICATION_HOSTNAME "cwmp.lwn.hostname"
#define LW_NOTIFICATION_PORT "cwmp.lwn.port"
#define UCI_DHCP_ACS_URL "cwmp.acs.dhcp_url"

#define UCI_CONFIG_DIR "/etc/config/"
#define LIB_DB_CONFIG "/lib/db/config"
#define ETC_DB_CONFIG "/etc/board-db/config"
#define VARSTATE_CONFIG "/var/state"

typedef enum uci_config_action {
	CWMP_CMD_SET,
	CWMP_CMD_SET_STATE,
	CWMP_CMD_ADD_LIST,
	CWMP_CMD_DEL,
} uci_config_action;

enum uci_paths_types
{
	UCI_STANDARD_CONFIG,
	UCI_DB_CONFIG,
	UCI_BOARD_DB_CONFIG,
	UCI_VARSTATE_CONFIG,
};

enum cwmp_uci_cmp
{
	CWMP_CMP_SECTION,
	CWMP_CMP_OPTION_EQUAL,
	CWMP_CMP_OPTION_REGEX,
	CWMP_CMP_OPTION_CONTAINING,
	CWMP_CMP_OPTION_CONT_WORD,
	CWMP_CMP_LIST_CONTAINING,
	CWMP_CMP_FILTER_FUNC
};

enum cwmp_uci_walk
{
	CWMP_GET_FIRST_SECTION,
	CWMP_GET_NEXT_SECTION
};

struct config_uci_list {
	struct list_head list;
	char *value;
};

struct uci_paths {
	char *conf_dir;
	char *save_dir;
};

int cwmp_uci_init(int uci_path_type);
void cwmp_uci_exit(void);
int cwmp_uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value);
int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct list_head *list);
int uci_get_state_value(char *cmd, char **value);
int uci_set_state_value(char *cmd);
int uci_set_value(char *cmd);
int uci_get_value(char *cmd, char **value);
char *cwmp_db_get_value_string(char *package, char *section, char *option);
struct uci_section *cwmp_uci_walk_section(char *package, char *stype, void *arg1, void *arg2, int cmp, int (*filter)(struct uci_section *s, void *value), struct uci_section *prev_section, int walk);
int cwmp_uci_get_value_by_section_string(struct uci_section *s, char *option, char **value);
int cwmp_uci_get_option_value_string(char *package, char *section, char *option, int uci_type, char **value);

#define cwmp_uci_path_foreach_option_eq(package, stype, option, val, section) \
	for (section = cwmp_uci_walk_section(package, stype, option, val, CWMP_CMP_OPTION_EQUAL, NULL, NULL, CWMP_GET_FIRST_SECTION); section != NULL; section = cwmp_uci_walk_section(package, stype, option, val, CWMP_CMP_OPTION_EQUAL, NULL, section, CWMP_GET_NEXT_SECTION))

#define cwmp_uci_path_foreach_sections(package, stype, section) for (section = cwmp_uci_walk_section(package, stype, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION); section != NULL; section = cwmp_uci_walk_section(package, stype, NULL, NULL, CMP_SECTION, NULL, section, GET_NEXT_SECTION))
#endif
