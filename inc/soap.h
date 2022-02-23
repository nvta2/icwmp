/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2019 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 *
 */

#ifndef __RPC__SOAP__H_
#define __RPC__SOAP__H_

#include <microxml.h>
#include "common.h"
#include "session.h"
#include "xml.h"


#define PROCESSING_DELAY (1) // In download/upload the message enqueued before sending the response, which cause the download/upload
			     // to start just before the time. This delay is to compensate the time lapsed during the message enqueue and response

#define MAX_NBRE_CUSTOM_INFORM 256
extern char *custom_forced_inform_parameters[MAX_NBRE_CUSTOM_INFORM];
extern char *boot_inform_parameters[MAX_NBRE_CUSTOM_INFORM];
extern int nbre_custom_inform;
extern int nbre_boot_inform;
extern const struct rpc_cpe_method rpc_cpe_methods[__RPC_CPE_MAX];
extern const struct rpc_acs_method rpc_acs_methods[__RPC_ACS_MAX];

int cwmp_handle_rpc_cpe_get_rpc_methods(struct rpc *rpc);
int cwmp_handle_rpc_cpe_set_parameter_values(struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_values(struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_names(struct rpc *rpc);
int cwmp_handle_rpc_cpe_set_parameter_attributes(struct rpc *rpc);
int cwmp_handle_rpc_cpe_get_parameter_attributes(struct rpc *rpc);
int cwmp_handle_rpc_cpe_add_object(struct rpc *rpc);
int cwmp_handle_rpc_cpe_delete_object(struct rpc *rpc);
int cwmp_handle_rpc_cpe_reboot(struct rpc *rpc);
int cwmp_handle_rpc_cpe_download(struct rpc *rpc);
int cwmp_handle_rpc_cpe_upload(struct rpc *rpc);
int cwmp_handle_rpc_cpe_factory_reset(struct rpc *rpc);
int cwmp_handle_rpc_cpe_x_factory_reset_soft(struct rpc *rpc);
int cancel_transfer(char *key);
int cwmp_handle_rpc_cpe_cancel_transfer(struct rpc *rpc);
int cwmp_handle_rpc_cpe_schedule_inform(struct rpc *rpc);
int cwmp_handle_rpc_cpe_schedule_download(struct rpc *rpc);
int cwmp_handle_rpc_cpe_change_du_state(struct rpc *rpc);
int cwmp_handle_rpc_cpe_fault(struct rpc *rpc);

int cwmp_rpc_acs_prepare_message_inform(const struct rpc *rpc);
int cwmp_rpc_acs_parse_response_inform(struct rpc *rpc);
int cwmp_rpc_acs_prepare_get_rpc_methods(struct rpc *rpc);
int cwmp_rpc_acs_prepare_transfer_complete(struct rpc *rpc);
int cwmp_rpc_acs_prepare_du_state_change_complete(struct rpc *rpc);
int cwmp_rpc_acs_destroy_data_inform(struct rpc *rpc);

int xml_handle_message();
int cwmp_create_fault_message(struct rpc *rpc_cpe, int fault_code);

#endif
