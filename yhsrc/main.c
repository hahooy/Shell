#include "shell.h"

int init_var(void);

int main(int argc, char *argv[])
{    
    int pid;
    init_var();
    sig_init(); /* initialize signal functions */
    parsecml(argc, argv);
    fprintf(stdout, "The sish shell is now executing\n");

    for (;;) {
	fprintf(stdout, "sish >> ");
	parse_input_line();
	if (isBuildIn(programs[0] -> argc, programs[0] -> argv)) {
	    continue;
	}
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
	
/* initialize global variable */
int init_var(void)
{
    numOfPipes = 0;
    dflag = 0;
    xflag = 0;
    fflag = 0;
    debugLevel = 0;
    historyptr = fopen("history", "w+");
    cmdIndex = 0;
    repeatCmd[0] = '\0';
    return 0;
}
