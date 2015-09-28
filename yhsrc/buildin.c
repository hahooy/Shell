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
	    fprintf(stderr, "Failed to open file\n");	    
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
	fprintf(stderr, "could not open history file!\n");
	return;
    }

    if (argc > 1) {
	targetLineNum = atoi(argv[1]);
    }
    
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
	if (linelen >= BUFFERSIZE) {	    
	    fprintf(stderr, "command is too long\n");
	    break;
	}
	++curLineNum;
	if (curLineNum == targetLineNum) {
	    if (strcmp(line + 2, "repeat\n") == 0) {
		fprintf(stderr, "repeat the repeat command is not allowed\n");
	    } else {
		strncpy(repeatCmd, line + 2, BUFFERSIZE);
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
    /* USED SYSTEM! */
    system("clear");
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
    if (argc < 2) {
	fprintf(stderr, "usage: chdir path\n");
	return;
    }

    if (chdir(argv[1]) == 0) {
	setenv("PWD", argv[1], 1);
	printf("directory changed to %s\n", argv[1]);
    } else {
	fprintf(stderr, "invalid path: %s\n", argv[1]);
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
	fprintf(stderr, "usage: export name value\n");
	return;
    }
    setenv(argv[1], argv[2], 1);
}

void unexport_sish(int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "usage: unexport name\n");
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
    /* USING EXEC WILL KILL THE SHELL */
    /* TODO */
    char *ptr1 = "more";
    char *ptr2 = "help";
    char *more[] = {ptr1, ptr2};
    execvp("more", more);
}

void dir_sish(int argc, char *argv[])
{
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(".");
    if (dirp == NULL) {
	fprintf(stderr, "error: could not open the current directory\n");
	return;
    }
    while ((dp = readdir(dirp)) != NULL) {
	if (strcmp(dp -> d_name, ".") && strcmp(dp -> d_name, "..")) {
	    printf("%s  ", dp -> d_name);
	}
    }
    closedir(dirp);
    printf("\n");
}

void kill_sish(int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "usage: kill [-n signum] pid\n");
	return;
    }
    int pid = atoi(argv[argc - 1]);
    int signum = 15; /* default is SIGTERM */
    if (argc == 3) {
	signum = atoi(argv[1] + 1);
    }



}

/* check to see it the command is a build-in command
   call the command if it exists */
int isBuildIn(int argc, char *argv[])
{
    if (argv[0] == NULL || argc == 0) {
	return 1;
    }
    if (!strcmp("history", argv[0])) {
	history_sish(argc, argv);
	return 1;
    } else if (!strcmp("exit", argv[0])) {
	exit_sish(argc, argv);
	return 1;
    } else if (!strcmp("repeat", argv[0])) {
	repeat_sish(argc, argv);
	return 1;
    } else if (!strcmp("clear", argv[0])) {
	clr_sish(argc, argv);
	return 1;
    } else if (!strcmp("echo", argv[0])) {
	echo_sish(argc, argv);
	return 1;
    } else if (!strcmp("chdir", argv[0])) {
	chdir_sish(argc, argv);
	return 1;
    } else if (!strcmp("environ", argv[0])) {
	environ_sish(argc, argv);
	return 1;
    } else if (!strcmp("export", argv[0])) {
	export_sish(argc, argv);
	return 1;
    } else if (!strcmp("unexport", argv[0])) {
	unexport_sish(argc, argv);
	return 1;
    } else if (!strcmp("show", argv[0])) {
	show_sish(argc, argv);
	return 1;
    } else if (!strcmp("help", argv[0])) {
	help_sish(argc, argv);
	return 1;
    } else if (!strcmp("dir", argv[0])) {
	dir_sish(argc, argv);
	return 1;
    } else if (!strcmp("set", argv[0])) {
	/* TODO */
	return 1;
    } else if (!strcmp("unset", argv[0])) {
	/* TODO */
	return 1;
    } else if (!strcmp("wait", argv[0])) {
	/* TODO */
	return 1;
    } else if (!strcmp("pause", argv[0])) {
	/* TODO */
	return 1;
    } else if (!strcmp("kill", argv[0])) {
	/* TODO */
	return 1;
    }
    return 0;
}
