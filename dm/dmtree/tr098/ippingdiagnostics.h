/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 */
#ifndef __IPPING_DIAGNOSTIC_H
#define __IPPING_DIAGNOSTIC_H

extern DMLEAF tIPPingDiagnosticsParam[];

int get_ipping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_dscp(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_failure_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_average_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_min_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_ipping_max_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_ipping_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_repetition_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_block_size(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_ipping_dscp(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
