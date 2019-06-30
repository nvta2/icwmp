/*	TR-069 STUN  client software
 *	Copyright (C) 2018 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 *		Author: Omar Kallel <omar.kallel@pivasoftware.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <uci.h>
#include "log.h"

#define VAR_STATE "/var/state"
#define DEFAULT_SERVERPORT 3478
#define DEFAULT_CLIENTPORT 7547
#define DEFAULT_MINKEEPALIVE 30
#define DEFAULT_RETRYTIME 3
#define DEFAULT_MAXKEEPALIVE 3600
#define DEFAULT_LOGLEVEL SINFO

struct stun_config {
	char *server_address;
	char *username;
	char *password;
	int server_port;
	int client_port;
	int max_keepalive;
	int min_keepalive;
	int loglevel;
};

extern struct stun_config conf;

int config_init(void);
int config_fini(void);

int suci_init(void);
int suci_fini(void);
void suci_print_list(struct uci_list *uh, char **val, char *delimiter);
char *suci_get_value(char *package, char *section, char *option);
char *suci_set_value(char *package, char *section, char *option, char *value);
char *suci_get_value_state(char *package, char *section, char *option);
char *suci_set_value_state(char *package, char *section, char *option, char *value);

#endif //__CONFIG_H
