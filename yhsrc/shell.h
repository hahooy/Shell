/* -------header--------- */

#include <string.h> /* for strdup */
#include <assert.h> /* for assertion */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for getopt */

/* -------macro definition--------- */

#define MAXFILEARG 100  /* maximum number of arguments associated with input file */
#define MAXTOKENS 1000  /* maximum number of tokens from command line input */
#define MAXNUMOFPROS 4  /* maximum number of input programs */
#define TOKENDELM " \t\n" /* delimiters for parsing the command line input */

/* -------define Program--------- */

typedef struct {
    int argc;
    char **argv;
    int stdout_redirect;
    int stdin_redirect;
    char *outfile;
    char *infile;
} Program;

/* -------function prototype--------- */

/* Program object methods */
Program *Program_create(int, char **, int, int, char *, char *);
void Program_destroy(Program *);
void Program_print(Program *);
/* parser related functions */
void parsecml(int, char **);
int tokcml(char *, char ***);
int token_destroy(char **);
void printerr(int, char*);
int getArgs(char **);
void setNumOfPipes(char **);
int getNumOfPipes(void);
int ispiped(void);
int parse_input_line(void);
/* io redirection functions */
void cmdPiped(Program *, int, int);
void cmdRedirection(Program *, int);

/* -------variable decalaration--------- */

int numOfPipes = 0;
int dflag = 0;
int xflag = 0;
int fflag = 0;
int debugLevel = 0;
char *batchfile;
char *filearg[MAXFILEARG];
Program *programs[MAXNUMOFPROS]; /* input programs */
