main: shell.c exec.h path.h
	gcc shell.c -o shell.exe -g -std=gnu99

clean:
	rm *.exe
