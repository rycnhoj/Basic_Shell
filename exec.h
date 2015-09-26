/********************************
		  == EXEC.H ==
  Author:	Evan Lee
  Course:	COP4610
  Project:	1
  Description:
	This file contains all the
	necessary functions for
	command execution and
	process forking.
 ********************************/
#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void executeCommand(cmdStruct); // Executes a single command
void executePipe(int, cmdStruct*); // Executes n piped commands
static void executeHelper(cmdStruct); // Main execution function
void copyStruct(cmdStruct, cmdStruct); // Copies to dest from src
char** buildCmdFromStruct(cmdStruct); // Builds a cstring array from struct
static void forkChild(int, int, cmdStruct); // Forks a new child and pipes

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
		executeHelper(c);
	}
	// PARENT
	else {
		if(!bg)
			waitpid(p, &status, 0);
	}
}

void executePipe(int numCmds, cmdStruct* cStrcts){
	int in = 0;
	int fd[2];
	pid_t pid;

	int i;
	for(i = 0; i < numCmds - 1; ++i){
		pipe(fd);
		forkChild(in, fd[1], cStrcts[i]);
		close(fd[1]);
		in = fd[0];
	}
	if (in != 0){
		close(STDIN_FILENO);
		dup(in);
		close(in);
	}
	executeHelper(cStructs[i]);
}

void executeHelper(cmdStruct c){
	if(c.redirect != -1){
		int rdFD = open(fileno(c.rdFile));
		if(c.redirect == 0)
			close(STDIN_FILENO);
		else if (c.redirect == 1)
			close(STDOUT_FILENO);
		dup(rdFD);
		close(rdFD);
	}
	char** cmdBuff = buildCmdFromStruct(c);
	execv(c.cmd, cmdBuff);
}

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

char** buildCmdFromStruct(cmdStruct c){
	char* cmd[sizeof(c.args)+2];
	strcpy(cmd[i], c.cmd);

	int i;
	for (i = 1; i < sizeof(c.args) + 1; i++) {
		strcpy(cmd[i], c.args[i]);
	}
	cmd[i] = NULL;

	return cmd;
}

void forkChild(int inFD, int outFD, cmdStruct c){
	pid_t pid = fork();
	if (pid == 0){
		if(in != 0){
			close(STDIN_FILENO);
			dup(inFD);
			close(inFD);
		}
		if(out != 1){
			close(STDOUT_FILENO);
			dup(outFD);
			close(outFD);
		}
		executeHelper(c);
	}
}

#endif
