/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/engine.h>
#include "cwmp.h"
#include "xml.h"
#include "backupSession.h"
#include "log.h"
#include "jshn.h"
#include "external.h"
#include "config.h"
#include "datamodel_interface.h"

LIST_HEAD(list_value_change);
LIST_HEAD(list_lw_value_change);
pthread_mutex_t mutex_value_change = PTHREAD_MUTEX_INITIALIZER;

const struct EVENT_CONST_STRUCT EVENT_CONST [] = {
        [EVENT_IDX_0BOOTSTRAP]                      = {"0 BOOTSTRAP",                       EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_1BOOT]                           = {"1 BOOT",                            EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL},
        [EVENT_IDX_2PERIODIC]                       = {"2 PERIODIC",                        EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_3SCHEDULED]                      = {"3 SCHEDULED",                       EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_4VALUE_CHANGE]                   = {"4 VALUE CHANGE",                    EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL},
        [EVENT_IDX_5KICKED]                         = {"5 KICKED",                          EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_6CONNECTION_REQUEST]             = {"6 CONNECTION REQUEST",              EVENT_TYPE_SINGLE,  0},
        [EVENT_IDX_7TRANSFER_COMPLETE]              = {"7 TRANSFER COMPLETE",               EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_8DIAGNOSTICS_COMPLETE]           = {"8 DIAGNOSTICS COMPLETE",            EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL},
        [EVENT_IDX_9REQUEST_DOWNLOAD]               = {"9 REQUEST DOWNLOAD",                EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_10AUTONOMOUS_TRANSFER_COMPLETE]  = {"10 AUTONOMOUS TRANSFER COMPLETE",   EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
		[EVENT_IDX_11DU_STATE_CHANGE_COMPLETE]  	= {"11 DU STATE CHANGE COMPLETE", 		EVENT_TYPE_SINGLE,  EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_M_Reboot]                        = {"M Reboot",                          EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_M_ScheduleInform]                = {"M ScheduleInform",                  EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
        [EVENT_IDX_M_Download]                      = {"M Download",                        EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
		[EVENT_IDX_M_Schedule_Download]             = {"M ScheduleDownload",                EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
		[EVENT_IDX_M_Upload]                        = {"M Upload",                          EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT},
		[EVENT_IDX_M_ChangeDUState]                 = {"M ChangeDUState",                          EVENT_TYPE_MULTIPLE,EVENT_RETRY_AFTER_TRANSMIT_FAIL|EVENT_RETRY_AFTER_REBOOT}
};

void cwmp_save_event_container (struct cwmp *cwmp,struct event_container *event_container)
{
    struct list_head *ilist;
    struct cwmp_dm_parameter *dm_parameter;
    mxml_node_t *b;

    if (EVENT_CONST[event_container->code].RETRY & EVENT_RETRY_AFTER_REBOOT) {
        b = bkp_session_insert_event(event_container->code, event_container->command_key, event_container->id, "queue");

        list_for_each(ilist,&(event_container->head_dm_parameter)) {
            dm_parameter = list_entry(ilist, struct cwmp_dm_parameter, list);
            bkp_session_insert_parameter(b, dm_parameter->name);
        }
        bkp_session_save();
    }
}

struct event_container *cwmp_add_event_container (struct cwmp *cwmp, int event_code, char *command_key)
{
    static int      id;
    struct event_container   *event_container;
    struct session           *session;
    struct list_head         *ilist;

    if (cwmp->head_event_container == NULL)
    {
        session = cwmp_add_queue_session(cwmp);
        if (session == NULL)
        {
            return NULL;
        }
        cwmp->head_event_container = &(session->head_event_container);
    }
    session = list_entry (cwmp->head_event_container, struct session,head_event_container);
    list_for_each(ilist, cwmp->head_event_container)
    {
        event_container = list_entry (ilist, struct event_container, list);
        if (event_container->code==event_code &&
            EVENT_CONST[event_code].TYPE==EVENT_TYPE_SINGLE)
        {
            return event_container;
        }
        if(event_container->code > event_code)
        {
            break;
        }
    }
    event_container = calloc (1,sizeof(struct event_container));
    if (event_container==NULL)
    {
        return NULL;
    }
    INIT_LIST_HEAD (&(event_container->head_dm_parameter));
    list_add (&(event_container->list), ilist->prev);
    event_container->code = event_code;
    event_container->command_key = command_key?strdup(command_key):strdup("");
    if((id<0) || (id>=MAX_INT_ID) )
    {
        id=0;
    }
    id++;
    event_container->id         = id;
    return event_container;
}

void add_dm_parameter_tolist(struct list_head *head, char *param_name, char *param_data, char *param_type)
{
	struct cwmp_dm_parameter *dm_parameter;
	struct list_head *ilist;
	int cmp;
	list_for_each (ilist, head) {
		dm_parameter = list_entry(ilist, struct cwmp_dm_parameter, list);
		cmp = strcmp(dm_parameter->name, param_name);
		if (cmp == 0) {
			if (param_data && strcmp(dm_parameter->data, param_data) != 0)
			{
				free(dm_parameter->data);
				dm_parameter->data = strdup(param_data);
			}
			return;
		} else if (cmp > 0) {
			break;
		}
	}
	dm_parameter = calloc(1, sizeof(struct cwmp_dm_parameter));
	_list_add(&dm_parameter->list, ilist->prev, ilist);
	if (param_name) dm_parameter->name = strdup(param_name);
	if (param_data) dm_parameter->data = strdup(param_data);
	if (param_type) dm_parameter->type = strdup(param_type ? param_type : "xsd:string");
}

void delete_dm_parameter_fromlist(struct cwmp_dm_parameter *dm_parameter)
{
	list_del(&dm_parameter->list);
	free(dm_parameter->name);
	free(dm_parameter->data);
	free(dm_parameter->type);
	free(dm_parameter);
}

void free_dm_parameter_all_fromlist(struct list_head *list)
{
	struct cwmp_dm_parameter *dm_parameter;
	while (list->next!=list) {
		dm_parameter = list_entry(list->next, struct cwmp_dm_parameter, list);
		delete_dm_parameter_fromlist(dm_parameter);
	}
}

void add_lw_list_value_change(char *param_name, char *param_data, char *param_type)
{
	add_dm_parameter_tolist(&list_lw_value_change, param_name, param_data, param_type);
	
}

void udplw_server_param(struct addrinfo **res)
{
	struct addrinfo hints = {0};
	struct cwmp   *cwmp = &cwmp_main;
	struct config   *conf;
	char *port;
	conf = &(cwmp->conf);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	asprintf(&port, "%d", conf->lw_notification_port);
	getaddrinfo(conf->lw_notification_hostname,port,&hints,res);
	//FREE(port);
}

static void message_compute_signature(char *msg_out, char *signature)
{
	int i;
	int result_len = 20;
	unsigned char *result;
	struct cwmp   *cwmp = &cwmp_main;
	struct config   *conf;
	conf = &(cwmp->conf);
	result = HMAC(EVP_sha1(), conf->acs_passwd, strlen(conf->acs_passwd),
			msg_out, strlen(msg_out), NULL, NULL);
	for (i = 0; i < result_len; i++) {
		sprintf(&(signature[i * 2]), "%02X", result[i]);
	}
	signature[i * 2 ] = '\0';
	FREE(result);
}

char *calculate_lwnotification_cnonce()
{
	int i;
	char *cnonce = malloc( 33 * sizeof(char));
	srand((unsigned int) time(NULL));
	for (i = 0; i < 4; i++) {
		sprintf(&(cnonce[i * 8]), "%08x", rand());
	}
	cnonce[i * 8 ] = '\0';
	return cnonce;
}

static void send_udp_message(struct addrinfo *servaddr, char *msg)
{
	int fd;

	fd = socket(servaddr->ai_family, SOCK_DGRAM, 0);	
	
	if ( fd >= 0) {
		sendto(fd, msg, strlen(msg), 0, servaddr->ai_addr, servaddr->ai_addrlen);
		close(fd);
	}
}

void del_list_lw_notify(struct dm_parameter *dm_parameter)
{
	
	list_del(&dm_parameter->list);
	free(dm_parameter->name);
	free(dm_parameter);
}

void free_all_list_lw_notify()
{
	struct dm_parameter *dm_parameter;
	while (list_lw_value_change.next != &list_lw_value_change) {
		dm_parameter = list_entry(list_lw_value_change.next, struct dm_parameter, list);
		del_list_lw_notify(dm_parameter);		
	}
}

void cwmp_lwnotification()
{
	char *msg, *msg_out;
	char signature[41];
	struct addrinfo *servaddr;
	struct cwmp   *cwmp = &cwmp_main;
	struct config   *conf;
	conf = &(cwmp->conf);

	udplw_server_param(&servaddr);
	xml_prepare_lwnotification_message(&msg_out);
	message_compute_signature(msg_out, signature);
	asprintf(&msg, "%s \n %s: %s \n %s: %s \n %s: %d\n %s: %s\n\n%s",
			"POST /HTTPS/1.1",
			"HOST",	conf->lw_notification_hostname,
			"Content-Type", "test/xml; charset=utf-8",
			"Content-Lenght", strlen(msg_out),
			"Signature",signature,
			msg_out);

	send_udp_message(servaddr, msg);
	free_all_list_lw_notify(); 
	//freeaddrinfo(servaddr); //To check
	FREE(msg);
	FREE(msg_out);
}

int cwmp_update_enabled_notify_file()
{
	struct cwmp *cwmp = &cwmp_main;
	FILE *fp;
	json_object *param_obj = NULL, *param_name_obj = NULL, *value_obj = NULL, *type_obj = NULL, *notification_obj = NULL;

	int e = cwmp_update_enabled_list_notify(cwmp->conf.instance_mode, OLD_LIST_NOTIFY);
	if (e)
		return 0;
	remove(DM_ENABLED_NOTIFY);

	fp = fopen(DM_ENABLED_NOTIFY, "a");
	if (fp == NULL) {
		return 0;
	}
	foreach_jsonobj_in_array(param_obj, old_list_notify) {
		json_object_object_get_ex(param_obj, "parameter", &param_name_obj);
		if (!param_name_obj || strlen((char*)json_object_get_string(param_name_obj))<=0)
			continue;
		json_object_object_get_ex(param_obj, "value", &value_obj);
		json_object_object_get_ex(param_obj, "type", &type_obj);
		json_object_object_get_ex(param_obj, "notification", &notification_obj);
		cwmp_json_fprintf(fp, 4, CWMP_JSON_ARGS{{"parameter", (char*)json_object_get_string(param_name_obj)}, {"notification", notification_obj?(char*)json_object_get_string(notification_obj):""}, {"value", value_obj?(char*)json_object_get_string(value_obj):""}, {"type", type_obj?(char*)json_object_get_string(type_obj):""}});
	}
	fclose(fp);
	return 1;
}

void get_parameter_value_from_parameters_list(json_object* list_params_obj, char* parameter_name, struct cwmp_dm_parameter **ret_dm_param)
{
	json_object *param_obj = NULL, *param_name_obj = NULL, *value_obj = NULL, *type_obj = NULL;

	foreach_jsonobj_in_array(param_obj, list_params_obj) {
		json_object_object_get_ex(param_obj, "parameter", &param_name_obj);
		if (!param_name_obj || strlen((char*)json_object_get_string(param_name_obj))<=0)
			continue;
		if (strcmp((char*)json_object_get_string(param_name_obj), parameter_name) != 0)
			continue;
		*ret_dm_param = (struct cwmp_dm_parameter*) calloc(1,sizeof(struct cwmp_dm_parameter));
		json_object_object_get_ex(param_obj, "value", &value_obj);
		(*ret_dm_param)->name = strdup(parameter_name);
		(*ret_dm_param)->data = strdup(value_obj?(char*)json_object_get_string(value_obj):"");
		json_object_object_get_ex(param_obj, "type", &type_obj);
		(*ret_dm_param)->type = strdup(type_obj?(char*)json_object_get_string(type_obj):"");
		break;
	}
}

int check_value_change(void)
{
	FILE *fp;
	char buf[512];
	char *parameter, *notification = NULL, *value = NULL, *jval = NULL;
	struct cwmp *cwmp = &cwmp_main;
	struct cwmp_dm_parameter *dm_parameter = NULL;
	json_object *buf_json_obj = NULL;
	int is_notify = 0;

	fp = fopen(DM_ENABLED_NOTIFY, "r");
	if (fp == NULL)
		return false;

	cwmp_update_enabled_list_notify(cwmp->conf.instance_mode, ACTUAL_LIST_NOTIFY);

	while (fgets(buf, 512, fp) != NULL) {
		int len = strlen(buf);
		if (len)
			buf[len-1] = '\0';
		buf_json_obj = json_tokener_parse((char*)buf);
		cwmp_json_obj_init(buf, &buf_json_obj);
		cwmp_json_get_string(buf_json_obj, "parameter", &parameter);
		if(parameter == NULL || parameter[0] == '\0')
			continue;
		cwmp_json_get_string(buf_json_obj, "value", &value);
		cwmp_json_get_string(buf_json_obj, "notification", &notification);

		cwmp_json_obj_clean(&buf_json_obj);
		get_parameter_value_from_parameters_list(actual_list_notify, parameter, &dm_parameter);
		if (dm_parameter == NULL)
			continue;

		if (notification && (strlen(notification) > 0) && (notification[0] >= '1')  && (strcmp(dm_parameter->data, value) != 0)){
			if (notification[0] == '1' || notification[0] == '2')
				add_list_value_change(parameter, dm_parameter->data, dm_parameter->type);
			if (notification[0] >= '3' )
				add_lw_list_value_change(parameter, dm_parameter->data, dm_parameter->type);

			if (notification[0] == '1')
				is_notify |= NOTIF_PASSIVE;
			if (notification[0] == '2')
				is_notify |= NOTIF_ACTIVE;

			if (notification[0] == '5' || notification[0] == '6')
				is_notify |= NOTIF_LW_ACTIVE;
		}
		FREE(value);
		FREE(notification);
		FREE(parameter);
		FREE(dm_parameter->name);
		FREE(dm_parameter->data);
		FREE(dm_parameter->type);
		FREE(dm_parameter);
	}
	fclose(fp);
	return is_notify;
}

void cwmp_root_cause_event_ipdiagnostic(void)
{
	struct cwmp   *cwmp = &cwmp_main;
	struct event_container   *event_container;
    
	pthread_mutex_lock (&(cwmp->mutex_session_queue));        
	event_container = cwmp_add_event_container (cwmp, EVENT_IDX_8DIAGNOSTICS_COMPLETE, "");
    if (event_container == NULL)
	{
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
		return;
	}    
    cwmp_save_event_container(cwmp,event_container);
	pthread_mutex_unlock (&(cwmp->mutex_session_queue));
	pthread_cond_signal(&(cwmp->threshold_session_send));
    return;		
}

int cwmp_root_cause_event_boot (struct cwmp *cwmp)
{
    struct event_container   *event_container;
    if (cwmp->env.boot == CWMP_START_BOOT)
    {
        pthread_mutex_lock (&(cwmp->mutex_session_queue));
        cwmp->env.boot = 0;
        event_container = cwmp_add_event_container (cwmp, EVENT_IDX_1BOOT, "");
        if (event_container == NULL)
        {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            return CWMP_MEM_ERR;
        }
        cwmp_save_event_container (cwmp,event_container);
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
    }
    return CWMP_OK;
}
int event_remove_all_event_container(struct session *session, int rem_from)
{
    struct event_container *event_container;

    while (session->head_event_container.next!=&(session->head_event_container)) {
        event_container = list_entry(session->head_event_container.next, struct event_container, list);
        bkp_session_delete_event(event_container->id, rem_from?"send":"queue");
        if (event_container->code == EVENT_IDX_1BOOT && rem_from == RPC_SEND) {
        	remove("/etc/icwmpd/.icwmpd_boot");
        }
        free (event_container->command_key);
        free_dm_parameter_all_fromlist(&(event_container->head_dm_parameter));
        list_del(&(event_container->list));
        free (event_container);
    }
    bkp_session_save();
    return CWMP_OK;
}

int event_remove_noretry_event_container(struct session *session, struct cwmp *cwmp)
{
	struct event_container *event_container;
	struct list_head *ilist, *q;

	list_for_each_safe(ilist,q,&(session->head_event_container)) {
		event_container = list_entry(ilist, struct event_container, list);
		if (EVENT_CONST[event_container->code].RETRY == 0) {
			free (event_container->command_key);
			free_dm_parameter_all_fromlist(&(event_container->head_dm_parameter));
			list_del(&(event_container->list));
			free (event_container);
		}

		if (EVENT_CONST[event_container->code].CODE[0] == '6')
			cwmp->cwmp_cr_event = 1;
	}
	return CWMP_OK;
}

int cwmp_root_cause_event_bootstrap (struct cwmp *cwmp)
{
    char *acsurl = NULL;
    int cmp = 0;
    struct event_container *event_container;
    struct session *session;

    cwmp_load_saved_session(cwmp, &acsurl, ACS);

    if (acsurl == NULL)
        save_acs_bkp_config (cwmp);

    if (acsurl == NULL || ((acsurl != NULL)&&(cmp = strcmp(cwmp->conf.acsurl,acsurl)))) {
        pthread_mutex_lock (&(cwmp->mutex_session_queue));
        if (cwmp->head_event_container!=NULL && cwmp->head_session_queue.next!=&(cwmp->head_session_queue)) {
            session = list_entry(cwmp->head_event_container,struct session, head_event_container);
            event_remove_all_event_container (session,RPC_QUEUE);
        }
        event_container = cwmp_add_event_container (cwmp, EVENT_IDX_0BOOTSTRAP, "");
        FREE(acsurl);
        if (event_container == NULL) {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            return CWMP_MEM_ERR;
        }
        cwmp_save_event_container (cwmp,event_container);
        cwmp_scheduleInform_remove_all();
        cwmp_scheduledDownload_remove_all();
		cwmp_scheduled_Download_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
        cwmp_scheduledUpload_remove_all();
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
    } else {
        FREE(acsurl);
    }

    if (cmp) {
        pthread_mutex_lock (&(cwmp->mutex_session_queue));
        event_container = cwmp_add_event_container (cwmp, EVENT_IDX_4VALUE_CHANGE, "");
        if (event_container == NULL) {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            return CWMP_MEM_ERR;
        }

        char buf[64] = "Device.ManagementServer.URL";
        add_dm_parameter_tolist(&(event_container->head_dm_parameter), buf, NULL, NULL);
        cwmp_save_event_container (cwmp,event_container);
        save_acs_bkp_config(cwmp);
        cwmp_scheduleInform_remove_all();
        cwmp_scheduledDownload_remove_all();
		cwmp_apply_scheduled_Download_remove_all();
		cwmp_scheduled_Download_remove_all();
        cwmp_scheduledUpload_remove_all();
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
    }

    return CWMP_OK;
}

int cwmp_root_cause_TransferComplete (struct cwmp *cwmp, struct transfer_complete *p)
{
    struct event_container                      *event_container;
    struct session                              *session;
	struct rpc									*rpc_acs;
	
	pthread_mutex_lock (&(cwmp->mutex_session_queue));
	event_container = cwmp_add_event_container (cwmp, EVENT_IDX_7TRANSFER_COMPLETE, "");
	if (event_container == NULL)
	{
		pthread_mutex_unlock (&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	switch (p->type) {
		case TYPE_DOWNLOAD:
			event_container = cwmp_add_event_container (cwmp, EVENT_IDX_M_Download, p->command_key?p->command_key:"");
			if (event_container == NULL)
			{
				pthread_mutex_unlock (&(cwmp->mutex_session_queue));
				return CWMP_MEM_ERR;
			}
			break;
		case TYPE_UPLOAD:
			event_container = cwmp_add_event_container (cwmp, EVENT_IDX_M_Upload, p->command_key?p->command_key:"");
			if (event_container == NULL)
			{
				pthread_mutex_unlock (&(cwmp->mutex_session_queue));
				return CWMP_MEM_ERR;
			}
			break;
		case TYPE_SCHEDULE_DOWNLOAD:
		event_container = cwmp_add_event_container (cwmp, EVENT_IDX_M_Schedule_Download, p->command_key?p->command_key:"");
			if (event_container == NULL)
			{
				pthread_mutex_unlock (&(cwmp->mutex_session_queue));
				return CWMP_MEM_ERR;
			}
			break;
	}
	session = list_entry (cwmp->head_event_container, struct session,head_event_container);
	if((rpc_acs = cwmp_add_session_rpc_acs(session, RPC_ACS_TRANSFER_COMPLETE)) == NULL)
	{
		pthread_mutex_unlock (&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
	rpc_acs->extra_data = (void *)p;
	pthread_mutex_unlock (&(cwmp->mutex_session_queue));
	return CWMP_OK;
}

int cwmp_root_cause_dustatechangeComplete (struct cwmp *cwmp, struct du_state_change_complete *p)
{
    struct event_container                      *event_container;
    struct session                              *session;
    struct rpc									*rpc_acs;

    pthread_mutex_lock (&(cwmp->mutex_session_queue));
    event_container = cwmp_add_event_container (cwmp, EVENT_IDX_11DU_STATE_CHANGE_COMPLETE, "");
    if (event_container == NULL)
    {
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
        return CWMP_MEM_ERR;
    }
	
	event_container = cwmp_add_event_container (cwmp, EVENT_IDX_M_ChangeDUState, p->command_key?p->command_key:"");
	if (event_container == NULL)
	{
		pthread_mutex_unlock (&(cwmp->mutex_session_queue));
		return CWMP_MEM_ERR;
	}
    session = list_entry (cwmp->head_event_container, struct session,head_event_container);
    if((rpc_acs = cwmp_add_session_rpc_acs(session, RPC_ACS_DU_STATE_CHANGE_COMPLETE)) == NULL)
    {
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
        return CWMP_MEM_ERR;
    }
    rpc_acs->extra_data = (void *)p;
    pthread_mutex_unlock (&(cwmp->mutex_session_queue));
    return CWMP_OK;
}

int cwmp_root_cause_getRPCMethod (struct cwmp *cwmp)
{
    struct event_container *event_container;
    struct session *session;

    if (cwmp->env.periodic == CWMP_START_PERIODIC) {
        pthread_mutex_lock (&(cwmp->mutex_session_queue));
        cwmp->env.periodic = 0;
        event_container = cwmp_add_event_container (cwmp, EVENT_IDX_2PERIODIC, "");
        if (event_container == NULL) {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            return CWMP_MEM_ERR;
        }
        cwmp_save_event_container (cwmp,event_container);
        session = list_entry (cwmp->head_event_container, struct session,head_event_container);
        if (cwmp_add_session_rpc_acs(session, RPC_ACS_GET_RPC_METHODS) == NULL) {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            return CWMP_MEM_ERR;
        }
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
    }

    return CWMP_OK;
}

void *thread_event_periodic (void *v)
{
    struct cwmp *cwmp = (struct cwmp *) v;
    struct event_container *event_container;
    static int periodic_interval;
    static bool periodic_enable;
    static time_t periodic_time;
    static struct timespec periodic_timeout = {0, 0};
    time_t current_time;
    long int delta_time;

    periodic_interval = cwmp->conf.period;
    periodic_enable = cwmp->conf.periodic_enable;
    periodic_time = cwmp->conf.time;

    for(;;) {
        pthread_mutex_lock (&(cwmp->mutex_periodic));
        if (cwmp->conf.periodic_enable) {
	        current_time = time(NULL);
		    if (periodic_time != 0) {
		    	delta_time = (current_time - periodic_time) % periodic_interval;
		    	if (delta_time >= 0)
		    		periodic_timeout.tv_sec = current_time + periodic_interval - delta_time;
	    		else
	    			periodic_timeout.tv_sec = current_time - delta_time;
		    } else {
		    	periodic_timeout.tv_sec = current_time + periodic_interval;
		    }
		    cwmp->session_status.next_periodic = periodic_timeout.tv_sec;
        	pthread_cond_timedwait(&(cwmp->threshold_periodic), &(cwmp->mutex_periodic), &periodic_timeout);
        } else {
            cwmp->session_status.next_periodic = 0;
        	pthread_cond_wait(&(cwmp->threshold_periodic), &(cwmp->mutex_periodic));
        }
        pthread_mutex_unlock (&(cwmp->mutex_periodic));
        if (periodic_interval != cwmp->conf.period ||
        		periodic_enable != cwmp->conf.periodic_enable ||
				periodic_time != cwmp->conf.time) {
        	periodic_enable = cwmp->conf.periodic_enable;
        	periodic_interval = cwmp->conf.period;
        	periodic_time = cwmp->conf.time;
            continue;
        }
        CWMP_LOG(INFO,"Periodic thread: add periodic event in the queue");
        pthread_mutex_lock (&(cwmp->mutex_session_queue));
        event_container = cwmp_add_event_container (cwmp, EVENT_IDX_2PERIODIC, "");
        if (event_container == NULL) {
            pthread_mutex_unlock (&(cwmp->mutex_session_queue));
            continue;
        }
        cwmp_save_event_container (cwmp,event_container);
        pthread_mutex_unlock (&(cwmp->mutex_session_queue));
        pthread_cond_signal(&(cwmp->threshold_session_send));
    }
    return CWMP_OK;
}

void *thread_periodic_check_notify (void *v)
{
    struct cwmp *cwmp = (struct cwmp *) v;
    static int periodic_interval;
    static bool periodic_enable;
    static struct timespec periodic_timeout = {0, 0};
    time_t current_time;
    int is_notify;

    periodic_interval = cwmp->conf.periodic_notify_interval;
    periodic_enable = cwmp->conf.periodic_notify_enable;

    for(;;) {
        if (periodic_enable) {
        	pthread_mutex_lock (&(cwmp->mutex_notify_periodic));
	        current_time = time(NULL);
	        periodic_timeout.tv_sec = current_time + periodic_interval;
        	pthread_cond_timedwait(&(cwmp->threshold_notify_periodic), &(cwmp->mutex_notify_periodic), &periodic_timeout);
        	pthread_mutex_lock(&(cwmp->mutex_session_send));
        	is_notify = check_value_change();
        	if (is_notify > 0)
        		cwmp_update_enabled_notify_file();
        	pthread_mutex_unlock(&(cwmp->mutex_session_send));
        	if (is_notify & NOTIF_ACTIVE)
        		send_active_value_change();
        	if (is_notify & NOTIF_LW_ACTIVE)
        		cwmp_lwnotification();
        	pthread_mutex_unlock (&(cwmp->mutex_notify_periodic));
        }
        else
        	break;
    }
    return CWMP_OK;
}

bool event_exist_in_list(struct cwmp *cwmp, int event)
{
	struct event_container   *event_container;
	list_for_each_entry(event_container, cwmp->head_event_container, list) {
		if (event_container->code == event)
			return true;
	}
	return false;
}
int cwmp_root_cause_event_periodic(struct cwmp *cwmp)
{
    static int period = 0;
    static bool periodic_enable = false;
    static time_t periodic_time = 0;
    char local_time[27] = {0};
    struct tm *t_tm;
    
    if (period == cwmp->conf.period &&
    		periodic_enable == cwmp->conf.periodic_enable &&
			periodic_time == cwmp->conf.time)
        return CWMP_OK;

    pthread_mutex_lock(&(cwmp->mutex_periodic));
    period = cwmp->conf.period;
    periodic_enable = cwmp->conf.periodic_enable;
    periodic_time = cwmp->conf.time;
    CWMP_LOG(INFO,periodic_enable?"Periodic event is enabled. Interval period = %ds":"Periodic event is disabled", period);
	
	t_tm = localtime(&periodic_time);
	if (t_tm == NULL)
		return CWMP_GEN_ERR;

	if (strftime(local_time, sizeof(local_time), "%FT%T%z", t_tm) == 0)
		return CWMP_GEN_ERR;
	
	local_time[25] = local_time[24];
	local_time[24] = local_time[23];
	local_time[22] = ':';
	local_time[26] = '\0';
	
    CWMP_LOG(INFO,periodic_time?"Periodic time is %s":"Periodic time is Unknown", local_time);
    pthread_mutex_unlock (&(cwmp->mutex_periodic));
    pthread_cond_signal(&(cwmp->threshold_periodic));
    return CWMP_OK;
}

void sotfware_version_value_change(struct cwmp *cwmp, struct transfer_complete *p)
{
	char *current_software_version = NULL;

	if (!p->old_software_version || p->old_software_version[0] == 0)
		return;

	current_software_version = cwmp->deviceid.softwareversion;
	if (p->old_software_version && current_software_version &&
		strcmp(p->old_software_version, current_software_version) != 0) {
		pthread_mutex_lock (&(cwmp->mutex_session_queue));
		cwmp_add_event_container (cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		pthread_mutex_unlock (&(cwmp->mutex_session_queue));
	}
}

void connection_request_ip_value_change(struct cwmp *cwmp, int version)
{
	char *bip = NULL;
	struct event_container *event_container;
	char *ip_version = (version == IPv6) ? strdup("ipv6") : strdup("ip");
	char *ip_value = (version == IPv6) ? strdup(cwmp->conf.ipv6) : strdup(cwmp->conf.ip);

	if (version == IPv6)
		cwmp_load_saved_session(cwmp, &bip, CR_IPv6);
	else
		cwmp_load_saved_session(cwmp, &bip, CR_IP);

	if (bip == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
		FREE(ip_version);
		FREE(ip_value);
		return;
	}
	if (strcmp(bip, ip_value) != 0) {
		pthread_mutex_lock (&(cwmp->mutex_session_queue));
		event_container = cwmp_add_event_container (cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			FREE(bip);
			pthread_mutex_unlock (&(cwmp->mutex_session_queue));
			FREE(ip_version);
			FREE(ip_value);
			return;
		}
		cwmp_save_event_container (cwmp,event_container);
		bkp_session_simple_insert_in_parent("connection_request", ip_version, ip_value);
		bkp_session_save();
		pthread_mutex_unlock (&(cwmp->mutex_session_queue));
		pthread_cond_signal(&(cwmp->threshold_session_send));
	}
	FREE(bip);
	FREE(ip_version);
	FREE(ip_value);
}

void connection_request_port_value_change(struct cwmp *cwmp, int port)
{
	char *bport = NULL;
	struct event_container *event_container;
	char bufport[16];

	snprintf(bufport, sizeof(bufport), "%d", port);

	cwmp_load_saved_session(cwmp, &bport, CR_PORT);

	if (bport == NULL) {
		bkp_session_simple_insert_in_parent("connection_request", "port", bufport);
		bkp_session_save();
		return;
	}
	if (strcmp(bport, bufport) != 0) {
		event_container = cwmp_add_event_container (cwmp, EVENT_IDX_4VALUE_CHANGE, "");
		if (event_container == NULL) {
			FREE(bport);
			return;
		}
		cwmp_save_event_container (cwmp,event_container);
		bkp_session_simple_insert_in_parent("connection_request", "port", bufport);
		bkp_session_save();
	}
	FREE(bport);
}

int cwmp_root_cause_events (struct cwmp *cwmp)
{
    int error;

    if ((error = cwmp_root_cause_event_bootstrap(cwmp)))
		return error;

    if ((error = cwmp_root_cause_event_boot(cwmp)))
        return error;

    if ((error = cwmp_root_cause_getRPCMethod(cwmp)))
        return error;

    if ((error = cwmp_root_cause_event_periodic(cwmp)))
        return error;

    return CWMP_OK;
}

