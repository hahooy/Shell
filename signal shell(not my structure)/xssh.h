#ifndef _XSSH_H
#define _XSSH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

//struct that stores local variables for the shell
//variable and value length cannot be more than 99

typedef struct variable {
    char key[100];
    char value[100];
    int empty;
}variable;

//Function definitions

void initialize();

//Shell is the function that accepts and executes given commands
int shell(int fflag, int xflag, char **fvalp, variable *variables);

//builtin checks to see if the entered command is a builtin command or not
int builtin(char **cmdString, variable *variables);

//Catches the Ctrl-C signal and kills the foreground process running in xssh
void sigint_handler(int sig);
void sigquit_handler(int sig);
void sigcont_handler(int sig);
void sigtstp_handler(int sig);
void sigchld_handler(int sig);



//Replaces the local variables of the shell
int varSubstitution(char *string, variable *variables);

//Checks and sets values for redirection
void redirection(char **cmdString,int numCmd, int *inputFile, int *outputFile);

#endif /* _XSSH_H */
