/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include "config.h"
#include "cwmp_uci.h"
#include "log.h"
#include "reboot.h"

pthread_mutex_t mutex_config_load = PTHREAD_MUTEX_INITIALIZER;

static int check_global_config(struct config *conf)
{
	if (conf->acsurl == NULL) {
		conf->acsurl = strdup(DEFAULT_ACSURL);
	}
	return CWMP_OK;
}

static time_t convert_datetime_to_timestamp(char *value)
{
	struct tm tm = { 0 };
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

	sscanf(value, "%4d-%2d-%2dT%2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec);
	tm.tm_year = year - 1900; /* years since 1900 */
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = min;
	tm.tm_sec = sec;

	return mktime(&tm);
}

int get_global_config(struct config *conf)
{
	int error, error2, error3;
	char *value = NULL, *value2 = NULL, *value3 = NULL;

	if ((error = uci_get_value(UCI_CPE_LOG_FILE_NAME, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_log_file_name(value);
			FREE(value);
		} else
			log_set_log_file_name(NULL);
	} else {
		log_set_log_file_name(NULL);
	}

	if ((error = uci_get_value(UCI_CPE_LOG_MAX_SIZE, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_file_max_size(value);
			FREE(value);
		} else
			log_set_file_max_size(NULL);
	} else {
		log_set_file_max_size(NULL);
	}

	if ((error = uci_get_value(UCI_CPE_ENABLE_STDOUT_LOG, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_on_console(value);
			FREE(value);
		}
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_ENABLE_FILE_LOG, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_on_file(value);
			FREE(value);
		}
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_LOG_SEVERITY_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_severity_idx(value);
			FREE(value);
		}
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_ENABLE_SYSLOG, &value)) == CWMP_OK) {
		if (value != NULL) {
			log_set_on_syslog(value);
			FREE(value);
		}
	} else {
		return error;
	}

	error = uci_get_value(UCI_DHCP_DISCOVERY_PATH, &value);
	error2 = uci_get_value(UCI_ACS_URL_PATH, &value2);
	error3 = uci_get_state_value(UCI_DHCP_ACS_URL, &value3);

	if ((((error == CWMP_OK) && (value != NULL) && (strcmp(value, "enable") == 0)) || ((error2 == CWMP_OK) && ((value2 == NULL) || (value2[0] == 0)))) && ((error3 == CWMP_OK) && (value3 != NULL) && (value3[0] != 0))) {
		FREE(conf->acsurl);
		conf->acsurl = strdup(value3);
	} else if ((error2 == CWMP_OK) && (value2 != NULL) && (value2[0] != 0)) {
		FREE(conf->acsurl);
		conf->acsurl = strdup(value2);
	}

	FREE(value);
	FREE(value2);
	FREE(value3);

	if ((error = uci_get_value(UCI_ACS_USERID_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->acs_userid);
			conf->acs_userid = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs username: %s", conf->acs_userid ? conf->acs_userid : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_PASSWD_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->acs_passwd);
			conf->acs_passwd = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs password: %s", conf->acs_passwd ? conf->acs_passwd : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_AMD_VERSION, &value)) == CWMP_OK) {
		int a = 0;

		conf->amd_version = DEFAULT_AMD_VERSION;
		if (value != NULL) {
			a = atoi(value);
			if (a >= 1) {
				conf->amd_version = a;
			}
			FREE(value);
		}
		conf->supported_amd_version = conf->amd_version;

		CWMP_LOG(DEBUG, "CWMP CONFIG - amendement version: %d", conf->amd_version);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_COMPRESSION, &value)) == CWMP_OK) {
		conf->compression = COMP_NONE;
		if (conf->amd_version >= AMD_5 && value != NULL) {
			if (0 == strcasecmp(value, "gzip")) {
				conf->compression = COMP_GZIP;
			} else if (0 == strcasecmp(value, "deflate")) {
				conf->compression = COMP_DEFLATE;
			} else {
				conf->compression = COMP_NONE;
			}
		}
		FREE(value);

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs compression: %d", conf->compression);
	} else {
		conf->compression = COMP_NONE;
	}

	if ((error = uci_get_value(UCI_ACS_RETRY_MIN_WAIT_INTERVAL, &value)) == CWMP_OK) {
		conf->retry_min_wait_interval = DEFAULT_RETRY_MINIMUM_WAIT_INTERVAL;
		if (conf->amd_version >= AMD_3 && value != NULL) {
			int a = atoi(value);
			if (a <= 65535 || a >= 1) {
				conf->retry_min_wait_interval = a;
			}
		}
		FREE(value);

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs retry minimum wait interval: %d", conf->retry_min_wait_interval);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_RETRY_INTERVAL_MULTIPLIER, &value)) == CWMP_OK) {
		conf->retry_interval_multiplier = DEFAULT_RETRY_INTERVAL_MULTIPLIER;
		if (conf->amd_version >= AMD_3 && value != NULL) {
			int a = atoi(value);
			if (a <= 65535 || a >= 1000) {
				conf->retry_interval_multiplier = a;
			}
		}
		FREE(value);

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs retry interval: %d", conf->retry_interval_multiplier);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_SSL_CAPATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->acs_ssl_capath);
			conf->acs_ssl_capath = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs ssl cpath: %s", conf->acs_ssl_capath ? conf->acs_ssl_capath : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_HTTPS_SSL_CAPATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->https_ssl_capath);
			conf->https_ssl_capath = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - https ssl cpath: %s", conf->https_ssl_capath ? conf->https_ssl_capath : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(HTTP_DISABLE_100CONTINUE, &value)) == CWMP_OK) {
		if (value != NULL) {
			if ((strcasecmp(value, "true") == 0) || (strcmp(value, "1") == 0))
				conf->http_disable_100continue = true;
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - http disable 100continue: %d", conf->http_disable_100continue);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_INSECURE_ENABLE, &value)) == CWMP_OK) {
		if (value != NULL) {
			if ((strcasecmp(value, "true") == 0) || (strcmp(value, "1") == 0)) {
				conf->insecure_enable = true;
			}
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - acs insecure enable: %d", conf->insecure_enable);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_ACS_IPV6_ENABLE, &value)) == CWMP_OK) {
		if (value != NULL) {
			if ((strcasecmp(value, "true") == 0) || (strcmp(value, "1") == 0)) {
				conf->ipv6_enable = true;
			}
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - ipv6 enable: %d", conf->ipv6_enable);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_INTERFACE_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->interface);
			conf->interface = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - cpe interface: %s", conf->interface ? conf->interface : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_USERID_PATH, &value)) == CWMP_OK) {
		FREE(conf->cpe_userid);
		if (value != NULL) {
			conf->cpe_userid = strdup(value);
			FREE(value);
		} else {
			conf->cpe_userid = strdup("");
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - cpe username: %s", conf->cpe_userid ? conf->cpe_userid : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_PASSWD_PATH, &value)) == CWMP_OK) {
		FREE(conf->cpe_passwd);
		if (value != NULL) {
			conf->cpe_passwd = strdup(value);
			FREE(value);
		} else {
			conf->cpe_passwd = strdup("");
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - cpe password: %s", conf->cpe_passwd ? conf->cpe_passwd : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_UBUS_SOCKET_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			FREE(conf->ubus_socket);
			conf->ubus_socket = strdup(value);
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - ubus socket: %s", conf->ubus_socket ? conf->ubus_socket : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_PORT_PATH, &value)) == CWMP_OK) {
		int a = 0;

		if (value != NULL) {
			a = atoi(value);
			FREE(value);
		}

		if (a == 0) {
			CWMP_LOG(INFO, "Set the connection request port to the default value: %d", DEFAULT_CONNECTION_REQUEST_PORT);
			conf->connection_request_port = DEFAULT_CONNECTION_REQUEST_PORT;
		} else {
			conf->connection_request_port = a;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - connection request port: %d", conf->connection_request_port);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_DEFAULT_WAN_IFACE, &value)) == CWMP_OK) {
		FREE(conf->default_wan_iface);
		if (value != NULL) {
			conf->default_wan_iface = strdup(value);
			FREE(value);
		} else {
			conf->default_wan_iface = strdup("wan");
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - default wan interface: %s", conf->default_wan_iface ? conf->default_wan_iface : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_CRPATH_PATH, &value)) == CWMP_OK) {
		FREE(conf->connection_request_path);
		if (value == NULL)
			conf->connection_request_path = strdup("/");
		else {
			if (value[0] == '/')
				conf->connection_request_path = strdup(value);
			else {
				char cr_path[512];
				snprintf(cr_path, sizeof(cr_path), "/%s", value);
				conf->connection_request_path = strdup(cr_path);
			}
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - connection request: %s", conf->connection_request_path ? conf->connection_request_path : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_NOTIFY_PERIODIC_ENABLE, &value)) == CWMP_OK) {
		bool a = true;
		if (value != NULL) {
			if ((strcasecmp(value, "FALSE") == 0) || (strcmp(value, "0") == 0)) {
				a = false;
			} else {
				a = true;
			}
			FREE(value);
		}
		conf->periodic_notify_enable = a;

		CWMP_LOG(DEBUG, "CWMP CONFIG - periodic notifiy enable: %d", conf->periodic_notify_enable);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_NOTIFY_PERIOD, &value)) == CWMP_OK) {
		int a = 0;

		if (value != NULL) {
			a = atoi(value);
			FREE(value);
		}

		if (a == 0) {
			CWMP_LOG(INFO, "Set notify period to the default value: %d", DEFAULT_NOTIFY_PERIOD);
			conf->periodic_notify_interval = DEFAULT_NOTIFY_PERIOD;
		} else {
			conf->periodic_notify_interval = a;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - periodic notifiy interval: %d", conf->periodic_notify_interval);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_PERIODIC_INFORM_TIME_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			conf->time = convert_datetime_to_timestamp(value);
			FREE(value);
		} else {
			conf->time = 0;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - periodic inform time: %ld", conf->time);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_PERIODIC_INFORM_INTERVAL_PATH, &value)) == CWMP_OK) {
		int a = 0;

		if (value != NULL) {
			a = atoi(value);
			FREE(value);
		}

		if (a >= PERIOD_INFORM_MIN) {
			conf->period = a;
		} else {
			CWMP_LOG(ERROR, "Period interval of periodic inform should be > %ds. Set to default: %ds", PERIOD_INFORM_MIN, PERIOD_INFORM_DEFAULT);
			conf->period = PERIOD_INFORM_DEFAULT;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - periodic inform interval: %d", conf->period);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_PERIODIC_INFORM_ENABLE_PATH, &value)) == CWMP_OK) {
		if (value != NULL) {
			if ((strcasecmp(value, "TRUE") == 0) || (strcmp(value, "1") == 0)) {
				conf->periodic_enable = true;
			} else {
				conf->periodic_enable = false;
			}
			FREE(value);
		} else {
			conf->periodic_enable = false;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - periodic inform enable: %d", conf->periodic_enable);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_INSTANCE_MODE, &value)) == CWMP_OK) {
		if (value != NULL) {
			if (0 == strcmp(value, "InstanceNumber")) {
				conf->instance_mode = INSTANCE_MODE_NUMBER;
			} else {
				conf->instance_mode = INSTANCE_MODE_ALIAS;
			}
			FREE(value);
		} else {
			conf->instance_mode = DEFAULT_INSTANCE_MODE;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - instance mode: %d (InstanceNumber=0, InstanceAlias=1)", conf->instance_mode);
	} else {
		return error;
	}

	if ((error = uci_get_value(UCI_CPE_SESSION_TIMEOUT, &value)) == CWMP_OK) {
		int a = 0;

		conf->session_timeout = DEFAULT_SESSION_TIMEOUT;
		if (value != NULL) {
			a = atoi(value);
			if (a >= 1) {
				conf->session_timeout = a;
			}
			FREE(value);
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - session timeout: %d", conf->session_timeout);
	} else {
		return error;
	}

	if ((error = uci_get_value(LW_NOTIFICATION_ENABLE, &value)) == CWMP_OK) {
		if (value != NULL) {
			if ((strcasecmp(value, "TRUE") == 0) || (strcmp(value, "1") == 0)) {
				conf->lw_notification_enable = true;
			} else {
				conf->lw_notification_enable = false;
			}
			FREE(value);
		} else {
			conf->lw_notification_enable = false;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - lightweight notification enable: %d", conf->lw_notification_enable);
	} else {
		return error;
	}

	if ((error = uci_get_value(LW_NOTIFICATION_HOSTNAME, &value)) == CWMP_OK) {
		FREE(conf->lw_notification_hostname);
		if (value != NULL) {
			conf->lw_notification_hostname = strdup(value);
			FREE(value);
		} else {
			conf->lw_notification_hostname = strdup(conf->acsurl ? conf->acsurl : "");
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - lightweight notification hostname: %s", conf->lw_notification_hostname ? conf->lw_notification_hostname : "");
	} else {
		return error;
	}

	if ((error = uci_get_value(LW_NOTIFICATION_PORT, &value)) == CWMP_OK) {
		int a = 0;

		if (value != NULL) {
			a = atoi(value);
			conf->lw_notification_port = a;
			FREE(value);
		} else {
			conf->lw_notification_port = DEFAULT_LWN_PORT;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - lightweight notification port: %d", conf->lw_notification_port);
	} else {
		return error;
	}

	if (uci_get_value(UCI_CPE_SCHEDULE_REBOOT, &value) == CWMP_OK) {
		if (value != NULL) {
			conf->schedule_reboot = convert_datetime_to_timestamp(value);
			FREE(value);
		} else {
			conf->schedule_reboot = 0;
		}

		CWMP_LOG(DEBUG, "CWMP CONFIG - schedule reboot: %ld", conf->schedule_reboot);
	} else {
		return error;
	}

	if (uci_get_value(UCI_CPE_DELAY_REBOOT, &value) == CWMP_OK) {
		int delay = -1;

		if (value != NULL) {
			delay = atoi(value);
			FREE(value);
		}

		conf->delay_reboot = delay;

		CWMP_LOG(DEBUG, "CWMP CONFIG - delay reboot: %d", conf->delay_reboot);
	} else {
		return error;
	}

	if (uci_get_value(UCI_CPE_FORCED_INFORM_JSON, &value) == CWMP_OK) {
		FREE(conf->forced_inform_json_file);
		if (value != NULL) {
			conf->forced_inform_json_file = strdup(value);
			FREE(value);
		} else {
			conf->forced_inform_json_file = NULL;
		}
		if (conf->forced_inform_json_file)
			CWMP_LOG(DEBUG, "CWMP CONFIG - cpe forced inform json file: %s", conf->forced_inform_json_file);
	}
	if (uci_get_value(UCI_CPE_BOOT_INFORM_JSON, &value) == CWMP_OK) {
		FREE(conf->boot_inform_json_file);
		if (value != NULL) {
			conf->boot_inform_json_file = strdup(value);
			FREE(value);
		} else {
			conf->boot_inform_json_file = NULL;
		}
		if (conf->boot_inform_json_file)
			CWMP_LOG(DEBUG, "CWMP CONFIG - cpe boot inform json file: %s", conf->forced_inform_json_file);
	}
	if (uci_get_value(UCI_CPE_JSON_CUSTOM_NOTIFY_FILE, &value) == CWMP_OK) {
		FREE(conf->custom_notify_json);
		if (value != NULL) {
			conf->custom_notify_json = strdup(value);
			FREE(value);
		} else {
			conf->custom_notify_json = NULL;
		}
		if (conf->custom_notify_json)
			CWMP_LOG(DEBUG, "CWMP CONFIG - cpe json custom notify file: %s", conf->custom_notify_json);
	}
	return CWMP_OK;
}

int global_conf_init(struct cwmp *cwmp)
{
	int error = CWMP_OK;

	pthread_mutex_lock(&mutex_config_load);

	if ((error = get_global_config(&(cwmp->conf))))
		goto end;

	if ((error = check_global_config(&(cwmp->conf))))
		goto end;

	/* Launch reboot methods if needed */
	launch_reboot_methods(cwmp);

end:
	pthread_mutex_unlock(&mutex_config_load);

	return error;
}

int cwmp_get_deviceid(struct cwmp *cwmp)
{
	if (folder_exists("/lib/db/config"))
		cwmp_uci_init(UCI_DB_CONFIG);
	else
		cwmp_uci_init(UCI_BOARD_DB_CONFIG);
	cwmp->deviceid.manufacturer = strdup(cwmp_db_get_value_string("device", "deviceinfo", "Manufacturer"));
	cwmp->deviceid.serialnumber = strdup(cwmp_db_get_value_string("device", "deviceinfo", "SerialNumber"));
	cwmp->deviceid.productclass = strdup(cwmp_db_get_value_string("device", "deviceinfo", "ProductClass"));
	cwmp->deviceid.oui = strdup(cwmp_db_get_value_string("device", "deviceinfo", "ManufacturerOUI"));
	cwmp->deviceid.softwareversion = strdup(cwmp_db_get_value_string("device", "deviceinfo", "SoftwareVersion"));
	cwmp_uci_exit();
	return CWMP_OK;
}

int cwmp_config_reload(struct cwmp *cwmp)
{
	memset(&cwmp->env, 0, sizeof(struct env));

	return global_conf_init(cwmp);
}
