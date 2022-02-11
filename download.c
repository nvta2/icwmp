/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <curl/curl.h>
#include <libubox/blobmsg_json.h>

#include "common.h"
#include "download.h"
#include "cwmp_uci.h"
#include "backupSession.h"
#include "ubus.h"
#include "log.h"
#include "cwmp_time.h"
#include "event.h"
#include "cwmp_uci.h"
#include "session.h"
#include "subprocess.h"

LIST_HEAD(list_download);
LIST_HEAD(list_schedule_download);

pthread_mutex_t mutex_download = PTHREAD_MUTEX_INITIALIZER;

int count_download_queue = 0;

/*
 * Download File
 */
int download_file(const char *file_path, const char *url, const char *username, const char *password)
{
	int res_code = 0;
	CURL *curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		if (username != NULL && strlen(username) > 0) {
			char userpass[1024];
			snprintf(userpass, sizeof(userpass), "%s:%s", username, password);
			curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
		}
		if (strncmp(url, "https://", 8) == 0)
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000L);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
		FILE *fp = fopen(file_path, "wb");
		if (fp) {
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			curl_easy_perform(curl);
			fclose(fp);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);

		curl_easy_cleanup(curl);
	}

	return res_code;
}

char *download_file_task_function(char *task)
{

	struct blob_buf bbuf;
	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);

	if (blobmsg_add_json_from_string(&bbuf, task) == false) {
		blob_buf_free(&bbuf);
		return NULL;
	}
	const struct blobmsg_policy p[5] = { { "task", BLOBMSG_TYPE_STRING }, { "file_path", BLOBMSG_TYPE_STRING }, { "url", BLOBMSG_TYPE_STRING }, { "username", BLOBMSG_TYPE_STRING }, { "password", BLOBMSG_TYPE_STRING } };

	struct blob_attr *tb[5] = { NULL, NULL, NULL, NULL, NULL};
	blobmsg_parse(p, 5, tb, blobmsg_data(bbuf.head), blobmsg_len(bbuf.head));
	char *task_name = blobmsg_get_string(tb[0]);
	if (!task_name || strcmp(task_name, "download") != 0)
		return NULL;
	char *file_path = blobmsg_get_string(tb[1]);
	char *url = blobmsg_get_string(tb[2]);
	char *username = blobmsg_get_string(tb[3]);
	char *password = blobmsg_get_string(tb[4]);

	int http_code = download_file(file_path, url, username, password);
	char *http_ret = (char *)malloc(4 * sizeof(char));
	snprintf(http_ret, 4, "%d", http_code);
	http_ret[3] = 0;
	return http_ret;
}

int download_file_in_subprocess(const char *file_path, const char *url, const char *username, const char *password)
{
	subprocess_start(download_file_task_function);

	struct blob_buf bbuf;
	memset(&bbuf, 0, sizeof(struct blob_buf));
	blob_buf_init(&bbuf, 0);
	blobmsg_add_string(&bbuf, "task", "download");
	blobmsg_add_string(&bbuf, "file_path", file_path ? file_path : "");
	blobmsg_add_string(&bbuf, "url", url ? url : "");
	blobmsg_add_string(&bbuf, "username", username ? username : "");
	blobmsg_add_string(&bbuf, "password", password ? password : "");
	char *download_task = blobmsg_format_json(bbuf.head, true);
	blob_buf_free(&bbuf);

	if (download_task != NULL) {
		char *ret = execute_task_in_subprocess(download_task);
		return atoi(ret);
	}
	return 500;
}
/*
 * Check if the downloaded image can be applied
 */
void ubus_check_image_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	int *code = (int *)req->priv;
	const struct blobmsg_policy p[2] = { { "code", BLOBMSG_TYPE_INT32 }, { "stdout", BLOBMSG_TYPE_STRING } };
	struct blob_attr *tb[2] = { NULL, NULL };
	blobmsg_parse(p, 2, tb, blobmsg_data(msg), blobmsg_len(msg));

	*code = tb[0] ? blobmsg_get_u32(tb[0]) : 1;
}

int cwmp_check_image()
{
	int code = 0, e;
	CWMP_LOG(INFO, "Check downloaded image ...");
	e = cwmp_ubus_call("rpc-sys", "upgrade_test", CWMP_UBUS_ARGS{ {} }, 0, ubus_check_image_callback, &code);
	if (e != 0) {
		CWMP_LOG(INFO, "rpc-sys upbrade_test ubus method failed: Ubus err code: %d", e);
		code = 1;
	}
	return code;
}

/*
 * Get available bank
 */
void ubus_get_available_bank_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	int *bank_id = (int *)req->priv;
	struct blob_attr *banks = NULL;
	struct blob_attr *cur;
	int rem;

	blobmsg_for_each_attr(cur, msg, rem)
	{
		if (blobmsg_type(cur) == BLOBMSG_TYPE_ARRAY) {
			banks = cur;
			break;
		}
	}

	const struct blobmsg_policy p[8] = { { "name", BLOBMSG_TYPE_STRING },  { "id", BLOBMSG_TYPE_INT32 },	 { "active", BLOBMSG_TYPE_BOOL },  { "upgrade", BLOBMSG_TYPE_BOOL },
					     { "fwver", BLOBMSG_TYPE_STRING }, { "swver", BLOBMSG_TYPE_STRING }, { "fwver", BLOBMSG_TYPE_STRING }, { "status", BLOBMSG_TYPE_STRING } };

	blobmsg_for_each_attr(cur, banks, rem)
	{
		struct blob_attr *tb[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 8, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;

		if (blobmsg_get_bool(tb[2]) == false) {
			*bank_id = blobmsg_get_u32(tb[1]);
			break;
		}
	}
}

int get_available_bank_id()
{
	int bank_id = 0, e;
	e = cwmp_ubus_call("fwbank", "dump", CWMP_UBUS_ARGS{ {} }, 0, ubus_get_available_bank_callback, &bank_id);
	if (e != 0) {
		CWMP_LOG(INFO, "fwbank dump ubus method failed: Ubus err code: %d", e);
	}
	return bank_id;
}

/*
 * Get Bank Status
 */
void ubus_get_bank_status_callback(struct ubus_request *req, int type __attribute__((unused)), struct blob_attr *msg)
{
	int *bank_id = (int *)req->priv;
	int *status = bank_id;
	bool bank_found = false;
	struct blob_attr *banks = NULL;
	struct blob_attr *cur;
	int rem;

	blobmsg_for_each_attr(cur, msg, rem)
	{
		if (blobmsg_type(cur) == BLOBMSG_TYPE_ARRAY) {
			banks = cur;
			break;
		}
	}

	const struct blobmsg_policy p[8] = { { "name", BLOBMSG_TYPE_STRING },  { "id", BLOBMSG_TYPE_INT32 },	 { "active", BLOBMSG_TYPE_BOOL },  { "upgrade", BLOBMSG_TYPE_BOOL },
					     { "fwver", BLOBMSG_TYPE_STRING }, { "swver", BLOBMSG_TYPE_STRING }, { "fwver", BLOBMSG_TYPE_STRING }, { "status", BLOBMSG_TYPE_STRING } };

	blobmsg_for_each_attr(cur, banks, rem)
	{
		struct blob_attr *tb[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		blobmsg_parse(p, 8, tb, blobmsg_data(cur), blobmsg_len(cur));
		if (!tb[0])
			continue;

		if (blobmsg_get_u32(tb[1]) == (uint32_t)*bank_id) {
			bank_found = true;
			if (strcmp(blobmsg_get_string(tb[7]), "Available") == 0 || strcmp(blobmsg_get_string(tb[7]), "Active"))
				*status = 1;
			else
				*status = 0;
		}
	}
	if (bank_found == false)
		*status = 0;
}

int get_applied_firmware_status(int *bank_id_status)
{
	int e;
	e = cwmp_ubus_call("fwbank", "dump", CWMP_UBUS_ARGS{ {} }, 0, ubus_get_bank_status_callback, &bank_id_status);
	if (e != 0) {
		CWMP_LOG(INFO, "fwbank dump ubus method failed: Ubus err code: %d", e);
	}
	return e;
}

/*
 * Apply the new firmware
 */
int cwmp_apply_firmware()
{
	int e;
	CWMP_LOG(INFO, "Apply downloaded image ...");
	e = cwmp_ubus_call("rpc-sys", "upgrade_start", CWMP_UBUS_ARGS{ { "keep", { .bool_val = true }, UBUS_Bool } }, 1, NULL, NULL);
	if (e != 0) {
		CWMP_LOG(INFO, "rpc-sys upgrade_start ubus method failed: Ubus err code: %d", e);
	}
	return e;
}

void wait_firmware_to_be_applied(int bank_id)
{
	int count = 0;

	do {
		int bank_id_status = bank_id;

		if (get_applied_firmware_status(&bank_id_status) != CWMP_OK)
			break;

		if (bank_id_status == 1)
			break;

		usleep(1000 * 1000);
		count++;
	} while(count < 15);
}

int cwmp_apply_multiple_firmware()
{
	int e;
	int bank_id = get_available_bank_id();
	if (bank_id <= 0)
		return -1;

	e = cwmp_ubus_call("fwbank", "upgrade", CWMP_UBUS_ARGS{ { "path", { .str_val = FIRMWARE_UPGRADE_IMAGE }, UBUS_String }, { "auto_activate", { .bool_val = false }, UBUS_Bool }, { "bank", { .int_val = bank_id }, UBUS_Integer } }, 3, NULL, NULL);
	if (e != 0) {
		CWMP_LOG(INFO, "fwbank upgrade ubus method failed: Ubus err code: %d", e);
		return -1;
	}
	//wait until the apply completes
	wait_firmware_to_be_applied(bank_id);
	return CWMP_OK;
}

char *apply_multiple_firmware_task_function(char *task __attribute__((unused)))
{
	int ret = cwmp_apply_multiple_firmware();

	char *ret_str = (char *)malloc(2 * sizeof(char));
	snprintf(ret_str, 2, "%d", ret);
	ret_str[1] = 0;
	return ret_str;
}

int cwmp_apply_multiple_firmware_in_subprocess()
{
	subprocess_start(apply_multiple_firmware_task_function);
	char *ret = execute_task_in_subprocess("{}"); //empty json object
	return atoi(ret);
}

int cwmp_launch_download(struct download *pdownload, char *download_file_name, enum load_type ltype, struct transfer_complete **ptransfer_complete)
{
	int error = FAULT_CPE_NO_FAULT;
	char *download_startTime;
	struct transfer_complete *p;

	download_startTime = mix_get_time();

	ltype == TYPE_DOWNLOAD ? bkp_session_delete_download(pdownload) : bkp_session_delete_schedule_download(pdownload);
	bkp_session_save();

	if (flashsize < pdownload->file_size) {
		error = FAULT_CPE_DOWNLOAD_FAILURE;
		goto end_download;
	}

	int http_code = download_file_in_subprocess(ICWMP_DOWNLOAD_FILE, pdownload->url, pdownload->username, pdownload->password);
	if (http_code == 404)
		error = FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER;
	else if (http_code == 401)
		error = FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION;
	else if (http_code != 200) {
		error = FAULT_CPE_DOWNLOAD_FAILURE;
	}

	if (error != FAULT_CPE_NO_FAULT)
		goto end_download;

	if (strcmp(pdownload->file_type, FIRMWARE_UPGRADE_IMAGE_FILE_TYPE) == 0 || strcmp(pdownload->file_type, STORED_FIRMWARE_IMAGE_FILE_TYPE) == 0) {
		rename(ICWMP_DOWNLOAD_FILE, FIRMWARE_UPGRADE_IMAGE);
		if (cwmp_check_image() == 0) {
			long int file_size = get_file_size(FIRMWARE_UPGRADE_IMAGE);
			if (file_size > flashsize) {
				error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;
				remove(FIRMWARE_UPGRADE_IMAGE);
				goto end_download;
			} else {
				error = FAULT_CPE_NO_FAULT;
				goto end_download;
			}
		} else {
			error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;
			remove(FIRMWARE_UPGRADE_IMAGE);
		}
	} else if (strcmp(pdownload->file_type, WEB_CONTENT_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;
	} else if (strcmp(pdownload->file_type, VENDOR_CONFIG_FILE_TYPE) == 0) {
		if (download_file_name != NULL) {
			char file_path[512];
			snprintf(file_path, sizeof(file_path), "/tmp/%s", download_file_name);
			rename(ICWMP_DOWNLOAD_FILE, file_path);
		} else
			rename(ICWMP_DOWNLOAD_FILE, VENDOR_CONFIG_FILE);

		error = FAULT_CPE_NO_FAULT;
	}  else if (strcmp(pdownload->file_type, TONE_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;
	} else if (strcmp(pdownload->file_type, RINGER_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;

	} else {
		remove(ICWMP_DOWNLOAD_FILE);
		error = FAULT_CPE_INVALID_ARGUMENTS;
	}

end_download:
	p = calloc(1, sizeof(struct transfer_complete));
	if (p == NULL) {
		error = FAULT_CPE_INTERNAL_ERROR;
		return error;
	}

	p->command_key = pdownload->command_key ? strdup(pdownload->command_key) : strdup("");
	p->start_time = strdup(download_startTime);
	p->complete_time = strdup(mix_get_time());
	p->type = ltype;
	if (error != FAULT_CPE_NO_FAULT) {
		p->fault_code = error;
	}
	*ptransfer_complete = p;

	return error;
}

char *get_file_name_by_download_url(char *url)
{
        char *slash = strrchr(url, '/');
        if (slash == NULL)
                return NULL;
        return slash+1;
}

int apply_downloaded_file(struct download *pdownload, char *download_file_name, struct transfer_complete *ptransfer_complete)
{
	int error = FAULT_CPE_NO_FAULT;
	if (pdownload->file_type[0] == '1') {
		ptransfer_complete->old_software_version = cwmp_main->deviceid.softwareversion;
	}
	bkp_session_insert_transfer_complete(ptransfer_complete);
	bkp_session_save();
	if (strcmp(pdownload->file_type, FIRMWARE_UPGRADE_IMAGE_FILE_TYPE) == 0) {
		cwmp_uci_set_value("cwmp", "cpe", "exec_download", "1");
		cwmp_commit_package("cwmp", UCI_STANDARD_CONFIG);
		if (cwmp_apply_firmware() != 0)
			error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;

		if (error == FAULT_CPE_NO_FAULT) {
			sleep(70);
			error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;
		}

	} else if (strcmp(pdownload->file_type, WEB_CONTENT_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;
	} else if (strcmp(pdownload->file_type, VENDOR_CONFIG_FILE_TYPE) == 0) {
		cwmp_uci_init();
		int err = CWMP_OK;
		if (download_file_name != NULL) {
			char file_path[512];
			snprintf(file_path, sizeof(file_path), "/tmp/%s", download_file_name);
			err = cwmp_uci_import(download_file_name, file_path, UCI_STANDARD_CONFIG);
			remove(file_path);
		} else {
			err = cwmp_uci_import("vendor_conf_file", VENDOR_CONFIG_FILE, UCI_STANDARD_CONFIG);
			remove(VENDOR_CONFIG_FILE);
		}

		cwmp_uci_exit();
		if (err == CWMP_OK)
			error = FAULT_CPE_NO_FAULT;
		else if (err == CWMP_GEN_ERR)
			error = FAULT_CPE_INTERNAL_ERROR;
		else if (err == -1)
			error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;
	} else if (strcmp(pdownload->file_type, TONE_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;
	} else if (strcmp(pdownload->file_type, RINGER_FILE_TYPE) == 0) {
		//TODO Not Supported
		error = FAULT_CPE_NO_FAULT;

	} else if (strcmp(pdownload->file_type, STORED_FIRMWARE_IMAGE_FILE_TYPE) == 0) {
		int err = cwmp_apply_multiple_firmware_in_subprocess();
		if (err == CWMP_OK)
			error = FAULT_CPE_NO_FAULT;
		else
			error = FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED;
	} else
		error = FAULT_CPE_INVALID_ARGUMENTS;

	if ((error == FAULT_CPE_NO_FAULT) && (pdownload->file_type[0] == '1' || pdownload->file_type[0] == '3')) {
		cwmp_uci_set_varstate_value("cwmp", "cpe", "ParameterKey", pdownload->command_key ? pdownload->command_key : "");
		cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
		if (pdownload->file_type[0] == '3') {
			CWMP_LOG(INFO, "Download and apply new vendor config file is done successfully");
		}
		return FAULT_CPE_NO_FAULT;
	}

	if (error != FAULT_CPE_NO_FAULT) {
		bkp_session_delete_transfer_complete(ptransfer_complete);
		ptransfer_complete->fault_code = error;
	}

	bkp_session_insert_transfer_complete(ptransfer_complete);
	bkp_session_save();
	cwmp_root_cause_transfer_complete(ptransfer_complete);
	return error;
}

struct transfer_complete *set_download_error_transfer_complete(struct download *pdownload, enum load_type ltype)
{
	struct transfer_complete *ptransfer_complete;
	ptransfer_complete = calloc(1, sizeof(struct transfer_complete));
	if (ptransfer_complete != NULL) {
		ptransfer_complete->command_key = strdup(pdownload->command_key);
		ptransfer_complete->start_time = strdup(mix_get_time());
		ptransfer_complete->complete_time = strdup(ptransfer_complete->start_time);
		ptransfer_complete->fault_code = ltype == TYPE_DOWNLOAD ? FAULT_CPE_DOWNLOAD_FAILURE : FAULT_CPE_DOWNLOAD_FAIL_WITHIN_TIME_WINDOW;
		ptransfer_complete->type = ltype;
		bkp_session_insert_transfer_complete(ptransfer_complete);
		cwmp_root_cause_transfer_complete(ptransfer_complete);
	}
	return ptransfer_complete;
}

int cwmp_free_download_request(struct download *download)
{
	if (download != NULL) {
		if (download->command_key != NULL)
			free(download->command_key);

		if (download->file_type != NULL)
			free(download->file_type);

		if (download->url != NULL)
			free(download->url);

		if (download->username != NULL)
			free(download->username);

		if (download->password != NULL)
			free(download->password);

		free(download);
	}
	return CWMP_OK;
}

int cwmp_free_schedule_download_request(struct download *schedule_download)
{
	if (schedule_download != NULL) {
		if (schedule_download->command_key != NULL)
			free(schedule_download->command_key);

		if (schedule_download->file_type != NULL)
			free(schedule_download->file_type);

		if (schedule_download->url != NULL)
			free(schedule_download->url);

		if (schedule_download->username != NULL)
			free(schedule_download->username);

		if (schedule_download->password != NULL)
			free(schedule_download->password);

		for (int i = 0; i <= 1; i++) {
			if (schedule_download->timewindowstruct[i].windowmode != NULL)
				free(schedule_download->timewindowstruct[i].windowmode);

			if (schedule_download->timewindowstruct[i].usermessage != NULL)
				free(schedule_download->timewindowstruct[i].usermessage);
		}
		free(schedule_download);
	}
	return CWMP_OK;
}

int cwmp_free_apply_schedule_download_request(struct apply_schedule_download *apply_schedule_download)
{
	if (apply_schedule_download != NULL) {
		if (apply_schedule_download->command_key != NULL)
			free(apply_schedule_download->command_key);

		if (apply_schedule_download->file_type != NULL)
			free(apply_schedule_download->file_type);

		if (apply_schedule_download->start_time != NULL)
			free(apply_schedule_download->start_time);

		free(apply_schedule_download);
	}
	return CWMP_OK;
}

int cwmp_scheduledDownload_remove_all()
{
	pthread_mutex_lock(&mutex_download);
	while (list_download.next != &(list_download)) {
		struct download *download;
		download = list_entry(list_download.next, struct download, list);
		list_del(&(download->list));
		bkp_session_delete_download(download);
		if (download->scheduled_time != 0)
			count_download_queue--;
		cwmp_free_download_request(download);
	}
	pthread_mutex_unlock(&mutex_download);

	return CWMP_OK;
}

int cwmp_scheduled_Download_remove_all()
{
	while (list_schedule_download.next != &(list_schedule_download)) {
		struct download *schedule_download;
		schedule_download = list_entry(list_schedule_download.next, struct download, list);
		list_del(&(schedule_download->list));
		bkp_session_delete_schedule_download(schedule_download);
		if (schedule_download->timewindowstruct[0].windowstart != 0)
			count_download_queue--;
		cwmp_free_schedule_download_request(schedule_download);
	}

	return CWMP_OK;
}

int cwmp_rpc_acs_destroy_data_transfer_complete(struct rpc *rpc)
{
	if (rpc->extra_data != NULL) {
		struct transfer_complete *p = (struct transfer_complete *)rpc->extra_data;
		bkp_session_delete_transfer_complete(p);
		bkp_session_save();
		FREE(p->command_key);
		FREE(p->start_time);
		FREE(p->complete_time);
		FREE(p->old_software_version);
	}
	FREE(rpc->extra_data);
	return 0;
}

void cwmp_start_download(struct uloop_timeout *timeout)
{
	struct download *pdownload;
	int error = FAULT_CPE_NO_FAULT;
	struct transfer_complete *ptransfer_complete;
	pdownload = container_of(timeout, struct download, handler_timer);

	char *download_file_name = get_file_name_by_download_url(pdownload->url);
	CWMP_LOG(INFO, "Launch download file %s", pdownload->url);
	error = cwmp_launch_download(pdownload, download_file_name, TYPE_DOWNLOAD, &ptransfer_complete);
	sleep(3);
	if (error != FAULT_CPE_NO_FAULT) {
		CWMP_LOG(ERROR, "Error while downloading the file: %s", pdownload->url);
		bkp_session_insert_transfer_complete(ptransfer_complete);
		bkp_session_save();
		cwmp_root_cause_transfer_complete(ptransfer_complete);
		bkp_session_delete_transfer_complete(ptransfer_complete);
	} else {
		error = apply_downloaded_file(pdownload, download_file_name, ptransfer_complete);
		if (error != FAULT_CPE_NO_FAULT) {
			CWMP_LOG(ERROR, "Error while applying the downloaded file: %s", download_file_name);
			bkp_session_insert_transfer_complete(ptransfer_complete);
			bkp_session_save();
			cwmp_root_cause_transfer_complete(ptransfer_complete);
			bkp_session_delete_transfer_complete(ptransfer_complete);
		}
	}
	if (error == FAULT_CPE_NO_FAULT && pdownload->file_type[0] == '3') {
		cwmp_root_cause_transfer_complete(ptransfer_complete);
		bkp_session_delete_download(pdownload);
		bkp_session_delete_transfer_complete(ptransfer_complete);
		bkp_session_save();
	}
	pthread_mutex_lock(&mutex_download);
	list_del(&(pdownload->list));
	if (pdownload->scheduled_time != 0)
		count_download_queue--;
	cwmp_free_download_request(pdownload);
	pthread_mutex_unlock(&mutex_download);
	trigger_cwmp_session_timer();
}

void cwmp_start_schedule_download(struct uloop_timeout *timeout)
{
	struct download *sched_download;
	int error = FAULT_CPE_NO_FAULT;
	struct transfer_complete *ptransfer_complete;
	sched_download = container_of(timeout, struct download, handler_timer);
	bool outdate = false;
	int delay;
	int window_index;

	time_t now = time(NULL);
	if (sched_download->timewindowstruct[0].windowstart > now) {
		delay = sched_download->timewindowstruct[0].windowstart - now;
		uloop_timeout_set(&sched_download->handler_timer, 1000 * delay);
		return;
	} else if (sched_download->timewindowstruct[0].windowstart <= now && sched_download->timewindowstruct[0].windowend >= now) {
		outdate = false;
		window_index = 0;
	} else if (sched_download->timewindowstruct[1].windowstart > now) {
		delay = sched_download->timewindowstruct[1].windowstart - now;
		uloop_timeout_set(&sched_download->handler_timer, 1000 * delay);
		return;
	} else if (sched_download->timewindowstruct[1].windowstart <= now && sched_download->timewindowstruct[1].windowend >= now) {
		outdate = false;
		window_index = 1;
	} else {
		outdate = true;
	}

	if (!outdate) {
		char *download_file_name = get_file_name_by_download_url(sched_download->url);
		CWMP_LOG(INFO, "Launch download file %s", sched_download->url);
		error = cwmp_launch_download(sched_download, download_file_name, TYPE_DOWNLOAD, &ptransfer_complete);
		sleep(3);
		if (error != FAULT_CPE_NO_FAULT) {
			CWMP_LOG(ERROR, "Error while downloading the file: %s", sched_download->url);
			goto retry;
		} else {
			error = apply_downloaded_file(sched_download, download_file_name, ptransfer_complete);
			if (error != FAULT_CPE_NO_FAULT) {
				CWMP_LOG(ERROR, "Error while applying the downloaded file: %s", download_file_name);
				goto retry;
			}
		}
		if (error == FAULT_CPE_NO_FAULT && sched_download->file_type[0] == '3') {
			cwmp_root_cause_transfer_complete(ptransfer_complete);
			bkp_session_delete_download(sched_download);
			bkp_session_delete_transfer_complete(ptransfer_complete);
			bkp_session_save();
		}
	} else {
		error = FAULT_CPE_INTERNAL_ERROR;
		CWMP_LOG(ERROR, "Schedule Download out of date");
		ptransfer_complete = calloc(1, sizeof(struct transfer_complete));
		if (ptransfer_complete == NULL) {
			error = FAULT_CPE_INTERNAL_ERROR;
			return;
		}

		ptransfer_complete->command_key = sched_download->command_key ? strdup(sched_download->command_key) : strdup("");
		ptransfer_complete->start_time = strdup(mix_get_time_of(now));
		ptransfer_complete->complete_time = strdup(mix_get_time());
		ptransfer_complete->type = TYPE_DOWNLOAD;
		if (error != FAULT_CPE_NO_FAULT) {
			ptransfer_complete->fault_code = error;
		}
		bkp_session_insert_transfer_complete(ptransfer_complete);
		bkp_session_save();
		cwmp_root_cause_transfer_complete(ptransfer_complete);
		bkp_session_delete_transfer_complete(ptransfer_complete);
	}

	return;

retry:
	if (sched_download->timewindowstruct[window_index].maxretries > 0) {
		uloop_timeout_set(&sched_download->handler_timer, 10);
		sched_download->timewindowstruct[window_index].maxretries--;
		return;
	} else {
		bkp_session_insert_transfer_complete(ptransfer_complete);
		bkp_session_save();
		cwmp_root_cause_transfer_complete(ptransfer_complete);
		bkp_session_delete_transfer_complete(ptransfer_complete);
		bkp_session_save();
	}
	list_del(&(sched_download->list));
	if (sched_download->scheduled_time != 0)
		count_download_queue--;
	cwmp_free_schedule_download_request(sched_download);
	trigger_cwmp_session_timer();
}
