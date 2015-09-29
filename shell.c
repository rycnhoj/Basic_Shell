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
	static char cmdLine[1024]; // 

	// Print Prompt
	/////////////////////////////////////////////////////
	
	char hostname[1024]; 		// hold hostname string
	hostname[1023] = '\0'; 		// adds Null character to end of string
	gethostname(hostname, 1023);// 

	printf("%s@", getenv("USER")); 	// print user followed by '@'
	printf("%s:", hostname);		// print hostname followed by ':'
	printf("%s => ", getenv("PWD"));// print working directory with arrow

	// Read input
	/////////////////////////////////////////////////////

	// gets input from user and stores in cmdLine or returns NULL
	if (fgets(cmdLine, sizeof(cmdLine), stdin) == NULL)
		return NULL;

	// checks and adds Null character if needed
	if (cmdLine[strlen(cmdLine) - 1] == '\n')
		cmdLine[strlen(cmdLine) - 1] = 0;

	return cmdLine; // returns cmdLine string
}

int changeEnvs(char * tokenArray[], int arraySize) {
	int i = 0;
	char* asdf;
	for (i = 0; i < arraySize; i++){
		if(i > arraySize)
			break;
		char* token = tokenArray[i];
		if(token[0] == '$'){
			char* env = token + 1;
			if (getenv(env) != NULL) { // Replace the value if it exists
				char envVar[strlen(env)+1];
				strcpy(envVar, getenv(env));
				token = (char*) realloc (tokenArray[i], strlen(envVar)+1);
				strcpy(token, envVar);
				free(envVar);
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
	if((strcmp(token, ">") == 0)
	|| (strcmp(token, "<") == 0)
	|| (strcmp(token, "|") == 0)) {
		fprintf(stdout, "%s: Command not found.\n", token);
		return NULL;
	}
	temp.cmd = (char*) malloc (strlen(token));
	strcpy(temp.cmd, token);

	while((token = strtok_r(rest, " ", &rest)) != NULL){
		if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0){
			if(strcmp(token, "<") == 0)
				temp.rd = 1;
			else if(strcmp(token, ">") == 0)
				temp.rd = 2;
			token = strtok_r(rest, " ", &rest);
			if(checkTokenForInvalid(token) == -1)
				return NULL;
			temp.rdFile = (char*) malloc (strlen(token));
			strcpy(temp.rdFile, token);
		}
		else if(strcmp(token, "&") == 0){
			temp.bg = 1;
		}
		else {
			temp.args[argIndex] = (char*) malloc(strlen(token));
			strcpy(temp.args[argIndex++], token);
		}
		if (changeEnvs(temp.args, argIndex) == -1){
			return NULL;
		}
	}
	return tempPtr;
}

int checkTokenForInvalid(char* token){
	if(strchr(token, '>') != NULL){
		fprintf(stderr, "%s: Invalid token found.\n", token);
		return -1;
	}
	else if(strchr(token, '<') != NULL){
		fprintf(stderr, "%s: Invalid token found.\n", token);
		return -1;
	}
	else if(strchr(token, '&') != NULL){
		fprintf(stderr, "%s: Invalid token found.\n", token);
		return -1;
	}
	else if(token == NULL){
		fprintf(stderr, "ERROR: No file name found for redirection.\n");
		return -1;
	}
	else
		return 0;
}

void initializeCommands(cmdStruct* cStruct, int size){
	int i;
	int j;
	for(i = 0; i < size; i++){
		cStruct[i].cmd = 0;
		for(j = 0; j < MAX; j++)
			cStruct[i].args[j] = NULL;
		cStruct[i].rdFile = NULL;
		cStruct[i].rd = -1;
		cStruct[i].bg = -1;
	}
}

void cleanCommands(cmdStruct* cStruct){
	int i = 0;
	int j;
	while(cStruct[i].cmd != NULL){
		if(cStruct[i].cmd != NULL)
			free(cStruct[i].cmd);
		for(j = 0; j < MAX; j++)
			if(cStruct[i].args[j] != NULL){
				free(cStruct[i].args[j]);
				cStruct[i].args[j] = NULL;
			}
		if(cStruct[i].rdFile != NULL)
			free(cStruct[i].rdFile);
		cStruct[i].rd = -1;
		cStruct[i].bg = -1;
		i++;
	}
}

FILE * getOutFile(char * fileName) {
	FILE * outFile;

	// If file doesn't exist then it is created
	// If exists overwrites file in write mode
	outFile = fopen(fileName, "w");

	if (outFile) {
		return outFile;
	} else {
		return NULL;
	}
}

FILE * getInFile(char * fileName) {

	FILE * inFile;

	// If file exists opens in read mode
	inFile = fopen(fileName, "r");

	if (inFile) {
		return inFile;
	} else {
		return NULL;
	}
}


int main() {
	char* cmdline; // The Whole Command Line
	int cmdStructIndex;

	// This is the main Loop
	// getCmd returns string of whole command line
	while ((cmdline = getCmd()) != NULL) {
		char* token;
		char* rest = cmdline;
		char* tokenArray[MAX];
		cmdStruct cmdStructs[MAX];
		cmdStructIndex = 0;

		char* backG = strchr(cmdline, '&');
		if(backG == NULL) { }
		else if((backG-cmdline+1) != strlen(cmdline)){
			fprintf(stderr, "&: Backgrounding can only occur at end of command.\n");
			continue;
		}

		initializeCommands(cmdStructs, MAX);

		if(strchr(cmdline, '|') == NULL){
			cmdStruct* newStruct = transformStruct(cmdline);
			if(newStruct == NULL)
				continue;
			else if(strcmp(newStruct->cmd, "exit") == 0){
				fprintf(stdout, "Exiting Shell....\n");
				return 0;
			}
			if(executeCommand(*newStruct) == -1)
				continue;
		}
		else {
			cmdStruct* newStruct;
			while((token = strtok_r(rest, "|", &rest)) != NULL){
				newStruct = transformStruct(token);
				if(newStruct == NULL)
					continue;
				copyStruct(cmdStructs + cmdStructIndex++, newStruct);
			}
			if(strcmp(cmdStructs[0].cmd, "exit") == 0){
				fprintf(stdout, "Exiting Shell....\n");
				return 0;
			}
			if(executePipe(cmdStructIndex, cmdStructs) == -1)
				continue;
		}
		cleanCommands(cmdStructs);
	}
	return 0;
}
