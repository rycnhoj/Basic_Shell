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
#include "define.h"

void executeCommand(cmdStruct); // Executes a single command
void executePipe(int, cmdStruct*); // Executes n piped commands
static void executeHelper(cmdStruct); // Main execution function
void copyStruct(cmdStruct*, cmdStruct*); // Copies to dest from src
void buildCmdFromStruct(char*[], int, cmdStruct); // Builds a cstring array from struct
static void forkChild(int, int, cmdStruct); // Forks a new child and pipes

void executeCommand(cmdStruct c){
	if(!strcmp(c.cmd, "clear"))
		system("clear");
	else if(!strcmp(c.cmd, "cd")){
		//cd(c);
		puts("CD");
	}
	else if(!strcmp(c.cmd, "echo")){
		puts("echo");
		printf("%s", c.args[0]);
	}
	else if(!strcmp(c.cmd, "etime")){
		//etime(c);
		puts("etime");
	}
	else if(!strcmp(c.cmd, "limits")){
		//limits(c);
		puts("limits");
	}
	else{
		int status;
		pid_t p = fork();
		if(p == -1){
			fprintf(stderr, "A forking error ocurred.\n");
			exit(EXIT_FAILURE);
		}
		// CHILD
		else if (p == 0) {
			executeHelper(c);
			exit(1);
		}
		// PARENT
		else {
			int bg = c.bg;
			if(!bg)
				waitpid(p, &status, 0);
		}
	}
}

void executePipe(int numCmds, cmdStruct* cStructs){
	int in = 0;
	int fd[2];
	pid_t pid;

	int i;
	for(i = 0; i < numCmds - 1; ++i){
		pipe(fd);
		forkChild(in, fd[1], cStructs[i]);
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
	if((c.rd == 1)||(c.rd == 2)){
		int rdFD = open(c.rdFile);
		if(c.rd == 1)
			close(STDIN_FILENO);
		else if (c.rd == 2)
			close(STDOUT_FILENO);
		dup(rdFD);
		close(rdFD);
	}
	int i = 0;
	int count = 0;
	while(c.args[i++] != NULL)
		count++;
	count = count + 2;
	char* cmd[count];
	buildCmdFromStruct(cmd, count, c);
	execv(cmd[0], cmd);
}


void copyStruct(cmdStruct* dest, cmdStruct* src){
	free(dest->cmd);
	dest->cmd = (char*) malloc (sizeof(src->cmd));
	strcpy(dest->cmd, src->cmd);
	int i = 0;
	while(src->args[i] != NULL){
		printf("%s\n", src->args[i]);
		free(dest->args[i]);
		dest->args[i] = (char*) malloc (sizeof(src->args[i]));
		strcpy(dest->args[i], src->args[i]);
		i++;
	}
	if(strcmp(src->rdFile, "") != 0){
		free(dest->rdFile);
		dest->rdFile = (char*) malloc (sizeof(src->rdFile));
		strcpy(dest->rdFile, src->rdFile);
	}
	dest->rd = src->rd;
	dest->bg = src->bg;
}

void buildCmdFromStruct(char* cmd[], int size, cmdStruct c){
	// NEED TO PARSE IN ENV PATH
	cmd[0] = (char*) malloc (strlen(c.cmd)+5);
	strcpy(cmd[0], "/bin/");
	strcat(cmd[0], c.cmd);
	if(size > 2){
		int i;
		int j = 0;
		for(i = 1; i < size-1; i++){
			cmd[i] = (char*) malloc (strlen(c.args[j]));
			strcpy(cmd[i], c.args[j++]);
		}
		cmd[size] = NULL;
	}
	else
		cmd[1] = NULL;
}

void forkChild(int inFD, int outFD, cmdStruct c){
	pid_t pid = fork();
	if (pid == 0){
		if(inFD != 0){
			close(STDIN_FILENO);
			dup(inFD);
			close(inFD);
		}
		if(outFD != 1){
			close(STDOUT_FILENO);
			dup(outFD);
			close(outFD);
		}
		executeHelper(c);
		exit(1);
	}
}

#endif
