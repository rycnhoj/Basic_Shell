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
	temp.et = 0;
	temp.lm = 0;

	token = strtok_r(rest, " ", &rest);
	if(strcmp(token, "etime") == 0)
	{
		printf("etime true");
		temp.et = 1;
		token = strtok_r(rest, " ", &rest);
	}
	else if(strcmp(token, "limits") == 0)
	{
		printf("limits true");
		temp.lm = 1;
		token = strtok_r(rest, " ", &rest);
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

FILE * getOutFile(char * fileName) {
	puts("asfa");
	FILE * outFile;

	// If file doesn't exist then it is created
	// If exists overwrites file in write mode
	outFile = fopen(fileName, "w");

	printf("%s\n", fileName);

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

	// This is the main Loop
	// getCmd returns string of whole command line
	while ((cmdline = getCmd()) != NULL) {
		char* token;
		char* rest = cmdline;
		char* tokenArray[MAX];
		cmdStruct cmdStructs[MAX];
		int cmdStructIndex = 0;
		cmdStruct* testStruct;

		if(strchr(cmdline, '|') == NULL){
			testStruct = transformStruct(cmdline);
			cmdStruct* firstStruct = &cmdStructs[0];
			if(newStruct == NULL)
				continue;
			//copyStruct(firstStruct, newStruct);
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



		if(cmdStructIndex == 0)
			executeCommand(testStruct);
		else
			executePipe(cmdStructIndex, cmdStructs);
		cleanCommands(cmdStructs, cmdStructIndex);
	}

	return 0;
}
