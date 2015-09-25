/* 	Author: Dionisius Mayr
 * 	Date: 04 March, 2015
 * 	This program implements a simple UNIX-shell that is able to execute commands given by the user,
 * 	with redirections and up to three pipes. It also has the command exit and history implemented.
 *	It is possible to compile this under a UNIX environment simply using:
 *		g++ -o shell shell.cpp
 * */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// USER_NAME is the the name to be used on the prompt.
#define USER_NAME "UserName"
// STRING_SIZE is the maximum size of the full command.
#define STRING_SIZE 100
// COMMAND_QUANTITY is the maximum number of commands a user can input in the same line.
#define COMMAND_QUANTITY 4
// NAME_SIZE is the maximum lenght of the name of a command.
#define NAME_SIZE 20
// ARGUMENTS_MAX is the maximum number of arguments per command.
#define ARGUMENTS_MAX 7
// PIPES_MAXIMUM is the maximum number of pipes that an entry can have.
#define PIPES_MAXIMUM 3
// FILEN_NAME is the name of the history file.
#define FILE_NAME "history.txt"

/* Prototypes */
bool isWhiteSpace(char character);
void initialize2dCharArray(char array[][NAME_SIZE], int size);
void initialize3dCharArray(char array[][ARGUMENTS_MAX][NAME_SIZE], int size);
int countArguments(char separatedArguments[][ARGUMENTS_MAX][NAME_SIZE], int size);

using namespace std;

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);	// Turning off the buffer
	char fullCommand[STRING_SIZE];		// The entire command will be stored here, with spaces, dashes, arguments, redirection tokens and so...

	// These two files are used to store/retrieve the commands for the "history" command.
	// If there isn't a file named FILE_NAME, one will be created.
	FILE * fileOut;
	fileOut = fopen(FILE_NAME, "w");
	FILE * fileIn;
	fileIn = fopen(FILE_NAME, "r");

	// General instructions
	printf("--------------------GENERAL INSTRUCTIONS----------------------\n");
	printf("Command pattern: command_name -flags arguments\n");
	printf("It is possible to exit the shell typing exit.\n");
	printf("The full command size should be less than %d characters.\n", STRING_SIZE);
	printf("There should be no more than %d commands.\n", COMMAND_QUANTITY);
	printf("The maximum length of each command should is %d characters.\n", NAME_SIZE - 1);
	printf("Each command cannot have more than %d arguments.\n", ARGUMENTS_MAX);
	printf("The history command reffers only to the current session.\n\n");

	int commandNumber = 0;					// Quantity of commands entered by the user.

	/* Main loop, keeps asking the user to insert commands. */
	while(true)
	{
		printf("%s>", USER_NAME); fflush(stdout);	// Prompt the user
		gets(fullCommand);							// Read the command
		// Note: even though the use of this function is discouraged, it still is enough for our purpose here,
		// which is just execute simple commands. Also it is on the TODO list to improve the parser to use std::strings,
		// and no C-style strings.

		// Increments the command counter.
		commandNumber++;

		// Store the command into the file named FILE_NAME.
		fprintf(fileOut, "%d: %s\n", commandNumber, fullCommand); fflush(fileOut);

		int pipeQuantity = 0;				// Quantity of pipes in the fullCommand.
		int redirectionQuantity = 0;		// Quantity of redirection tokens in the fullCommand.
		bool outputRedirection = false;		// Is there an output redirection token?
		bool inputRedirection = false;		// Is there an input redirection token?

		/* Count how many pipes are in the command and if it has redirections. */
		for(int i = 0; fullCommand[i] != '\0'; i++) {	
			if(fullCommand[i] == '|')
				pipeQuantity++;

			if(fullCommand[i] == '>') {
				outputRedirection = true;
				redirectionQuantity++;
			}

			if(fullCommand[i] == '<') {
				inputRedirection = true;
				redirectionQuantity++;
			}
		}

		/* Check if the command has both redirections and pipes (invalid case). */
		if((inputRedirection || outputRedirection) && pipeQuantity > 0) {
			perror("Invalid command.\n");
			exit(1);
		}

		int amountOfCommands;				// Quantity of commands inserted.
		/* amountOfCommands = the first command + one for each of pipes/redirection tokens.
		   Note that the arguments of the redirection tokens are being treated as a "command" here,
		   but it is just to make it easier to get the argument's names later. */
		amountOfCommands = pipeQuantity + redirectionQuantity + 1;		

		// These arrays will store the parsed commands, already separeted by category.
		// They are already initialized by the specific functions.
		char separatedCommand[amountOfCommands][NAME_SIZE]; 					initialize2dCharArray(separatedCommand, amountOfCommands);
		char separatedFlags[amountOfCommands][NAME_SIZE];						initialize2dCharArray(separatedFlags, amountOfCommands);
		char separatedArguments[amountOfCommands][ARGUMENTS_MAX][NAME_SIZE]; 	initialize3dCharArray(separatedArguments, amountOfCommands);

		/* Parsing the full command. */
		if(!inputRedirection && !outputRedirection) {		// It doesn't have any redirection token.
			for(int i = 0, j = 0, k; i < amountOfCommands; i++) {
				int argumentsRead = 0;

				while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
					j++;

				k = 0;
				while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '|' && fullCommand[j] != '\0') {	// Copying the command name.
					separatedCommand[i][k] = fullCommand[j];
					k++;
					j++;
				}

				separatedCommand[i][k] = '\0';			// Setting the null terminator.

				k = 0;
				while(fullCommand[j] != '|' && fullCommand[j] != '\0') {
					if(fullCommand[j] == '-') {	// It is a flag
						while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '|' && fullCommand[j] != '\0') {	// Copying the flags.
							separatedFlags[i][k] = fullCommand[j];
							k++;
							j++;
						}
						separatedFlags[i][k] = '\0';	// Setting the null terminator.
					}
					else if(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '|' && fullCommand[j] != '\0') {		// It is an argument
						k = 0;
						while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '|' && fullCommand[j] != '\0') {	// Copying the argument.
							separatedArguments[i][argumentsRead][k] = fullCommand[j];
							k++;
							j++;
						}
						separatedArguments[i][argumentsRead][k] = '\0';		// Setting the null terminator.

						argumentsRead++;
					}

					if(fullCommand[j] != '\0')
						j++;
				}

				if(fullCommand[j] != '\0')
					j++;

				k = 0;
				argumentsRead = 0;
			}
		}
		else if(inputRedirection || outputRedirection) {	// It has a redirection token.
			for(int i = 0, j = 0, k; i < 1; i++) {
				int argumentsRead = 0;

				while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
					j++;

				k = 0;
				while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '<' && fullCommand[j] != '>' && fullCommand[j] != '\0') {	// Copying the command name.
					separatedCommand[0][k] = fullCommand[j];
					separatedCommand[0][k + 1] = '\0';	// Setting the null terminator.
					k++;
					j++;
				}

				while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
					j++;

				k = 0;
				if(fullCommand[j] == '-') {			// Has flags
					while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '<' && fullCommand[j] != '>' && fullCommand[j] != '\0') {	// Copying the flags.
						separatedFlags[0][k] = fullCommand[j];
						k++;
						j++;
					}
				}
				separatedFlags[0][k] = '\0';		// Setting the null terminator.

				for(int i = 0; i < ARGUMENTS_MAX;i++) {
					while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
						j++;

					if(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '<' && fullCommand[j] != '>' && fullCommand[j] != '\0') {		// It is an argument
						k = 0;
						while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '<' && fullCommand[j] != '>' && fullCommand[j] != '\0') {	// Copying the argument.
							separatedArguments[0][argumentsRead][k] = fullCommand[j];
							k++;
							j++;
						}
						separatedArguments[0][argumentsRead][k] = '\0';		// Setting the null terminator.

						argumentsRead++;
					}
				}

				while(fullCommand[j] != '\0') {
					if(fullCommand[j] == '<') {
						j++;
						k = 0;
						while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
							j++;

						while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '\0') {
							separatedCommand[1][k] = fullCommand[j];
							separatedCommand[1][k + 1] = '\0';
							j++;
							k++;
						}
					}
					else if(fullCommand[j] == '>') {
						j++;
						k = 0;
						while(isWhiteSpace(fullCommand[j]))	// Skipping white spaces.
							j++;

						while(!isWhiteSpace(fullCommand[j]) && fullCommand[j] != '\0') {
							if(outputRedirection && inputRedirection) {
								separatedCommand[2][k] = fullCommand[j];
								separatedCommand[2][k + 1] = '\0';
							}
							if(outputRedirection && !inputRedirection) {
								separatedCommand[1][k] = fullCommand[j];
								separatedCommand[1][k + 1] = '\0';
							}
							j++;
							k++;
						}
					}
					else
						j++;
				}
			}
		}
		/* End of the parsing part. */
		 for(int i = 0; i < amountOfCommands; i++)
		 	cout << "!!!" << separatedCommand[i][0] << "!!!"<< endl;

  //       for(int i = 0; i < amountOfCommands; i++)
  //           for(int j = 0; j < ARGUMENTS_MAX; j++)
  //           	for(int k = 0; k < NAME_SIZE; k++)
  //              cout << "???" << separatedArguments[i][j][k] << "???"<< endl;


		int flagQuantity = 0;		// Amount of flags of the fullCommand.
		int argumentQuantity = 0;	// Amount of arguments of the fullCommand.

		/* Counting how many flags: */
		for(int i = 0; i < amountOfCommands; i++) {
			if(separatedFlags[i][0] != '\0')
				flagQuantity++;
		}

		/* Couting how many arguments: */
		for(int i = 0; i < amountOfCommands; i++) {
			argumentQuantity += countArguments(separatedArguments, i);
		}

		// This array will hold the organized separated commands/flags/arguments arrays in the format of the execvp function.
		char *execArguments[amountOfCommands][amountOfCommands + flagQuantity + argumentQuantity];

		/* Putting together the separated arrays into the execArguments. */
		for(int i = 0; i < amountOfCommands; i++) {
			bool hasFlags;

			execArguments[i][0] = separatedCommand[i];

			if(separatedFlags[i][0] != '\0') {
				hasFlags = true;
				execArguments[i][1] = separatedFlags[i];
			}
			else
				hasFlags = false;

			for(int j = 0; j < countArguments(separatedArguments, i); j++) {
				if(hasFlags)
					execArguments[i][2 + j] = separatedArguments[i][j];
				else 	// If it doesn't have flags, it will have one less "argument".
					execArguments[i][2 + j - 1] = separatedArguments[i][j];
			}

			if(hasFlags)
				execArguments[i][2 + countArguments(separatedArguments, i)] = NULL;
			else
				execArguments[i][2 + countArguments(separatedArguments, i) - 1] = NULL;
		}

		/* Check if it is the exit command. */
		if(!strcmp("exit", separatedCommand[0]))
			break;

		/* Check if it is the history command. */
		if(!strcmp("history", separatedCommand[0])) {
			char historyLine[STRING_SIZE];	// Variable to store the read line.
			int startPoint = 0;

			fseek(fileIn, 0, SEEK_SET);		// Goes to the beginning of the file.

			if(separatedArguments[0][0][0] != '\0')				// Has argument.
				startPoint = atoi(separatedArguments[0][0]);	// Converts the argument to int type.
			else 												// Doesn't have argument.
				startPoint = 10;								// Will treat it as 10.

			int i;

			/* Skip the undesired lines. */
			for(i = 0; i < commandNumber - startPoint; i++) {
				fgets(historyLine, STRING_SIZE, fileIn);
			}

			/* Print the history. */
			for(; i < commandNumber; i++) {
				fgets(historyLine, STRING_SIZE, fileIn);
				printf("%s", historyLine);
			}
			
			/* Goes to the next iteration of the main while loop. */
			continue;
		}

		pid_t pid;			// To use in the forks.
		int status = 0;		// To use in the wait function or to get the status of execvp.

		/* CASE 1: No pipes and no redirections*/
		if(pipeQuantity == 0 && !(outputRedirection || inputRedirection)) {
			pid = fork();

			if(pid < 0) {
				perror("Fork return is < 0.\n");
				exit(1);
			}

			if(pid > 0) {		// Parent
				wait(&status);
			}
			else {				// Child
				if(pid == 0) {
					status = execvp(execArguments[0][0], execArguments[0]);
					perror("Problem with execvp\n");
					exit(1);
				}
			}
		}
		/* CASE 2: Pipes */
		else if(pipeQuantity > 0 && !outputRedirection && !inputRedirection){
			if(pipeQuantity == 1) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd[2]; // For the pipe.
						pipe(fd);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child -> Parent
							close(fd[1]);
							if (dup2(fd[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							status = execvp(execArguments[1][0], execArguments[1]);
							perror("Problem with execvp1\n");
							exit(1);
						}
						else {			// Child -> Child
							close(fd[0]);
							if (dup2(fd[1], STDOUT_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							status = execvp(execArguments[0][0], execArguments[0]);
							perror("Problem with execvp2\n");
							exit(1);
						}
					}
				}
			}
			else if(pipeQuantity == 2) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd1[2];
						int fd2[2];
						pipe(fd1);
						pipe(fd2);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child-Parent
							close(fd2[0]);
							close(fd2[1]);
							close(fd1[1]);
							if (dup2(fd1[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							status = execvp(execArguments[2][0], execArguments[2]);
							perror("Problem with execvp\n");
							//Executes third
						}
						else {			// Child-Child
							pid = fork();

							if(pid < 0) {
								perror("Fork return is < 0.\n");
								exit(1);
							}

							if(pid > 0) {	// Child-Child-Parent
								close(fd1[0]);
								close(fd2[1]);
								if (dup2(fd1[1], STDOUT_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								if (dup2(fd2[0], STDIN_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								status = execvp(execArguments[1][0], execArguments[1]);
								perror("Problem with execvp\n");
								exit(1);
								//Executes second
							}
							else {			// Child-Child-Child
								close(fd1[0]);
								close(fd2[0]);
								close(fd1[1]);
								if (dup2(fd2[1], STDOUT_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								status = execvp(execArguments[0][0], execArguments[0]);
								perror("Problem with execvp\n");
								exit(1);
								//Executes first
							}
						}
					}
				}
			}
			else if(pipeQuantity == 3) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd0[2];
						int fd1[2];
						int fd2[2];
						pipe(fd0);
						pipe(fd1);
						pipe(fd2);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child-Parent
							close(fd2[0]);
							close(fd2[1]);
							close(fd1[0]);
							close(fd1[1]);
							if (dup2(fd0[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd0[1]);
							status = execvp(execArguments[3][0], execArguments[3]);
							perror("Problem with execvp\n");
							exit(1);
							//Executes fourth
						}
						else {			// Child-Child
							pid = fork();

							if(pid < 0) {
								perror("Fork return is < 0.\n");
								exit(1);
							}

							if(pid > 0) {	// Child-Child-Parent
								close(fd2[0]);
								close(fd2[1]);
								if (dup2(fd1[0], STDIN_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								close(fd1[1]);
								close(fd0[0]);
								if (dup2(fd0[1], STDOUT_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								status = execvp(execArguments[2][0], execArguments[2]);
								perror("Problem with execvp\n");
								exit(1);
								//Executes third
							}
							else {			// Child-Child-Child
								pid = fork();

								if(pid < 0) {
									perror("Fork return is < 0.\n");
									exit(1);
								}

								if(pid > 0) {	// Child-Child-Child-Parent
									if (dup2(fd2[0], STDIN_FILENO) < 0) {
										perror("Can't dup");
										exit(1);
									}
									close(fd2[1]);
									close(fd1[0]);
									if (dup2(fd1[1], STDOUT_FILENO) < 0) {
										perror("Can't dup");
										exit(1);
									}
									close(fd0[0]);
									close(fd0[1]);
									status = execvp(execArguments[1][0], execArguments[1]);
									perror("Problem with execvp\n");
									exit(1);
									//Executes second
								}
								else {			// Child-Child-Child-Child
									close(fd2[0]);
									if (dup2(fd2[1], STDOUT_FILENO) < 0) {
										perror("Can't dup");
										exit(1);
									}
									close(fd1[0]);
									close(fd1[1]);
									close(fd0[0]);
									close(fd0[1]);
									status = execvp(execArguments[0][0], execArguments[0]);
									perror("Problem with execvp\n");
									exit(1);
									//Executes first
								}
							}
						}
					}
				}
			}
		}
		/* CASE 3: Redirections */
		else if(outputRedirection || inputRedirection) {
			if(outputRedirection && !inputRedirection) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd[2];
						int file;
						pipe(fd);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child -> Parent
							if (dup2(fd[1], STDOUT_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd[0]);

							status = execvp(execArguments[0][0], execArguments[0]);
							perror("Problem with execvp\n");
							exit(1);
						}
						else {			// Child -> Child
							printf("THE FILE ARGUMENT IS: %s\n", execArguments[1][0]); fflush(stdout);
							file = open(execArguments[1][0], O_RDWR | O_CREAT, 0777);
							
							if (dup2(fd[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd[1]);

							int aux;
							int count;
							while ((count = read(0, &aux, 1)) > 0)
								write(file, &aux, 1);

							exit(0);
						}
					}
				}
			}
			else if(!outputRedirection && inputRedirection) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd[2];
						int file;
						pipe(fd);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child -> Parent
							if (dup2(fd[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd[1]);
							status = execvp(execArguments[0][0], execArguments[0]);
							perror("Problem with execvp\n");
							exit(1);
						}
						else {			// Child -> Child
							file = open(execArguments[1][0], O_RDONLY);
							if (dup2(fd[1], STDOUT_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd[0]);

							int aux;
							int count;

							while ((count = read(file, &aux, 1)) > 0)
								write(fd[1], &aux, 1);

							exit(0);
						}
					}
				}
			}
			else if(outputRedirection && inputRedirection) {
				pid = fork();

				if(pid < 0) {
					perror("Fork return is < 0.\n");
					exit(1);
				}

				if(pid > 0) {		// Parent
					wait(&status);
				}
				else {				// Child
					if(pid == 0) {
						int fd1[2];
						int fd2[2];
						pipe(fd1);
						pipe(fd2);

						pid = fork();

						if(pid < 0) {
							perror("Fork return is < 0.\n");
							exit(1);
						}

						if(pid > 0) {	// Child -> Parent
							if (dup2(fd2[1], STDOUT_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							if (dup2(fd1[0], STDIN_FILENO) < 0) {
								perror("Can't dup");
								exit(1);
							}
							close(fd1[1]);
							close(fd2[0]);

							status = execvp(execArguments[0][0], execArguments[0]);
							perror("Problem with execvp\n");
							exit(1);
							// 1
						}
						else {			// Child -> Child
							pid = fork();

							if(pid < 0) {
								perror("Fork return is < 0.\n");
								exit(1);
							}

							if(pid > 0)	{
								int file = open(execArguments[1][0], O_RDONLY);
								if (dup2(fd1[1], STDOUT_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								close(fd1[0]);
								close(fd2[0]);
								close(fd2[1]);

								int aux;
								int count;

								while ((count = read(file, &aux, 1)) > 0)
									write(fd1[1], &aux, 1);

								exit(0);
								// 2
							}
							else {
								int file = open(execArguments[2][0], O_RDWR | O_CREAT, 0777);
								if (dup2(fd2[0], STDIN_FILENO) < 0) {
									perror("Can't dup");
									exit(1);
								}
								close(fd2[1]);
								close(fd1[0]);
								close(fd1[1]);

								int aux;
								int count;
								while ((count = read(0, &aux, 1)) > 0)
									write(file, &aux, 1);

								exit(0);
								// 3
							}
						}
					}
				}
			}
		}
	}

	// Close the file pointers.
	fclose(fileOut);
	fclose(fileIn);

	return 0;
}

/* Function that returns true if the character passed by value is space or tab. */
bool isWhiteSpace(char character) {
	if(character == ' ' || character == '\t')
		return true;
	else
		return false;
}

/* Function used to initialize separatedCommand and separatedFlags with null terminators. */
void initialize2dCharArray(char array[][NAME_SIZE], int size) {
	for(int i = 0; i < size; i++)
		array[i][0] = '\0';
}

/* Function used to initialize separatedArguments with null terminators. */
void initialize3dCharArray(char array[][ARGUMENTS_MAX][NAME_SIZE], int size) {
	for(int i = 0; i < size; i++)
		for(int j = 0; j < ARGUMENTS_MAX; j++)
			for(int k = 0; k < NAME_SIZE; k++)
				array[i][j][k] = '\0';
}

/* Returns the total number of arguments of the variable separatedArguments. */
int countArguments(char separatedArguments[][ARGUMENTS_MAX][NAME_SIZE], int commandNumber) {
	int argumentQuantity = 0;

	for(int i = 0; i < ARGUMENTS_MAX; i++)
		if(separatedArguments[commandNumber][i][0] != '\0')
			argumentQuantity++;

	return argumentQuantity;
}
