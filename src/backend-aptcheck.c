/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *  USA.
 *
 *  MATE Software Update applet written by Assen Totin <assen.totin@gmail.com>
 *  
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "../config.h"
#include "applet.h"

void aptcheck_main (softupd_applet *applet) {
	int pipefd[2];
	pipe(pipefd);

	int pid = fork();

	// CHILD
	if (pid == 0) {
                close(pipefd[0]);
                dup2(pipefd[1], 1);
                dup2(pipefd[1], 2);
                close(pipefd[1]);

		execlp(APTCHECK_BINARY, APTCHECK_BINARY, (char *)NULL);
	}

	// PARENT
	else {
		int status;

                char line[1024], res[32];
		char *res_p = &res[0];
                close(pipefd[1]);
                FILE *fp = fdopen(pipefd[0], "r");
                fgets(&line[0], 1024, fp); 
		res_p = strtok(&line[0], ";");

		waitpid(pid, &status, 0);

		applet->pending = atoi(res_p);

		int tmp_icon = applet->icon_status;

		if (applet->pending != 0)
			applet->icon_status = 1;
		else
			applet->icon_status = 0;

	        if (tmp_icon != applet->icon_status)
                        applet->flip_icon = 1;

	}
}

