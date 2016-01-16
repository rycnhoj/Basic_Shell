/** ==============================
			DEFINE.H
	Holds the global definition for
	the command struct in order to
	span multiple source code files.
	================================ **/
	
#ifndef DEFINE_H
#define DEFINE_H

typedef struct {
	char* cmd;		// String of the command
	char* args[256];  	// Array of argument strings
	char* rdFile;  		// String holding the name of the file to redirect to

	int rd;
				// -1 for not redirected
				// 0 for '>'
				// 1 for '<'
	int bg;
				// -1 for not backgrounded
				// 1 for backgrounded
} cmdStruct;

#endif
