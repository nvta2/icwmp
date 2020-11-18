#ifndef __CWMPUCI_H
#define __CWMPUCI_H

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <uci.h>
#include <libubox/list.h>

#define LIB_DB_CONFIG "/lib/db/config"
#define ETC_DB_CONFIG "/etc/board-db/config"
#define VARSTATE_CONFIG "/var/state"
#define BBFDM_CONFIG "/etc/bbfdm"
#define BBFDM_SAVEDIR "/tmp/.bbfdm"
#define UCI_CONFIG_DIR "/etc/config/"

struct config_uci_list {
    struct list_head                    list;
    char                                *value;
};

int cwmp_uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value);
int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct list_head *list);
#endif
