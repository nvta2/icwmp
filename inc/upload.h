/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef CWMP_UPLOAD_H
#define CWMP_UPLOAD_H

extern struct list_head list_upload;
extern pthread_mutex_t mutex_upload;

int cwmp_launch_upload(struct upload *pupload, struct transfer_complete **ptransfer_complete);
void *thread_cwmp_rpc_cpe_upload(void *v);
int cwmp_scheduledUpload_remove_all();
int cwmp_free_upload_request(struct upload *upload);
void cwmp_start_upload(struct uloop_timeout *timeout);
#endif
