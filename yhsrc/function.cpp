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

//define the largest length of a path
#define MAX_PATH 50

int cmdIndex = 0; /* global */
char *delimitedCmd[]; /* global - arguments for build-in commands */
extern char **environ;
extern  int alphasort(); //Inbuilt sorting function
char pathname[MAX_PATH];
string repeatCmd = ""; /* global */
ofstream cmdHistory("history");

/* function declaration */
int isBuildIn(char **);

int isBuildIn(int argc, char *delimitedCmd[]) {
        //store the command into the file named history
        if (cmdHistory.is_open()){
            cmdHistory << cmdIndex << ": " << cmdLine <<endl;
        }
        else{
            cout << "Unable to open file" << endl;
        }
        

        repeatCmd = "";
 //------------------------------------------------------------------               
        //implement the repeat command
        if(!strcmp("repeat", delimitedCmd[0])) {
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
                    lineNum = atoi(delimitedCmd[1]);
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


        ///////* if use delimitedCmd[0] and within it use delimitedCmd[1] *////////
        //implement simple "exit" command
        if (!strcmp("exit", delimitedCmd[0])){
            break;
        }
        
        //implement simple "clr" command
        if (!strcmp("clr", delimitedCmd[0])) {
            system("clear");
        }
        
        //implement simple "echo" command
        if (!strcmp("echo", delimitedCmd[0])){
            for (int i=1; i<argc; ++i){
                printf("%s ", delimitedCmd[i]);
            }
            printf("\n");
        }

        //implement the "chdir" command
        if (!strcmp("chdir", delimitedCmd[0])) {
            if(chdir(delimitedCmd[1] == 0)){
                setenv("PWD", delimitedCmd[1], 1);
                printf("directory changed to %s\n", delimitedCmd[1]);
            }else{
                perror(delimitedCmd[1]);
            }
        }
        
        //implement the "environ" command
        if (!strcmp("environ", delimitedCmd[0])) {
            char **env;
            for (env = environ; *env != 0; env++) {
                printf("%s\n", *env);
            }
        }

        //implement the "export" command
        if (!strcmp("export", delimitedCmd[0])) {
            setenv(delimitedCmd[1], delimitedCmd[2], 1);
        }
       
        //implement the "unexport" command
        if (!strcmp("unexport", delimitedCmd[0])) {
            unsetenv(delimitedCmd[1]);
        }

        if (!strcmp("show", delimitedCmd[0])) {      
            for(int i=1; i<numArgs; i++) {
                printf("%s\n",delimitedCmd[i]);
            }
        }


        if (!strcmp("help", delimitedCmd[0])) {      
            char *ptr1 = "more";
            char *ptr2 = "help";
            char *more[] = {ptr1, ptr2};
            execvp("more", more);
        }

        //implement the dir command
        if(!strcmp("dir", delimitedCmd[0])) {
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
        if(!strcmp("history", delimitedCmd[0])) {
            ifstream readHistory("history");
            string line;
            int lineNum = 0;
            if (readHistory.is_open()){
                //parsedArg[0][0] whole name
                //parsedArg[0][0][0] the first char of name
                if(argc == 1){
                    //n is not specified
                    while (!readHistory.eof())
                    {
                        getline (readHistory,line);
                        cout << line << endl;
                    }
                }else{
                    //n is specified
                    lineNum = atoi(delimitedCmd[1]);
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
        }
    
        
    }

