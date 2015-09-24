#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void copyStruct(cmdStruct dest, cmdStruct src){
	strcpy(dest.cmd, src.cmd);
	int i;
	int l = sizeof(src.args)/sizeof(char*);
	for(i = 0; i < l; i++) {
		strcpy(dest.args[i], src.args[i]);
	}
	if(strcmp(src.rdFile, "") != 0){
		strcpy(dest.rdFile, src.rdFile);
	}
	dest.rd = src.rd;
	dest.bg = src.bg;
}

void processCommand(cmdStruct c){
	if(strcmp(c.cmd, "cd") == 0){
		cd(c);
	}
	else if(strcmp(c.cmd, "echo") == 0){
		echo (c);
	}
	else if(strcmp(c.cmd, "etime") == 0){
		etime(c);
	}
	else if(strcmp(c.cmd, "limits") == 0){
		limits(c);
	}
	else
		executeCommand(c);
}

char* buildCmdFromStruct(cmdStruct c){
	char* cmd = (char*) malloc (sizeof(c.cmd)+sizeof(c.args)+1);
	strcat(cmd, c.cmd);
	strcat(cmd, c.args);
	return cmd;
}

void executeCommand(cmdStruct c){
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
		char* cmdBuff = buildCmdFromStruct(c);
		execv(c.cmd, cmdBuff);
		exit(1);
	}
	// PARENT
	else {
		if(!bg)
			waitpid(p, &status, 0);
	}
}

