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
	static char buf[MAX];

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
	puts("hi");
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
				temp.rd = 0;
			else if(strcmp(token, ">") == 0)
				temp.rd = 1;
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

int main() {
	char* cmdline; // The Whole Command Line

	while ((cmdline = getCmd()) != NULL) {
		char* token;
		char* rest = cmdline;
		char* tokenArray[MAX];
		cmdStruct cmdStructs[MAX];
		int cmdStructIndex = 0;

		//Transform input
		/////////////////////////////////////////////////

		if(strchr(cmdline, '|') == NULL){
			cmdStruct* newStruct = transformStruct(cmdline);
			if(newStruct == NULL)
				continue;
			copyStruct(cmdStructs, newStruct);
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

			//Match against patterns
			/////////////////////////////////////////////////

			// CLEAR
			/////////////////////////////////////////////////
			// if(!strcmp(tokenArray[0], "clear"))
			// 	system("clear");

			// EXIT
			/////////////////////////////////////////////////
			// if(!strcmp(tokenArray[0], "exit")) {
			// 	printf("Exiting Shell....\n");
			// 	return 0;
			// }

			// ECHO
			/////////////////////////////////////////////////
			// Outputs whatever the user specifies

			// if(!strcmp(tokenArray[0], "echo")) {
			// 	int k = 0;

			// 	// For each argument passed to echo	print
			// 	while(k < argIndex) { printf("%s ", tokenArray[++k]); }

			// 	printf("\n");
			// }

			// // LS
			// /////////////////////////////////////////////////
			// else if(!strcmp(tokenArray[argIndex], "ls")) {

			// }

			// // MORE
			// /////////////////////////////////////////////////
			// else if(!strcmp(tokenArray[argIndex], "more")) {
			// }

			// CD
			/////////////////////////////////////////////////
			// else if(!strcmp(tokenArray[0], "cd")) {
			// 	changeDir(tokenArray);
			// }

			/////////////////////////////////////////////////
		// }

		//Cleanup
		/////////////////////////////////////////////////

		// int j = 0;
		// while(j < 256) {
		// 	tokenArray[j] = 0;
		// 	j++;
		// }
	}

	return 0;
}
