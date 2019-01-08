/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
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

/*** DeviceInfo. ***/
DMOBJ tDeviceInfoObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{CUSTOM_PREFIX"CATV", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tCatTvParams, NULL},
{"VendorConfigFile", &DMREAD, NULL, NULL, NULL, browseVcfInst, NULL, NULL, NULL, tVcfParams, NULL},
{"VendorLogFile", &DMREAD, NULL, NULL, NULL, browseVlfInst, NULL, NULL, NULL, tVlfParams, NULL},
{"MemoryStatus", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tMemoryStatusParams, NULL},
{"ProcessStatus", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tProcessEntriesObj, tProcessStatusParams, NULL},
{0}
};

DMLEAF tMemoryStatusParams[] = {
{"Total", &DMREAD, DMT_UNINT, get_memory_status_total, NULL, NULL, NULL},
{"Free", &DMREAD, DMT_UNINT, get_memory_status_free, NULL, NULL, NULL},
{0}
};

DMOBJ tProcessEntriesObj[] = {
{"Process", &DMREAD, NULL, NULL, NULL, browsePocessEntriesInst, NULL, NULL, NULL, tProcessEntrieParams, NULL},
{0}
};

DMLEAF tProcessStatusParams[] = {
{"CPUUsage", &DMREAD, DMT_UNINT, get_process_cpu_usage, NULL, NULL, NULL},
{"ProcessNumberOfEntries", &DMREAD, DMT_UNINT, get_process_number_of_entries, NULL, NULL, NULL},
{0}
};

DMLEAF tProcessEntrieParams[] = {
{"PID", &DMREAD, DMT_UNINT, get_process_pid, NULL, NULL, NULL},
{"Command", &DMREAD, DMT_STRING, get_process_command, NULL, NULL, NULL},
{"Size", &DMREAD, DMT_UNINT, get_process_size, NULL, NULL, NULL},
{"Priority", &DMREAD, DMT_UNINT, get_process_priority, NULL, NULL, NULL},
{"CPUTime", &DMREAD, DMT_UNINT, get_process_cpu_time, NULL, NULL, NULL},
{"State", &DMREAD, DMT_STRING, get_process_state, NULL, NULL, NULL},
{0}
};

DMLEAF tDeviceInfoParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification, linker*/
{"Manufacturer", &DMREAD, DMT_STRING, get_device_manufacturer, NULL, &DMFINFRM, NULL},
{"ManufacturerOUI", &DMREAD, DMT_STRING, get_device_manufactureroui, NULL, &DMFINFRM, NULL},
{"ModelName", &DMREAD, DMT_STRING, get_device_routermodel, NULL, &DMFINFRM, NULL},
{"ProductClass", &DMREAD, DMT_STRING, get_device_productclass, NULL, &DMFINFRM, NULL},
{"SerialNumber", &DMREAD, DMT_STRING, get_device_serialnumber, NULL,  &DMFINFRM, NULL},
{"HardwareVersion", &DMREAD, DMT_STRING, get_device_hardwareversion, NULL, &DMFINFRM, NULL},
{"SoftwareVersion", &DMREAD, DMT_STRING, get_device_softwareversion, NULL, &DMFINFRM, &DMACTIVE},
{"UpTime", &DMREAD, DMT_UNINT, get_device_info_uptime, NULL, NULL, NULL},
{"DeviceLog", &DMREAD, DMT_STRING, get_device_devicelog, NULL, NULL, NULL},
{"SpecVersion", &DMREAD, DMT_STRING, get_device_specversion, NULL,  &DMFINFRM, NULL},
{"ProvisioningCode", &DMWRITE, DMT_STRING, get_device_provisioningcode, set_device_provisioningcode, &DMFINFRM, &DMACTIVE},
{CUSTOM_PREFIX"BaseMacAddr", &DMREAD, DMT_STRING, get_base_mac_addr, NULL, NULL, NULL},
{CUSTOM_PREFIX"CATVEnabled", &DMWRITE, DMT_STRING, get_catv_enabled, set_device_catvenabled, NULL, NULL},
{CUSTOM_PREFIX"MemoryBank", &DMWRITE, DMT_STRING, get_device_memory_bank, set_device_memory_bank, NULL, NULL},
{0}
};

/*** DeviceInfo.X_IOPSYS_EU_CATV. ***/
DMLEAF tCatTvParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enabled", &DMWRITE, DMT_STRING, get_catv_enabled, set_device_catvenabled, NULL, NULL},
{"OpticalInputLevel", &DMREAD, DMT_STRING, get_catv_optical_input_level, NULL, NULL, NULL},
{"RFOutputLevel", &DMREAD, DMT_STRING, get_catv_rf_output_level, NULL, NULL, NULL},
{"Temperature", &DMREAD, DMT_STRING, get_catv_temperature, NULL, NULL, NULL},
{"Voltage", &DMREAD, DMT_STRING, get_catv_voltage, NULL, NULL, NULL},
{0}
};

/*** DeviceInfo.VendorConfigFile.{i}. ***/
DMLEAF tVcfParams[] = {
/* PARAM, permission, type, getvalue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_vcf_alias, set_vcf_alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_vcf_name, NULL, NULL, NULL},
{"Version", &DMREAD, DMT_STRING, get_vcf_version, NULL, NULL, NULL},
{"Date", &DMREAD, DMT_TIME, get_vcf_date, NULL, NULL, NULL},
{"Description", &DMREAD, DMT_STRING, get_vcf_desc, NULL, NULL, NULL},
{"UseForBackupRestore", &DMREAD, DMT_BOOL, get_vcf_backup_restore, NULL, NULL, NULL},
{0}
};

/*** DeviceInfo.VendorLogFile.{i}. ***/
DMLEAF tVlfParams[] = {
/* PARAM, permission, type, getvalue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_vlf_alias, set_vlf_alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_vlf_name, NULL, NULL, NULL},
{"MaximumSize", &DMREAD, DMT_UNINT, get_vlf_max_size, NULL, NULL, NULL},
{"Persistent", &DMREAD, DMT_BOOL, get_vlf_persistent, NULL, NULL, NULL},
{0}
};

/*************************************************************
 * INIT
/*************************************************************/
inline int init_process_args(struct process_args *args, char *pid, char *command, char* size, char* priority, char *cputime, char *state)
{
	args->pid = pid;
	args->command= command;
	args->size= size;
	args->priority= priority;
	args->state= state;
	args->cputime= cputime;
	return 0;
}


/*
 *DeviceInfo. functions
 */
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
	dmuci_get_option_value_string("cwmp", "cpe", "override_productclass", &v);
	if (v[0] == '\0')
	{
		db_get_value_string("hw", "board", "iopVerBoard", &v);
		tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
		val = tmp;
		return val;
	}

	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	val = tmp;
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
	
	db_get_value_string("hw", "board", "iopVersion", &v);
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	val = tmp;
	return val;
}

int get_device_manufacturer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_deviceid_manufacturer();
	return 0;
}

int get_device_manufactureroui(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_deviceid_manufactureroui();
	return 0;
}

int get_device_productclass(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_deviceid_productclass();
	return 0;
}

int get_device_serialnumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_deviceid_serialnumber();
	return 0;
}

int get_device_softwareversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_softwareversion();
	return 0;
}

int get_device_hardwareversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	db_get_value_string("hw", "board", "hardwareVersion", value);
	return 0;
}

int get_device_routermodel(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	db_get_value_string("hw", "board", "routerModel", value);
	return 0;
}

int get_device_info_uptime(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_device_devicelog(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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
					*(p-1) = '\0';
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

int get_device_specversion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1.0";
	return 0;
}

int get_device_provisioningcode(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("cwmp", "cpe", "provisioning_code", value);
	return 0;
}

int set_device_provisioningcode(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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


int get_base_mac_addr(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{	
	json_object *res;
	
	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 2, "system", "basemac");
	return 0;
}

int get_device_memory_bank(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;

	dmubus_call("router.system", "memory_bank", UBUS_ARGS{{}}, 0, &res);
	DM_ASSERT(res, *value = "");
	*value = dm_ubus_get_value(res, 1, "code");
	return 0;
}

int set_device_memory_bank(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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
int get_catv_enabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int set_device_catvenabled(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
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

int get_catv_optical_input_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_catv_rf_output_level(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_catv_temperature(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_catv_voltage(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_vcf_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(vcf_sec, "name", value);
	return 0;
}

int get_vcf_version(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(vcf_sec, "version", value);
	return 0;
}

int get_vcf_date(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	DIR *dir;
	struct dirent *d_file;
	struct stat attr;
	char path[128];
	char date[sizeof "AAAA-MM-JJTHH:MM:SS.000Z"];
	struct uci_section *vcf_sec = (struct uci_section *)data;
	*value = "";
	dmuci_get_value_by_section_string(vcf_sec, "name", value);
	if ((dir = opendir (DEFAULT_CONFIG_DIR)) != NULL) {
		while ((d_file = readdir (dir)) != NULL) {
			if(strcmp(*value, d_file->d_name) == 0) {
				sprintf(path, DEFAULT_CONFIG_DIR"%s", d_file->d_name);
				stat(path, &attr);
				strftime(date, sizeof date, "%Y-%m-%dT%H:%M:%S.000Z", localtime(&attr.st_mtime));
				*value = dmstrdup(date);
			}
		}
	closedir (dir);
	}
	return 0;
}

int get_vcf_backup_restore(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(vcf_sec, "backup_restore", value);
	return 0;
}

int get_vcf_desc(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(vcf_sec, "description", value);
	return 0;
}

int get_vcf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(vcf_sec, "vcf_alias", value);
	return 0;
}

int set_vcf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *vcf_sec = (struct uci_section *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(vcf_sec, "vcf_alias", value);
			return 0;
	}
	return 0;
}

int lookup_vcf_name(char *instance, char **value)
{
	struct uci_section *s = NULL;
	uci_path_foreach_option_eq(icwmpd, DMMAP, "vcf", "vcf_instance", instance, s) {
		dmuci_get_value_by_section_string(s, "name", value);
	}
	return 0;
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

int get_vlf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *sys_log_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(sys_log_sec, "vlf_alias", value);
	return 0;
}

int set_vlf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	struct uci_section *sys_log_sec = (struct uci_section *)data;
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(sys_log_sec, "vlf_alias", value);
			return 0;
	}
	return 0;
}

int get_vlf_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *sys_log_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(sys_log_sec, "log_file", value);
	return 0;
}

int get_vlf_max_size (char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *sys_log_sec = (struct uci_section *)data;
	dmuci_get_value_by_section_string(sys_log_sec, "log_size", value);
	*value = (**value) ? *value : "0";
	return 0;
}

int get_vlf_persistent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "0";
	return 0;
}

int get_memory_status_total(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res;

	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	*value = dm_ubus_get_value(res, 2, "memoryKB", "total");
	return 0;
}

int get_memory_status_free(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res;

	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	*value = dm_ubus_get_value(res, 2, "memoryKB", "free");
	return 0;
}


int get_process_cpu_usage(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res;

	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	*value = dm_ubus_get_value(res, 2, "system", "cpu_per");
	return 0;
}

int get_process_number_of_entries(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	json_object *res,  *processes;
	int nbre_process = 0;

	dmubus_call("router.system", "processes", UBUS_ARGS{{}}, 0, &res);
	processes = json_object_object_get(res, "processes");
	nbre_process= json_object_array_length(processes);
	dmasprintf(value,"%d",nbre_process);
	return 0;
}

int get_process_pid(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	*value= proc_args->pid;
	return 0;
}

int get_process_command(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	*value= proc_args->command;
	return 0;
}

int get_process_size(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	if(proc_args->size!=NULL) *value= proc_args->size;
	else *value= "0";
	return 0;
}

int get_process_priority(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	long val;

	if(proc_args->priority!=NULL) {
		val = atol(proc_args->priority);
		if(val<0) val=0;
		dmasprintf(value, "%ld", val);
	}
	else *value= "0";
	return 0;
}

int get_process_cpu_time(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	if(proc_args->cputime!=NULL) *value= proc_args->cputime;
	return 0;
}

int get_process_state(char* refparam, struct dmctx *ctx, void *data, char *instance, char **value){
	struct process_args *proc_args= (struct process_args*) data;
	if(strchr(proc_args->state, 'S')!=NULL) *value="Sleeping";
	else if(strchr(proc_args->state, 'R')!=NULL) *value= "Running";
	else if(strchr(proc_args->state, 'T')!=NULL) *value= "Stopped";
	else if(strchr(proc_args->state, 'D')!=NULL) *value= "Uninterruptible";
	else if(strchr(proc_args->state, 'Z')!=NULL) *value= "Zombie";
	else *value= proc_args->state;

	return 0;
}

int browsePocessEntriesInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance){
	json_object *res,  *processes, *fields, *process;
	char *pid_field, *command_field, *state_field, *mem_size_field, *cpu_time_field, *priority_field, *pid, *command, *mem_size, *state, *cpu_time, *priority, *idx, *idx_last= NULL;
	int i, id=0;
	struct process_args proc_args={};

	dmubus_call("router.system", "processes", UBUS_ARGS{{}}, 0, &res);
	fields = json_object_object_get(res, "fields");
	processes = json_object_object_get(res, "processes");
	size_t nbre_process= json_object_array_length(processes);
	pid_field= dmjson_get_value_in_array_idx(fields, 0, 0, NULL);
	command_field= dmjson_get_value_in_array_idx(fields, 7, 0, NULL);
	state_field= dmjson_get_value_in_array_idx(fields, 3, 0, NULL);
	mem_size_field= dmjson_get_value_in_array_idx(fields, 4, 0, NULL);
	priority_field= dmjson_get_value_in_array_idx(fields, 8, 0, NULL);
	cpu_time_field= dmjson_get_value_in_array_idx(fields, 9, 0, NULL);

	if(nbre_process>0){
		for(i=0; i<nbre_process; i++){
			process= json_object_array_get_idx(processes, i);
			pid = dmjson_get_value(process, 1, pid_field);
			command = dmjson_get_value(process, 1, command_field);
			state = dmjson_get_value(process, 1, state_field);
			mem_size= dmjson_get_value(process, 1, mem_size_field);
			cpu_time= dmjson_get_value(process, 1, cpu_time_field);
			priority= dmjson_get_value(process, 1, priority_field);

			init_process_args(&proc_args, pid, command, mem_size, priority, cpu_time, state);

			idx = handle_update_instance(2, dmctx, &idx_last, update_instance_without_section, 1, ++id);
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&proc_args, idx) == DM_STOP)
				break;
		}
	}
}

int browseVcfInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
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
		vcf = handle_update_instance(1, dmctx, &vcf_last, update_instance_alias_icwmpd, 3, s, "vcf_instance", "vcf_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, vcf) == DM_STOP)
			break;
	}
	if(del_sec)
		DMUCI_DELETE_BY_SECTION(icwmpd, del_sec, NULL, NULL);
	return 0;
}

//Browse VendorLogFile instances
int browseVlfInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *sys_log_sec, *s, *dm_sec, *del_sec=NULL;
	char *instance, *last_instance, *log_file,*log_size, *add_value, *lfile;
	int i=1, n=0;
	uci_foreach_sections("system", "system", sys_log_sec) {
		if(!sys_log_sec)
			break;
		dmuci_get_value_by_section_string(sys_log_sec, "log_file", &log_file);
		dmuci_get_value_by_section_string(sys_log_sec, "log_size", &log_size);
		uci_path_foreach_sections(icwmpd, "dmmap", "vlf", dm_sec) {
			if(dm_sec)
				break;
		}
		if(!dm_sec){
			update_section_list(DMMAP,"vlf", NULL, i++, NULL, "log_file", log_file, "log_size", log_size);
		}
		else{
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dm_sec, "log_file", log_file);
			DMUCI_SET_VALUE_BY_SECTION(icwmpd, dm_sec, "log_size", log_size);
		}
	}
	uci_path_foreach_sections(icwmpd, "dmmap", "vlf", dm_sec) {
		instance = handle_update_instance(1, dmctx, &last_instance, update_instance_alias_icwmpd, 3, dm_sec, "vlf_instance", "vlf_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)dm_sec, instance) == DM_STOP){
			break;
		}
	}
	return 0;
}
