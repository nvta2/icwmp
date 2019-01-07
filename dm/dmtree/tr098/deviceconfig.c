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

/*** DMROOT.ConfigFile. ***/
DMLEAF tDeviceConfigParam[] = {
{"ConfigFile", &DMWRITE, DMT_STRING, get_config_file, set_config_file, NULL, NULL},
{0}
};

int get_config_file(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	FILE *f = NULL;
	char *buffer = NULL;
	long length;

	*value = "";
	dmcmd("/bin/sh", 2, RESTORE_FUNCTION_PATH, "export_conf");
	f = fopen (BACKUP_CONFIG_FILE, "rb");
	if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		buffer = dmcalloc(1, length+1);
		if (buffer)
		{
			fseek(f, 0, SEEK_SET);
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}
	if (buffer)
		*value = buffer;
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


