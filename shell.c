/**
SHELL.C

This file contains the main structure and workflow for the shell.

**/


#include <string.h>		// Cstring library
#include <stdio.h>		// Standard I/O
#include <stdlib.h>		// getenv(), Malloc, Realloc
#include <errno.h>		// ENOENT, error handling
#include "exec.h"		// Execution functions
#include "path.h"		// Path parsing
#include "define.h"		// Struct definition

// DEFINITIONS
#define MAX 256

// FUNCTION PROTOTYPES
static char* getCmd();
int changeEnvs(char*[], int);
cmdStruct* transformStruct(char*);
static int checkTokenForInvalid(char*);
static void initializeCommands(cmdStruct*, int);
static void cleanCommands(cmdStruct*);
FILE* getOutFile(char*);
FILE* getInFile(char*);

/**
GETCMD()

Main function for printing shell prompt and getting
user inputs.
**/
static char* getCmd() {
	static char cmdLine[1024];

	// Prints prompt
	char hostname[1024]; 		// hold hostname string
	hostname[1023] = '\0'; 		// adds Null character to end of string
	gethostname(hostname, 1023);//

	printf("%s@", getenv("USER")); 	// print user followed by '@'
	printf("%s:", hostname);		// print hostname followed by ':'
	printf("%s => ", getenv("PWD"));// print working directory with arrow

	// gets input from user and stores in cmdLine or returns NULL
	if (fgets(cmdLine, sizeof(cmdLine), stdin) == NULL)
		return NULL;

	// checks and adds Null character if needed
	if (cmdLine[strlen(cmdLine) - 1] == '\n')
		cmdLine[strlen(cmdLine) - 1] = 0;

	return cmdLine; // returns cmdLine string
}

/**
CHANGEENVS()
Iterates through the array and replaces environment variables
with their values.
**/
int changeEnvs(char * tokenArray[], int arraySize) {
	int i = 0;
	char* asdf;
	for (i = 0; i < arraySize; i++){
		char* token = tokenArray[i];
		if(token[0] == '$'){
			char* env = token + 1;
			if (getenv(env) != NULL) { // Replace the value if it exists
				char envVar[strlen(env)+1];
				strcpy(envVar, getenv(env));
				token = (char*) realloc (tokenArray[i], strlen(envVar)+1);
				strcpy(token, envVar);
			} else { // Signal an error if it does not exist
				printf("%s: Undefined variable.\n", env);
				return -1;
	  		}
	  	}
	}
	return 0;
}

/**
TRANSFORMSTRUCT
Transforms a c-string into a command struct following the
basic format:
[cmd] [args]
Also parses redirection and backgrounding.
**/
cmdStruct* transformStruct(char* cmdToken){
	cmdStruct temp;
	cmdStruct* tempPtr = &temp;
	char* rest = cmdToken;
	char* token;
	int argIndex = 0;

	int i;

	// Zero-initializes temp struct
	temp.cmd = 0;
	for(i = 0; i < MAX; i++)
		temp.args[i] = 0;
	temp.rdFile = 0;
	temp.rd = -1;
	temp.bg = -1;

	// Parses the initial token as the command
	token = strtok_r(rest, " ", &rest);
	if(checkTokenForInvalid(token) == -1)
		return NULL;
	temp.cmd = (char*) malloc (strlen(token));
	strcpy(temp.cmd, token);

	// Loops and parses the rest of the tokens
	while((token = strtok_r(rest, " ", &rest)) != NULL){
		// If redirection is encountered
		if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0){
			if(strcmp(token, "<") == 0)
				temp.rd = 1;	// Set 1 or
			else if(strcmp(token, ">") == 0)
				temp.rd = 2;	// Set 2
			// Grab the next token as the filename
			token = strtok_r(rest, " ", &rest);
			if(checkTokenForInvalid(token) == -1)
				return NULL;
			temp.rdFile = (char*) malloc (strlen(token));
			// Place it into the struct
			strcpy(temp.rdFile, token);
		}
		// If the token is &, turn on backgrounding
		else if(strcmp(token, "&") == 0){
			temp.bg = 1;
		}
		// Otherwise, push the tokens into the arguments array
		else {
			temp.args[argIndex] = (char*) malloc(strlen(token));
			strcpy(temp.args[argIndex++], token);
		}
		// Lastly, convert all the environment variables in their places
		if (changeEnvs(temp.args, argIndex) == -1){
			return NULL;
		}
	}
	return tempPtr;
}

/**
CHECKTOKENFORINVALID

Checks tokens for characters that signal invalidity.
**/
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
	else if(strchr(token, '|') != NULL){
		fprintf(stderr, "$s: Invalid token found.\n", token);
		return -1;
	}
	else if(token == NULL){
		fprintf(stderr, "ERROR: No token found.\n");
		return -1;
	}
	else
		return 0;
}

/**
INITIALIZE COMMANDS

Initializes command struct array to NULL or 0.
**/
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

/**
CLEAN COMMANDS

Frees allocated memory and NULLs pointers.
**/
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

/**
GETOUTFILE

Opens the file passed in if it exists,
Creates it and opens it if it does not.
**/
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

/**
GETINFILE

Opens the file for reading it if exists,
return NULL otherwise.
**/
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
	char* cmdline;
	int cmdStructIndex;

	// MAIN EXECUTION LOOP
	while ((cmdline = getCmd()) != NULL) {
		// Checks for single '|' or '|' at the beginning or end
		if(cmdline[0] == '|'){
			fprintf(stderr, "|: No commands piped.\n");
			continue;
		}
		else if((strchr(cmdline, '|')-cmdline+1) == strlen(cmdline)){
			fprintf(stderr, "|: No commands piped.\n");
			continue;
		}

		char* token;				// Holds cmdline broken up by '|'
		char* rest = cmdline;		// Holds the not yet parsed cmdline
		char* tokenArray[MAX];		// Holds the cstring of the commands
									//   broken up by '|'
		cmdStruct cmdStructs[MAX];	// Array of all the command structs
		cmdStructIndex = 0;			// Indexes the cmd struct array

		// Checks that the only & is at the end of the command
		char* backG = strchr(cmdline, '&');
		if(backG == NULL) { }
		else if((backG-cmdline+1) != strlen(cmdline)){
			fprintf(stderr, "&: Backgrounding can only occur at end of command.\n");
			continue;
		}

		initializeCommands(cmdStructs, MAX);

		// The case where there are no pipes
		if(strchr(cmdline, '|') == NULL){
			cmdStruct* newStruct = transformStruct(cmdline);
			if(newStruct == NULL){
				cleanCommands(cmdStructs);
				continue;
			}
			else if(strcmp(newStruct->cmd, "exit") == 0){
				fprintf(stdout, "Exiting Shell....\n");
				return 0;
			}
			if(executeCommand(*newStruct) == -1){
				cleanCommands(cmdStructs);
				continue;
			}
		}
		// The case where there exists pipes
		else {
			cmdStruct* newStruct;
			while((token = strtok_r(rest, "|", &rest)) != NULL){
				newStruct = transformStruct(token);
				if(newStruct == NULL){
					cleanCommands(cmdStructs);
					continue;
				}
				copyStruct(cmdStructs + cmdStructIndex++, newStruct);
			}
			if(strcmp(cmdStructs[0].cmd, "exit") == 0){
				fprintf(stdout, "Exiting Shell....\n");
				return 0;
			}
			if(executePipe(cmdStructIndex, cmdStructs) == -1){
				cleanCommands(cmdStructs);
				continue;
			}
		}
		cleanCommands(cmdStructs);
	}
	return 0;
}
