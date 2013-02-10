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

                char line[32], res[32];
		char *res_p = &res[0];
                close(pipefd[1]);
                FILE *fp = fdopen(pipefd[0], "r");
                fgets(&line[0], 1024, fp); 
		res_p = strtok(&line[0], ";");

		waitpid(pid, &status, 0);

		applet->pending = atoi(res_p);

		int tmp_icon = applet->icon_status;

		if (applet->data.pending != 0)
			applet->icon_status = 1;
		else
			aplet->icon_status = 0;

	        if (tmp_icon != applet->icon_status)
                        applet->flip_icon = 1;

	}
}

