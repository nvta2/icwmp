/*
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef __XMPPCMD_H
#define __XMPPCMD_H

int xmpp_cmd(unsigned char wait, int argc, ...);
void xmpp_cmd_close(void);

#define XMPP_CMD(ARGC, args...) \
	do { \
		xmpp_cmd(1, ARGC, ##args); \
		xmpp_cmd_close(); \
	} while(0)


#endif //__XMPPCMD_H
