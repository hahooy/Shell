#include "shell.h"


int main(int argc, char *argv[])
{    
    historyptr = fopen("history", "w+");
    int pid;
    sig_init(); /* initialize signal functions */
    parsecml(argc, argv);
    fprintf(stdout, "The sish shell is now executing\n");

    for (;;) {
	fprintf(stdout, "sish >> ");
	parse_input_line();
	if (!ispiped()) {
	    if ((pid = fork()) == 0) {
		// code executed by child
		cmdRedirection(programs[0], dflag);
		execvp((programs[0] -> argv)[0], (programs[0] -> argv));
		exit(1);
	    } else {
		while(wait(0) != pid);
	    }
	}else {
	    cmdPiped(programs, getNumOfPipes(), dflag);
	}

    }
}
	
