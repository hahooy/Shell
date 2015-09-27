#include "shell.h"

// Execute piped commands
void cmdPiped(Program *command[],
              int numPipes,
              int dFlag) {
    int status;
    pid_t pid;
    // Create file descriptors according to 
    // number of pipes
    int pipefds[2*numPipes];
    // create all pipes up-front
    for(int i = 0; i < numPipes; i++){
        if(pipe(pipefds + i*2) < 0){
            printerr(dFlag,"Can not create pipe\n");
            exit(1);
        }
    }


    // Iterate through the argv, on each iteration, 
    // fork a new child process, set up its pipe 
    // according to its position
    // no parent process is involved in this while 
    // loop.  Because the parent process waits for 
    // all its children to finish, then can the parent 
    // process continue
    for( int k = 0, j = 0; command[k] != '\0'; k++, j+=2){
        pid = fork();//forking a new child on each loop
        if(pid == 0) {//child

            //if not last command
            if(command[k+1] != '\0'){
                // copy pipe write end to stdout
                if(dup2(pipefds[j + 1], 1) < 0){
                    printerr(dFlag, "dup2 error\n");
                    exit(1);
                }
            }

            //if not first command
            if(j != 0 ){
                // copy pipe read end to stdin
                if(dup2(pipefds[j-2], 0) < 0){
                    printerr(dFlag, "dup2 error\n");
                    exit(1);
                }
            }
            //close pipe file descriptors
            for(int i = 0; i < 2*numPipes; i++){
                close(pipefds[i]);
            }
            // execute command
            if( execvp(*command[k]->argv, command[k]->argv) < 0 ){
                printerr(dFlag, "execvp error\n");
                exit(1);
            }
        }// child done
        else if(pid < 0){ // fork error
            printerr(dFlag, "fork error\n");
            exit(1);
        }
    }//for done


    // only the parent get here and waits for 'numPipes' 
    // many children to finish
    //Parent closes the pipes
    for(int i = 0; i < 2 * numPipes; i++){
        close(pipefds[i]);
    }
    //Parent wait for children
    for(int i = 0; i < numPipes + 1; i++)
        wait(&status);
}

// Execute redirection commands
void cmdRedirection(Program* command,
                    int dFlag){
    // If output redirection
    if(command -> stdout_redirect){
        int fout = open(command -> outfile, O_WRONLY | O_CREAT, 0666);
        if(fout < 0){
            printerr(dFlag, "Can not open output file\n");
        }
        // copy fout to stdout
        dup2(fout, STDOUT_FILENO);
        close(fout);
    }
    // If input redirecction
    if(command -> stdin_redirect){
        int fin = open(command -> infile, O_RDONLY);
        if(fin < 0){
            printerr(dFlag, "Can not open input file\n");
        }
        // copy fin to stdin
        dup2(fin, STDIN_FILENO);
        close(fin);
    }
}
