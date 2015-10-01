#include "shell.h"

/* all build-in commands are implemented in this file */

void exit_sish(int argc, char *argv[])
{
    fclose(historyptr);
    exit(0);
}

void history_sish(int argc, char *argv[])
{
	FILE *fp = fopen("history", "r");
	char* line = NULL;
	size_t len = 0;
	int lineNum = 0;

	if (fp == NULL) {
	    printerr(debugLevel, "Failed to open file\n");	    
	} 

	if (argc == 1) {
	    //n is not specified
	    for (int lineno = 1; getline(&line, &len, fp) != -1; lineno++) {
		printf("%s", line);
	    }
	} else {
	    //n is specified
	    lineNum = atoi(argv[1]);
	    for (int lineno = 1; getline(&line, &len, fp) != -1 ; lineno++) {
		if(lineno >= cmdIndex - lineNum + 1) {
		    printf("%s", line);
		}
	    }	    
	}
	assert(line != NULL);
	free(line);
	fclose(fp);    
}

void repeat_sish(int argc, char *argv[])
{
    char *line = NULL;
    int curLineNum = 0;
    int targetLineNum = cmdIndex - 1;
    size_t linecap = 0;
    ssize_t linelen;
    FILE *fp = fopen("history", "r");    

    if (fp == NULL) {
	printerr(debugLevel, "could not open history file!\n");
	return;
    }

    if (argc > 1) {
	targetLineNum = atoi(argv[1]);
    }
    
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
	if (linelen >= BUFFERSIZE) {	    
	    printerr(debugLevel, "command is too long\n");
	    break;
	}
	++curLineNum;
	if (curLineNum == targetLineNum) {
	    if (strcmp(line + 2, "repeat\n") == 0) {
		printerr(debugLevel, "repeat the repeat command is not allowed\n");
	    } else {
		strncpy(repeatCmd, line + 2, BUFFERSIZE);
		printf("%s", repeatCmd);
	    }
	    break;
	}
    }
    
    assert(line != NULL);
    free(line);
    fclose(fp);
}

void clr_sish(int argc, char *argv[])
{
	for (int i = 0; i < 50; i++ )
    	putchar ( '\n' );
}

void echo_sish(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
	printf("%s ", argv[i]);
    }
    printf("\n");
}

void chdir_sish(int argc, char *argv[])
{
    if (argc < 2 || argc > 2) {
	printerr(debugLevel, "usage: chdir path\n");
	return;
    }
    if (chdir(argv[1]) == 0) {
	setenv("PWD", argv[1], 1);
	printf("directory changed to %s\n", argv[1]);
    } else {
    	size_t len = strlen("invalid path: ") + strlen(argv[1]) + 2;
		char * buffer = malloc(len);
		sprintf(buffer, "invalid path: %s\n", argv[1]);
		printerr(debugLevel, buffer);
    }
}
    
void environ_sish(int argc, char *argv[])
{
    int i = 0;
    /* the last entry is '\0' */
    while(environ[i]) {
	printf("%s\n", environ[i]);
	i++;
    }    
}

void export_sish(int argc, char *argv[])
{
    if (argc < 3) {
	printerr(debugLevel, "usage: export name value\n");
	return;
    }
    setenv(argv[1], argv[2], 1);
}

void unexport_sish(int argc, char *argv[])
{
    if (argc < 2) {
	printerr(debugLevel, "usage: unexport name\n");
	return;
    }
    unsetenv(argv[1]);
}

void show_sish(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
	printf("%s\n", argv[i]);
    }
}

void help_sish(int argc, char *argv[])
{
    char *temp = "more help\n";
    strncpy(repeatCmd, temp, BUFFERSIZE);
}

void dir_sish(int argc, char *argv[])
{
    DIR *dirp;
	struct dirent *dp;

	dirp = opendir(".");
	if (dirp == NULL) {
		printerr(debugLevel, "error: could not open the current directory\n");
	return;
    }
	while((dp = readdir(dirp)) != NULL){
		printf("%s\n", dp -> d_name);
	}
	closedir(dirp);
}

void kill_sish(int argc, char *argv[])
{
    if (argc < 2) {
	printerr(debugLevel, "usage: kill [-n signum] pid\n");
	return;
    }
    long pid = atol(argv[argc - 1]);
    int signum = 15; /* default is SIGTERM */
    if (argc == 3) {
	signum = atoi(argv[1] + 1);
    }
    if (kill(pid, signum) == -1) {
	printerr(debugLevel, "kill failed\n");
    }
}

void pause_sish(int argc, char *argv[]){
	//fprintf(stdout, "sish >> ");
	while(getchar() != '\n');
	// fflush(stdout);
	// pause();
}

/* check to see it the command is a build-in command
   call the command if it exists */
int isBuildIn(int argc, char *argv[])
{
    if (argv[0] == NULL || argc == 0) {
	printerr(debugLevel, "enter your command\n");
	return 1;
    }
    if (!strcmp("history", argv[0])) {
	history_sish(argc, argv);
    } else if (!strcmp("exit", argv[0])) {
	exit_sish(argc, argv);
    } else if (!strcmp("repeat", argv[0])) {
	repeat_sish(argc, argv);
    } else if (!strcmp("clr", argv[0])) {
	clr_sish(argc, argv);
    } else if (!strcmp("echo", argv[0])) {
	echo_sish(argc, argv);
    } else if (!strcmp("chdir", argv[0])) {
	chdir_sish(argc, argv);
    } else if (!strcmp("environ", argv[0])) {
	environ_sish(argc, argv);
    } else if (!strcmp("export", argv[0])) {
	export_sish(argc, argv);
    } else if (!strcmp("unexport", argv[0])) {
	unexport_sish(argc, argv);
    } else if (!strcmp("show", argv[0])) {
	show_sish(argc, argv);
    } else if (!strcmp("help", argv[0])) {
	help_sish(argc, argv);
    } else if (!strcmp("dir", argv[0])) {
	dir_sish(argc, argv);
    } else if (!strcmp("set", argv[0])) {
	/* TODO */
    } else if (!strcmp("unset", argv[0])) {
	/* TODO */
    } else if (!strcmp("wait", argv[0])) {
	/* TODO */
    } else if (!strcmp("pause", argv[0])) {
	pause_sish(argc, argv);
    } else if (!strcmp("kill", argv[0])) {
	kill_sish(argc, argv);
    } else {
	return 0;
    }
    return 1;
}
