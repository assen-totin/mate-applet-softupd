#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "../config.h"
#include "applet.h"

void aptcheck_main () {
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

		glob_data.pending = atoi(res_p);

		int tmp_icon = glob_data.icon_status;

		if (glob_data.pending != 0)
			glob_data.icon_status = 1;
		else
			glob_data.icon_status = 0;

	        if (tmp_icon != glob_data.icon_status)
                        glob_data.flip_icon = 1;

	}
}

