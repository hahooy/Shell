#include <iostream>
#include <istream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


using namespace std;
//define the largest length of a command line
#define MAX_STRING 50
//define the largest length of a path
#define MAX_PATH 50
//define the largest number of arguments per line
#define MAX_ARG 5
//define the largest length of a name
#define MAX_NAME 5

//a struct to store local variables
typedef struct variable {
    char key[100];
    char value[100];
    int empty;
}variable;

//record the current index of commands entered by the user
int cmdIndex = 0;
extern char **environ;
pid_t fg_pid;
char last_fg_value[10];
char shell_pid[10];
pid_t last_bg_pid;
int fg = 0;
int dval = 0;
extern  int alphasort(); //Inbuilt sorting function
char pathname[MAX_PATH];
char ***repeatCommand;
variable **repeatVars;


//function declarations
bool isWhiteSpace(char inputChar);
int varSubstitution(char *string, variable *variables);
void parseCommand(char cmdLine[MAX_STRING], int cmdNum, char parsedCmd[][MAX_NAME], char parsedFlag[][MAX_NAME], char parsedArg[][MAX_ARG][MAX_NAME]);

int main(int argc, char* argv[])
{
    //an array of chars storing the whole command line
    char rawLine[MAX_STRING];
    char cmdLine[MAX_STRING];

    //an array of chars to compare and implement simple commands
    char simpleCmd[MAX_STRING];
    //open the ofstream for storing the history of commands
    ofstream cmdHistory("history");
    //Store shell pid
    sprintf(shell_pid, "%d", getpid());
    //initialize variables
    variable *variables = new variable[100];
    for (int i = 0; i < 100; ++i) {
        variables[i].empty = 1;
    }
    string repeatCmd = "";
    string previousCmd = "";
    while (true){
        //initialize the variables for counding cmd, pipes and handling i/o redirection
        int pipeNum = 0;
        int redirectNum = 0;
        int cmdNum = 0;
        int numArgs = 0;
        int substitution = 0;
        bool outputRedirect = false;
        bool inputRedirect = false;
        int previousFlag = 0;
        
        
        
        //there is no need to repeat the command || previousFlag == 0
        if(repeatCmd == "" ){
            //print out a new line in shell started with "sish >> "
            cout << "sish >> ";
            //cout<<"Asdasd ";
            //get the whole line from the user input
            gets(rawLine);
            ++cmdIndex;
            strncpy(cmdLine, rawLine, MAX_STRING);

        }else{
            ++cmdIndex;
            char temp[MAX_STRING];
            strcpy(temp, repeatCmd.c_str());
            strncpy(cmdLine, temp, MAX_STRING);

            //cmdLine = repeatCmd;
        }

        char delimitedCmd[cmdNum][MAX_NAME];
        for(int i = 0; i < cmdNum; i++)
            delimitedCmd[i][0] = '\0';
        
        //basic splitting by space
        //and store each part into an array
        for(int i = 0, j = 0; cmdLine[j]!= '\0'; i++) {
            while(isWhiteSpace(cmdLine[j]))	// Skipping white spaces.
                j++;
            int k = 0;
            while(!isWhiteSpace(cmdLine[j]) && cmdLine[j] != '\0') {
                delimitedCmd[i][k] = cmdLine[j];
                k++;
                j++;
            }
            delimitedCmd[i][k] = '\0';
            numArgs = i;
        }
        
        numArgs++;
        
        
        for (int i = 0; i < numArgs; ++i) {
            //Search for variables
            char first = delimitedCmd[i][0];
            char second = delimitedCmd[i][1];
            int result = 0;
            result = (first == '$');
            if (result) {
                //try local variables
                if (second == '$') {
                    strncpy(delimitedCmd[i],shell_pid,100);
                    substitution++;
                } else if (second == '?') {
                    strncpy(delimitedCmd[i],last_fg_value,100);
                    substitution++;
                } else if (second == '!') {
                    char fg_pid_string[10];
                    sprintf(fg_pid_string,"%d",fg_pid);
                    strncpy(delimitedCmd[i],fg_pid_string,100);
                    substitution++;
                } else {
                    if ((varSubstitution(delimitedCmd[i], variables)) == 1) {
                        if (dval) {
                            printf("Variable %s doesn't exist\n",delimitedCmd[i]);
                        }
                    } else {
                        substitution++;
                    }
                }
            }
        }
        
        
   
        
        //store the command into the file named history
        if (cmdHistory.is_open()){
            cmdHistory << cmdIndex << ": " << cmdLine <<endl;
        }
        else{
            cout << "Unable to open file" << endl;
        }
        
        for (int i = 0; cmdLine[i] != '\0'; i++){
            //count how many > in the cmd line
            if (cmdLine[i] == '>'){
                ++redirectNum;
                outputRedirect = true;
            }
            //count how many < in the cmd line
            else if (cmdLine[i] == '<'){
                ++redirectNum;
                inputRedirect = true;
            }
            //count how many pipes in the cmd line
            else if (cmdLine[i] == '|'){
                ++pipeNum;
            }
        }
        
        //we can't have i/o redirection and pipes at the same time
        if((inputRedirect || outputRedirect) && pipeNum > 0) {
            perror("Illegal input\n");
            exit(1);
        }else if (( inputRedirect || outputRedirect) && pipeNum == 0){
            //the command line only has i/o redirection
            cmdNum = redirectNum + 1;
        }else if((!inputRedirect && !outputRedirect) && pipeNum > 0){
            //the command line only has pipes
            cmdNum = pipeNum + 1;
        }else{
            //the command line does not have redirection or pipes
            cmdNum = 1;
        }
        
        //initialize the arrays for parsing arguments later
        //char parsedBasic[cmdNum]
        
        char parsedCmd[cmdNum][MAX_NAME];
        for(int i = 0; i < cmdNum; i++)
            parsedCmd[i][0] = '\0';
        char parsedFlag[cmdNum][MAX_NAME];
        for(int i = 0; i < cmdNum; i++)
            parsedFlag[i][0] = '\0';
        
        char parsedArg[cmdNum][MAX_ARG][MAX_NAME];
        for(int i = 0; i < cmdNum; i++)
           for(int j = 0; j < MAX_ARG; j++)
             for(int k = 0; k < MAX_NAME; k++)
                 parsedArg[i][j][k] = '\0';
        



        //parsing the command line without i/o redirection
        if(!inputRedirect && !outputRedirect) {
            parseCommand(cmdLine, cmdNum, parsedCmd, parsedFlag, parsedArg);
        }else if(inputRedirect || outputRedirect) {
            //parsing the command line with i/o redirection
            //parseIOCommand(cmdLine, cmdNum, parsedCmd, parsedFlag, parsedArg, inputRedirect, outputRedirect);
        }


        repeatCmd = "";
 //------------------------------------------------------------------               
        //implement the repeat command
        if(!strcmp("repeat", parsedCmd[0])) {
            ifstream readHistory("history");
            string line = "";
            int lineNum = 0;
            if (readHistory.is_open()){
                //n is not specified 
                if(numArgs == 1){
                    int totalLineNum = 0;
                    //get the total line number
                    while(getline(readHistory ,line)){
                        totalLineNum ++ ;
                    } 
                    readHistory.close();
                    //set the pointer to the beginning of the file
                    ifstream readHistory("history");
                    //find the line before the last line
                    for (int lineno = 1; getline (readHistory ,line); lineno++)
                    {
                        if(lineno == totalLineNum - 1){
                            repeatCmd = line.substr(3, line.length()-3);
                        }
                    }

                }else{
                    //n is specified
                    lineNum = atoi(parsedArg[0][0]);
                    for (int lineno = 1; getline (readHistory ,line) ; lineno++)
                    {
                        if(lineno == lineNum ){
                            repeatCmd = line.substr(3, line.length()-3);
                        }
                    }
                }
            }else{
                cout << "Unable to open file" <<endl;
            }
        }


        ///////* if use parsedCmd[0] and within it use delimitedCmd[1] *////////
        //implement simple "exit" command
        if (!strcmp("exit", parsedCmd[0])){
            break;
        }
        
        //implement simple "clr" command
        if (!strcmp("clr", parsedCmd[0])) {
            system("clear");
        }
        
        //implement simple "echo" command
        if (!strcmp("echo", parsedCmd[0])){
            cout << delimitedCmd[1] << endl;
        }

        //implement the "chdir" command
        if (!strcmp("chdir", parsedCmd[0])) {
            if (chdir(delimitedCmd[1]) == -1){
                    printf("Directory change error\n");
            }
        }
        
        //implement the "environ" command
        if (!strcmp("environ", parsedCmd[0])) {
            char **env;
            for (env = environ; *env != 0; env++) {
                printf("%s\n", *env);
            }
        }

        //implement the "export" command
        if (!strcmp("export", parsedCmd[0])) {
            setenv(delimitedCmd[1], delimitedCmd[2], 1);
        }
       
        //implement the "unexport" command
        if (!strcmp("unexport", parsedCmd[0])) {
            unsetenv(delimitedCmd[1]);
        }
        
        //implement the "set" command
        if(!strcmp("set", parsedCmd[0])) {
            int i = 0;
            int lastEmpty = 0;
            int exists = 0;
            for (i = 0; i < 100; ++i) {
                if (variables[i].empty) {
                    lastEmpty = i;
                } else {
                    //If already exists, replace value
                    if (!strcmp(variables[i].key,delimitedCmd[1])) {
                        strncpy(variables[i].value,delimitedCmd[2],100);
                        variables[i].empty = 0;
                        exists = 1;
                        break;
                    }
                }
            }
            //Else create new value at last available empty position
            if (!exists) {
                strncpy(variables[lastEmpty].key,delimitedCmd[1],100);
                strncpy(variables[lastEmpty].value,delimitedCmd[2],100);
                variables[lastEmpty].empty = 0;
            }
        }
        
        if (!strcmp("unset",parsedCmd[0])) {
            cout<<"unset"<<endl;
            int i = 0;
            for (i = 0; i < 100; ++i) {
                if (!strcmp(variables[i].key,parsedCmd[1])) {
                    variables[i].empty = 1;
                    break;
                }
            }
            if ((varSubstitution(delimitedCmd[i], variables)) == 1){
                cout<<"unset success"<<endl;
            }
        }
        
        if (!strcmp("show", parsedCmd[0])) {      
            for(int i=1; i<numArgs; i++) {
                printf("%s\n",delimitedCmd[i]);
            }
        }

        //implement the dir command
        if(!strcmp("dir", parsedCmd[0])) {
            char *curr_dir = NULL; 
            DIR *dp = NULL; 
            struct dirent *dptr = NULL; 
            unsigned int count = 0; 
          
            // Get the value of environment variable PWD 
            curr_dir = getenv("PWD"); 
            if(NULL == curr_dir) 
            { 
                printf("\n ERROR : Could not get the working directory\n"); 
                return -1; 
            } 
          
            // Open the current directory 
            dp = opendir((const char*)curr_dir); 
            if(NULL == dp) 
            { 
                printf("\n ERROR : Could not open the working directory\n"); 
                return -1; 
            } 

            // Go through and display all the names (files or folders) 
            // Contained in the directory. 
            for(count = 0; NULL != (dptr = readdir(dp)); count++) 
            { 
                if(strcmp(".", dptr->d_name) && strcmp("..", dptr->d_name))
                    printf("%s  ",dptr->d_name); 
            } 
            cout<<endl;
        }


        //implement the history command
        if(!strcmp("history", parsedCmd[0])) {
            ifstream readHistory("history");
            string line;
            int lineNum = 0;
            if (readHistory.is_open()){
                //parsedArg[0][0] whole name
                //parsedArg[0][0][0] the first char of name
                if(parsedArg[0][0][0] == '\0'){
                    //n is not specified
                    while (!readHistory.eof())
                    {
                        getline (readHistory,line);
                        cout << line << endl;
                    }
                }else{
                    //n is specified
                    lineNum = atoi(parsedArg[0][0]);
                    for (int lineno = 0; getline (readHistory ,line) && ! readHistory.eof(); lineno++)
                    {
                        if(lineno >= cmdIndex - lineNum){
                            cout << line << endl;
                        }
                    }
                    repeatCmd = "";
                }
                readHistory.close();
            }else{
                cout << "Unable to open file" <<endl;
            }
            continue;
        }
    
        
    }

    return 0;  
}

//return true if the input is a space or a tab, otherwise return false
bool isWhiteSpace(char inputChar){
    return (inputChar == ' ' || inputChar == '\t') ? true : false;
}

//Replaces the local variables of the shell
int varSubstitution(char *string, variable *variables) {
    int i = 0;
    for (i = 0; i < 100; ++i) {
        if(!variables[i].empty) {
            if(!strcmp(&string[1],variables[i].key)) {
                strncpy(string,variables[i].value ,5);
                return 0;
            }
        }
    }
    return 1;
}

//parsing the command line without i/o redirection
void parseCommand(char cmdLine[MAX_STRING], int cmdNum, char parsedCmd[][MAX_NAME], char parsedFlag[][MAX_NAME], char parsedArg[][MAX_ARG][MAX_NAME]){
    for(int i = 0, j = 0, k = 0 ; i < cmdNum; i++) {
        //actually cmdNum could be greater than 1?
        int argumentsRead = 0;
        while(isWhiteSpace(cmdLine[j]))	// Skipping white spaces.
            j++;
        
        while(!isWhiteSpace(cmdLine[j]) && cmdLine[j] != '|' && cmdLine[j] != '\0') {	// Copying the command name.
            parsedCmd[i][k] = cmdLine[j];
            k++;
            j++;
        }
        
        parsedCmd[i][k] = '\0';			// Setting the null terminator.
        
        k = 0;
        while(cmdLine[j] != '|' && cmdLine[j] != '\0') {
            if(cmdLine[j] == '-') {	// It is a flag
                while(!isWhiteSpace(cmdLine[j]) && cmdLine[j] != '|' && cmdLine[j] != '\0') {	// Copying the flags.
                    parsedFlag[i][k] = cmdLine[j];
                    k++;
                    j++;
                }
                parsedFlag[i][k] = '\0';	// Setting the null terminator.
            }
            else if(!isWhiteSpace(cmdLine[j]) && cmdLine[j] != '|' && cmdLine[j] != '\0') {		// It is an argument
                k = 0;
                while(!isWhiteSpace(cmdLine[j]) && cmdLine[j] != '|' && cmdLine[j] != '\0') {	// Copying the argument.
                    parsedArg[i][argumentsRead][k] = cmdLine[j];
                    k++;
                    j++;
                }
                parsedArg[i][argumentsRead][k] = '\0';		// Setting the null terminator.
                
                argumentsRead++;
            }   
            if(cmdLine[j] != '\0')
                j++;
        }
        
        if(cmdLine[j] != '\0')
            j++;
        
        k = 0;
        argumentsRead = 0;
    }
}
