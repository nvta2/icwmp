/*
 * config.c -- contains functions that allows reading and loading of uci config parameters of stun
 *
 *  Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 *  Author: Omar Kallel <omar.kallel@pivasoftware.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

#include "config.h"
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

static struct uci_context *uci_ctx = NULL;
static struct uci_context *uci_ctx_state = NULL;
struct stun_config conf;

int config_fini(void)
{
	free(conf.server_address);
	free(conf.password);
	free(conf.username);
	conf.server_address = NULL;
	conf.username = NULL;
	conf.password = NULL;
	return 0;
}

int config_init(void)
{
	char *v;
	suci_init();
	memset(&conf, 0, sizeof(struct stun_config));

	v = suci_get_value("cwmp_stun", "stun", "server_address");
	if (!*v) {
		stun_log(SCRIT, "Missing Server Address in the STUN config");
		goto error;
	}
	conf.server_address = strdup(v);

	v = suci_get_value("cwmp_stun", "stun", "username");
	if (*v)
		conf.username = strdup(v);

	v = suci_get_value("cwmp_stun", "stun", "password");
	if (*v)
		conf.password = strdup(v);

	v = suci_get_value("cwmp_stun", "stun", "server_port");
	if (*v)
		conf.server_port = atoi(v);
	else
		conf.server_port = DEFAULT_SERVERPORT;

	v = suci_get_value("cwmp_stun", "stun", "log_level");
	if (*v)
		conf.loglevel = atoi(v);
	else
		conf.loglevel = DEFAULT_LOGLEVEL;

	v = suci_get_value("cwmp_stun", "stun", "min_keepalive");
	if (*v)
		conf.min_keepalive = atoi(v);
	else
		conf.min_keepalive = DEFAULT_MINKEEPALIVE;

	v = suci_get_value("cwmp_stun", "stun", "max_keepalive");
	if (*v)
		conf.max_keepalive = atoi(v);
	else
		conf.max_keepalive = DEFAULT_MAXKEEPALIVE;

	v = suci_get_value("cwmp_stun", "stun", "client_port");
	if (*v)
		conf.client_port = atoi(v);

	if (conf.max_keepalive <= 0)
		conf.max_keepalive = DEFAULT_MAXKEEPALIVE;

	if (conf.min_keepalive <= 0)
		conf.min_keepalive = DEFAULT_MINKEEPALIVE;

	if (conf.server_port <= 0)
		conf.server_port = DEFAULT_SERVERPORT;

	if (conf.loglevel >= __MAX_SLOG || conf.loglevel < 0) {
		conf.loglevel = DEFAULT_LOGLEVEL;
	}

	stun_log(SINFO, "STUN CONFIG - Server Address: %s", conf.server_address);
	stun_log(SINFO, "STUN CONFIG - Username: %s", conf.username ? conf.username : "<not defined>");
	stun_log(SINFO, "STUN CONFIG - Server port: %d", conf.server_port);
	stun_log(SINFO, "STUN CONFIG - min keepalive: %d", conf.min_keepalive);
	stun_log(SINFO, "STUN CONFIG - max keepalive: %d", conf.max_keepalive);
	stun_log(SINFO, "STUN CONFIG - Client port: %d", (conf.client_port > 0) ? conf.client_port : -1);
	stun_log(SINFO, "STUN CONFIG - LOG Level: %d (Critical=0, Warning=1, Notice=2, Info=3, Debug=4)", conf.loglevel);
	suci_fini();
	return 0;

error:
	suci_fini();
	config_fini();
	exit(1);
}

int suci_init(void)
{
	uci_ctx = uci_alloc_context();
	if (!uci_ctx) {
		return -1;
	}
	uci_ctx_state = uci_alloc_context();
	if (!uci_ctx_state) {
		return -1;
	}
	uci_add_delta_path(uci_ctx_state, uci_ctx_state->savedir);
	uci_set_savedir(uci_ctx_state, VAR_STATE);
	return 0;
}

int suci_fini(void)
{
	if (uci_ctx) {
		uci_free_context(uci_ctx);
	}
	if (uci_ctx_state) {
		uci_free_context(uci_ctx_state);
	}
	uci_ctx = NULL;
	uci_ctx_state = NULL;
	return 0;
}

static bool suci_validate_section(const char *str)
{
	if (!*str)
		return false;

	for (; *str; str++) {
		unsigned char c = *str;

		if (isalnum(c) || c == '_')
			continue;

		return false;
	}
	return true;
}

static int suci_init_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value)
{
	char *last = NULL;
	char *tmp;

	memset(ptr, 0, sizeof(struct uci_ptr));

	/* value */
	if (value) {
		ptr->value = value;
	}
	ptr->package = package;
	if (!ptr->package)
		goto error;

	ptr->section = section;
	if (!ptr->section) {
		ptr->target = UCI_TYPE_PACKAGE;
		goto lastval;
	}

	ptr->option = option;
	if (!ptr->option) {
		ptr->target = UCI_TYPE_SECTION;
		goto lastval;
	} else {
		ptr->target = UCI_TYPE_OPTION;
	}

lastval:
	if (ptr->section && !suci_validate_section(ptr->section))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	return 0;

error:
	return -1;
}

void suci_print_list(struct uci_list *uh, char **val, char *delimiter)
{
	struct uci_element *e;
	static char buffer[512];
	int dlen = strlen(delimiter);
	int olen = 0;
	char *buf = buffer;
	*buf = '\0';

	uci_foreach_element(uh, e) {
		if (*buf) {
			strcat(buf, delimiter);
			strcat(buf, e->name);
		}
		else {
			strcpy(buf, e->name);
		}
	}
	*val = buf;
}

char *suci_get_value(char *package, char *section, char *option)
{
	struct uci_ptr ptr;
	char *val = "";

	if (!section || !option)
		return val;

	if (suci_init_ptr(uci_ctx, &ptr, package, section, option, NULL)) {
		return val;
	}
	if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
		return val;
	}

	if (!ptr.o)
		return val;

	if(ptr.o->type == UCI_TYPE_LIST) {
		suci_print_list(&ptr.o->v.list, &val, " ");
		return val;
	}

	if (ptr.o->v.string)
		return ptr.o->v.string;
	else
		return val;
}

char *suci_set_value(char *package, char *section, char *option, char *value)
{
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (!section)
		return "";

	if (suci_init_ptr(uci_ctx, &ptr, package, section, option, value)) {
		return "";
	}
	if (uci_lookup_ptr(uci_ctx, &ptr, NULL, true) != UCI_OK) {
		return "";
	}

	uci_set(uci_ctx, &ptr);

	if (ret == UCI_OK)
		ret = uci_save(uci_ctx, ptr.p);

	if (ptr.o && ptr.o->v.string)
		return ptr.o->v.string;

	return "";
}

/*************************---/var/state--***************************/

char *suci_get_value_state(char *package, char *section, char *option)
{
	char *val;
	struct uci_context *save_uci_ctx = uci_ctx;
	uci_ctx = uci_ctx_state;
	val = suci_get_value(package, section, option);
	uci_ctx = save_uci_ctx;
	return val;
}

char *suci_set_value_state(char *package, char *section, char *option, char *value)
{
	char *val;
	struct uci_context *save_uci_ctx = uci_ctx;
	uci_ctx = uci_ctx_state;
	val = suci_set_value(package, section, option, value);
	uci_ctx = save_uci_ctx;
	return val;
}
