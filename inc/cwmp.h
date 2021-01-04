/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef _CWMP_H__
#define _CWMP_H__

#include <pthread.h>
#include <libubox/uloop.h>
#include <sys/file.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "session.h"
#include "xml.h"
#include "backupSession.h"
#include "http.h"
#include "external.h"
#include "diagnostic.h"
#include "config.h"
#include "ubus.h"
#include "log.h"
#include "notifications.h"
#include "cwmp_uci.h"

int cwmp_rpc_cpe_handle_message (struct session *session, struct rpc *rpc_cpe);
#endif /* _CWMP_H__ */
