#include "shell.h"

// updates 
// shell PID, 
// decimal value returned by the last 
// foreground command PID
void set_shell_pid(void);
void set_foreground_return_value(int cmd_exit_status);
void set_background_pid(int background_pid);

void environmentVariablesSetUp(int cmd_exit_status, int background_pid)
{
	// $$: PID of this shell
	// $?: Decimal value returned by the last foreground command
	// $!: PID of the last background command

	set_shell_pid();
	set_foreground_return_value(cmd_exit_status);
	set_background_pid(background_pid);
}

// set "$" the shell pid
void set_shell_pid(void) {
	char shell_pid[50];
	sprintf(shell_pid, "%d", getpid());
	setenv("$", shell_pid, 1);
}

// set "?" the value returned by the last foreground command
void set_foreground_return_value(int cmd_exit_status) {
	char last_exit_status[50];
	if (WIFEXITED(cmd_exit_status)) { /* process exited normally */
	    sprintf(last_exit_status, "%d", WEXITSTATUS(cmd_exit_status));
	}
	else if (WIFSIGNALED(cmd_exit_status)) { /* child exited on a signal */
	    sprintf(last_exit_status, "%d", WTERMSIG(cmd_exit_status));
	}
	else if (WIFSTOPPED(cmd_exit_status)) { /* child was stopped */
	    sprintf(last_exit_status, "%d", WSTOPSIG(cmd_exit_status));
	}

	setenv("?", last_exit_status, 1);
}

// set "!" the last background command pid
void set_background_pid(int background_pid) {
	char last_bg_pid[50];
	sprintf(last_bg_pid, "%d", background_pid);
	setenv("!", last_bg_pid, 1);	
}
