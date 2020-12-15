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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include "cwmp.h"
#include "backupSession.h"
#include "xml.h"
#include "log.h"
#include "config.h"

pthread_mutex_t  mutex_config_load = PTHREAD_MUTEX_INITIALIZER;


struct option long_options[] = {
	{"boot-event", no_argument, NULL, 'b'},
	{"get-rpc-methods", no_argument, NULL, 'g'},
	{"command-input", no_argument, NULL, 'c'},
	{"shell-cli", required_argument, NULL, 'm'},
	{"alias-based-addressing", no_argument, NULL, 'a'},
	{"instance-mode-number", no_argument, NULL, 'N'},
	{"instance-mode-alias", no_argument, NULL, 'A'},
	{"upnp", no_argument, NULL, 'U'},
	{"user-acl", required_argument, NULL, 'u'},
	{"amendment", required_argument, NULL, 'M'},
	{"time-tracking", no_argument, NULL, 't'},
	{"evaluating-test", no_argument, NULL, 'E'},
	{"file", required_argument, NULL, 'f'},
	{"wep", required_argument, NULL, 'w'},
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{NULL, 0, NULL, 0}
};

static void show_help(void)
{
	printf("Usage: icwmpd [OPTIONS]\n");
	printf(" -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event\n");
	printf(" -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS\n");
	printf(" -h, --help                                          Display this help text\n");
	printf(" -v, --version                                       Display the version\n");
}

void show_version()
{
#ifndef CWMP_REVISION
    fprintf(stdout, "\nVersion: %s\n\n",CWMP_VERSION);
#else
    fprintf(stdout, "\nVersion: %s revision %s\n\n",CWMP_VERSION,CWMP_REVISION);
#endif
}

int check_global_config (struct config *conf)
{
    if (conf->acsurl==NULL)
    {
        conf->acsurl = strdup(DEFAULT_ACSURL);
    }
    return CWMP_OK;
}

static void uppercase ( char *sPtr )
{
	while ( *sPtr != '\0' )
	{
		*sPtr = toupper ( ( unsigned char ) *sPtr );
		++sPtr;
	}
}

int get_global_config(struct config *conf)
{
    int                     error, error2, error3;
    char                    *value = NULL, *value2 = NULL, *value3 = NULL;

    if((error = uci_get_value(UCI_CPE_LOG_FILE_NAME,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            log_set_log_file_name (value);
            free(value);
            value = NULL;
        }
    }

    if((error = uci_get_value(UCI_CPE_LOG_MAX_SIZE,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            log_set_file_max_size(value);
            free(value);
            value = NULL;
        }
    }

    if((error = uci_get_value(UCI_CPE_ENABLE_STDOUT_LOG,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            log_set_on_console(value);
            free(value);
            value = NULL;
        }
    }

    if((error = uci_get_value(UCI_CPE_ENABLE_FILE_LOG,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            log_set_on_file(value);
            free(value);
            value = NULL;
        }
    }

    error 	= uci_get_value(UCI_DHCP_DISCOVERY_PATH,&value);
    error2 	= uci_get_value(UCI_ACS_URL_PATH,&value2);
    error3 	= uci_get_state_value(UCI_DHCP_ACS_URL,&value3);

    if ((((error == CWMP_OK) && (value != NULL) && (strcmp(value,"enable") == 0)) ||
	   ((error2 == CWMP_OK) && ((value2 == NULL) || (value2[0] == 0)))) &&
	   ((error3 == CWMP_OK) && (value3 != NULL) && (value3[0] != 0)))
    {
		if (conf->acsurl!=NULL)
		{
			free(conf->acsurl);
		}
		conf->acsurl = value3;
		value3 = NULL;
    }
    else if ((error2 == CWMP_OK) && (value2 != NULL) && (value2[0] != 0))
    {
		if (conf->acsurl!=NULL)
		{
			free(conf->acsurl);
		}
		conf->acsurl = value2;
		value2 = NULL;
    }
    if (value!=NULL)
    {
    	free(value);
    	value = NULL;
    }
    if (value2!=NULL)
	{
		free(value2);
		value2 = NULL;
	}
    if (value3!=NULL)
	{
		free(value3);
		value3 = NULL;
	}

    if((error = uci_get_value(UCI_ACS_USERID_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->acs_userid!=NULL)
            {
                free(conf->acs_userid);
            }
            conf->acs_userid = value;
            value = NULL;
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_ACS_PASSWD_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->acs_passwd!=NULL)
            {
                free(conf->acs_passwd);
            }
            conf->acs_passwd = value;
            value = NULL;
        }
    }
    else
    {
        return error;
    }
    if((error = get_amd_version_config())!= CWMP_OK)
    {
        return error;
    }
    if((error = uci_get_value(UCI_ACS_COMPRESSION ,&value)) == CWMP_OK)
    {
        conf->compression = COMP_NONE;
        if(conf->amd_version >= AMD_5 && value != NULL)
        {
            if (0 == strcasecmp(value, "gzip")) {
                conf->compression = COMP_GZIP;
            } else if (0 == strcasecmp(value, "deflate")) {
                conf->compression = COMP_DEFLATE;
            } else {
                conf->compression = COMP_NONE;
            }
        }
        free(value);
        value = NULL;
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_ACS_RETRY_MIN_WAIT_INTERVAL ,&value)) == CWMP_OK)
    {
        conf->retry_min_wait_interval = DEFAULT_RETRY_MINIMUM_WAIT_INTERVAL;
        if(conf->amd_version >= AMD_3 && value != NULL)
        {
            int a = atoi(value) ;
            if ( a <= 65535 || a >=1) {
                conf->retry_min_wait_interval = a;
            }
        }
        free(value);
        value = NULL;
 
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_ACS_RETRY_INTERVAL_MULTIPLIER ,&value)) == CWMP_OK)
    {
        conf->retry_interval_multiplier = DEFAULT_RETRY_INTERVAL_MULTIPLIER;
        if(conf->amd_version >= AMD_3 && value != NULL)
        {
            int a = atoi(value) ;
            if ( a <= 65535 || a >=1000) {
                conf->retry_interval_multiplier = a;
            }
        }
        free(value);
        value = NULL;
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_ACS_SSL_CAPATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->acs_ssl_capath != NULL)
            {
                free(conf->acs_ssl_capath);
            }
            conf->acs_ssl_capath = value;
            value = NULL;
        }
    }
    else
    {
        FREE(conf->acs_ssl_capath);
    }
    if((error = uci_get_value(UCI_HTTPS_SSL_CAPATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->https_ssl_capath != NULL)
            {
                free(conf->https_ssl_capath);
            }
            conf->https_ssl_capath = value;
            value = NULL;
        }
    }
    else
    {
        FREE(conf->https_ssl_capath);
    }
    if((error = uci_get_value(HTTP_DISABLE_100CONTINUE,&value)) == CWMP_OK)
	{
		if(value != NULL)
		{
			if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0))
				conf->http_disable_100continue = true;
            free(value);
			value = NULL;
		}
	}
    if((error = uci_get_value(UCI_ACS_INSECURE_ENABLE,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {			
            if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0))
            {
                conf->insecure_enable = true;
            }
            free(value);	
            value = NULL;
        }
    }
    if((error = uci_get_value(UCI_ACS_IPV6_ENABLE,&value)) == CWMP_OK)
	{
		if(value != NULL)
		{
			if ((strcasecmp(value,"true")==0) || (strcmp(value,"1")==0))
			{
				conf->ipv6_enable = true;
			}
            free(value);
			value = NULL;
		}
	}
    if((error = uci_get_value(UCI_ACS_SSL_VERSION,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->acs_ssl_version != NULL)
            {
                free(conf->acs_ssl_version);
            }
            conf->acs_ssl_version = value;
            value = NULL;
        }
    }
    else
    {
        FREE(conf->acs_ssl_version);
    }
    if((error = uci_get_value(UCI_CPE_INTERFACE_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->interface!=NULL)
            {
                free(conf->interface);
            }
            conf->interface = value;
            value = NULL;
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_CPE_USERID_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->cpe_userid!=NULL)
            {
                free(conf->cpe_userid);
            }
            conf->cpe_userid = value;
            value = NULL;
        }
    	else
    	{
               if (conf->cpe_userid!=NULL)
                {
                    free(conf->cpe_userid);
                }
                conf->cpe_userid = strdup("");
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_CPE_PASSWD_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            if (conf->cpe_passwd!=NULL)
            {
                free(conf->cpe_passwd);
            }
            conf->cpe_passwd = value;
            value = NULL;
        }
    	else
    	{
               if (conf->cpe_passwd!=NULL)
                {
                    free(conf->cpe_passwd);
                }
                conf->cpe_passwd = strdup("");
        }
    }
    else
    {
        return error;
    }

    if((error = uci_get_value(UCI_CPE_UBUS_SOCKET_PATH,&value)) == CWMP_OK)
	{
		if(value != NULL)
		{
			if (conf->ubus_socket!=NULL)
			{
				free(conf->ubus_socket);
			}
			conf->ubus_socket = value;
			value = NULL;
		}
	}
	else
	{
		return error;
	}

    if((error = uci_get_value(UCI_LOG_SEVERITY_PATH,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            log_set_severity_idx (value);
            free(value);
            value = NULL;
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_CPE_PORT_PATH,&value)) == CWMP_OK)
    {
        int a = 0;

        if(value != NULL)
        {
            a = atoi(value);
            free(value);
            value = NULL;
        }
        if(a==0)
        {
            CWMP_LOG(INFO,"Set the connection request port to the default value: %d",DEFAULT_CONNECTION_REQUEST_PORT);
            conf->connection_request_port = DEFAULT_CONNECTION_REQUEST_PORT;
        }
        else
        {
            conf->connection_request_port = a;
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_CPE_NOTIFY_PERIODIC_ENABLE,&value)) == CWMP_OK)
    {
    	bool a = true;
	    if(value != NULL)
        {
            if ((strcasecmp(value,"FALSE")==0) || (strcmp(value,"0")==0))
            {
                a = false;
            }
            else
            {
                a = true;
            }
            free(value);
            value = NULL;
        }
	    conf->periodic_notify_enable = a;
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_CPE_NOTIFY_PERIOD,&value)) == CWMP_OK)
    {
        int a = 0;

        if(value != NULL)
        {
            a = atoi(value);
            free(value);
            value = NULL;
        }
        if(a==0)
        {
            CWMP_LOG(INFO,"Set notify period to the default value: %d",DEFAULT_NOTIFY_PERIOD);
            conf->periodic_notify_interval = DEFAULT_NOTIFY_PERIOD;
        }
        else
        {
            conf->periodic_notify_interval = a;
        }
    }
    else
    {
        return error;
    }
     if((error = uci_get_value(UCI_PERIODIC_INFORM_TIME_PATH,&value)) == CWMP_OK)
    {
        int a = 0;

        if(value != NULL)
        {
            a = atol(value);
            free(value);
            value = NULL;
        }
        conf->time = a;
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_PERIODIC_INFORM_INTERVAL_PATH,&value)) == CWMP_OK)
    {
        int a = 0;

        if(value != NULL)
        {
            a = atoi(value);
            free(value);
            value = NULL;
        }
        if(a>=PERIOD_INFORM_MIN)
        {
            conf->period = a;
        }
        else
        {
            CWMP_LOG(ERROR,"Period interval of periodic inform should be > %ds. Set to default: %ds",PERIOD_INFORM_MIN,PERIOD_INFORM_DEFAULT);
            conf->period = PERIOD_INFORM_DEFAULT;
        }
    }
    else
    {
        return error;
    }
    if((error = uci_get_value(UCI_PERIODIC_INFORM_ENABLE_PATH,&value)) == CWMP_OK)
	{
		if(value != NULL)
		{
			uppercase(value);
			if ((strcmp(value,"TRUE")==0) || (strcmp(value,"1")==0))
			{
				conf->periodic_enable = true;
			}
			else
			{
				conf->periodic_enable = false;
			}
			free(value);
			value = NULL;
		}
		else
		{
			conf->periodic_enable = false;
		}
	}
	else
	{
		return error;
	}
    if((error = get_instance_mode_config())!= CWMP_OK)
    {
        return error;
    }
	if((error = get_session_timeout_config())!= CWMP_OK)
    {
        return error;
    }
	return CWMP_OK;
}

int get_amd_version_config()
{
	 int error;
	 int a = 0;
	 char *value = NULL;
	 struct cwmp   *cwmp = &cwmp_main;
	 if((error = uci_get_value(UCI_CPE_AMD_VERSION ,&value)) == CWMP_OK)
	 {
		 cwmp->conf.amd_version = DEFAULT_AMD_VERSION;
		 if(value != NULL)
		 {
			 a = atoi(value) ;
			 if ( a >= 1 ) {
				 cwmp->conf.amd_version = a;
			 }
			 free(value);
			 value = NULL;
		 }
		 cwmp->conf.supported_amd_version = cwmp->conf.amd_version;
	 }
	 else
	 {
		 return error;
	 }
	 return CWMP_OK;
}

int get_session_timeout_config()
{
	 int error;
	 int a = 0;
	 char *value = NULL;
	 struct cwmp   *cwmp = &cwmp_main;
	 if((error = uci_get_value(UCI_CPE_SESSION_TIMEOUT ,&value)) == CWMP_OK)
	 {
		 cwmp->conf.session_timeout = DEFAULT_SESSION_TIMEOUT;
		 if(value != NULL)
		 {
			 a = atoi(value) ;
			 if ( a >= 1 ) {
				 cwmp->conf.session_timeout = a;
			 }
			 free(value);
			 value = NULL;
		 }
	 }
	 else
	 {
		 return error;
	 }
	 return CWMP_OK;
}

int get_instance_mode_config()
{
	 int error;
	 char *value = NULL;
	 struct cwmp   *cwmp = &cwmp_main;
	 if((error = uci_get_value(UCI_CPE_INSTANCE_MODE ,&value)) == CWMP_OK)
	    {
		 cwmp->conf.instance_mode = DEFAULT_INSTANCE_MODE;
	        if(value != NULL)
	        {
	            if ( 0 == strcmp(value, "InstanceNumber") ) {
	            	cwmp->conf.instance_mode = INSTANCE_MODE_NUMBER;
	            } else {
	            	cwmp->conf.instance_mode = INSTANCE_MODE_ALIAS;
	            }
	            free(value);
	            value = NULL;
	        }
	    }
	    else
	    {
	        return error;
	    }
	 return CWMP_OK;
}

int get_lwn_config(struct config *conf)
{
    int error;
    int a = 0;    
    char *value = NULL;
    if((error = uci_get_value(LW_NOTIFICATION_ENABLE,&value)) == CWMP_OK)
    {
	    if(value != NULL)
        {
            uppercase(value);
            if ((strcmp(value,"TRUE")==0) || (strcmp(value,"1")==0))
            {
                conf->lw_notification_enable = true;
            }
            else
            {
                conf->lw_notification_enable = false;
            }
            free(value);
            value = NULL;
        }
    }
    if((error = uci_get_value(LW_NOTIFICATION_HOSTNAME,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            conf->lw_notification_hostname = strdup(value);
            free(value);
            value = NULL;
        }
        else
        {
            conf->lw_notification_hostname = strdup(conf->acsurl);
        }
                
    }
    if((error = uci_get_value(LW_NOTIFICATION_PORT,&value)) == CWMP_OK)
    {
        if(value != NULL)
        {
            a = atoi(value);
            conf->lw_notification_port = a;
            free(value);
            value = NULL;
        }
        else
        {
            conf->lw_notification_port = DEFAULT_LWN_PORT;
        }
	}
    return CWMP_OK;
}

int global_env_init (int argc, char** argv, struct env *env)
{

	int c, option_index = 0;

	while ((c = getopt_long(argc, argv, "bghv", long_options, &option_index)) != -1) {

		switch (c)
		{
		case 'b':
			env->boot = CWMP_START_BOOT;
			break;

		case 'g':
			env->periodic = CWMP_START_PERIODIC;
			break;

		case 'h':
			show_help();
			exit(0);

		case 'v':
			show_version();
			exit(0);
		}
	}
	return CWMP_OK;
}

int global_conf_init (struct config *conf)
{
    int error;

    pthread_mutex_lock (&mutex_config_load);
    if ((error = get_global_config(conf)))
    {
    	pthread_mutex_unlock (&mutex_config_load);
        return error;
    }
    if ((error = check_global_config(conf)))
    {
    	pthread_mutex_unlock (&mutex_config_load);
        return error;
    }
    get_lwn_config(conf);
    pthread_mutex_unlock (&mutex_config_load);
    return CWMP_OK;
}

int save_acs_bkp_config(struct cwmp *cwmp)
{
    struct config   *conf;

    conf = &(cwmp->conf);
	bkp_session_simple_insert("acs", "url", conf->acsurl);
	bkp_session_save();
    return CWMP_OK;
}

int cwmp_get_deviceid(struct cwmp *cwmp) {
	cwmp->deviceid.manufacturer = strdup(cwmp_db_get_value_string("device", "deviceinfo", "Manufacturer")); //TODO free
	cwmp->deviceid.serialnumber = strdup(cwmp_db_get_value_string("device", "deviceinfo", "SerialNumber"));
	cwmp->deviceid.productclass = strdup(cwmp_db_get_value_string("device", "deviceinfo", "ProductClass"));
	cwmp->deviceid.oui = strdup(cwmp_db_get_value_string("device", "deviceinfo", "ManufacturerOUI"));
	cwmp->deviceid.softwareversion = strdup(cwmp_db_get_value_string("device", "deviceinfo", "SoftwareVersion"));
	return CWMP_OK;
}

int cwmp_init(int argc, char** argv,struct cwmp *cwmp)
{
    int         error;
    struct env  env;

    memset(&env,0,sizeof(struct env));
    if ((error = global_env_init (argc, argv, &env)))
        return error;

    /* Only One instance should run*/
    cwmp->pid_file = open("/var/run/icwmpd.pid", O_CREAT | O_RDWR, 0666);
    fcntl(cwmp->pid_file, F_SETFD, fcntl(cwmp->pid_file, F_GETFD) | FD_CLOEXEC);
    int rc = flock(cwmp->pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK != errno)
        {
        	char *piderr = "PID file creation failed: Quit the daemon!";
        	fprintf(stderr, "%s\n", piderr);
        	CWMP_LOG(ERROR, "%s",piderr);
        	exit(EXIT_FAILURE);
        }
        else exit(EXIT_SUCCESS);
    }

    pthread_mutex_init(&cwmp->mutex_periodic, NULL);
    pthread_mutex_init(&cwmp->mutex_session_queue, NULL);
    pthread_mutex_init(&cwmp->mutex_session_send, NULL);
    memcpy(&(cwmp->env),&env,sizeof(struct env));
    INIT_LIST_HEAD(&(cwmp->head_session_queue));

    if ((error = global_conf_init(&(cwmp->conf))))
        return error;

    cwmp_get_deviceid(cwmp);
    return CWMP_OK;
}

int cwmp_config_reload(struct cwmp *cwmp)
{
    int error;

    memset(&cwmp->env, 0, sizeof(struct env));
    memset(&cwmp->conf, 0, sizeof(struct config));

    if ((error = global_conf_init(&(cwmp->conf))))
        return error;

    return CWMP_OK;
}
