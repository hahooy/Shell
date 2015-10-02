#include "shell.h"

void sig_catch(int); /* signal handling for all signals */

/* to initialize the signal functions, the sig_init function 
needs to be called at the begining of the code execution */

/* init signal functions */
void sig_init(void)
{
    /* signals to be catch */
    if (signal(SIGINT, sig_catch) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGINT");
    }
    if (signal(SIGQUIT, sig_catch) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGQUIT");
    }
    if (signal(SIGCONT, sig_catch) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGCONT");
    }
    if (signal(SIGTSTP, sig_catch) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGTSTP");
    }
    /* signals to be ignored */
    if (signal(SIGABRT, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGABRT");
    }
    if (signal(SIGALRM, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGALRM");
    }
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGHUP");
    }
    if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGTERM");
    }
    if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGUSR1");
    }
    if (signal(SIGUSR2, SIG_IGN) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGUSR2");
    }
    if (signal(SIGCHLD, sig_catch) == SIG_ERR) {
    printerr(debugLevel, "Can't catch SIGCHLD");
    }
}

// signal handler
void sig_catch(int signo)
{

    switch (signo) {
    case SIGINT:
	printf(" catched SIGINT: %d\n", signo);
	kill(signo, fg_process_id);
	printf("signal has been passed to %d\n", fg_process_id);
	fflush(stdout);
	break;
    case SIGQUIT:
	printf(" catched SIGQUIT: %d\n", signo);
	fflush(stdout);
	break;
    case SIGCONT:
	printf(" catched SIGQUIT: %d\n", signo);
	fflush(stdout);
	break;
    case SIGTSTP:
	printf(" catched SIGTSTP: %d\n", signo);
	fflush(stdout);
	break;
    case SIGCHLD:
    printf(" catched SIGCHLD: %d\n", signo);
    fflush(stdout);
    break;
    default:
	printf(" other signals: %d\n", signo);
	fflush(stdout);
	break;
    }
    return;
}
