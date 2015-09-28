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
#include <sys/stat.h>
#include "define.h"

char* executeCommand(cmdStruct); // Executes a single command
char* executePipe(int, cmdStruct*); // Executes n piped commands
static int executeHelper(cmdStruct); // Main execution function
void copyStruct(cmdStruct*, cmdStruct*); // Copies to dest from src
void buildCmdFromStruct(char*[], int, cmdStruct, char*); // Builds a cstring array from struct
static char* forkChild(int, int, cmdStruct); // Forks a new child and pipes
char* getCmdPath(char*); // Gets the path of a command without "/"
extern FILE* getOutFile(char*);
extern FILE* getInFile(char*);

char* executeCommand(cmdStruct c){
	if(!strcmp(c.cmd, "clear"))
		system("clear");
	else if(!strcmp(c.cmd, "cd")){
		//cd(c);
		puts("CD");
	}
	else if(!strcmp(c.cmd, "echo")){
		puts("echo");
		// NEED TO CHANGE THIS
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
			int err = executeHelper(c);
			if(err == -1)
				return c.cmd;
			exit(1);
		}
		// PARENT
		else {
			int bg = c.bg;
			if(!bg)
				waitpid(p, &status, 0);
		}
	}
	return "";
}

char* executePipe(int numCmds, cmdStruct* cStructs){
	int in = 0;
	int err;
	int fd[2];
	char* cmdErr = NULL;
	pid_t pid;

	int i;
	for(i = 0; i < numCmds - 1; ++i){
		pipe(fd);
		cmdErr = forkChild(in, fd[1], cStructs[i]);
		close(fd[1]);
		in = fd[0];
		if(strcmp(cmdErr, "") != 0)
			return cmdErr;
	}
	if (in != 0){
		close(STDIN_FILENO);
		dup(in);
		close(in);
	}
	err = executeHelper(cStructs[i]);
	if(err == -1)
		return cStructs[i].cmd;
	return "";
}

int executeHelper(cmdStruct c){
	if((c.rd == 1)||(c.rd == 2)){
		puts("test");
		printf("%i\n", c.rd);
		int rdFD;
		if(c.rd == 1) {
			FILE* inFile = getInFile(c.rdFile);
			if(inFile == NULL)
				return -1;
			open(inFile);
			close(STDIN_FILENO);
		}
		else if (c.rd == 2){
			puts("ou");
			FILE* outFile = getOutFile(c.rdFile);
			if(outFile == NULL);
				return -1;
			open(outFile);
			close(STDOUT_FILENO);
		}
		dup(rdFD);
		close(rdFD);
	}
	int i = 0;
	int count = 0;
	while(c.args[i++] != NULL)
		count++;
	count = count + 2;
	char* cmd[count];
	char* cmdPath;
	if(strchr(c.cmd, '/') == NULL)
		cmdPath = getCmdPath(c.cmd);
	else
		cmdPath = c.cmd;
	if(cmdPath == NULL){
		return -1;
	}
	buildCmdFromStruct(cmd, count, c, cmdPath);
// etime
	execv(cmd[0], cmd);
// etime
	return 0;
}


void copyStruct(cmdStruct* dest, cmdStruct* src){
	free(dest->cmd);
	dest->cmd = (char*) malloc (sizeof(src->cmd));
	strcpy(dest->cmd, src->cmd);
	int i = 0;
	while(src->args[i] != NULL){
		free(dest->args[i]);
		dest->args[i] = (char*) malloc (sizeof(src->args[i]));
		strcpy(dest->args[i], src->args[i]);
		i++;
	}
	if(!strcmp(src->rdFile, "")){
		if(dest->rdFile != NULL)
			free(dest->rdFile);
		dest->rdFile = (char*) malloc (sizeof(src->rdFile));
		strcpy(dest->rdFile, src->rdFile);
	}
	dest->rd = src->rd;
	dest->bg = src->bg;
}

void buildCmdFromStruct(char* cmd[], int size, cmdStruct c, char* newCmd){
	cmd[0] = (char*) malloc(strlen(newCmd));
	strcpy(cmd[0], newCmd);
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

char* forkChild(int inFD, int outFD, cmdStruct c){
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
		if(executeHelper(c) == -1)
			return c.cmd;
		exit(1);
	}
	return "";
}

char* getCmdPath(char* cmd) {
	char* getAllPaths = getenv("PATH");
	char* path;
	char* tempPath;

	while(path = strtok_r(getAllPaths, ":", &getAllPaths)) {
		tempPath = (char*)malloc(strlen(path)+strlen(cmd)+2);
		strcat(tempPath, path);
		strcat(tempPath, "/");
		strcat(tempPath, cmd);

		struct stat FileAttrib;
		if(stat(tempPath, &FileAttrib) == 0)
			return tempPath;
	}
	return "";
}

#endif
