/*
 * Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
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

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>


static int pfds[2];

int xmpp_cmd(unsigned char dowait, int argc, ...)
{
	int pid;
	char *c = NULL;
	va_list arg;

	if (pipe(pfds) < 0)
		return -1;

	if ((pid = fork()) == -1)
		return -1;

	if (pid == 0) {
		/* child */
		int i;
		const char *argv[64];

		va_start(arg, argc);
		for (i = 0; i < argc; i++)
		{
			argv[i] = va_arg(arg, char *);
		}
		argv[i] = NULL;
		va_end(arg);

		close(pfds[0]);
		dup2(pfds[1], 1);
		close(pfds[1]);

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (pid < 0)
		return -1;

	/* parent */
	close(pfds[1]);

	if (dowait) {
		int status;
		while (wait(&status) != pid);
	}

	return 0;
}

void xmpp_cmd_close(void)
{
	close(pfds[0]);
}
