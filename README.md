#COP4610 - Project 1 - Shell
##Due Date: 09-28-2015 11:59pm

##### Team Members

* Evan Lee (el12b)
* John Cyr (jrc11v)
* Abraheem Omari (afo12)

##### Tar Archive / Files

* shell.c - This is our main C file that contains the main execution loop. It parses input and organizes it as needed. It determines when and in which order commands need to be piped to each other, input/output needs to be redirected, and generally prepares them for execution. It also keeps track of environment variables and performs other miscellaneous tasks.

* exec.h - Handles single commands, multiple commands that need to be piped, backgrounding and handles redirection. Generally deals with actually executing the commands in the order determined necessary by the main loop of the shell. Also handles built-ins among other things.

* define.h - The struct we used to organize the commands into dynamically manageable blocks.

* path.h - Handles path parsing and resolution. 

##### Server

* Linprog with -std=gnu99 flag set in gcc

##### Makefile Commands

* The make file includes the "make" command to produce shell.exe. It also includes the "clean" command to remove any executables.

##### Known Bugs / Incomplete

* Limits outputs the contents of the /proc file, but doesn't print the output of the actual command.
* Can't print a processes place in queue when it finishes. Only when it is started.
* $PWD is whack, yo
* grep doesn't work


##### Bonus

* Implemented the "clear" command to clear all text on the screen. 
* There is no limit to how much you can pipe.
* Handles piping and I/O redirection together
