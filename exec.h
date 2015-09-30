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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "define.h"

#define clear() printf("\033[H\033[J")

int executeCommand(cmdStruct); // Executes a single command
int executePipe(int, cmdStruct*); // Executes n piped commands
static int executeHelper(cmdStruct); // Main execution function
void copyStruct(cmdStruct*, cmdStruct*); // Copies to dest from src
void buildCmdFromStruct(char*[], int, cmdStruct, char*); // Builds a cstring array from struct
static int forkChild(int, int, cmdStruct); // Forks a new child and pipes
char* getCmdPath(char*); // Gets the path of a command without "/"
extern FILE* getOutFile(char*);
extern FILE* getInFile(char*);
extern cmdStruct* transformStruct(char*);
extern char* to_path(const char*);
void limits(pid_t);

int lim;
int processQueue[256];
int queueStart = 0;
int queueEnd = 0;

void queueAdd(int pid)
{
	processQueue[queueEnd] = pid;
	++queueEnd;
}

int queuePos(int pid)
{
	for(int i = 0; i < queueEnd; i++)
        {
		if (processQueue[i] != -1)
		{
			int status;
			int x = waitpid(processQueue[i], &status, WNOHANG);		
			if (x != 0)
			{
				processQueue[i] = -1;
			}
		}
        }

	int pos = 0;
	for(int i = 0; i < queueEnd; i++)
        {
                if (processQueue[i] == pid)
                {
                        return ++pos;
                }
		else if (processQueue[i] != -1)
		{
			++pos;
		}
        }
}

int executeCommand(cmdStruct c){
	int ret = 0;
	if(!strcmp(c.cmd, "clear"))
		clear();

	else if(!strcmp(c.cmd, "cd")){
		char* path = getenv("HOME");
		if(c.args[0] != NULL)
			if(c.args[1] != NULL){
				fprintf(stdout, "cd: Too many arguments.\n");
				return -1;
			}
			else
				path = to_path(c.args[0]);
		struct stat s;
		int error = stat(path, &s);
		if(error == -1)
			if(ENOENT == errno){
				fprintf(stdout, "%s: Does not exist or is not a directory.\n", path);
				return -1;
		}
		else if(!S_ISDIR(s.st_mode)){
			fprintf(stdout, "%s: Does not exist or is not a directory.\n", path);
			return -1;
		}

		if(chdir(path) != 0){
			fprintf(stdout, "cd: An error occurred.\n");
			return -1;
		}
		setenv("PWD", path, 1);
	}
	else if(!strcmp(c.cmd, "echo")){
		char* buffer = (char*) malloc(1);
		strcpy(buffer, "");
		int i = 0;
		int tot = 0;
		while(c.args[i] != NULL){
			tot = tot + strlen(c.args[i]);
			buffer = (char*) realloc(buffer, tot+2);
			strcat(buffer, c.args[i]);
			strcat(buffer, " ");
			i++;
		}
		fprintf(stdout, "%s\n", buffer);
		free(buffer);
	}
	else if(!strcmp(c.cmd, "etime")){
		struct timeval befTV, aftTV;
		time_t s;
		time_t ms;
		char* buffer = (char*) malloc(1);
		strcpy(buffer, "");
		int i = 0;
		int tot = 0;
		while(c.args[i] != NULL){
			tot = tot + strlen(c.args[i]);
			buffer = (char*) realloc(buffer, tot+2);
			strcat(buffer, c.args[i]);
			strcat(buffer, " ");
			i++;
		}
		cmdStruct* etimeCmd = transformStruct(buffer);
		free(buffer);
		gettimeofday(&befTV, NULL);
		ret = executeCommand(*etimeCmd);
		gettimeofday(&aftTV, NULL);
		s = aftTV.tv_sec - befTV.tv_sec;
		ms = aftTV.tv_usec - befTV.tv_usec;
		printf("Elapsed Time: %lld.%06llds\n", (long long)s, (long long)ms);
	}
	else if(!strcmp(c.cmd, "limits")){
		char* buffer = (char*) malloc(1);
		strcpy(buffer, "");
		int i = 0;
		int tot = 0;
		while(c.args[i] != NULL){
			tot = tot + strlen(c.args[i]);
			buffer = (char*) realloc(buffer, tot+2);
			strcat(buffer, c.args[i]);
			strcat(buffer, " ");
			i++;
		}
		lim = 1;
		cmdStruct* etimeCmd = transformStruct(buffer);
		free(buffer);
		ret = executeCommand(*etimeCmd);
		lim = 0;
	}
	else{
		int rdFD = -1;
		if(c.rd == 1)
			rdFD = open(c.rdFile, O_RDONLY);
		else if(c.rd == 2)
			rdFD = open(c.rdFile, O_WRONLY|O_CREAT, 0777);

		int status;
		pid_t p = fork();
		if (lim == 1){
			limits(p);
		}
		if(p == -1){
			fprintf(stdout, "A forking error ocurred.\n");
			exit(EXIT_FAILURE);
		}
		// CHILD
		else if (p == 0) {
			if(c.rd == 1) {
				close(STDIN_FILENO);
				dup(rdFD);
				close(rdFD);
			}
			else if (c.rd == 2){
				close(STDOUT_FILENO);
				dup(rdFD);
				close(rdFD);
			}

			ret = executeHelper(c);
			exit(1);
		}
		// PARENT
		else {
			if(rdFD != -1)
				close(rdFD);
			int bg = c.bg;
			if(bg != 1)
				waitpid(p, &status, 0);
			else
				queueAdd(p);
				printf("[%d]\t[%d]\n", queuePos(p) ,getpid());
		}
	}
	return ret;
}

int executePipe(int numCmds, cmdStruct* cStructs){
	int in = 0;
	int ret;
	int fd[2];
	pid_t pid;

	int i;
	for(i = 0; i < numCmds - 1; ++i){
		pipe(fd);
		ret = forkChild(in, fd[1], cStructs[i]);
		close(fd[1]);
		in = fd[0];
	}
	forkChild(in, 1, cStructs[i]);
	return 0;
}

int executeHelper(cmdStruct c){
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
		fprintf(stderr, "%s: Command not found.\n", c.cmd);
		return -1;
	}
	buildCmdFromStruct(cmd, count, c, cmdPath);
	execv(cmd[0], cmd);
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

int forkChild(int inFD, int outFD, cmdStruct c){
	int ret = 0;
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
		ret = executeHelper(c);
		exit(1);
	}
	else
		waitpid(pid, NULL, 0);
	return 0;
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

void limits(pid_t x) {
        char pid[15];
        snprintf(pid, 10, "%d", (int) x);
        char path[100] = "/proc/";
        strcat(path, pid);
        strcat(path, "/limits");
        char tempLine[256];

        FILE* file = fopen(path, "r");

        int counter = 0;

        while(fgets(tempLine, sizeof(tempLine), file))
        {
                if (counter == 2 || counter ==  7 || counter ==  8 || counter == 12)
                {
                        printf("%s", tempLine);
                }
                ++counter;

        }

        fclose(file);
}

#endif
