/* CSE422 Project 1: Toy Shell */
/* Name:            ID:        */
/* Junyuan Suo,                */
/* Yilong Hu,                  */
/* Yongzheng Huang, 445575     */

#include "shell.h"

int init_var(void);
void init_batchfile(void);
int isInteractive(Program *);

char *shellcommand; /* store argv[0], the command to invoke sish */


int main(int argc, char *argv[])
{    
    int pid;
    shellcommand = argv[0];
    init_var();    
    sig_init(); /* initialize signal functions */
    parsecml(argc, argv);
    init_batchfile();
    
    printerr(debugLevel, "The sish shell is now executing\n");

    for (;;) {
	parse_input_line();
	if (isInternal(programs[0] -> argc, programs[0] -> argv)) {
	    continue;
	}
	if (!ispiped()) {
	    if ((pid = fork()) == 0) {
		// code executed by child
		cmdRedirection(programs[0], dflag);
		setenv("parent", shellpath, 1);

		/* change the group pid of the background process so that it won't be killed */
		if (!isInteractive(programs[0])) {		    
		    setpgid(0, 0); 
		}

		if (!isBuildIn(programs[0] -> argc, programs[0] -> argv)) {
		    execvp((programs[0] -> argv)[0], (programs[0] -> argv));
		}
		exit(1);
	    } else {
		if (isInteractive(programs[0])) {
		    fg_process_id = pid;
		    int status;
		    waitpid(pid, &status, WUNTRACED);
		    set_foreground_return_value(status);
		} else {
		    /* record the pid of the last background process */
		    set_background_pid(pid);
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
    set_shell_pid();
    /* initialize the array of var structs */
    init_localVar();
 
    return 0;
}

int isInteractive(Program *pro)
{    
    return !(pro -> bg);
}

/* initialize the input file */
void init_batchfile(void)
{
    if (!fflag) {
	return;
    }
    if (batchfile == NULL) {
	printerr(debugLevel, "file name is empty\n");
	exit(1);
    }

    batchfileptr = fopen(batchfile, "r");
    for (int i = 0; filearg[i] != NULL && i < MAXFILEARG; i++) {	
	char name[20];
	sprintf(name, "%d", i + 1);
	char *tempargv[] = {"set", name, filearg[i]};
	set_sish(3, tempargv);
    }

    if (batchfileptr == NULL) {
	printerr(debugLevel, "could not open the batch file\n");
    }
}
