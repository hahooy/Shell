#include "shell.h"

int main(int argc, char *argv[])
{    
    parsecml(argc, argv);
    int pid;
    fprintf(stdout, "The sish shell is now executing\n");

    for (;;) {
	fprintf(stdout, "sish >> ");
	parse_input_line();
	if ((pid = fork()) == 0) {
	    // code executed by child
	    cmdRedirection(programs[0], dflag);
	    execvp((programs[0] -> argv)[0], programs[0] -> argv);
	    exit(1);
	} else {
	    cmdPiped(programs, getNumOfPipes(), dflag);
	}
	while (wait(0) != pid);
    }
}
	
