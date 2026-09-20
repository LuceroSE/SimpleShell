/*--------------------Libraries needed-------------------*/
#include "run.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "cmd_config.h"
#include <fcntl.h> //file descriptions control header

/*--------------------------------Function prototyping--------------------------------*/
int apply_redirection(CommandConfig* cmd_config);



/*Function that runs external commands that are not implemented internally.
Here system() from stage 1 is replaced by fork() and execvp() that allow control 
of the child process enabling  redirection and background execution*/
int run_external(CommandConfig* cmd_config)
{
    pid_t pid;
    /*Fork a child process, from this point, 2 processes continue execution, each independently with the same information*/
    
    pid = fork(); //fork returns the PID of the newly created child in the parent process, in the child process, fork behaves differently and returns zero.
    
    if(pid < 0) //there was an error while forking
    {
        perror("Error while forking");
        return 1; //return 1 on failure
    }
    
    else if(pid == 0) // if the pid returned by fork is 0 we are in the child process
    {
        //get the path to the shell executable
        char* shell_exe = getenv("shell");
        if(shell_exe == NULL)
        {
            perror("Error setting parent variable");
            exit(1);  //exit the entire child process to avoid 2 shells running
        }
        
        //set parent environment variable
        int n = setenv("parent", shell_exe, 1);
        if(n == -1)
        {
            perror("Error setting parent variable");
            exit(1); //exit the entire child process to avoid 2 shells running
        }
        
        //checking I/O redirection. Note that becuase of how apply function is defined 
        //input or output streams are not change when we do no have the corresponding files
        int redir = apply_redirection(cmd_config);
        if(redir != 0)
        {
            //if redirection error occurs, exit child process 
            exit(1);
        }

        //execvp overwrites the current process but does not overwrite the process ID or the file descriptors
        execvp(cmd_config->args[0], cmd_config->args); 

        //If execvp succeeds, it never returns, this forked child process (that had everything copied from the parent, even simpleshell.c disappears) is completely replaced by the new program that execvp is calling
        //Therefore, we know that we are running this error below only because execvp returned, meaning that it did not succeed 
        fprintf(stderr, "%s: command not found\n", cmd_config->args[0]);
        //if execvp fails in the child process, exit the child process completely to avoid having 2 shells running at the same time 
        exit(1);

    }
    else //we are in the parent process
    {
        //Checking whether execution is in the foreground or background, 0 for foreground, 1 for background
        if(cmd_config->fore_back == 0)
        {
            //integer where the child process exit status is stored
            int status = 0;
            //waitpid returns the pid of the child process
            pid_t childpid = waitpid(pid, &status, 0);  //0 in options means block the parent to wait for this child to terminate
            //on failure waitpid returns -1
            if(childpid == -1)
            {
                perror("Error in waitpid"); 
            }
        }
    }
    return 0;  
}


/*apply_redirection is responsible for changing the input and output streams
 of the child process. If no input or output files were provided, no redirection 
is applied and the child uses regular stdin and stdout*/
int apply_redirection(CommandConfig* cmd_config)
{
    //if we have an input file name (I/O redirection, "<" was found in our cmd_config function), we change input file descriptor
    if(cmd_config->in_file != NULL)
    {
        //use open() system call to open a file and get its file descriptor, if it fails it returns -1
        //it takes a pointer to the file path and lets you specify the access options (open to read, open to write, create if it doesn't exist or append)
        //O_RDONLY means the file is opened in read-only mode
        int file_desc = open(cmd_config->in_file, O_RDONLY);
        if(file_desc == -1)
        {
            perror("open file input");
            return 1; //1 for error
        }

        //make standard input point to the opened file descriptor
        int result = dup2(file_desc, STDIN_FILENO); //take the file descriptor of the opened file, duplicate it and make standard input file descriptor (0) point to this duplicated file descriptor
        if(result == -1)
        {
            perror("dup2 input file error");
            //system call close to close our open file descriptor file_desc because there was an error with dup2 
            close(file_desc);
            return 1;
        }

        //standard input now refers to the opened file (because it now refers to the file we just opened), so close this copy of the just opened file
        close(file_desc);
    }

    //if we have a file name for output (I/O redirection, ">" or ">>" were found in our cmd_config function), we change output file descriptor
    if(cmd_config->out_file != NULL)
    {
        int flags;
        //note that the flags for the open system call are of integer type
        if(cmd_config->trunc_appn == 1)
        {
            //If trunc_appn is 1, we append to the file or create it if it does not exist
            //use bitwise OR to specify multiple flags
            flags = O_WRONLY | O_CREAT | O_APPEND;
        }
        else
        {
            //If trunc_appn is 0, we truncate the file or create it if it does not exist
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        }

        //use the open system call to open the file specified, with the defined flags and set file permissions so everyone has access to it (in octal)
        int file_desc = open(cmd_config->out_file, flags, 0777);
        if(file_desc == -1)
        {
            perror("open file output");
            return 1; //1 for error           
        }

        //make standard output point to the opened file descriptor 
        //take the file descriptor of the opened file, duplicate it and make standard output file descriptor (1) point to this duplicated file descriptor
        int result = dup2(file_desc, STDOUT_FILENO); 
        if(result == -1)
        {
            perror("dup2 output file error");
            //system call close to close our open file descriptor file_desc because there was an error with dup2 
            close(file_desc);
            return 1;
        }
        //standard output now refers to the opened file, so close this extra file descriptor
        close(file_desc);        
    }
    return 0;
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/
