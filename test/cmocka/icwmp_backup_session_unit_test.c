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

#include "common.h"
#include "backupSession.h"
#include "xml.h"
#include "config.h"
#include "cwmp_time.h"
#include "event.h"

static struct cwmp cwmp_main_test = { 0 };

static int bkp_session_unit_tests_clean(void **state)
{
	icwmp_cleanmem();
	return 0;
}

static void cwmp_backup_session_unit_test(void **state)
{
	remove(CWMP_BKP_FILE);
	struct cwmp *cwmp_test = &cwmp_main_test;
	mxml_node_t *backup_tree = NULL, *n = NULL;

	/*
	 * Init backup session
	 */
	int error = cwmp_init_backup_session(cwmp_test, NULL, ALL);
	assert_int_equal(error, 0);
	bkp_session_save();
	FILE *pFile;
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(backup_tree);
	assert_string_equal(backup_tree->value.element.name, "cwmp");
	assert_null(backup_tree->child);
	MXML_DELETE(backup_tree);

	/*
	 * Insert Event
	 */
	mxml_node_t *bkp_event1 = NULL, *bkp_event2 = NULL, *queue_tree1 = NULL, *queue_tree2 = NULL;

	// Case of one event
	bkp_event1 = bkp_session_insert_event(EVENT_IDX_4VALUE_CHANGE, "4 VALUE CHANGE", 0, "queue");
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(bkp_event1);
	queue_tree1 = mxmlFindElement(backup_tree, backup_tree, "queue_event", NULL, NULL, MXML_DESCEND);
	assert_non_null(queue_tree1);
	n = mxmlFindElement(queue_tree1, queue_tree1, "index", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), EVENT_IDX_4VALUE_CHANGE);
	n = mxmlFindElement(queue_tree1, queue_tree1, "id", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), 0);
	n = mxmlFindElement(queue_tree1, queue_tree1, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "4 VALUE CHANGE");
	MXML_DELETE(bkp_event1);
	bkp_session_save();
	MXML_DELETE(backup_tree);

	//case of two events with different ids under the same queue
	bkp_event1 = bkp_session_insert_event(EVENT_IDX_1BOOT, "1 BOOT", 0, "queue");
	bkp_event2 = bkp_session_insert_event(EVENT_IDX_4VALUE_CHANGE, "4 VALUE CHANGE", 1, "queue");
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(bkp_event1);
	assert_non_null(bkp_event2);
	queue_tree1 = mxmlFindElement(backup_tree, backup_tree, "queue_event", NULL, NULL, MXML_DESCEND);
	queue_tree2 = mxmlFindElement(queue_tree1, backup_tree, "queue_event", NULL, NULL, MXML_DESCEND);

	assert_non_null(queue_tree1);
	n = mxmlFindElement(queue_tree1, queue_tree1, "index", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), EVENT_IDX_1BOOT);
	n = mxmlFindElement(queue_tree1, queue_tree1, "id", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), 0);
	n = mxmlFindElement(queue_tree1, queue_tree1, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1 BOOT");

	assert_non_null(queue_tree2);
	n = mxmlFindElement(queue_tree2, queue_tree2, "index", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), EVENT_IDX_4VALUE_CHANGE);
	n = mxmlFindElement(queue_tree2, queue_tree2, "id", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), 1);
	n = mxmlFindElement(queue_tree2, queue_tree2, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "4 VALUE CHANGE");

	MXML_DELETE(bkp_event1);
	MXML_DELETE(bkp_event2);
	bkp_session_save();
	MXML_DELETE(backup_tree);
	bkp_event1 = NULL;
	bkp_event2 = NULL;

	//case of two events with same id under different queues
	bkp_event1 = bkp_session_insert_event(EVENT_IDX_1BOOT, "1 BOOT", 0, "queue1");
	bkp_event2 = bkp_session_insert_event(EVENT_IDX_4VALUE_CHANGE, "4 VALUE CHANGE", 0, "queue2");
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(bkp_event1);
	assert_non_null(bkp_event2);
	queue_tree1 = mxmlFindElement(backup_tree, backup_tree, "queue1_event", NULL, NULL, MXML_DESCEND);
	queue_tree2 = mxmlFindElement(queue_tree1, backup_tree, "queue2_event", NULL, NULL, MXML_DESCEND);

	assert_non_null(queue_tree1);
	n = mxmlFindElement(queue_tree1, queue_tree1, "index", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), EVENT_IDX_1BOOT);
	n = mxmlFindElement(queue_tree1, queue_tree1, "id", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), 0);
	n = mxmlFindElement(queue_tree1, queue_tree1, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1 BOOT");

	assert_non_null(queue_tree2);
	n = mxmlFindElement(queue_tree2, queue_tree2, "index", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), EVENT_IDX_4VALUE_CHANGE);
	n = mxmlFindElement(queue_tree2, queue_tree2, "id", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), 0);
	n = mxmlFindElement(queue_tree2, queue_tree2, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "4 VALUE CHANGE");

	MXML_DELETE(bkp_event1);
	MXML_DELETE(bkp_event2);
	bkp_session_save();
	MXML_DELETE(backup_tree);


	/*
	 * Insert Download
	 */
	struct download *download = NULL;
	download = icwmp_calloc(1, sizeof(struct download));
	download->command_key = icwmp_strdup("download_key");
	download->file_size = 0;
	download->file_type = icwmp_strdup("1 Firmware Upgrade Image");
	download->password = icwmp_strdup("iopsys");
	download->username = icwmp_strdup("iopsys");
	download->url = icwmp_strdup("http://192.168.1.160:8080/openacs/acs");
	bkp_session_insert_download(download);
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);

	mxml_node_t *download_tree = NULL;
	download_tree = mxmlFindElement(backup_tree, backup_tree, "download", NULL, NULL, MXML_DESCEND);
	assert_non_null(download_tree);
	n = mxmlFindElement(download_tree, download_tree, "url", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "http://192.168.1.160:8080/openacs/acs");
	n = mxmlFindElement(download_tree, download_tree, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "download_key");
	n = mxmlFindElement(download_tree, download_tree, "file_type", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "1 Firmware Upgrade Image");
	n = mxmlFindElement(download_tree, download_tree, "username", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "iopsys");
	n = mxmlFindElement(download_tree, download_tree, "password", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "iopsys");
	n = mxmlFindElement(download_tree, download_tree, "file_size", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "0");
	n = mxmlFindElement(download_tree, download_tree, "time", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	MXML_DELETE(backup_tree);

	/*
	 * Delete download
	 */
	bkp_session_delete_download(download);
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(backup_tree);
	assert_string_equal(backup_tree->value.element.name, "cwmp");
	assert_null(backup_tree->child);
	MXML_DELETE(backup_tree);

	/*
	 * Insert TransferComplete bkp_session_delete_transfer_complete
	 */
	struct transfer_complete *p;
	p = icwmp_calloc(1, sizeof(struct transfer_complete));
	p->command_key = icwmp_strdup("transfer_complete_key");
	p->start_time = icwmp_strdup(mix_get_time());
	p->complete_time = icwmp_strdup(mix_get_time());
	p->old_software_version = icwmp_strdup("iopsys_img_old");
	p->type = TYPE_DOWNLOAD;
	p->fault_code = FAULT_CPE_NO_FAULT;
	bkp_session_insert_transfer_complete(p);
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);

	mxml_node_t *transfer_complete_tree = NULL;
	transfer_complete_tree = mxmlFindElement(backup_tree, backup_tree, "transfer_complete", NULL, NULL, MXML_DESCEND);
	assert_non_null(transfer_complete_tree);
	n = mxmlFindElement(transfer_complete_tree, transfer_complete_tree, "command_key", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "transfer_complete_key");
	n = mxmlFindElement(transfer_complete_tree, transfer_complete_tree, "start_time", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(transfer_complete_tree, transfer_complete_tree, "complete_time", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	n = mxmlFindElement(transfer_complete_tree, transfer_complete_tree, "old_software_version", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_string_equal(n->child->value.opaque, "iopsys_img_old");
	n = mxmlFindElement(transfer_complete_tree, transfer_complete_tree, "fault_code", NULL, NULL, MXML_DESCEND);
	assert_non_null(n);
	assert_int_equal(atoi(n->child->value.opaque), FAULT_CPE_NO_FAULT);
	MXML_DELETE(backup_tree);

	/*
	 * Delete TransferComplete
	 */
	bkp_session_delete_transfer_complete(p);
	bkp_session_save();
	pFile = fopen(CWMP_BKP_FILE, "r");
	backup_tree = mxmlLoadFile(NULL, pFile, MXML_OPAQUE_CALLBACK);
	fclose(pFile);
	assert_non_null(backup_tree);
	assert_string_equal(backup_tree->value.element.name, "cwmp");
	assert_null(backup_tree->child);
	MXML_DELETE(backup_tree);

	bkp_tree_clean();
}

int icwmp_backup_session_test(void)
{
	const struct CMUnitTest tests[] = { //
		    cmocka_unit_test(cwmp_backup_session_unit_test),
	};

	return cmocka_run_group_tests(tests, NULL, bkp_session_unit_tests_clean);
}
