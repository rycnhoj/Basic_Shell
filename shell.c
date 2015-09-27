#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "exec.h"
#include "path.h"
#include "define.h"

#define MAX 256

static char* getCmd() {
	static char buf[1024];

	// Print Prompt
	/////////////////////////////////////////////////////

	char hostname[1024]; hostname[1023] = '\0';
	gethostname(hostname, 1023);

	printf("%s@", getenv("USER"));
	printf("%s:", hostname);
	printf("%s => ", getenv("PWD"));

	//Read input
	/////////////////////////////////////////////////////

	if (fgets(buf, sizeof(buf), stdin) == NULL)
		return NULL;

	if (buf[strlen(buf) - 1] == '\n')
		buf[strlen(buf) - 1] = 0;

	return buf;
}

int changeEnvs(char * tokenArray[], int arraySize) {
	int i;
	for (i = 0; i < arraySize; i++){
		char* token = tokenArray[i];
		if(token[0] == '$') {
			char* env = token + 1;
			if (getenv(env) != NULL) { // Replace the value if it exists
				tokenArray[i] = getenv(env);
			} else { // Signal an error if it does not exist
				printf("%s: Undefined variable.\n", env);
				return -1;
	  		}
	  	}
	}
	return 0;
}

cmdStruct* transformStruct(char* cmdToken){
	cmdStruct temp;
	cmdStruct* tempPtr = &temp;
	char* rest = cmdToken;
	char* token;
	int argIndex = 0;

	token = strtok_r(rest, " ", &rest);
	temp.cmd = (char*) malloc (strlen(token));
	strcpy(temp.cmd, token);

	while((token = strtok_r(rest, " ", &rest)) != NULL){
		if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0){
			if(strcmp(token, "<") == 0)
				temp.rd = 1;
			else if(strcmp(token, ">") == 0)
				temp.rd = 2;
			token = strtok_r(rest, " ", &rest);
			temp.rdFile = (char*) malloc (strlen(token));
			strcpy(temp.rdFile, token);
		}
		else {
			temp.args[argIndex] = (char*) malloc(strlen(token));
			strcpy(temp.args[argIndex++], token);
		}
		if (changeEnvs(temp.args, argIndex) == -1)
			return NULL;
	}
	return tempPtr;
}

void cleanCommands(cmdStruct* cStruct, int cIndex){
	int i;
	int j;
	for (i = 0; i < cIndex; i++){
		j = 0;
		free(cStruct[i].cmd);
		while(cStruct[i].args[j] != NULL){
			free(cStruct[i].args[j]);
			j++;
		}
		free(cStruct[i].rdFile);
		cStruct[i].rd = -1;
		cStruct[i].bg = -1;
	}
}

int main() {
	char* cmdline; // The Whole Command Line

	while ((cmdline = getCmd()) != NULL) {
		puts("Starting while loop");
		char* token;
		char* rest = cmdline;
		char* tokenArray[MAX];
		cmdStruct cmdStructs[MAX];
		int cmdStructIndex = 0;

		if(strchr(cmdline, '|') == NULL){
			cmdStruct* newStruct = transformStruct(cmdline);
			cmdStruct* firstStruct = &cmdStructs[0];
			if(newStruct == NULL)
				continue;
			copyStruct(firstStruct, newStruct);
		}
		else {
			cmdStruct* newStruct;
			while(token = strtok_r(rest, "|", &rest)){
				newStruct = transformStruct(token);
				if(newStruct == NULL)
					continue;
				copyStruct(cmdStructs + cmdStructIndex++, newStruct);
			}
			if(newStruct == NULL)
				continue;
			newStruct = transformStruct(rest);
			if(newStruct == NULL)
				continue;
			copyStruct(cmdStructs + cmdStructIndex, transformStruct(rest));
		}

		if(strcmp(cmdStructs[0].cmd, "exit") == 0){
			fprintf(stdout, "Exiting Shell....\n");
			exit(1);
		}

		if(cmdStructIndex == 0){
			executeCommand(cmdStructs[0]);
		}
		else{
			executePipe(cmdStructIndex, cmdStructs);
		}

		puts("Cleaning commands");
		cleanCommands(cmdStructs, cmdStructIndex);
		puts("Done cleaning commands");
		puts("Done while loop");
	}

	return 0;
}
