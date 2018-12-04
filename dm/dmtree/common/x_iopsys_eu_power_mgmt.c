/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
*		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 *		Author: Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 */

#include <unistd.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmcommon.h"
#include "x_iopsys_eu_power_mgmt.h"

/*** DMROOT.X_IOPSYS_EU_PowerManagement. ***/
DMLEAF tSe_PowerManagementParam[] = {
{"EthernetAutoPowerDownEnable", &DMWRITE, DMT_BOOL, get_pwr_mgmt_value_ethapd, set_power_mgmt_param_ethapd, NULL, NULL},
{"EnergyEfficientEthernetEnable", &DMWRITE, DMT_BOOL, get_pwr_mgmt_value_eee, set_power_mgmt_param_eee, NULL, NULL},
{"NumberOfEthernetInterfacesPoweredUp", &DMREAD, DMT_UNINT, get_pwr_nbr_interfaces_up, NULL, NULL, NULL},
{"NumberOfEthernetInterfacesPoweredDown", &DMREAD, DMT_UNINT, get_pwr_nbr_interfaces_down, NULL, NULL, NULL},
{0}
};

int get_pwr_mgmt_value_ethapd(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("power_mgmt", "power_mgmt", "ethapd", value);
	return 0;
}

int get_pwr_mgmt_value_eee(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_option_value_string("power_mgmt", "power_mgmt", "eee", value);
	return 0;
}

int get_pwr_nbr_interfaces_up(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char buf[256];
	int pp, r;

	*value = "";
	pp = dmcmd("pwrctl", 1, "show"); //TODO wait ubus command
	if (pp) {
		r = dmcmd_read(pp, buf, 256);
		close(pp);
		//TODO output command is changed
		return 0;
	}
	return 0;
}

int get_pwr_nbr_interfaces_down(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char buf[256];
	int pp, r;

	*value = "";
	pp = dmcmd("pwrctl", 1, "show"); //TODO wait ubus command
	if (pp) {
		r = dmcmd_read(pp, buf, 256);
		close(pp);
		//TODO output command is changed
		return 0;
	}
	return 0;
}

int set_power_mgmt_param_ethapd(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value("power_mgmt", "power_mgmt", "ethapd", "1");
			else
				dmuci_set_value("power_mgmt", "power_mgmt", "ethapd", "0");
			return 0;
	}
	return 0;
}

int set_power_mgmt_param_eee(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmuci_set_value("power_mgmt", "power_mgmt", "eee", "1");
			else
				dmuci_set_value("power_mgmt", "power_mgmt", "eee", "0");
			return 0;
	}
	return 0;
}

bool dm_powermgmt_enable_set(void)
{
	if( access("/etc/init.d/power_mgmt", F_OK ) != -1 ) {
		return true;
	}
  else {
		return false;
	}
}
