/* -------header--------- */

#include <string.h> /* for strdup */
#include <assert.h> /* for assertion */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for getopt */
#include <signal.h> /* needed for signal */
#include <fcntl.h> /* needed for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> /* needed for alphasort */

/* -------macro definition--------- */

#define MAXFILEARG 100  /* maximum number of arguments associated with input file */
#define MAXTOKENS 1000  /* maximum number of tokens from command line input */
#define BUFFERSIZE 1000 /* size of all buffer strings */
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
    int bg;
} Program;

/* -------function prototype--------- */

/* Program object methods */
Program *Program_create(int, char **, int, int, char *, char *, int);
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
void cmdPiped(Program **, int, int);
void cmdRedirection(Program *, int);
/* signal handling */
void sig_init(void); /* initialize signal functions */
/* build-in function */
int isBuildIn(int, char **);

/* -------variable decalaration--------- */

int numOfPipes;
int dflag;
int xflag;
int fflag;
int debugLevel;
int cmdIndex;
char *batchfile;
char *filearg[MAXFILEARG];
Program *programs[MAXNUMOFPROS]; /* input programs */
FILE *historyptr;
char repeatCmd[BUFFERSIZE]; /* buffer for repeat command */
extern char **environ;
extern int alphasort(); //Inbuilt sorting function
