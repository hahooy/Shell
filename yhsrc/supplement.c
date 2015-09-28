// updates 
// shell PID, 
// decimal value returned by the last 
// foreground command PID
void environmentVariablesSetUp(int cmd_exit_status,
							   int background_pid)
{
	// $$: PID of this shell
	// $?: Decimal value returned by the last foreground command
	// $!: PID of the last background command

	char shell_pid[50], last_exit_status[50], last_bg_pid[50];
	sprintf(shell_pid, "%d", getpid());
	setenv("$", shell_pid, 1);
	
	sprintf(last_exit_status, "%d", cmd_exit_status);
	setenv("?", last_exit_status, 1);
	
	sprintf(last_bg_pid, "%d", background_pid);
	setenv("!", last_bg_pid, 1);	
}