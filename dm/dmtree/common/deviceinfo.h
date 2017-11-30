/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 */

#ifndef __DEVICE_INFO_H
#define __DEVICE_INFO_H
#include "dmcwmp.h"

#define UPTIME "/proc/uptime"
#define DEFAULT_CONFIG_DIR "/etc/config/"
extern DMLEAF tDeviceInfoParams[];
extern DMLEAF tCatTvParams[];
extern DMLEAF tVcfParams[];
extern DMOBJ tDeviceInfoObj[];

char *get_deviceid_manufacturer();
char *get_deviceid_manufactureroui();
char *get_deviceid_productclass();
char *get_deviceid_serialnumber();
char *get_softwareversion();
int lookup_vcf_name(char *instance, char **value);
int browseVcfInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_device_manufacturer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_manufactureroui(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_routermodel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_productclass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_serialnumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_hardwareversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_softwareversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_info_uptime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_devicelog(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_specversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_provisioningcode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_base_mac_addr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_device_memory_bank(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_optical_input_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_rf_output_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_temperature(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_catv_voltage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_date(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_desc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_vcf_backup_restore(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_device_provisioningcode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_device_catvenabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_device_memory_bank(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_device_catvenabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_vcf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
