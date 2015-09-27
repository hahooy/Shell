#include "my_signo.h"

/* to initialize the signal functions, the sig_init function 
needs to be called at the begining of the code execution */

/* init signal functions */
void sig_init(void)
{
    /* signals to be catch */
    if (signal(SIGINT, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGINT");
    }
    if (signal(SIGQUIT, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGQUIT");
    }
    if (signal(SIGCONT, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGCONT");
    }
    if (signal(SIGTSTP, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGTSTP");
    }
    /* signals to be ignored */
    if (signal(SIGABRT, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGABRT");
    }
    if (signal(SIGALRM, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGALRM");
    }
    if (signal(SIGHUP, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGHUP");
    }
    if (signal(SIGTERM, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGTERM");
    }
    if (signal(SIGUSR1, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGUSR1");
    }
    if (signal(SIGUSR2, sig_catch) == SIG_ERR) {
	fprintf(stderr, "Can't catch SIGUSR2");
    }
}

// signal handler
void sig_catch(int signo)
{
    switch (signo) {
    case SIGINT:
	printf(" catched SIGINT: %d\n", signo);
	/* pass signal to child: kill(signo, fg_process_id); */
	printf("sish >> ");
	fflush(stdout);
	break;
    case SIGQUIT:
	printf(" catched SIGQUIT: %d\n", signo);
	printf("sish >> ");
	fflush(stdout);
	break;
    case SIGCONT:
	printf(" catched SIGQUIT: %d\n", signo);
	printf("sish >> ");
	fflush(stdout);
	break;
    case SIGTSTP:
	printf(" catched SIGTSTP: %d\n", signo);
	printf("sish >> ");
	fflush(stdout);
	break;
    default:
	printf(" other signals: %d\n", signo);
	printf("sish >> ");
	fflush(stdout);
	break;
    }
    return;
}
