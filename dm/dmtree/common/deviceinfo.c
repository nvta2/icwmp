/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include <ctype.h>
#include <uci.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/klog.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "deviceinfo.h"
#include "dmjson.h"

inline int entry_method_device_info_vcf(struct dmctx *ctx);
inline int entry_method_device_info_vcf_instance(struct dmctx *ctx, char *ivcf);
inline int entry_method_device_info_vlf(struct dmctx *ctx);
inline int entry_method_device_info_vlf_instance(struct dmctx *ctx, char *ivlf);
struct dev_vcf cur_dev_vcf = {0};
struct dev_vlf cur_dev_vlf = {0};

inline int init_args_vcf(struct dmctx *ctx, struct uci_section *s)
{
	struct dev_vcf *args = &cur_dev_vcf;
	ctx->args = (void *)args;
	args->vcf_sec = s;
	return 0;
}

inline int init_args_vlf(struct dmctx *ctx, struct uci_section *s)
{
	struct dev_vlf *args = &cur_dev_vlf;
	ctx->args = (void *)args;
	args->vlf_sec = s;
	return 0;
}
char *get_deviceid_manufacturer()
{
	char *v;
	dmuci_get_option_value_string("cwmp","cpe","manufacturer", &v);
	return v;
}

char *get_deviceid_manufactureroui()
{
	char *v;
	char str[16];
	char *mac = NULL;	
	json_object *res;
	FILE *nvrammac=NULL;
	char macreadfile[18]={0};

	dmuci_get_option_value_string("cwmp", "cpe", "override_oui", &v);
	if (v[0] == '\0')
	{
		dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
		if(!(res)){
			db_get_value_string("hw", "board", "BaseMacAddr", &mac);
			if(!mac || strlen(mac)==0 ){
				if ((nvrammac = fopen("/proc/nvram/BaseMacAddr", "r")) == NULL)
				{
					mac = NULL;
				}
				else{
					fscanf(nvrammac,"%[^\n]", macreadfile);
					macreadfile[17]='\0';
					sscanf(macreadfile,"%2c %2c %2c", str, str+2, str+4);
					str[6]='\0';
					v = dmstrdup(str); // MEM WILL BE FREED IN DMMEMCLEAN
					fclose(nvrammac);
					return v;
				}
			}
		}
		else
			mac = dm_ubus_get_value(res, 2, "system", "basemac");
		if(mac)
		{
			size_t ln = strlen(mac);
			if (ln<17) goto not_found;
			sscanf (mac,"%2c:%2c:%2c",str,str+2,str+4);
			str[6] = '\0';
			v = dmstrdup(str); // MEM WILL BE FREED IN DMMEMCLEAN
			return v;
		}
		else
			goto not_found;
	}
	return v;
not_found:
	v = "";
	return v;
}

char *get_deviceid_productclass()
{
	char *v, *tmp, *val;
	char delimiter[] = "_";
	
	db_get_value_string("hw", "board", "iopVersion", &v);
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	val = cut_fx(tmp, delimiter, 1);
	return val;
}


char *get_deviceid_serialnumber()
{
	char *v;
	db_get_value_string("hw", "board", "serialNumber", &v);
	return v;
}

char *get_softwareversion()
{
	char *v, *tmp, *val;
	char delimiter[] = "_";
	
	db_get_value_string("hw", "board", "iopVersion", &v);
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	val = cut_fx(tmp, delimiter, 2);
	return val;
}

int get_device_manufacturer(char *refparam, struct dmctx *ctx, char **value)
{
	*value = get_deviceid_manufacturer();
	return 0;
}

int get_device_manufactureroui(char *refparam, struct dmctx *ctx, char **value)
{
	*value = get_deviceid_manufactureroui();
	return 0;
}

int get_device_productclass(char *refparam, struct dmctx *ctx, char **value)
{
	*value = get_deviceid_productclass();
	return 0;
}

int get_device_serialnumber(char *refparam, struct dmctx *ctx, char **value)
{
	*value = get_deviceid_serialnumber();
	return 0;
}

int get_device_softwareversion(char *refparam, struct dmctx *ctx, char **value)
{
	*value = get_softwareversion();
	return 0;
}

int get_device_hardwareversion(char *refparam, struct dmctx *ctx, char **value)
{
	db_get_value_string("hw", "board", "hardwareVersion", value);
	return 0;
}

int get_device_routermodel(char *refparam, struct dmctx *ctx, char **value)
{
	db_get_value_string("hw", "board", "routerModel", value);
	return 0;
}

int get_device_info_uptime(char *refparam, struct dmctx *ctx, char **value)
{
	FILE* fp = NULL;
	char *pch, *spch;
	char buf[64];
	*value = "0";

	fp = fopen(UPTIME, "r");
	if (fp != NULL) {		
		fgets(buf, 64, fp);
		pch = strtok_r(buf, ".", &spch);
		if (pch)
			*value = dmstrdup(pch); // MEM WILL BE FREED IN DMMEMCLEAN
		fclose(fp);
	}
	return 0;
}

int get_device_devicelog(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "";
	int i = 0, nbrlines = 4;
	char buff[512], *msg = NULL;
	int len = klogctl(3 , buff, sizeof(buff) - 1); /* read ring buffer */
	if (len <= 0)
		return 0;
	buff[len] = '\0';
	char *p = buff;
	while (*p) { //TODO to optimize, we can avoid this if the '<' and '>' does not cause problem in the tests.
		if (*p == '<') {
			*p = '(';
			if (p == buff || *(p-1) == '\n') {
				if(msg == NULL) msg = p;
				i++;
				if (i == nbrlines) {
					break;
				}
			}
		}
		else if (*p == '>')
			*p = ')';
		p++;
	}
	if(msg == NULL)
		*value = "";
	else
		*value = dmstrdup(msg);// MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_device_specversion(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "1.0";
	return 0;
}

int get_device_provisioningcode(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_option_value_string("cwmp", "cpe", "provisioning_code", value);
	return 0;
}

int set_device_provisioningcode(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value("cwmp", "cpe", "provisioning_code", value);
			return 0;
	}
	return 0;
}


int get_base_mac_addr(char *refparam, struct dmctx *ctx, char **value)
{	
	json_object *res;
	
	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "system", "basemac");
	return 0;
}

int get_device_memory_bank(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;

	dmubus_call("router.system", "memory_bank", UBUS_ARGS{{}}, 0, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 1, "code");
	return 0;
}

int set_device_memory_bank(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmubus_call_set("router.system", "memory_bank", UBUS_ARGS{{"bank", value, Integer}}, 1);
			return 0;
	}
	return 0;
}
int get_catv_enabled(char *refparam, struct dmctx *ctx, char **value)
{
	char *catv;
	dmuci_get_option_value_string("catv", "catv", "enable", &catv);
	if (strcmp(catv, "on") == 0) {
		*value = "1";
	} 
	else 
		*value = "0";
	return 0;	
}

int set_device_catvenabled(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	char *stat;
	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if (b)
				stat = "on";
			else
				stat = "off";
			dmuci_set_value("catv", "catv", "enable", stat);
			return 0;
	}
	return 0;
}

int get_catv_optical_input_level(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *str;
	*value = "";
	dmubus_call("catv", "vpd", UBUS_ARGS{}, 0, &res);
	if (!res)
		return 0;
	*value = dm_ubus_get_value(res, 1, "VPD");
	return 0;
}

int get_catv_rf_output_level(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *str;
	*value = "";
	dmubus_call("catv", "rf", UBUS_ARGS{}, 0, &res);
	if (!res)
		return 0;
	*value = dm_ubus_get_value(res, 1, "RF");
	return 0;
}

int get_catv_temperature(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *str;
	*value = "";
	dmubus_call("catv", "temp", UBUS_ARGS{}, 0, &res);
	if (!res)
		return 0;
	*value = dm_ubus_get_value(res, 1, "Temperature");
	return 0;
}

int get_catv_voltage(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	char *str;
	*value = "";
	dmubus_call("catv", "vcc", UBUS_ARGS{}, 0, &res);
	if (!res)
		return 0;
	*value = dm_ubus_get_value(res, 1, "VCC");
	return 0;
}

int get_vcf_name(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "name", value);
	return 0;
}

int get_vlf_name(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vlf.vlf_sec, "log_file", value);
	return 0;
}

int get_vcf_version(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "version", value);
	return 0;
}

int set_vcf_version(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section_icwmpd(cur_dev_vcf.vcf_sec, "version", value);
			return 0;
	}
	return 0;
}

int get_vcf_date(char *refparam, struct dmctx *ctx, char **value)
{
	DIR *dir;
	struct dirent *d_file;
	struct stat attr;
	char path[128];
	char date[sizeof "AAAA-MM-JJTHH:MM:SS.000Z"];
	*value = "";
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "name", value);
	if ((dir = opendir (DEFAULT_CONFIG_DIR)) != NULL) {
		while ((d_file = readdir (dir)) != NULL) {
			if(strcmp(*value, d_file->d_name) == 0) {
				sprintf(path, "DEFAULT_CONFIG_DIR%s", d_file->d_name);
				stat(path, &attr);
				strftime(date, sizeof date, "%Y-%m-%dT%H:%M:%S.000Z", localtime(&attr.st_mtime));
				*value = dmstrdup(date);
			}
		}
		closedir (dir);
	}
	return 0;
}

int get_vcf_backup_restore(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "backup_restore", value);
	return 0;
}

int get_vcf_desc(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "description", value);
	return 0;
}

int set_vcf_desc(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section_icwmpd(cur_dev_vcf.vcf_sec, "description", value);
			return 0;
	}
	return 0;
}

int get_vcf_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vcf.vcf_sec, "vcf_alias", value);
	return 0;
}

int set_vcf_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value_by_section_icwmpd(cur_dev_vcf.vcf_sec, "vcf_alias", value);
			return 0;
	}
	return 0;
}

int get_vlf_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vlf.vlf_sec, "vlf_alias", value);
	return 0;
}

int set_vlf_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;
	switch (action) {
		case VALUECHECK:			
			return 0;
		case VALUESET:
			dmuci_set_value("system", cur_dev_vlf.vlf_sec, "vlf_alias", value);
			return 0;
	}
	return 0;
}

int get_vlf_maxsize(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dev_vlf.vlf_sec, "log_size", value);
	return 0;
}

int get_vlf_persistent(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "0";
	return 0;
}

int lookup_vcf_name(char *instance, char **value) {
	struct uci_section *s = NULL;
	uci_path_foreach_option_eq(icwmpd, DMMAP, "vcf", "vcf_instance", instance, s) {
		dmuci_get_value_by_section_string(s, "name", value);
	}
	return 0;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int entry_method_root_DeviceInfo(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"DeviceInfo.") {
		DMOBJECT(DMROOT"DeviceInfo.", ctx, "0", 0, NULL, NULL, NULL);
		DMPARAM("Manufacturer", ctx, "0", get_device_manufacturer, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("ManufacturerOUI", ctx, "0", get_device_manufactureroui, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("ModelName", ctx, "0", get_device_routermodel, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("ProductClass", ctx, "0", get_device_productclass, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("SerialNumber", ctx, "0", get_device_serialnumber, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("HardwareVersion", ctx, "0", get_device_hardwareversion, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("SoftwareVersion", ctx, "0", get_device_softwareversion, NULL, NULL, 1, 0, 2, NULL);
		DMPARAM("UpTime", ctx, "0", get_device_info_uptime, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("DeviceLog", ctx, "0", get_device_devicelog, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("SpecVersion", ctx, "0", get_device_specversion, NULL, NULL, 1, 1, UNDEF, NULL);
		DMPARAM("ProvisioningCode", ctx, "1", get_device_provisioningcode, set_device_provisioningcode, NULL, 1, 0, 2, NULL);
		DMPARAM("X_INTENO_SE_BaseMacAddr", ctx, "0", get_base_mac_addr, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_MemoryBank", ctx, "1", get_device_memory_bank, set_device_memory_bank, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_CATVEnabled", ctx, "1", get_catv_enabled, set_device_catvenabled, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"DeviceInfo.X_INTENO_SE_CATV.", ctx, "0", 0, NULL, NULL, NULL);
		DMPARAM("Enabled", ctx, "1", get_catv_enabled, set_device_catvenabled, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("OpticalInputLevel", ctx, "0", get_catv_optical_input_level, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("RFOutputLevel", ctx, "0", get_catv_rf_output_level, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Temperature", ctx, "0", get_catv_temperature, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Voltage", ctx, "0", get_catv_voltage, NULL, NULL, 0, 1, UNDEF, NULL);
		DMOBJECT(DMROOT"DeviceInfo.VendorConfigFile.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"DeviceInfo.VendorLogFile.", ctx, "0", 0, NULL, NULL, NULL);
		SUBENTRY(entry_method_device_info_vcf, ctx);
		SUBENTRY(entry_method_device_info_vlf, ctx);
		return 0;
	}
	return FAULT_9005;
}

int check_file_dir(char *name)
{
	DIR *dir;
	struct dirent *d_file;
	if ((dir = opendir (DEFAULT_CONFIG_DIR)) != NULL) {
		while ((d_file = readdir (dir)) != NULL) {
			if(strcmp(name, d_file->d_name) == 0) {
				closedir(dir);
				return 1;
			}
		}
		closedir(dir);
	}
	return 0;
}

inline int entry_method_device_info_vcf(struct dmctx *ctx)
{
	char *vcf = NULL, *vcf_last = NULL, *name;
	struct uci_section *s = NULL, *del_sec = NULL;
	DIR *dir;
	struct dirent *d_file;

	if ((dir = opendir (DEFAULT_CONFIG_DIR)) != NULL) {
		while ((d_file = readdir (dir)) != NULL) {
			if(d_file->d_name[0] == '.')
				continue;
			update_section_list(DMMAP,"vcf", "name", 1, d_file->d_name, NULL, NULL, "backup_restore", "1");
		}
		closedir (dir);
	}
	uci_path_foreach_sections(icwmpd, DMMAP, "vcf", s) {
		dmuci_get_value_by_section_string(s, "name", &name);
		if(del_sec) {
			DMUCI_DELETE_BY_SECTION(icwmpd, del_sec, NULL, NULL);
			del_sec = NULL;
		}
		if (check_file_dir(name) == 0) {
			del_sec = s;
			continue;
		}
		init_args_vcf(ctx, s);
		vcf = handle_update_instance(1, ctx, &vcf_last, update_instance_alias_icwmpd, 3, s, "vcf_instance", "vcf_alias");
		SUBENTRY(entry_method_device_info_vcf_instance, ctx, vcf);
	}
	if(del_sec)
		DMUCI_DELETE_BY_SECTION(icwmpd, del_sec, NULL, NULL);
	return 0;
}

inline int entry_method_device_info_vcf_instance(struct dmctx *ctx, char *ivcf)
{
	IF_MATCH(ctx, DMROOT"DeviceInfo.VendorConfigFile.%s.", ivcf) {
		DMOBJECT(DMROOT"DeviceInfo.VendorConfigFile.%s.", ctx, "0", 1, NULL, NULL, NULL, ivcf);
		DMPARAM("Alias", ctx, "1", get_vcf_alias, set_vcf_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Name", ctx, "0",  get_vcf_name, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Version", ctx, "1",  get_vcf_version, set_vcf_version, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Date", ctx, "0",  get_vcf_date, NULL, "xsd:dateTime", 0, 1, UNDEF, NULL);
		DMPARAM("Description", ctx, "1",  get_vcf_desc, set_vcf_desc, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("UseForBackupRestore", ctx, "0",  get_vcf_backup_restore, NULL, "xsd:boolean", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

/*** DeviceInfo.VendorLogFile.{i}. ***/
inline int entry_method_device_info_vlf(struct dmctx *ctx)
{
	char *vlf = NULL, *vlf_last = NULL, *name;
	struct uci_section *s = NULL, *del_sec = NULL;
	DIR *dir;
	struct dirent *d_file;

	uci_foreach_sections("system", "system", s) {
		init_args_vlf(ctx, s);
		vlf = handle_update_instance(1, ctx, &vlf_last, update_instance_alias_icwmpd, 3, s, "vlf_instance", "vlf_alias");
		SUBENTRY(entry_method_device_info_vlf_instance, ctx, vlf);
	}
	return 0;
}


inline int entry_method_device_info_vlf_instance(struct dmctx *ctx, char *ivlf)
{
	IF_MATCH(ctx, DMROOT"DeviceInfo.VendorLogFile.%s.", ivlf) {
		DMOBJECT(DMROOT"DeviceInfo.VendorLogFile.%s.", ctx, "0", 1, NULL, NULL, NULL, ivlf);
		DMPARAM("Alias", ctx, "0", get_vlf_alias, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Name", ctx, "0",  get_vlf_name, NULL, "xsd:string", 0, 1, UNDEF, NULL);
		DMPARAM("MaximumSize", ctx, "0",  get_vlf_maxsize, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Persistent", ctx, "0",  get_vlf_persistent, NULL, "xsd:boolean", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}
