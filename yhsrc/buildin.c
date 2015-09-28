#include "shell.h"

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
    }
    return 0;
}
