#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void executeCommand(Commmand c){
	int status;
	pid_t p = fork();
	int bg = c.bg;
	if(p == -1){
		fprintf(stderr, "A forking error ocurred.\n");
		exit(EXIT_FAILURE);
	}
	// CHILD
	else if (p == 0) {
		// REDIRECTION
		if(c.redirect != -1){
			int rdFD = open(fileno(c.rdfile));
			if(c.redirect == 0)
				close(STDIN_FILENO);
			else if(c.redirect == 1)
				close(STDOUT_FILENO);
			dup(rdFD);
			close(rdFD);
		}
		execv(cmd[0], cmd);
		exit(1);
	}
	// PARENT
	else {
		if(!bg)
			waitpid(p, &status, 0);
	}
}

int main() {
	char* cmd[3] = {"/bin/ls", "-l", NULL};
	puts("I will execute the command.");
	executeCommand(cmd);
	puts("I have executed the command.");
	return 0;
}
