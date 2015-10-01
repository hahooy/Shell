#include "shell.h"

int init_var(void);
int isInteractive(Program *);
char *shellcommand; /* store argv[0], the command to invoke sish */

int main(int argc, char *argv[])
{    
    int pid;
    shellcommand = argv[0];
    init_var();
    sig_init(); /* initialize signal functions */
    parsecml(argc, argv);
    fprintf(stdout, "The sish shell is now executing\n");

    for (;;) {
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
		if (isInteractive(programs[0])) {
		    fg_process_id = pid;
		    while(wait(0) != pid);
		}
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
    shellpath = malloc(BUFFERSIZE * sizeof(char));

    /* define the environment variable: shell */
    getcwd(shellpath, BUFFERSIZE);
    if (shellcommand[0] == '.') {
	shellpath = strncat(shellpath, "/sish", 6);
    } else {
	shellpath = strncat(shellpath, "/", 2);
	shellpath = strncat(shellpath, shellcommand, BUFFERSIZE / 2);
    }
    setenv("shell", shellpath, 1);
    return 0;
}

int isInteractive(Program *pro)
{    
    return !(pro -> bg);
}
