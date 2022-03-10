/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#include "cwmp_config.h"
#include "datamodel_interface.h"

int cwmp_get_deviceid()
{
	cwmp_get_leaf_value("Device.DeviceInfo.Manufacturer", &cwmp_main->deviceid.manufacturer);
	cwmp_get_leaf_value("Device.DeviceInfo.SerialNumber", &cwmp_main->deviceid.serialnumber);
	cwmp_get_leaf_value("Device.DeviceInfo.ProductClass", &cwmp_main->deviceid.productclass);
	cwmp_get_leaf_value("Device.DeviceInfo.ManufacturerOUI", &cwmp_main->deviceid.oui);
	cwmp_get_leaf_value("Device.DeviceInfo.SoftwareVersion", &cwmp_main->deviceid.softwareversion);
	return CWMP_OK;
}
