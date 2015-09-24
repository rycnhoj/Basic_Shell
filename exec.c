#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void executeCommand(char* cmd[]){
	int status;
	pid_t p = fork();
	bool bg = false;
	if(p == -1){
		fprintf(stderr, "A forking error ocurred.\n");
		exit(EXIT_FAILURE);
	}
	// CHILD
	else if (p == 0) {
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
