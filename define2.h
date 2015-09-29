#ifndef DEFINE_H
#define DEFINE_H

typedef struct {
	char* cmd;	// String of the command
	char* args[256];  // Array of argument strings
	char* rdFile;  // String holding the name of the file to redirect to

	int rd;
		// 0 for '>'
		// 1 for '<'
		// Other for other
	int bg;
		// 0 for not backgrounded
		// 1 for backgrounded
	int et;
		// 0 for not etime
		// 1 for etime
	int lm;
		// 0 for not limits
		// 1 for limits
} cmdStruct;

#endif
