#include <string.h>	// c-string functions
#include <stdio.h>	// standard print/input
#include <stdlib.h>	// for malloc/realloc

char* to_path(const char*);  // Main function
char* main_parse(const char*, char*, char*[], const int); // Handles generic parsing
char* get_parent(const char*);  // Gets parent of passed in absolute path
void raise_error(const char*, const char*);  // Raises beyond-root error

/** MAIN FUNCTION **/

/**
 * TO_PATH
 * Main function for path parsing.
 * Converts a passed-in relative path to
 * a parsed-out absolute path.
 */
char* to_path(const char* path){
	char* totPath;
	char* pArr[25];
	char ppath[strlen(path)];
	char* p;
	int pCount = 0;

	/* Main branch, parses off first character */
	// Root token
	if (path[0] == '/'){
		if (strlen(path) == 1)
			return "/";
		// sets the current total parsed path to root
		else{
			totPath = (char*) malloc (2);
			strcpy(totPath, "/");
			path++;
		}
	}
	// Home token
	else if (path[0] == '~') {
		if (strlen(path) == 1)
			return getenv("HOME");
		// sets the current total parsed path to HOME
		else{
			totPath = (char*) malloc (strlen(getenv("HOME"))+1);
			strcpy(totPath, getenv("HOME"));
			path++;
		}
	}
	// All other tokens
	else {
		totPath = (char*) malloc (strlen(getenv("PWD")));
		strcpy(totPath, getenv("PWD"));
	}

	// Loop for parsing the original path to an array, delimited by '/'
	strcpy(ppath, path);
	p = strtok(ppath, "/");
	pArr[pCount++] = p;
	while (p != NULL){
		puts(p);
		p = strtok(NULL, "/");
		pArr[pCount++] = p;
	}
	pCount -= 1;

	return main_parse(path, totPath, pArr, pCount);
}

/** END MAIN FUNCTION **/

/** PARSING FUNCTION **/
/*
 * MAIN_PARSE
 * Main parsing loop that iterates through the original path
 * and makes decisions based on each token.
 *
 * PARAMETERS:
 * iniPath - the original path from user input
 * totPath - holds the aggregate total parsed path
 * pArr - array of c-strings that holds each element of the original path
 * pCount - the number of elements in pArr
 */
char* main_parse(const char* iniPath, char* totPath, char* pArr[25], const int pCount){
	int i;

	/* Main parsing loop; branches depending on token */
	for (i = 0; i < pCount; i++) {
		// ".." token
		if (strcmp(pArr[i], "..") == 0){
			// if the total path thus far is just root, raises error
			if (strcmp(totPath, "/") == 0) {
				raise_error(iniPath, "No folder above root");
			}
			// or if the total path is "/<dir>/..", just returns "/"
			else if (strcmp(strrchr(totPath, '/'), totPath) == 0) {
					free(totPath);
					totPath = (char*) realloc (totPath, 2);
					strcpy(totPath, "/");
			}
			// otherwise just gets parents
			else
				totPath = get_parent(totPath);
		}
		// for current directory, do nothing to the aggregate path
		else if (strcmp(pArr[i], ".") == 0){ }
		// otherwise add the directory to the path
		else {
			if (totPath[strlen(totPath)-1] != '/')
				strcat(totPath, "/");
			strcat(totPath, pArr[i]);
		}
	}
    return totPath;
}
/** END PARSING FUNCTION **/

/** HELPER FUNCTIONS **/

/*
 * GET_PARENT
 * Removes the last directory of the passed-in path.
 */
char* get_parent(const char* parent){
	// Get the index of the last '/'
	char* child = strrchr(parent, '/');
	// Find the index of the last '/' in the passed in path
	int index = strlen(parent) - strlen(child);
	// Allocate temp holder
	char* tempPath = (char*) malloc(index);
	// Copy only up until the last '/'
	strncpy(tempPath, parent, index);
	return tempPath;
}

/*
 * RAISE_ERROR
 * Raises error in the case that the path
 * goes beyond the root.
 *
 * PARAMETERS:
 * cmd - The path that was input originally.
 * err - The error message displayed to stderr.
 */
void raise_error(const char* cmd, const char* err){
	fprintf(stderr, "ERROR[\"%s\"]: %s\n", cmd, err);
	// Exits with failure state
    exit(EXIT_FAILURE);
}

/** END HELPER FUNCTIONS **/

int main(){
	char relPath[256];
	printf("%s", "Enter your path: ");
	scanf("%s", relPath);

	printf("\n== Initial:\t%s\n\n", relPath);
	printf("== Parsed:\t%s\n\n", to_path(relPath));
	return 0;
}
