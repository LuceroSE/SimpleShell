/*--------------------Libraries needed-----------------------*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include "builtin_cmd_redirect.h"
#include "commands.h"

/*--------------------------------Function prototyping--------------------------------*/
int cmd_support_redirect(char* command);

/*Function run_builtin_redirect is a wrapper for internal commands. 
It first checks whether the parameter is an internal command. 
If it's not, then it returns -2. 
If it is an internal command, then it proceeds.
If no output file provided, then we run the command normally to standard output. 

Otherwise, it executes the output redirection logic. If the command does not support redirection, 
such as clear or pause, it displays a corresponding message to inform the user. 

Otherwise, it saves the current output stream (stdout) and sets the flags for truncation or append depending on the data 
stored in cmd_config->trunc_appn. 

It then executes the command normally to the specified output file
and restores the previous output stream afterwards.*/
int run_builtin_redirect(CommandConfig* cmd_config)
{
    //note that the internal commands that support redirection only need an output file
    //checking for edge cases to avoid segmentation errors
    if(cmd_config == NULL || cmd_config->args == NULL || cmd_config->args[0] == NULL) 
    {
        return 1; //1 for errors
    }

    //Checking whether the command is internal or not before even 
    //considering to check for I/O redirection. 
    if(is_internal_command(cmd_config->args[0]) == 0) // if not internal allow external execution
    {
        return -2;
    }

    if(cmd_config->in_file != NULL)
    {
        fprintf(stderr, "Input redirection not supported for internal commands\n");
        return 1;
    }

    //if no output file was given (no redirection), just run command normally (STDOUT)
    if(cmd_config->out_file == NULL)
    {
        return command_caller(cmd_config->args);
    }

    //if redirection was requested but the internal command does not support it, then output an error message
    if(cmd_support_redirect(cmd_config->args[0]) == 0)
    {
        fprintf(stderr, "%s: stdout redirection not supported for this internal command\n", cmd_config->args[0]);
        return 1;
    }

    //saving the current stdout so that we can restore the default stdout when we finish redirection
    //STDOUT_FILENO represents file descriptor number 1 (standard output)
    int save_stdout = dup(STDOUT_FILENO);
    if(save_stdout == -1)
    {
        perror("Error saving stdout file descriptor");
        return 1;
    }

    int flags;
    //note that the flags for the open system call are of type int
    if(cmd_config->trunc_appn == 1)
    {
        //If trunc_appn is 1, we append or create (if file doesn't exist)
        //bitwise OR to specify multiple flags
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    else
    {
        //if trunc_appn is 0, we truncate or create (if file doesn't exist)
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }

    //getting the file descriptor of the given file with the open system call
    int file_desc = open(cmd_config->out_file, flags, 0777);
    if(file_desc == -1)
    {
        perror("Error output file");
        close(save_stdout);
        return 1;
    }

    //make standard output point to the opened file descriptor
    //take the file descriptor of the opened file, duplicate it and make standard output file descriptor (1) point to this duplicated file descriptor
    int status = dup2(file_desc, STDOUT_FILENO); 
    if(status == -1)
    {
        perror("dup2 output file error");
        //use the close system call to close file_desc and save_stdout because there was an error with dup2 
        close(file_desc);
        close(save_stdout);
        return 1;
    }
    //we are using file descriptor 1 now for output (because it now refers to the file we just opened), close this copy of the just opened file
    close(file_desc);

    //running the command while stdout points to the given output file 
    int result = command_caller(cmd_config->args);
    
    //flush stdout before restoring it so that everything printed by the command functions (output buffer)
    //goes directly into the file before switching back to normal terminal output
    fflush(stdout);


    //after redirection, we go back to regular stdout (1)
    status = dup2(save_stdout, STDOUT_FILENO); 
    if(status == -1)
    {
        perror("restore stdout");
        //use system call close() to close our open file descriptor save_stdout because there was an error with dup2 
        close(save_stdout);
        return 1;
    }
    close(save_stdout);
    
    //return the result status of the command called while stdout pointed to the given file
    return result;
}


/*function cmd_support_redirect only checks whether the command 
provided supports redirection.*/
int cmd_support_redirect(char* command)
{
    char* support_redirect[] = {"dir", "environ", "echo", "help", NULL};
    int flag = 0; //flag 0, command does not support builtin redirection
    for(int i = 0; support_redirect[i] != NULL; i++)
    {
        if(strcmp(command, support_redirect[i]) == 0 )
        {
            flag = 1; //command is matched, it means it supports dierction so we change flag
            return flag;
        }
    }
    return flag;
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/