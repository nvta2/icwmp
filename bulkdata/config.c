/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*				Omar Kallel <omar.kallel@pivasoftware.com>
*/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libbbfdm/deviceinfo.h>

#include "log.h"
#include "config.h"
#include "buci.h"
#include "common.h"
#include "bulkdata.h"

int get_log_level_config(struct bulkdata *bulkdata)
{
	char *value = NULL;

	buci_init();
	value = buci_get_value("cwmp_bulkdata", "bulkdata", "log_level");
	if(value != NULL && *value != '\0')
		bulkdata->log_level = atoi(value);
	else
		bulkdata->log_level = DEFAULT_LOGLEVEL;
	bulkdata_log(SDEBUG,"Log Level of Bulkdata is : %d", bulkdata->log_level);
	buci_fini();
	return 0;
}

int get_amd_version_config(struct bulkdata *bulkdata)
{
	char *value = NULL;

	buci_init();
	value = buci_get_value("cwmp", "cpe", "amd_version");
	if(value != NULL && *value != '\0')
		bulkdata->amd_version = atoi(value);
	else
		bulkdata->amd_version = DEFAULT_AMD_VERSION;
	bulkdata_log(SDEBUG,"CWMP Amendment Version is : %d", bulkdata->amd_version);
	buci_fini();
	return 0;
}

int get_instance_mode_config(struct bulkdata *bulkdata)
{
	char *value = NULL;

	buci_init();
	value = buci_get_value("cwmp", "cpe", "instance_mode");
	if(value != NULL && *value != '\0') {
		if(!strcmp(value, "InstanceNumber"))
			bulkdata->instance_mode = INSTANCE_MODE_NUMBER;
		else
			bulkdata->instance_mode = INSTANCE_MODE_ALIAS;
	}
	else
		bulkdata->instance_mode = DEFAULT_INSTANCE_MODE;
	bulkdata_log(SDEBUG,"CWMP Instance Mode is : %d", bulkdata->instance_mode);
	buci_fini();

	return 0;
}

int get_device_id_config(struct bulkdata *bulkdata)
{
	struct dmctx dmctx = {0};

	bulkdata_dm_ctx_init(&dmctx);
	bulkdata->device_id.manufacturer_oui = strdup(get_deviceid_manufactureroui());
	bulkdata->device_id.product_class = strdup(get_deviceid_productclass());
	bulkdata->device_id.serial_number = strdup(get_deviceid_serialnumber());
	bulkdata_dm_ctx_clean(&dmctx);

	return 0;
}

int load_profile_config(struct bulkdata *bulkdata, struct uci_section *s, int i)
{
	char *value = NULL;

	value = buci_get_value_bysection(s, "profile_id");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].profile_id = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The profile_id of profile_id %d is : %d", i, bulkdata->profile[i].profile_id);
	} else
		return -1;

	value = buci_get_value_bysection(s, "nbre_of_retained_failed_reports");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].nbre_of_retained_failed_reports = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The nombre of retained failed reports of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].nbre_of_retained_failed_reports);
	}

	value = buci_get_value_bysection(s, "protocol");
	if(value != NULL && *value != '\0' && strcasecmp(value, "http")==0) {
		bulkdata->profile[i].protocol = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The protocol of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].protocol);
	} else
		return -1;
	
	value = buci_get_value_bysection(s, "encoding_type");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].encoding_type = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The encoding type of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].encoding_type);
	} else
		return -1;

	value = buci_get_value_bysection(s, "reporting_interval");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].reporting_interval = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The reporting interval of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].reporting_interval);
	} else
		return -1;

	value = buci_get_value_bysection(s, "time_reference");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].time_reference = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The time reference of profile_id %d is : %ld", bulkdata->profile[i].profile_id, bulkdata->profile[i].time_reference);
	} else
		return -1;

	value = buci_get_value_bysection(s, "csv_encoding_field_separator");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].csv_encoding_field_separator = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The csv encoding field separator of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].csv_encoding_field_separator);
	} else
		return -1;

	value = buci_get_value_bysection(s, "csv_encoding_row_separator");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].csv_encoding_row_separator = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The csv encoding row separator of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].csv_encoding_row_separator);
	} else
		return -1;

	value = buci_get_value_bysection(s, "csv_encoding_escape_character");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].csv_encoding_escape_character = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The csv encoding escape character of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].csv_encoding_escape_character);
	}

	value = buci_get_value_bysection(s, "csv_encoding_report_format");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].csv_encoding_report_format = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The csv encoding report format of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].csv_encoding_report_format);
	} else
		return -1;

	value = buci_get_value_bysection(s, "csv_encoding_row_time_stamp");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].csv_encoding_row_time_stamp = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The csv encoding row time stamp of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].csv_encoding_row_time_stamp);
	} else
		return -1;

	value = buci_get_value_bysection(s, "json_encoding_report_format");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].json_encoding_report_format = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The json encoding report format of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].json_encoding_report_format);
	} else
		return -1;

	value = buci_get_value_bysection(s, "json_encoding_report_time_stamp");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].json_encoding_report_time_stamp = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The json encoding report time stamp of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].json_encoding_report_time_stamp);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_url");
	if(value != NULL && *value != '\0') {
		char *url = NULL;
		asprintf(&url, "%s?oui=%s&pc=%s&sn=%s", value, bulkdata->device_id.manufacturer_oui, bulkdata->device_id.serial_number, bulkdata->device_id.serial_number);
		bulkdata->profile[i].http_url = strdup(url);
		free(url);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP url of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_url);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_username");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_username = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP username of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_username);
	} else {
		bulkdata->profile[i].http_username = NULL;
	}

	value = buci_get_value_bysection(s, "http_password");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_password = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP password of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_password);
	} else {
		bulkdata->profile[i].http_password = NULL;
	}

	value = buci_get_value_bysection(s, "http_compression");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_compression = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP compression of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_compression);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_method");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_method = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP method of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_method);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_use_date_header");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_use_date_header = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP use date header of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_use_date_header);
	}

	value = buci_get_value_bysection(s, "http_retry_enable");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_retry_enable = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP retry enable of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_retry_enable);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_retry_minimum_wait_interval");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_retry_minimum_wait_interval = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP retry minimum wait interval of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_retry_minimum_wait_interval);
	}

	value = buci_get_value_bysection(s, "http_retry_interval_multiplier");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_retry_interval_multiplier = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP retry interval multiplier of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_retry_interval_multiplier);
	}

	value = buci_get_value_bysection(s, "http_persist_across_reboot");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_persist_across_reboot = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP persist across reboot of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_persist_across_reboot);
	} else
		return -1;

	value = buci_get_value_bysection(s, "http_ssl_capath");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_ssl_capath = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP ssl capath of profile_id %d is : %s", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_ssl_capath);
	} else {
		bulkdata->profile[i].http_ssl_capath = NULL;
	}

	value = buci_get_value_bysection(s, "http_insecure_enable");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].http_insecure_enable = atoi(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP insecure enable of profile_id %d is : %d", bulkdata->profile[i].profile_id, bulkdata->profile[i].http_insecure_enable);
	}

	bulkdata->profile[i].retry_count = 0;
	bulkdata->profile[i].nbre_failed_reports = 0;

	return 0;
}

int get_profiles_enable(struct bulkdata *bulkdata)
{
	struct uci_section *s;
	char *enable;
	int i = 0, nbr_profiles = 0;

	buci_init();

	buci_foreach_section("cwmp_bulkdata", "profile", s) {
		enable = buci_get_value_bysection(s, "enable");
		if(strcmp(enable, "1") == 0) {
			nbr_profiles++;
		}
	}

	if(nbr_profiles != 0)
		bulkdata->profile = calloc(2, sizeof(struct profile));
	
	buci_foreach_section("cwmp_bulkdata", "profile", s) {
		enable = buci_get_value_bysection(s, "enable");
		if(strcmp(enable, "1") == 0) {
			if(load_profile_config(bulkdata, s, i) == -1) {
				bulkdata_log(SCRIT,"Not able to start bulkdata: some required bulkdata configurations in profile must be set");
				return -1;
			}
			i++;
		}
	}
	profiles_number = nbr_profiles;

	buci_fini();
	return 0;
}

int load_profile_parameter_config(struct bulkdata *bulkdata, struct uci_section *s, int i, int j)
{
	char *value = NULL;

	value = buci_get_value_bysection(s, "name");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].profile_parameter[j].name = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The parameter name %d of profile_id %d is : %s", j+1, bulkdata->profile[i].profile_id, bulkdata->profile[i].profile_parameter[j].name);
	} else {
		bulkdata->profile[i].profile_parameter[j].name = NULL;
	}

	value = buci_get_value_bysection(s, "reference");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].profile_parameter[j].reference = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The parameter reference %d of profile_id %d is : %s", j+1, bulkdata->profile[i].profile_id, bulkdata->profile[i].profile_parameter[j].reference);
	} else
		return -1;

	return 0;
}

int get_profiles_parameters(struct bulkdata *bulkdata)
{
	struct uci_section *s;
	char *profile_id;
	int i, j, nbr_profile_parameters;

	buci_init();

	for (i = 0; i < profiles_number; i++) {
		j = 0;
		nbr_profile_parameters = 0;
		buci_foreach_section("cwmp_bulkdata", "profile_parameter", s) {
			profile_id = buci_get_value_bysection(s, "profile_id");
			if(bulkdata->profile[i].profile_id != atoi(profile_id))
				continue;
			nbr_profile_parameters++;
			if(nbr_profile_parameters == 1) {
				bulkdata->profile[i].profile_parameter = calloc(1, sizeof(struct profile_parameter));
			} else {
				bulkdata->profile[i].profile_parameter = realloc(bulkdata->profile[i].profile_parameter, nbr_profile_parameters * sizeof(struct profile_parameter));
			}
			if(load_profile_parameter_config(bulkdata, s, i, j) == -1) {
				bulkdata_log(SCRIT,"Not able to start bulkdata: some required bulkdata configurations in profile_parameter must be set");
				return -1;
			}
			j++;
		}
		bulkdata->profile[i].profile_parameter_number = nbr_profile_parameters;
	}

	buci_fini();
	return 0;
}

int load_profile_http_request_uri_parameter_config(struct bulkdata *bulkdata, struct uci_section *s, int i, int j)
{
	char *value = NULL;

	value = buci_get_value_bysection(s, "name");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].profile_http_uri_parameter[j].name = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP resuest URI parameter name %d of profile_id %d is : %s", j+1, bulkdata->profile[i].profile_id, bulkdata->profile[i].profile_http_uri_parameter[j].name);
	} else
		return -1;

	value = buci_get_value_bysection(s, "reference");
	if(value != NULL && *value != '\0') {
		bulkdata->profile[i].profile_http_uri_parameter[j].reference = strdup(value);
		value = NULL;
		bulkdata_log(SDEBUG,"The HTTP resuest URI parameter reference %d of profile_id %d is : %s", j+1, bulkdata->profile[i].profile_id, bulkdata->profile[i].profile_http_uri_parameter[j].reference);
	} else
		return -1;

	return 0;
}

int get_profile_http_request_uri_parameter(struct bulkdata *bulkdata)
{
	struct uci_section *s;
	char *profile_id;
	int i, j, nbr_profile_http_request_uri_parameter;

	buci_init();

	for (i = 0; i < profiles_number; i++) {
		j = 0;
		nbr_profile_http_request_uri_parameter = 0;
		buci_foreach_section("cwmp_bulkdata", "profile_http_request_uri_parameter", s) {
			profile_id = buci_get_value_bysection(s, "profile_id");
			if(bulkdata->profile[i].profile_id != atoi(profile_id))
				continue;
			nbr_profile_http_request_uri_parameter++;
			if(nbr_profile_http_request_uri_parameter == 1) {
				bulkdata->profile[i].profile_http_uri_parameter = calloc(1, sizeof(struct profile_http_request_uri_parameter));
			} else {
				bulkdata->profile[i].profile_http_uri_parameter = realloc(bulkdata->profile[i].profile_http_uri_parameter, nbr_profile_http_request_uri_parameter * sizeof(struct profile_http_request_uri_parameter));
			}
			if(load_profile_http_request_uri_parameter_config(bulkdata, s, i, j)== -1) {
				bulkdata_log(SCRIT,"Not able to start bulkdata: some required bulkdata configurations in profile_http_request_uri_parameter must be set");
				return -1;
			}
			j++;
		}
		bulkdata->profile[i].profile_http_request_uri_parameter_number = nbr_profile_http_request_uri_parameter;
	}

	buci_fini();
	return 0;
}

int bulkdata_config_init(struct bulkdata *bulkdata)
{
	get_log_level_config(bulkdata);
	get_amd_version_config(bulkdata);
	get_instance_mode_config(bulkdata);
	get_device_id_config(bulkdata);
	if (get_profiles_enable(bulkdata) == -1)
		return -1;
	if (get_profiles_parameters(bulkdata) == -1)
		return -1;
	if (get_profile_http_request_uri_parameter(bulkdata) == -1)
		return -1;
	return 0;
}

int free_device_id_config(struct bulkdata *bulkdata)
{
	FREE(bulkdata->device_id.manufacturer_oui);
	FREE(bulkdata->device_id.product_class);
	FREE(bulkdata->device_id.serial_number);
	return 0;
}

int free_profiles_enable(struct bulkdata *bulkdata)
{
	for(int i = 0; i < profiles_number; i++) {
		FREE(bulkdata->profile[i].protocol);
		FREE(bulkdata->profile[i].encoding_type);
		FREE(bulkdata->profile[i].csv_encoding_field_separator);
		FREE(bulkdata->profile[i].csv_encoding_row_separator);
		FREE(bulkdata->profile[i].csv_encoding_escape_character);
		FREE(bulkdata->profile[i].csv_encoding_report_format);
		FREE(bulkdata->profile[i].csv_encoding_row_time_stamp);
		FREE(bulkdata->profile[i].json_encoding_report_format);
		FREE(bulkdata->profile[i].json_encoding_report_time_stamp);
		FREE(bulkdata->profile[i].http_url);
		FREE(bulkdata->profile[i].http_username);
		FREE(bulkdata->profile[i].http_password);
		FREE(bulkdata->profile[i].http_compression);
		FREE(bulkdata->profile[i].http_method);
		FREE(bulkdata->profile[i].http_ssl_capath);
	}
	FREE(bulkdata->profile);
	return 0;
}

int free_profiles_parameters(struct bulkdata *bulkdata)
{
	for(int i = 0; i < profiles_number; i++) {
		for(int j = 0; j < bulkdata->profile[i].profile_parameter_number; j++) {
			FREE(bulkdata->profile[i].profile_parameter[j].name);
			FREE(bulkdata->profile[i].profile_parameter[j].reference);
		}
		FREE(bulkdata->profile[i].profile_parameter);
	}
	return 0;
}

int free_profile_http_request_uri_parameter(struct bulkdata *bulkdata)
{
	for(int i = 0; i < profiles_number; i++) {
		for(int j = 0; j < bulkdata->profile[i].profile_http_request_uri_parameter_number; j++) {
			FREE(bulkdata->profile[i].profile_http_uri_parameter[j].name);
			FREE(bulkdata->profile[i].profile_http_uri_parameter[j].reference);
		}
		FREE(bulkdata->profile[i].profile_http_uri_parameter);
	}
	return 0;
}

int bulkdata_config_fini(struct bulkdata *bulkdata)
{
	free_device_id_config(bulkdata);
	free_profiles_parameters(bulkdata);
	free_profile_http_request_uri_parameter(bulkdata);
	free_profiles_enable(bulkdata);
	return 0;
}
