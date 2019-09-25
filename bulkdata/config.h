/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*
*/

#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <libubox/uloop.h>

#define DEFAULT_AMD_VERSION 2
#define DEFAULT_INSTANCE_MODE 0

typedef struct device_id {
	char *manufacturer_oui;
	char *product_class;
	char *serial_number;
} device_id;

typedef struct profile_parameter {
	int profile_id;
	char *name;
	char *reference;
} profile_parameter;

typedef struct profile_http_request_uri_parameter {
	int profile_id;
	char *name;
	char *reference;
} profile_http_request_uri_parameter;

typedef struct profile {
	struct uloop_timeout utimer;
	int profile_id;
	int nbre_of_retained_failed_reports;
	int nbre_failed_reports;
	int reporting_interval;
	int profile_parameter_number;
	int profile_http_request_uri_parameter_number;
	int http_retry_minimum_wait_interval;
	int http_retry_interval_multiplier;
	int min_retry;
	int retry_count;
	char *protocol;
	char *encoding_type;
	char *csv_encoding_field_separator;
	char *csv_encoding_row_separator;
	char *csv_encoding_escape_character;
	char *csv_encoding_report_format;
	char *csv_encoding_row_time_stamp;
	char *json_encoding_report_format;
	char *json_encoding_report_time_stamp;
	char *http_url;
	char *http_username;
	char *http_password;
	char *http_compression;
	char *http_method;
	char *http_ssl_capath;
	char *new_report;
	time_t time_reference;
	time_t next_retry;
	time_t next_period;
	bool http_persist_across_reboot;
	bool http_insecure_enable;
	bool enable;
	bool http_use_date_header;
	bool http_retry_enable;
	struct profile_parameter *profile_parameter;
	struct profile_http_request_uri_parameter *profile_http_uri_parameter;
	struct list_head *failed_reports;
} profile;

typedef struct bulkdata {
	struct device_id							device_id;
	struct profile								*profile;
	int 										log_level;
	int 										amd_version;
	unsigned int 								instance_mode;
} bulkdata;

int bulkdata_config_init(struct bulkdata *bulkdata);
int bulkdata_config_fini(struct bulkdata *bulkdata);

#ifndef FREE
#define FREE(x) do { if(x) {free(x); x = NULL;} } while (0)
#endif

#endif //__CONFIG_H





