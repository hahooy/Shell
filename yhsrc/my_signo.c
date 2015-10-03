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
    /* signal from dead child */
    if (signal(SIGCHLD, sig_catch) == SIG_ERR) {
	printerr(debugLevel, "Can't catch SIGCHLD");
    }
}

// signal handler
void sig_catch(int signo)
{

    switch (signo) {
    case SIGINT:
	printerr(debugLevel, " catched SIGINT\n");
	fflush(stdout);
	break;
    case SIGQUIT:
	printerr(debugLevel, " catched SIGQUIT\n");
	fflush(stdout);
	break;
    case SIGCONT:
	printerr(debugLevel, " catched SIGQUIT\n");
	fflush(stdout);
	break;
    case SIGTSTP:
	printerr(debugLevel, " catched SIGTSTP\n");
	fflush(stdout);
	break;
    case SIGCHLD:
	printerr(debugLevel, " catched SIGCHLD\n");
	waitpid(0, 0, WNOHANG);
	fflush(stdout);
	break;
    default:
	printerr(debugLevel, " other signals\n");
	fflush(stdout);
	break;
    }
    return;
}
