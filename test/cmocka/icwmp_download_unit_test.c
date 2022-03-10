/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <dirent.h>
#include <unistd.h>

#include "xml.h"
#include "event.h"
#include "session.h"
#include "config.h"
#include "backupSession.h"
#include "log.h"
#include "download.h"

struct transfer_complete *transfer_complete_test = NULL;

void free_transfer_complete(struct transfer_complete *p)
{
	FREE(p->command_key);
	FREE(p->start_time);
	FREE(p->complete_time);
	FREE(p->old_software_version);
	FREE(p);
}

static void free_download(struct download *p)
{
	if (p==NULL)
		return;
	icwmp_free(p->command_key);
	icwmp_free(p->file_type);
	icwmp_free(p->username);
	icwmp_free(p->password);
	icwmp_free(p->url);
	icwmp_free(p);
}

static int download_unit_tests_clean(void **state)
{
	icwmp_cleanmem();
	if (transfer_complete_test != NULL)
		free_transfer_complete(transfer_complete_test);
	remove(ICWMP_DOWNLOAD_FILE);
	remove(FIRMWARE_UPGRADE_IMAGE);
	return 0;
}

static void cwmp_download_file_unit_test(void **state)
{
	remove(ICWMP_DOWNLOAD_FILE);
	/*
	 * Valid URL
	 */
	int http_code = download_file(ICWMP_DOWNLOAD_FILE, "http://127.0.0.1/firmware_v1.0.bin", NULL, NULL);
	assert_int_equal(http_code, 200);
	assert_int_equal(access( ICWMP_DOWNLOAD_FILE, F_OK ), 0);
	remove(ICWMP_DOWNLOAD_FILE);

	/*
	 * Not Valid URL
	 */
	http_code = download_file(ICWMP_DOWNLOAD_FILE, "http://127.0.0.1/firmware.bin", NULL, NULL);
	assert_int_equal(http_code, 404);
	assert_int_equal(access( ICWMP_DOWNLOAD_FILE, F_OK ), 0);
	remove(ICWMP_DOWNLOAD_FILE);
}

static void cwmp_launch_download_unit_test(void **state)
{

	struct transfer_complete *ptransfer_complete = NULL;
	struct download *pdownload;

	remove(FIRMWARE_UPGRADE_IMAGE);
	/*
	 * All arguments are valid
	 */
	pdownload = icwmp_calloc(1, sizeof(struct download));
	pdownload->command_key = icwmp_strdup("download_key");
	pdownload->file_size = 0;
	pdownload->file_type = icwmp_strdup(FIRMWARE_UPGRADE_IMAGE_FILE_TYPE);
	pdownload->username = icwmp_strdup("iopsys");
	pdownload->password = icwmp_strdup("iopsys");
	pdownload->url = icwmp_strdup("http://127.0.0.1/firmware_v1.0.bin");

	int error = cwmp_launch_download(pdownload, "firmware_v1.0.bin", TYPE_DOWNLOAD, &ptransfer_complete);
	transfer_complete_test = ptransfer_complete;

	assert_int_equal(error, FAULT_CPE_NO_FAULT);
	assert_int_equal(access( FIRMWARE_UPGRADE_IMAGE, F_OK ), 0);
	assert_int_equal(ptransfer_complete->fault_code, FAULT_CPE_NO_FAULT);
	assert_string_equal(ptransfer_complete->command_key, "download_key");
	assert_non_null(ptransfer_complete->start_time);
	assert_non_null(ptransfer_complete->complete_time);

	free_transfer_complete(ptransfer_complete);
	free_download(pdownload);
	remove(FIRMWARE_UPGRADE_IMAGE);

	/*
	 * Not valid url
	 */
	pdownload = icwmp_calloc(1, sizeof(struct download));
	pdownload->command_key = icwmp_strdup("download_key");
	pdownload->file_size = 0;
	pdownload->file_type = icwmp_strdup(FIRMWARE_UPGRADE_IMAGE_FILE_TYPE);
	pdownload->username = icwmp_strdup("iopsys");
	pdownload->password = icwmp_strdup("iopsys");
	pdownload->url = icwmp_strdup("http://127.0.0.1/firmware.bin");

	error = cwmp_launch_download(pdownload, "firmware_v1.0.bin", TYPE_DOWNLOAD, &ptransfer_complete);
	transfer_complete_test = ptransfer_complete;

	assert_int_equal(error, FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER);
	assert_int_equal(ptransfer_complete->fault_code, FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER);
	assert_string_equal(ptransfer_complete->command_key, "download_key");
	assert_non_null(ptransfer_complete->start_time);
	assert_non_null(ptransfer_complete->complete_time);

	free_transfer_complete(ptransfer_complete);
	free_download(pdownload);
	remove(FIRMWARE_UPGRADE_IMAGE);

	/*
	 * Not valid FileType
	 */
	pdownload = icwmp_calloc(1, sizeof(struct download));
	pdownload->command_key = icwmp_strdup("download_key");
	pdownload->file_size = 0;
	pdownload->file_type = icwmp_strdup("7 New File Type");
	pdownload->username = icwmp_strdup("iopsys");
	pdownload->password = icwmp_strdup("iopsys");
	pdownload->url = icwmp_strdup("http://127.0.0.1/firmware_v1.0.bin");

	error = cwmp_launch_download(pdownload, "firmware_v1.0.bin", TYPE_DOWNLOAD, &ptransfer_complete);
	transfer_complete_test = ptransfer_complete;

	assert_int_equal(error, FAULT_CPE_INVALID_ARGUMENTS);
	assert_int_equal(ptransfer_complete->fault_code, FAULT_CPE_INVALID_ARGUMENTS);
	assert_string_equal(ptransfer_complete->command_key, "download_key");
	assert_non_null(ptransfer_complete->start_time);
	assert_non_null(ptransfer_complete->complete_time);

	free_transfer_complete(ptransfer_complete);
	free_download(pdownload);
	remove(FIRMWARE_UPGRADE_IMAGE);

	/*
	 * Not valid image
	 */
	pdownload = icwmp_calloc(1, sizeof(struct download));
	pdownload->command_key = icwmp_strdup("download_key");
	pdownload->file_size = 0;
	pdownload->file_type = icwmp_strdup(FIRMWARE_UPGRADE_IMAGE_FILE_TYPE);
	pdownload->username = icwmp_strdup("iopsys");
	pdownload->password = icwmp_strdup("iopsys");
	pdownload->url = icwmp_strdup("http://127.0.0.1/invalid_firmware_v1.0.bin");

	error = cwmp_launch_download(pdownload, "firmware_v1.0.bin", TYPE_DOWNLOAD, &ptransfer_complete);
	transfer_complete_test = ptransfer_complete;

	assert_int_equal(error, FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED);
	assert_int_equal(ptransfer_complete->fault_code, FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED);
	assert_string_equal(ptransfer_complete->command_key, "download_key");
	assert_non_null(ptransfer_complete->start_time);
	assert_non_null(ptransfer_complete->complete_time);

	free_transfer_complete(ptransfer_complete);
	free_download(pdownload);
	remove(FIRMWARE_UPGRADE_IMAGE);
}

int icwmp_download_unit_test(void)
{
	const struct CMUnitTest tests[] = { //
			cmocka_unit_test(cwmp_download_file_unit_test),
			cmocka_unit_test(cwmp_launch_download_unit_test)
	};

	return cmocka_run_group_tests(tests, NULL, download_unit_tests_clean);
}
