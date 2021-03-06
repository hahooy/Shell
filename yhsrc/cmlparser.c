#include "shell.h"

/* -------function definition--------- */

/* methods of program object */
Program *Program_create(int argc, char **argv, int stdout_redirect, int stdin_redirect, char *outfile, char *infile, int bg)
{
    Program *p = malloc(sizeof(Program));
    assert(p != NULL);

    p -> argc = argc;
    p -> stdout_redirect = stdout_redirect;
    p -> stdin_redirect = stdin_redirect;
    p -> outfile = strdup(outfile);
    p -> infile = strdup(infile);
    p -> argv = malloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++) {
	(p -> argv)[i] = strdup(argv[i]);
    }
    (p -> argv)[argc] = NULL;
    p -> bg = bg;

    return p;
}

void Program_destroy(Program *p)
{
    assert(p != NULL);

    free(p -> outfile);
    free(p -> infile);
    for (int i = 0; i < p -> argc; i++) {
	assert((p -> argv)[i] != NULL);
	free((p -> argv)[i]);
    }
    free(p -> argv);
    free(p);
}

void Program_print(Program *p)
{
    printf("argc: %d\n", p -> argc);
    printf("stdout_redirect: %d\n", p -> stdout_redirect);
    printf("stdin_redirect: %d\n", p -> stdin_redirect);
    printf("outfile: %s\n", p -> outfile);
    printf("infile: %s\n", p -> infile);
    printf("bg: %d\n", p -> bg);
    for (int i = 0; i < p -> argc; i++) {
	printf("argv[%d]: %s\n", i, (p -> argv)[i]);
    }    
}


/* parse the command line arguments when executing sish */
void parsecml(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind;
    int c, err = 0;
    char *usage = "usage: sish [-x] [-d <level>] [-f file [arg] ...]\n";

    while ((c = getopt(argc, argv, "xd:f:")) != -1) {
	switch (c) {
	case 'x':
	    xflag = 1;
	    break;
	case 'd':
	    if (dflag == 1) {
		printerr(debugLevel, "Error: -d has been set already\n");
	    }
	    dflag = 1;
	    debugLevel = atoi(optarg);
	    break;
	case 'f':
	    if (fflag == 1) {
		printerr(debugLevel, "Error: -f has been set already\n");
	    }
	    fflag = 1;
	    batchfile = optarg;
	    break;
	case '?':
	    err = 1;
	    break;
	default:
	    break;
	}
	if (err) {
	    printerr(debugLevel, usage);
	    exit(1);
	}	
    }

    if (optind < argc) { /* arguments assigned to shell variables */
	for (int i = 0; optind < argc; i++) {
	    filearg[i] = argv[optind];
	    optind++;
	    if (i >= MAXFILEARG) {
		printerr(debugLevel, "entered too many input arguments, the maximum is 100\n");
		break;
	    }
	}
    }
}

/* tokenize the command line arguments,
it takes the input string and the address
of the pointer of an array of string tokens */
int tokcml(char *input, char **tokens[])
{
    int maxsize = MAXTOKENS, cursize = 0;
    char *inputcpy = strdup(input);    
    char *token;

    if ((*tokens =  malloc(maxsize * sizeof(char*))) == NULL) {
	printerr(debugLevel, "error: memory allocation failed\n");
    }
    token = strtok(inputcpy, TOKENDELM);

    while (token != NULL && token[0] != '#') {
	(*tokens)[cursize] = strdup(token);
	cursize++;

	if (cursize >= maxsize) {
	    maxsize *= 2;
	    if ((*tokens = realloc(*tokens, maxsize * sizeof(char *))) == NULL) {
		printerr(debugLevel, "error: memory allocation failed\n");
	    }
	}
	
	token = strtok(NULL, TOKENDELM);
    }

    (*tokens)[cursize] = NULL;
    free(inputcpy);
    return 0;
}

/* free the tokens, need to be called after done with tokens */
int token_destroy(char *tokens[])
{
    for (int i = 0; tokens[i] != NULL; i++) {
	assert(tokens[i] != NULL);
	free(tokens[i]);
    }
    assert(tokens != NULL);
    free(tokens);
    return 0;
}

/* print the error message if debug level is larger than 0 */
void printerr(int debugLevel, char *errmsg) {
    if (debugLevel > 0) {
	fprintf(stderr, "%s", errmsg);
    }
}

/* go through tokens and store the 
arguments for each program, this function
should be called after tokcml */
int getArgs(char *tokens[])
{
    int position = 0;
    int numPro = 0;
    char *temp[MAXTOKENS];
    char *infile = "";
    char *outfile = "";
    int stdin_redirect = 0;
    int stdout_redirect = 0;
    int bg = 0;
    
    /* empty old programs */
    for (int i = 0; i < MAXNUMOFPROS; i++) {
	if (programs[i] != NULL) {
	    Program_destroy(programs[i]);
	    programs[i] = NULL;
	}	
    }

    if (ispiped()) {
	for (int i = 0; tokens[i] != NULL; i++) {
	    if (strcmp("|", tokens[i]) != 0) {
		if (strcmp("!", tokens[i]) == 0) {
		    bg = 1;
		} else {
		    temp[position++] = tokens[i];
		}
	    } else {
		Program *p = Program_create(position, temp, 0, 0, "", "", bg);
		programs[numPro++] = p;
		position = 0;
	    }	    
	}
	/* the last program */
	Program *p = Program_create(position, temp, 0, 0, "", "", bg);
	programs[numPro] = p;
    } else {
	position = 0;
	for (int i = 0; tokens[i] != NULL; i++) {
	    if (strcmp(">", tokens[i]) == 0) {
		outfile = tokens[++i];
		stdout_redirect = 1;
	    } else if (strcmp("<", tokens[i]) == 0) {
		infile = tokens[++i];
		stdin_redirect = 1;
	    } else if (strcmp("!", tokens[i]) == 0) {
		bg = 1;
	    }else {
		temp[position++] = tokens[i];
	    }
	}
	
	Program *p = Program_create(position, temp, stdout_redirect, stdin_redirect, outfile, infile, bg);
	programs[0] = p;
    }
    assert(temp != NULL);
    return 0;
}

/* set the number of pipes, must be called before getArgs() */
void setNumOfPipes(char *tokens[])
{
    int temp = 0;

    for (int i = 0; tokens[i] != NULL; i++) {
	if (strcmp("|", tokens[i]) == 0) {
	    temp++;
	}
    }
    numOfPipes = temp;
}

/* get the number of pipes */
int getNumOfPipes(void)
{
    return numOfPipes;
}

/* return the number of pipes */
int ispiped(void)
{
    return getNumOfPipes() > 0;
}

int writeHistory(char *line)
{
    if (historyptr == NULL) {
	printerr(debugLevel, "Can't open history file!\n");
	exit(1);
    }
    
    /* do not write repeat command into history */
    if (programs[0] == NULL ||
	programs[0] -> argc == 0 ||
	(programs[0] -> argv)[0] == NULL ||
	!strcmp((programs[0] -> argv)[0], "repeat")) {
	return 0;
    }
    
    cmdIndex++;
    fprintf(historyptr, "%d %s", cmdIndex, line);

    fflush(historyptr); /* flush the buffer immediately */
    return 0;
}

/* the only parser needed in the shell loop.
It needs to be called at the begining of the
loop. It construct all the arguments for programs*/
int parse_input_line(void)
{
    char **tokens;
    char *line = NULL;
    size_t linecap = 0;

    if (strcmp(repeatCmd, "") != 0) {
	line = strdup(repeatCmd); /* get input from the buffer */
	repeatCmd[0] = '\0'; /* empty command buffer */
    } else if (fflag && getline(&line, &linecap, batchfileptr) != -1) {
	/* get input from the batch file if file is not empty */
    } else {	
        /* get input from the command line if buffer is empty */
	fprintf(stdout, "sish >> ");
	getline(&line, &linecap, stdin);
    }
    
    tokcml(line, &tokens);
    replaceVar_sish(tokens);

    if (xflag == 1) {
	for (int i = 0; tokens[i] != NULL; i++) {
	    printf("%s ", tokens[i]);
	}
	printf("\n");
    }

    
    setNumOfPipes(tokens);
    getArgs(tokens);
    writeHistory(line);

    /* clean up */
    token_destroy(tokens);
    free(line);

    return 0;
}


/* ----------unit testing--------------- */
 /* int main(int argc, char *argv[]) */
 /* {  */
 /*     /\* parse input line test case *\/ */
 /*     parse_input_line(); */
 /*     for (int i = 0; programs[i] != NULL; i++) { */
 /* 	Program_print(programs[i]);  */
 /*     }  */

 /*     /\* clean up *\/ */
 /*     for (int i = 0; i < MAXNUMOFPROS; i++) { */
 /* 	if (programs[i] != NULL) { */
 /* 	    printf("%d\n", i); */
 /* 	    Program_destroy(programs[i]); */
 /* 	    programs[i] = NULL; */
 /* 	} */
 /*     } */
 /* } */
