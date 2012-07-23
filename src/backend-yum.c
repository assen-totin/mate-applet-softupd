#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../config.h"
#include "applet.h"

void yum_main () {
	int pipefd[2];
	pipe(pipefd);

	int pid = fork();
	
	// CHILD
	if (pid == 0) {
		// If there are aupdates, yum will return with exit code 100
		// and print the updates on stdout, one per line. 
		// We need to count them.
                close(pipefd[0]);
                dup2(pipefd[1], 1);
                dup2(pipefd[1], 2);
                close(pipefd[1]);

		execlp(YUM_BINARY, YUM_BINARY, "check-update", (char *)NULL);
	}

	// PARENT
	else {
		int status;
                char line[1024];
		// Count the lines, skipping the first which yum always prints.
                int line_cnt = -1;

                close(pipefd[1]);
                FILE *fp = fdopen(pipefd[0], "r");
                while (fgets(&line[0], 1024, fp)) {
                	line_cnt++;
                }

		waitpid(pid, &status, 0);
		int exit_status = WEXITSTATUS(status);

		if (exit_status == 0) 
			glob_data.pending = 0;
		else if (exit_status == 100)
			glob_data.pending = line_cnt;
		else
			// Some error have occured
			glob_data.pending = -1;

		int tmp_icon = glob_data.icon_status;

		if (glob_data.pending != 0)
			glob_data.icon_status = 1;
		else
			glob_data.icon_status = 0;

	        if (tmp_icon != glob_data.icon_status)
        	        glob_data.flip_icon = 1;

	}
}

