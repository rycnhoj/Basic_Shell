#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "exec.h"
#include "path.h"

#define MAX 256

typedef struct {
	char * cmd;
	char * args[MAX];
	char * rdFile;

	int rd;
	int bg;
} cmdStruct;

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
	int i = 0;

	while(i < arraySize + 1) {
		if(tokenArray[i][0] == '$') {

			char* env;
			env = (char*) malloc(strlen(tokenArray[i]) - 1);

			strncpy(env, tokenArray[i]+1, strlen(tokenArray[i])); // Remove '$'

			if (getenv(env) != NULL) { // Replace the value if it exists
				tokenArray[i] = getenv(env);
			} else { // Signal an error if it does not exist
				printf("%s: Undefined Variable.\n", env);
				return -1;
	  		}
	  	}
		i++;
	}
	return 0;
}

cmdStruct* transformStruct(char* cmdToken){
	cmdStruct temp;
	char* rest = cmdToken;
	char* token;
	int argIndex = 0;

	token = strtok_r(rest, ' ', &rest);
	temp.cmd = (char*) malloc (strlen(token));
	strcpy(temp.cmd, token);

	while(token = strtok_r(rest, ' ', &rest)){
		if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0){
			if(strcmp(token, "<") == 0)
				temp.rd = 0;
			else if(strcmp(token, ">") == 0)
				temp.rd = 1;
			token = strtok_r(rest, ' ', &rest);
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
	return temp;
}

int main() {

	char* cmdline; // The Whole Command Line

	while ((cmdline = getcmd()) != NULL) {
		char* token;
		char* rest = cmdline;
		char* tokenArray[MAX];
		cmdStruct cmdStructs[MAX];
		int cmdStructIndex = 0;

		//Transform input
		/////////////////////////////////////////////////

		if(strchr(cmdline, '|') == NULL){
			cmdStruct* newStruct = transformStruct(cmdline);
			if(newStruct == NULL){
				continue;
			}
			copyStruct(cmdStructs[0], newStruct);
		}
		else {
			while(token = strtok_r(rest, '|', &rest)){
				cmdStruct* newStruct = transformStruct(cmdline);
				if(newStruct == NULL)
					continue;
				copyStruct(cmdStructs[cmdStructIndex++], newStruct);
			}
			if(newStruct == NULL)
				continue;
			cmdStruct* newStruct = transformStruct(rest);
			if(newStruct == NULL){
				continue;
			copyStruct(cmdStructs[cmdStructIndex], transformStruct(rest));
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
