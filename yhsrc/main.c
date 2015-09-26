#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h> /* needed for signal */
#include "my_signo.h" /* needed for signal handling */


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **split_line(char *line); /* tokenize a line of string */

int main(int argc, char *argv[]) {
    extern char **environ;
    int pid;
    char *pathname = "/bin/echo";
    char *line = NULL;
    size_t linecap = 0;
    char **tokens = malloc(100 * sizeof(char *));

    sig_init(); /* signal functions initialization */

    fprintf(stdout, "The sish shell is now executing\n");

    for(;;) {

	fprintf(stdout, "sish >> ");
	getline(&line, &linecap, stdin);
	tokens = split_line(line);	

	if (system(line) < 0) {
	    fprintf(stderr, "system is not available");
	}

	/*
	  if ((pid = fork()) == 0) {
	  // code executed by child	    
	  execvp(tokens[0], tokens);
	  } else {
	  // code executed by parent
	  while (wait(0) != pid);
	  }
	*/
    }
    return 0;
}

char **split_line(char *line) {    
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char *linecpy = strdup(line);
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
	fprintf(stderr, "lsh: allocation error\n");
	exit(EXIT_FAILURE);
    }

    token = strtok(linecpy, LSH_TOK_DELIM);
    while (token != NULL) {
	tokens[position] = token;
	position++;

	if (position >= bufsize) {
	    bufsize += LSH_TOK_BUFSIZE;
	    tokens = realloc(tokens, bufsize * sizeof(char*));
	    if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	    }
	}

	token = strtok(NULL, LSH_TOK_DELIM);
    }
    assert(linecpy != NULL);
    free(linecpy);
    tokens[position] = NULL;
    return tokens;
}
