#include "shell.h"

/* all build-in commands are implemented in this file */

/* function prototypes */

void exit_sish(int argc, char *argv[]);
void cleanup_sish(void);
void history_sish(int argc, char *argv[]);
void repeat_sish(int argc, char *argv[]);
void clr_sish(int argc, char *argv[]);
void echo_sish(int argc, char *argv[]);
void chdir_sish(int argc, char *argv[]);
void environ_sish(int argc, char *argv[]);
void export_sish(int argc, char *argv[]);
void unexport_sish(int argc, char *argv[]);
void show_sish(int argc, char *argv[]);
void help_sish(int argc, char *argv[]);
void dir_sish(int argc, char *argv[]);
void kill_sish(int argc, char *argv[]);    
void pause_sish(int argc, char *argv[]);  
int isBuildIn(int argc, char *argv[]);
char *findVar(char* search_key);
int isInternal(int argc, char *argv[]);
void set_sish(int argc, char *argv[]);
void unset_sish(int argc, char *argv[]);
void destroy_localVar(void);

/* function definition */


typedef struct variable {
    char key[100];
    char value[100];
    int empty;
}variable;
variable *variables;

void init_localVar(){
    //allocate the memory space for an array of 100 variable structs
    variables = malloc(sizeof(variable)*100);
    for (int i = 0; i < 100; ++i) {
    	//initilize all the struct with empty flag to be 1
	variables[i].empty = 1;
    }
}

void destroy_localVar(void)
{
    if (variables != NULL) {
	free(variables);
    }
}

void set_sish(int argc, char *argv[]){
    if (argc < 3) {
	printerr(debugLevel, "usage: set name value\n");
	return;
    }
    int emptySpot = 0;
    int exists = 0;
    for (int i = 0; i < 100; ++i) {
	//go through the struct array and find whether there is an empty variable
	if (variables[i].empty) {
	    //record the empty struct
	    emptySpot = i;
	} else {
            //if the key conflict, overwrite the value
	    if (!strcmp(variables[i].key, argv[1])) {
		strncpy(variables[i].value, argv[2], 100);
		variables[i].empty = 0;
		exists = 1;
		break;
	    } 
	}
    }
    //if the key is a new one, insert it into the empty spot
    if (!exists) {
	strncpy(variables[emptySpot].key, argv[1], 100);
	strncpy(variables[emptySpot].value, argv[2], 100);
	variables[emptySpot].empty = 0;
    }
}

//a function to unset the variable
void unset_sish(int argc, char *argv[]){
    if (argc < 2) {
	printerr(debugLevel, "usage: set name\n");
	return;
    }
    for (int i = 0; i < 100; ++i) {
	//go over the struct array and set the empty flag 
	//of the struct with found key to 1
	if (!strcmp(variables[i].key,argv[1])) {
	    variables[i].empty = 1;
	    break;
	}
    }
}

//a function to substitute the variables in argument with their values
void replaceVar_sish(char *argv[])
{    
    if (argv == NULL || argv[0] == NULL) {
	return;
    }
    for (int i=1; argv[i] != NULL; ++i){
	if (argv[i][0] == '$') {
	    char *tempstr;
	    if ((tempstr = findVar(argv[i] + 1)) != NULL /* search in local */
		|| (tempstr = getenv(argv[i] + 1)) != NULL) { /* search in global */
		free(argv[i]); /* clean up the old token */
		argv[i] = strdup(tempstr); /* replicate the new token */
	    }
	}
    }	
}

//a function to search for a give key in the struct array
//return 0 if the key is found, otherwise return 1
char *findVar(char* search_key)
{
    for (int i = 0; i < 100; ++i) {
    	//go through the array of varibles and check for all variables
	if(!variables[i].empty) {
	    if(!strcmp(search_key, variables[i].key)) {
		/* search hit */
		return variables[i].value;
	    }
	}
    }

    /* return null if not found */
    return NULL;
}


void exit_sish(int argc, char *argv[])
{
    int status = 0;
    if (argc > 1) {
	status = atoi(argv[1]);
    }
    /* clean up the memory */
    cleanup_sish();
    exit(status);
}

/* a helper function to clean up the program before exit */
void cleanup_sish(void)
{    
    /* close open history file */
    if (fclose(historyptr) != 0) {
	printerr(debugLevel, "fail to close the history file\n");
	return;
    }

    /* delete history file */
    if (remove(historyfilename) != 0) {
	printerr(debugLevel, "fail to delete the history file\n");
	return;
    }

    /* close open batch file */
    if (fflag) {
	if (fclose(batchfileptr) != 0) {
	    printerr(debugLevel, "fail to close the batch file\n");
	    return;
	}
    }

    /* free string variables */
    assert(shellpath != NULL);
    free(shellpath);
    
    /* free the memory space for the array storing local variables */
    destroy_localVar();

    /* free created program structs */
    for (int i = 0; i < MAXNUMOFPROS; i++) {
	if (programs[i] != NULL) {
	    Program_destroy(programs[i]);
	}
    }
}

void wait_sish(int argc, char*argv[])
{
    if (argc < 2) {
	printerr(debugLevel, "usage: wait I\n");
	return;
    }
    waitpid(atoi(argv[1]), 0, 0);
}

void history_sish(int argc, char *argv[])
{
    FILE *fp = fopen(historyfilename, "r");
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
    int targetLineNum = cmdIndex;
    size_t linecap = 0;
    ssize_t linelen;
    FILE *fp = fopen(historyfilename, "r");    

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
	    strncpy(repeatCmd, strstr(line, " ") + 1, BUFFERSIZE);
	    printf("%s", repeatCmd);	    
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
    if (argc != 2) {
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
	free(buffer);	     
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
    if (argc < 2) {
	printerr(debugLevel, "usage: show name ...\n");
	return;
    }
    for (int i = 1; i < argc; i++) {
	printf("%s\n", argv[i]);
    }
}

void help_sish(int argc, char *argv[])
{
    /* construct command "more readmepath\n" */
    strncpy(repeatCmd, "more ", BUFFERSIZE);
    strncat(repeatCmd, readmepath, BUFFERSIZE);
    strncat(repeatCmd, "\n", BUFFERSIZE);
}

void dir_sish(int argc, char *argv[])
{
    struct dirent **namelist;
    int n = scandir(".", &namelist, 0, alphasort);
    if (n < 0)
	perror("scandir");
    else {
	for (int i = 0; i < n; i++) {
	    printf("%s\n", namelist[i]->d_name);
	    free(namelist[i]);
	}
    }
    free(namelist);
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
    while(getchar() != '\n');
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
    } else if (!strcmp("echo", argv[0])) {
	echo_sish(argc, argv);
    } else if (!strcmp("dir", argv[0])) {
	dir_sish(argc, argv);
    } else {
	return 0;
    }
    return 1;
}

/* commands that have side effects and can not be executed by the child */
int isInternal(int argc, char *argv[])
{
    if (argv[0] == NULL || argc == 0) {
	printerr(debugLevel, "enter your command\n");
	return 1;
    }
    if (!strcmp("exit", argv[0])) {
	exit_sish(argc, argv);
    } else if (!strcmp("environ", argv[0])) {
	environ_sish(argc, argv);
    } else if (!strcmp("help", argv[0])) {
	help_sish(argc, argv);
    } else if (!strcmp("repeat", argv[0])) {
	repeat_sish(argc, argv);
    } else if (!strcmp("clr", argv[0])) {
	clr_sish(argc, argv);
    } else if (!strcmp("chdir", argv[0])) {
	chdir_sish(argc, argv);
    } else if (!strcmp("export", argv[0])) {
	export_sish(argc, argv);
    } else if (!strcmp("unexport", argv[0])) {
	unexport_sish(argc, argv);
    } else if (!strcmp("show", argv[0])) {
	show_sish(argc, argv);
    } else if (!strcmp("set", argv[0])) {
	set_sish(argc, argv);
    } else if (!strcmp("unset", argv[0])) {
	unset_sish(argc, argv);
    } else if (!strcmp("wait", argv[0])) {
	wait_sish(argc, argv);
    } else if (!strcmp("pause", argv[0])) {
	pause_sish(argc, argv);
    } else if (!strcmp("kill", argv[0])) {
	kill_sish(argc, argv);
    } else {
	return 0;
    }
    return 1;
}
