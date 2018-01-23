/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 Inteno Broadband Technology AB
 *		Author Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author Omar Kallel <omar.kallel@pivasoftware.com>
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

int get_config_file(char *refparam, struct dmctx *ctx, char **value)
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

int set_config_file(char *refparam, struct dmctx *ctx, int action, char *value)
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

//////////////////////////////////////

int entry_method_root_device_config(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"DeviceConfig.") {
		DMOBJECT(DMROOT"DeviceConfig.", ctx, "0", 1, NULL, NULL, NULL);
		DMPARAM("ConfigFile", ctx, "1", get_config_file, set_config_file, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}


