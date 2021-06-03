/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <pthread.h>
#include <curl/curl.h>

#include "common.h"
#include "upload.h"
#include "datamodel_interface.h"
#include "download.h"
#include "log.h"
#include "cwmp_time.h"
#include "backupSession.h"
#include "cwmp_uci.h"
#include "event.h"

#define CURL_TIMEOUT 20

LIST_HEAD(list_upload);

pthread_cond_t threshold_upload;
pthread_mutex_t mutex_upload = PTHREAD_MUTEX_INITIALIZER;

int lookup_vcf_name(char *instance, char **value)
{
	char vcf_name_parameter[256];
	char *err = NULL;
	LIST_HEAD(vcf_parameters);
	snprintf(vcf_name_parameter, sizeof(vcf_name_parameter), "Device.DeviceInfo.VendorConfigFile.%s.Name", instance);
	if ((err = cwmp_get_parameter_values(vcf_name_parameter, &vcf_parameters)) != NULL)
		return -1;
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &vcf_parameters, list) {
		*value = strdup(param_value->value);
		break;
	}
	cwmp_free_all_dm_parameter_list(&vcf_parameters);
	return 0;
}

int lookup_vlf_name(char *instance, char **value)
{
	char vlf_name_parameter[256];
	char *err = NULL;
	LIST_HEAD(vlf_parameters);
	snprintf(vlf_name_parameter, sizeof(vlf_name_parameter), "Device.DeviceInfo.VendorLogFile.%s.Name", instance);
	if ((err = cwmp_get_parameter_values(vlf_name_parameter, &vlf_parameters)) != NULL)
		return -1;
	struct cwmp_dm_parameter *param_value = NULL;
	list_for_each_entry (param_value, &vlf_parameters, list) {
		*value = strdup(param_value->value);
		break;
	}
	cwmp_free_all_dm_parameter_list(&vlf_parameters);
	return 0;
}

int upload_file(const char *file_path, const char *url, const char *username, const char *password)
{
	int res_code = 0;

	CURL *curl = curl_easy_init();
	if (curl) {
		char userpass[256];

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
		snprintf(userpass, sizeof(userpass), "%s:%s", username, password);
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		FILE *fp = fopen(file_path, "rb");
		if (fp) {
			curl_easy_setopt(curl, CURLOPT_READDATA, fp);
			curl_easy_perform(curl);
			fclose(fp);
		}

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
		curl_easy_cleanup(curl);
	}

	return res_code;
}

int cwmp_launch_upload(struct upload *pupload, struct transfer_complete **ptransfer_complete)
{
	int error = FAULT_CPE_NO_FAULT;
	char *upload_startTime;
	struct transfer_complete *p;
	char *name = "";
	upload_startTime = mix_get_time();
	char file_path[128];
	bkp_session_delete_upload(pupload);
	bkp_session_save();

	if (pupload->file_type[0] == '1' || pupload->file_type[0] == '3') {
		if (pupload->f_instance && isdigit(pupload->f_instance[0])) {
			lookup_vcf_name(pupload->f_instance, &name);
			if (name && strlen(name) > 0) {
				snprintf(file_path, sizeof(file_path), "/tmp/%s", name);
				cwmp_uci_export_package(name, file_path);
				FREE(name);
			} else {
				error = FAULT_CPE_UPLOAD_FAILURE;
				goto end_upload;
			}
		} else {
			snprintf(file_path, sizeof(file_path), "/tmp/all_configs");
			cwmp_uci_export(file_path);
		}
	} else {
		if (pupload->f_instance && isdigit(pupload->f_instance[0])) {
			lookup_vlf_name(pupload->f_instance, &name);
			if (name && strlen(name) > 0) {
				snprintf(file_path, sizeof(file_path), "/tmp/%s", name);
				copy(name, file_path);
				FREE(name);
			} else
				error = FAULT_CPE_UPLOAD_FAILURE;

		} else
			error = FAULT_CPE_UPLOAD_FAILURE;
	}
	if (error != FAULT_CPE_NO_FAULT || strlen(file_path) <= 0) {
		error = FAULT_CPE_UPLOAD_FAILURE;
		goto end_upload;
	}

	if (upload_file(file_path, pupload->url, pupload->username, pupload->password) == 200)
		error = FAULT_CPE_NO_FAULT;
	else
		error = FAULT_CPE_UPLOAD_FAILURE;
	remove(file_path);

end_upload:
	p = calloc(1, sizeof(struct transfer_complete));
	if (p == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		return error;
	}

	p->command_key = pupload->command_key ? strdup(pupload->command_key) : strdup("");
	p->start_time = strdup(upload_startTime);
	p->complete_time = strdup(mix_get_time());
	if (error != FAULT_CPE_NO_FAULT) {
		p->fault_code = error;
	}

	*ptransfer_complete = p;
	return error;
}

void *thread_cwmp_rpc_cpe_upload(void *v)
{
	struct cwmp *cwmp = (struct cwmp *)v;
	struct upload *pupload;
	struct timespec upload_timeout = { 0, 0 };
	time_t current_time, stime;
	int error = FAULT_CPE_NO_FAULT;
	struct transfer_complete *ptransfer_complete;
	long int time_of_grace = 3600, timeout;

	for (;;) {
		if (list_upload.next != &(list_upload)) {
			pupload = list_entry(list_upload.next, struct upload, list);
			stime = pupload->scheduled_time;
			current_time = time(NULL);
			if (pupload->scheduled_time != 0)
				timeout = current_time - pupload->scheduled_time;
			else
				timeout = 0;
			if ((timeout >= 0) && (timeout > time_of_grace)) {
				pthread_mutex_lock(&mutex_upload);
				bkp_session_delete_upload(pupload);
				ptransfer_complete = calloc(1, sizeof(struct transfer_complete));
				if (ptransfer_complete != NULL) {
					error = FAULT_CPE_DOWNLOAD_FAILURE;

					ptransfer_complete->command_key = strdup(pupload->command_key);
					ptransfer_complete->start_time = strdup(mix_get_time());
					ptransfer_complete->complete_time = strdup(ptransfer_complete->start_time);
					ptransfer_complete->fault_code = error;
					ptransfer_complete->type = TYPE_UPLOAD;
					bkp_session_insert_transfer_complete(ptransfer_complete);
					cwmp_root_cause_transfer_complete(cwmp, ptransfer_complete);
				}
				list_del(&(pupload->list));
				if (pupload->scheduled_time != 0)
					count_download_queue--;
				cwmp_free_upload_request(pupload);
				pthread_mutex_unlock(&mutex_download);
				continue;
			}
			if ((timeout >= 0) && (timeout <= time_of_grace)) {
				pthread_mutex_lock(&(cwmp->mutex_session_send));
				CWMP_LOG(INFO, "Launch upload file %s", pupload->url);
				error = cwmp_launch_upload(pupload, &ptransfer_complete);
				if (error != FAULT_CPE_NO_FAULT) {
					bkp_session_insert_transfer_complete(ptransfer_complete);
					bkp_session_save();
					cwmp_root_cause_transfer_complete(cwmp, ptransfer_complete);
					bkp_session_delete_transfer_complete(ptransfer_complete);
				} else {
					bkp_session_delete_transfer_complete(ptransfer_complete);
					ptransfer_complete->fault_code = error;
					bkp_session_insert_transfer_complete(ptransfer_complete);
					bkp_session_save();
					cwmp_root_cause_transfer_complete(cwmp, ptransfer_complete);
				}
				pthread_mutex_unlock(&(cwmp->mutex_session_send));
				pthread_cond_signal(&(cwmp->threshold_session_send));
				pthread_mutex_lock(&mutex_upload);
				list_del(&(pupload->list));
				if (pupload->scheduled_time != 0)
					count_download_queue--;
				cwmp_free_upload_request(pupload);
				pthread_mutex_unlock(&mutex_upload);
				continue;
			}
			pthread_mutex_lock(&mutex_upload);
			upload_timeout.tv_sec = stime;
			pthread_cond_timedwait(&threshold_upload, &mutex_upload, &upload_timeout);
			pthread_mutex_unlock(&mutex_upload);
		} else {
			pthread_mutex_lock(&mutex_upload);
			pthread_cond_wait(&threshold_upload, &mutex_upload);
			pthread_mutex_unlock(&mutex_upload);
		}
	}
	return NULL;
}

int cwmp_free_upload_request(struct upload *upload)
{
	if (upload != NULL) {
		if (upload->command_key != NULL)
			FREE(upload->command_key);

		if (upload->file_type != NULL)
			FREE(upload->file_type);

		if (upload->url != NULL)
			FREE(upload->url);

		if (upload->username != NULL)
			FREE(upload->username);

		if (upload->password != NULL)
			FREE(upload->password);

		if (upload->f_instance != NULL)
			FREE(upload->f_instance);

		FREE(upload);
	}
	return CWMP_OK;
}

int cwmp_scheduledUpload_remove_all()
{
	struct upload *upload;

	pthread_mutex_lock(&mutex_upload);
	while (list_upload.next != &(list_upload)) {
		upload = list_entry(list_upload.next, struct upload, list);
		list_del(&(upload->list));
		bkp_session_delete_upload(upload);
		if (upload->scheduled_time != 0)
			count_download_queue--;
		cwmp_free_upload_request(upload);
	}
	pthread_mutex_unlock(&mutex_upload);

	return CWMP_OK;
}
