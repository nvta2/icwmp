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
#include "common.h"
#include "log.h"

//struct uci_context *cwmp_uci_ctx = ((void *)0);
#define UCI_CONFIG_DIR "/etc/config/"
#define LIB_DB_CONFIG "/lib/db/config"
#define ETC_DB_CONFIG "/etc/board-db/config"
#define VARSTATE_CONFIG "/var/state"
#define BBFDM_CONFIG "/etc/bbfdm"
#define BBFDM_SAVEDIR "/tmp/.bbfdm"

typedef enum uci_config_action {
	CWMP_CMD_SET,
	CWMP_CMD_SET_STATE,
	CWMP_CMD_ADD_LIST,
	CWMP_CMD_DEL,
} uci_config_action;

enum uci_paths_types {
	UCI_STANDARD_CONFIG,
	UCI_DB_CONFIG,
	UCI_BOARD_DB_CONFIG,
	UCI_VARSTATE_CONFIG,
	UCI_BBFDM_CONFIG
};

enum cwmp_uci_cmp {
	CWMP_CMP_SECTION,
	CWMP_CMP_OPTION_EQUAL,
	CWMP_CMP_OPTION_REGEX,
	CWMP_CMP_OPTION_CONTAINING,
	CWMP_CMP_OPTION_CONT_WORD,
	CWMP_CMP_LIST_CONTAINING,
	CWMP_CMP_FILTER_FUNC
};

enum cwmp_uci_walk {
	CWMP_GET_FIRST_SECTION,
	CWMP_GET_NEXT_SECTION
};


struct config_uci_list {
    struct list_head                    list;
    char                                *value;
};

struct uci_paths {
	char *conf_dir;
	char *save_dir;
};

int cwmp_uci_init(int uci_path_type);
void cwmp_uci_exit(void);
int cwmp_uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value);
int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct list_head *list);
int uci_get_state_value(char *cmd,char **value);
int uci_set_state_value(char *cmd);
int uci_set_value(char *cmd);
int uci_get_value(char *cmd,char **value);
char* cwmp_db_get_value_string(char *package, char *section, char *option);
struct uci_section *cwmp_uci_walk_section (char *package, char *stype, void *arg1, void *arg2, int cmp , int (*filter)(struct uci_section *s, void *value), struct uci_section *prev_section, int walk);
int cwmp_uci_get_value_by_section_string(struct uci_section *s, char *option, char **value);

#define cwmp_uci_path_foreach_option_eq(package, stype, option, val, section) \
	for (section = cwmp_uci_walk_section(package, stype, option, val, CWMP_CMP_OPTION_EQUAL, NULL, NULL, CWMP_GET_FIRST_SECTION); \
		section != NULL; \
		section = cwmp_uci_walk_section(package, stype, option, val, CWMP_CMP_OPTION_EQUAL, NULL, section, CWMP_GET_NEXT_SECTION))

#define cwmp_uci_path_foreach_sections(package, stype, section) \
	for (section = cwmp_uci_walk_section(package, stype, NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION); \
		section != NULL; \
		section = cwmp_uci_walk_section(package, stype, NULL, NULL, CMP_SECTION, NULL, section, GET_NEXT_SECTION))
#endif
