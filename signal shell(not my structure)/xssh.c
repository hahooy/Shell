#include "xssh.h"

extern char **environ;

pid_t fg_pid;
char last_fg_value[10];
char shell_pid[10];
pid_t last_bg_pid;
int fg = 0;
int dval = 0;
FILE *cmd;
FILE *inputFilename;
FILE *outputFilename;

char histBuffer[100][99];

char ***repeatCommand;
variable **repeatVars;

int nth = 0;
int cmd_count = 0;
int recmd_count = 0;

int main(int argc, char **argv) 
{

    initialize();
    //Store shell pid
    sprintf(shell_pid, "%d", getpid());

    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    signal(SIGCONT, sigcont_handler);
    signal(SIGTSTP, sigquit_handler);

    //Parse command line arguments
    int xflag = 0;
    int fflag = 0;
    char *fvalue = NULL;
    int dflag = 0;
    char *dvalue = NULL;
    char c;
    while (1)
    {
        c = getopt(argc, argv, "xd:f:");
        if (c == -1) {
            break;
        }
        switch (c)
        {
        case 'x':
            xflag = 1;
            break;
        case 'f':
            fflag = 1;
            fvalue = optarg;
            break;
        case 'd':
            dflag = 1;
            dvalue = optarg;
            break;
        }
    }
    //Create array of 100 variable structs to store variables and mark
    //them all as empty
    if (dflag) {
        dval = atoi(dvalue);
    }
    int i;
    variable *variables = malloc(sizeof(variable)*100);
    for (i = 0; i < 100; ++i) {
        variables[i].empty = 1;
    }
    //Run shell
    shell(fflag,xflag,&fvalue,variables);
    return 0;
}

int shell(int fflag, int xflag, char **fvalp, variable *variables){
    char *fvalue = *fvalp;
    char line[100];
    char *strings[50];
    char *saveptr;
    char *token;
    char *read;
    pid_t pid;

    //Open file if -f
    if (fflag) {
        cmd = fopen(fvalue,"r");
        if (cmd == NULL) {
            printf("Couldn't open file for read");
        }
    }

    //Main shell loop always running
    while (1) {
        //if -f read from flag
        if (fflag) {
            if (fgets(line,99,cmd) == NULL) {
                fclose(cmd);
                free(variables);
                exit(0);
            }
        } else {
            //read from command line
            read = readline(">> ");
            strncpy(line, read, 99);
            strncpy(histBuffer[cmd_count++], line, 99);
        }

        //Process given command
        token = strtok_r(line," ",&saveptr);
        int numArgs = 0;
        while (token != NULL) {
            strings[numArgs] = token;
            token = strtok_r(NULL," \n",&saveptr);
            numArgs++;
        }
        strings[numArgs] = NULL;

        //check for comment and end pointer there if it exists
        int i = 0;
        for (i = 0; i < numArgs; i++) {
            if (!strcmp(strings[i],"#")) {
                strings[i] = NULL;
                break;
            }
        }
        numArgs = i; 

        //See if line is a comment
        int runshell = 0;
        if (numArgs != 0) {
            if (strings[0][1] != '#')
                runshell = 1;
        }
        //Start substitution for variables
        //substitution true if substitution takes place.
        if (runshell) {
            int substitution = 0;
            int i;
            for (i = 0; i < numArgs; ++i) {
                //Search for variables
                char first = strings[i][0];
                char second = strings[i][1];
                int result = 0;
                result = (first == '$');
                if (result) {
                    //try local variables
                    if (second == '$') {
                        strncpy(strings[i],shell_pid,100);
                        substitution++;
                    } else if (second == '?') {
                        strncpy(strings[i],last_fg_value,100);
                        substitution++;
                    } else if (second == '!') {
                        char fg_pid_string[10];
                        sprintf(fg_pid_string,"%d",fg_pid);
                        strncpy(strings[i],fg_pid_string,100);
                        substitution++;
                    } else {
                        if ((varSubstitution(strings[i], variables)) == 1) {
                            if (dval) {
                                printf("Variable %s doesn't exist\n",strings[i]);
                            }
                        } else {
                            substitution++;
                        }
                    }
                }
            }

            int bflag = 0;
            //Check if background command
            if (!strcmp(strings[numArgs-1],"&")) {
                bflag = 1;
                strings[numArgs-1] = NULL;
                numArgs--;
            }
            if (numArgs == 0) {
                printf("please enter a command\n");
            } else {
                //if -x option and substitution done then print replaced command
                if (xflag && substitution) {
                    printf("%s",strings[0]);
                    int i = 1;
                    while (strings[i] != NULL) {
                        printf(" %s",strings[i]);
                        i++;
                    }
                    printf("\n");
                }
                //Fork process and run command if not builtin command
                int fileIn = 0;
                int fileOut = 0;
                redirection(&strings[0], numArgs, &fileIn, &fileOut);
                if(!builtin(&strings[0],variables)) {
                    if ((pid = fork()) == 0) {
                        //If not backgroud flag then it is a foreground process
                        if (!bflag) {
                            fg = 1;
                            fg_pid = pid;
                        }
                        signal(SIGTSTP, sigtstp_handler);
                        if (fileOut) {
                            fflush(stdout);
                            dup2(fileno(outputFilename), STDOUT_FILENO);
                            fclose(outputFilename);
                        }
                        if (fileIn) {
                            fflush(stdin);
                            dup2(fileno(inputFilename), STDIN_FILENO);
                            fclose(inputFilename);
                        }
                        //Check if command was run properly
                        if ((execvp(strings[0],strings) == -1)) {
                            if (dval) {
                                printf("Error executing command");
                            }
                            return 1;
                        } else {
                            return 0;
                        }
                    } else {
                        //If not a background process, wait until child process
                        //returns

                        pid_t wait_pid;
                        if (!bflag) {
                            int status;
                            while ((wait_pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) > 0) {
                                if (WIFSTOPPED(status))
                                    break;
                            }
                            sprintf(last_fg_value, "%d",status);
                        }
                    }
                }
            }
        }
        //Free memory allocated to readline.
        free(read);
    }
}

int builtin(char **cmdString, variable *variables)
{
    char **command = cmdString;

    int i = 0;
    char *c; 
    while((c = command[i]) != NULL) {
        strncpy(repeatCommand[recmd_count][i], c, strlen(c));
        strncpy(repeatVars[recmd_count][i].key, variables -> key, 100);
        strncpy(repeatVars[recmd_count][i].value, variables -> value, 100);
        repeatVars[recmd_count][i].empty = variables -> empty;
        i++;
    }
    repeatCommand[recmd_count][i] = NULL;
    recmd_count++;

    //If show command
    if (!strcmp(command[0],"show")) {
        //go through command strings and print them out.
        //Variables have already been substituted so it's fine
        int i = 1;
        while (command[i] != 0) {
            printf("%s\n",command[i]);
            i++;
        }
        return 1;
    }
    //if set find first empty spot in array and fill struct
    if (!strcmp(command[0],"set")) {
    
        return 1;
    }
    //If "bg" or "fg" fun function
    if (!strcmp(command[0],"unset")) {
        int i = 0;
        for (i = 0; i < 100; ++i) {
            if (!strcmp(variables[i].key,command[1])) {
                variables[i].empty = 1;
                break;
            }
        }
        return 1;
    }
    if (!strcmp(command[0],"export")) {
        if (command[2] == NULL) {
            if(dval) {
                printf("Need two arguments\n");
            }
        } else {
            setenv(command[1],command[2],1);
        }
        return 1;
    }
    if (!strcmp(command[0],"unexport")) {
        if (command[2] == NULL) {
            if (dval) {
                printf("Need two arguments\n");
            }
            return 1;
        }
        unsetenv(command[1]);
        return 1;
    }
    if (!strcmp(command[0],"chdir")) {
        if (command[1] == NULL) {
            printf("This command requires an argument\n");
            return 1;
        }
        if (chdir(command[1]) == -1)
            if (dval) {
                printf("Directory change error\n");
            }
        return 1;
    }
    if (!strcmp(command[0],"exit")) {
        //free malloced variables
        free(variables);
        if (command[1] == NULL) {
            if (dval) {
                printf("No exit status given\n");
            }
            exit(0);
        } else {
            exit(atoi(command[1]));
        }
    }
    if (!strcmp(command[0],"wait")) {
        int status;
        pid_t pid;
        int val = atoi(command[1]);
        if (val == -1) {
            while ((pid = wait(&status)) > 0);
        } else {
            while ((pid = wait(&status)) != atoi(command[1]));
        }
        return 1;
    }
    if (!strcmp(command[0], "clr")) {
        system("clear");
        return 1;
    }
    if (!strcmp(command[0], "environ")) {
        char **env;
        for (env = environ; *env != 0; env++) {
            printf("%s\n", *env);
        }
        return 1;
    }
    if (!strcmp(command[0], "history")) {
        int cmd_num = 0;
        if (command[1] != NULL) {
            cmd_num = atoi(command[1]);
            if (cmd_num < cmd_count) {
                printf("%s\n", histBuffer[cmd_num]);
                nth = cmd_num;
            }
            else {
                printf("%s\n", histBuffer[cmd_count-1]);
                nth = cmd_count-1;
            }
        }
        else {
            int i = 0;
            for (i = 0; i < cmd_count; i++) {
                printf("%s\n", histBuffer[i]);
            }
            nth = cmd_count-1;
        }
    }
    if (!strcmp(command[0], "repeat")) {
        builtin(repeatCommand[nth], repeatVars[nth]);
    }
    //Else not a built in command
    else {
        return 0;
    }
}

void sigint_handler(int sig)
{
    if (fg) {
        kill(fg_pid, SIGINT);
    }
    return;
}

void sigquit_handler(int sig)
{
    if (fg) {
        kill(fg_pid, SIGQUIT);
    }
    return;
}

void sigcont_handler(int sig)
{
    if (fg) {
        kill(fg_pid, SIGCONT);      
    }
    return;
}

void sigtstp_handler(int sig)
{
}

void initialize()
{
    int m=100,n=100,p=100;

    repeatCommand = malloc(m * sizeof(char **));
    int i, j;
    for (i = 0; i < m; ++i)
    {
        repeatCommand[i] = malloc(n * sizeof(char *));
        for (j = 0; j < n; ++j)
        {
            repeatCommand[i][j] = malloc(p);
        }
    }
    repeatVars = malloc(m * sizeof(variable *));
    for (i = 0; i < m; i ++)
    {
        repeatVars[i] = malloc(n * sizeof(variable));
    }
}

int varSubstitution(char *string, variable *variables) {
    int i = 0;
    for (i = 0; i < 100; ++i) {
        if(!variables[i].empty) {
            if(!strcmp(&string[1],variables[i].key)) {
                strncpy(string,variables[i].value ,10);
                return 0;
            }
        }
    }
    return 1;
}

void redirection(char **cmdString,int numCmd, int *inputFile, int *outputFile) {
    int i;
    for (i = 0; i < numCmd; i++) {
        if (!strcmp(cmdString[i],"<")) {
            *inputFile = 1;
            inputFilename = fopen(cmdString[i+1],"r");
            cmdString[i] = NULL;
        } else if (!strcmp(cmdString[i],">")) {
            *outputFile = 1;
            outputFilename = fopen(cmdString[i+1],"w");
            cmdString[i] = NULL;
        }
    }
}
