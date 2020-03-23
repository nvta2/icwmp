/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*
*/

#ifndef __REPORT_H_
#define __REPORT_H_

#include <json-c/json.h>
#include "common.h"
#include "times.h"
#include "config.h"

void create_encoding_bulkdata_report(struct profile *profile, char **report);
void create_failed_report(struct profile *profile, char **report);

#endif /* __REPORT_H_ */
