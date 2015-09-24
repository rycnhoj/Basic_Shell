#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define MAX 256

static char* getcmd(void) {

	static char buf[256];
	
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
				return 1;
	  		}
	  	}

		i++;
	}

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - 

//Execute command
//Print results

// - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct {
	char * cmd;
	char * args[MAX];
	char * rdFile;

	int T;
	int bg;

} cmdStruct;

int main() {

	char* cmdline; // The Whole Command Line

	while ((cmdline = getcmd()) != NULL) {

		char* token; 
		char* rest = cmdline;

		char* tokenArray[MAX];

		cmdStruct cmdStructs[MAX];
		int argIndex = -1;

		//Transform input
		/////////////////////////////////////////////////

		// Load all the command tokens into tokenArray array
		while(token = strtok_r(rest, " ", &rest)) {
			tokenArray[++argIndex] = token;
		}

		// while(not end of token_array)
		// create temp Struct
		// put first token into Struct.cmd
		// while(not "|" or NULL)
		// 	if(token == <)
		// 		Struct.redirect = 0
		// 	else if(token == >)
		// 		Struct.redirect = 1
		// 	else if(token == &)
		// 		Struct.bg = true
		// 	else
		// 		copy token into Struct.args
		// push temp Struct into array of structs

		int p = 0;
		int cmdStructIndex = 0;

		cmdStruct * temp;

		while(tokenArray[p] != NULL) {
			temp = cmdStructs[cmdStructIndex++];

			temp.cmd = tokenArray[p]

			while(tokenArray[p] != NULL || tokenArray[p] != '|'){
				if (strcmp(tokenArray[p], "<") == 0)
				{
					temp.rd = 0;
				} else if (strcmp(tokenArray[p], "<") == 0) {


				} else {



				}
			}

			p++;
		}


		// Checks all the enviromental variables
		// if(changeEnvs(tokenArray, argIndex) == 0) {

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
