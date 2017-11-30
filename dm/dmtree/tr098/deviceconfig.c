/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *		Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *
 */

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "deviceconfig.h"

#define BACKUP_CONFIG_FILE "/etc/backup"
#define RESTORE_FUNCTION_PATH "/usr/share/icwmp/functions/conf_backup"
////////////////////////SET AND GET ALIAS/////////////////////////////////

/*** DMROOT.DownloadDiagnostics. ***/
DMLEAF tDeviceConfigParam[] = {
{"ConfigFile", &DMWRITE, DMT_STRING, get_config_file, set_config_file, NULL, NULL},
{0}
};

int get_config_file(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	long length;	
	FILE * f = NULL; 
	char *pch = NULL, *spch = NULL;
	char * buffer = 0;
	int first = 1;


	dmcmd("/bin/sh", 2, RESTORE_FUNCTION_PATH, "export_conf");

	f = fopen (BACKUP_CONFIG_FILE, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = dmmalloc(length);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}
	if (buffer)
	{
		*value = dmstrdup(buffer);
		dmfree(buffer);
	}
	return 0;
}

int set_config_file(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmcmd("/bin/sh", 2, RESTORE_FUNCTION_PATH, "import_conf");
			return 0;
	}
	return 0;
}


