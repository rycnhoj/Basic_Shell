// http://www.lemoda.net/c/recursive-directory/


// #include <unistd.h>
// #include <pwd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

int MAX_ARGS = 256;

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

int changeEnvs(char * args[], int arraySize) {
	int i = 0;

	while(i < arraySize + 1) {
		if(args[i][0] == '$') {

			char* env;
			env = (char*) malloc(strlen(args[i]) - 1); 

			strncpy(env, args[i]+1, strlen(args[i])); // Remove '$'

			if (getenv(env) != NULL) { // Replace the value if it exists
				args[i] = getenv(env);
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

int main() {

	// FILE *fp;
	// int c;

	// fp = fopen("file.txt","r");
	// while(1) {
	// 	c = fgetc(fp);
	// 	if(feof(fp)) { 
	// 		break;
	// 	}
		
	// 	printf("%c", c);
	// }

	// fclose(fp);
	
	char* cmdline; // The Whole Command Line

	while ((cmdline = getcmd()) != NULL) {

		char* token; 
		char* rest = cmdline;

		char* args[MAX_ARGS];
		int argIndex = -1;

		//Transform input
		/////////////////////////////////////////////////

		// Load all the command tokens into args array
		while(token = strtok_r(rest, " ", &rest)) {
			args[++argIndex] = token;
		}

		// Checks all the enviromental variables
		// if(changeEnvs(args, argIndex) == 0) {

			//Match against patterns
			/////////////////////////////////////////////////

			// CLEAR
			/////////////////////////////////////////////////
			if(!strcmp(args[0], "clear")) 
				system("clear");

			// EXIT
			/////////////////////////////////////////////////
			if(!strcmp(args[0], "exit")) {
				printf("Exiting Shell....\n");
				return 0;
			}

			// ECHO
			/////////////////////////////////////////////////
			// Outputs whatever the user specifies

			if(!strcmp(args[0], "echo")) {
				int k = 0;

				// For each argument passed to echo	print		
				while(k < argIndex) { printf("%s ", args[++k]); }

				printf("\n");
			}

			// // LS
			// /////////////////////////////////////////////////
			// else if(!strcmp(args[argIndex], "ls")) { 

			// }

			// // MORE
			// /////////////////////////////////////////////////
			// else if(!strcmp(args[argIndex], "more")) { 
		
			// }

			// CD
			/////////////////////////////////////////////////
			// else if(!strcmp(args[0], "cd")) { 
			// 	changeDir(args);
			// }

			/////////////////////////////////////////////////	
			
		}

		//Cleanup
		/////////////////////////////////////////////////	

		int j = 0;
		
		while(j < 256) {
			args[j] = 0;
			j++;
		}
	}

	return 0;
}


