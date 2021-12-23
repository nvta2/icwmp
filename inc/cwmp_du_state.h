/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef CWMP_DU_STATE_H
#define CWMP_DU_STATE_H

extern struct list_head list_change_du_state;
extern pthread_mutex_t mutex_change_du_state;
extern pthread_cond_t threshold_change_du_state;

int cwmp_du_install(char *url, char *uuid, char *user, char *pass, char *env, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code);
int cwmp_du_update(char *url, char *uuid, char *user, char *pass, char **package_version, char **package_name, char **package_uuid, char **package_env, char **fault_code);
int cwmp_du_uninstall(char *package_name, char *package_env, char **fault_code);
int cwmp_rpc_acs_destroy_data_du_state_change_complete(struct rpc *rpc);
void *thread_cwmp_rpc_cpe_change_du_state(void *v);
int cwmp_free_change_du_state_request(struct change_du_state *change_du_state);
#endif
