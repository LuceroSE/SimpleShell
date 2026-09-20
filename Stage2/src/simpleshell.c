/*--------------------Libraries needed-----------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "paths.h"
#include "tokens.h"
#include "commands.h"
#include "input.h"
#include "run.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "cmd_config.h"
#include "builtin_cmd_redirect.h"


/*------------------------Main function-----------------------*/
int main(int argc, char** argv)
{
    /*Creating an environment variable called shell that holds the full path to the shell executable*/
    char* path = get_shell_path();               
    if(path != NULL)
    {
        int result = setenv("shell", path, 1);   //Adding new environment variable
        if(result == -1)
        {
            perror("setenv");
        }
        free(path);
    }
    
    //Setting default input stream for the shell state to be stdin
    ShellState curr_shell;
    curr_shell.in_data = stdin;   //Passing the file pointer, file is stdin
    curr_shell.batch_stdin = 1;   //batch_stdin is 1 when using stdin and 0 when using batch mode
    

    //Checking if argc has an extra argument, which means we are in batch mode
    if(argc == 2)
    {
        //Running in batch mode, so open file given in command line arguments
        FILE* file = fopen(argv[1], "r");
        if(file == NULL)
        {
            perror("batch mode");
            exit(1);
        }
        curr_shell.in_data = file;   //Batch mode, passing input from batch file
        curr_shell.batch_stdin = 0;  //Updating to 0 for batch mode
    }

    /*Setting the shell state (stdin or batch mode) in the struct so it can be used in the while loop. 
    This is done to preserve encapsulation.*/
    set_shell_state(&curr_shell);
    ShellState *st = get_shell_state();

    
    while(1)
    {
        /*This while loop below gets rid of any zombie processes (which appear when processes are running in the background). 
        However, because of the read_line function, the loop waits for input and zombie cleanup
        only happens in the next iteration. 
        
        Therefore, if there is a zombie process and ps -l is run, this loop will wait 
        for read_line and won't collect the exit status of this zombie process until the next 
        iteration, which is why the first time ps -l is run, it shows a zombie process and the second time ps -l is run, 
        it doesn't, since the cleanup has already run. 
        
        In a nutshell, for a short period of time, there is a zombie process that is then collected in the next 
        iteration before the shell prompt appears*/
        
        int status = 0;
        while(waitpid(-1, &status, WNOHANG) > 0)
        {
            //Collect the exit status of the zombie child processes
            //-1 means that we are waiting for the exit status of any child process
            //WNOHANG means that we do not block so the shell stays responsive

            /*It's worth noting that this only runs after the waitpid for foreground execution.

            In foreground execution the parent waits for the forked process to 
            finish with waitpid and then execution comes back to this main loop, so the exit status of foreground 
            child processes are collected before the main shell loop iterates. Thus, the loop’s waitpid(-1, WNOHANG) 
            only runs afterwards, meaning it only collects background children (that later become zombie processes)*/
        }

        //Getting the shell state and checking if we are in batch mode or standard input to determine if shell prompt should be displayed
        if(st->batch_stdin == 1)
        {
            show_prompt();
        }

        //Reading the commands from input (batch or standard input)
        char* line_orig = read_line();
        

        /*Checking if there was an interruption in the input stream or 
        if the show_prompt function failed (could fail through the built-in 
        function getline) and exiting gracefully*/
        if(line_orig == NULL)
        {
            exit(0);
        }

        //Creating a copy of the input line to tokenise (so original line doesn't get affected)
        char* line_cpy = strdup(line_orig);
        if(line_cpy == NULL)                   //if built-in function strdup fails we exit gracefully
        {
            free(line_orig);
            exit(1);
        }

        //Tokenising the copy of the input line for further processing
        int num_tokens = 0;                    //getting the number of tokens
        char** tokens = create_tokens(line_cpy, &num_tokens);
        if(tokens == NULL)                     //if realloc in create_tokens fails to reallocate memory, exit gracefully and free memory
        {
            free(line_cpy);
            free(tokens);
            free(line_orig);
            exit(1);    
        }

        /*We are placing this background and foreground check before the next if statement
        "if(tokens[0] == NULL)" because if we put it after it could cause a segmentation error
        if the user only inputs &. 
        
        This is because we will replace that & in the array by NULL in 
        "tokens[num_tokens - 1] = NULL;" this will create an array that only holds NULL causing all the
        internal commands called by command_caller function to basically fail leading to a segmentation error since
        all the functions in commands.c assume that the tokens array has at least one pointer that is not NULL.

        To avoid this, the logic for checking foreground and background execution is placed before the "if(tokens[0] == NULL)" statement
        because in this sense, the shell loops normally, and there is no need to add a specific handler for when the user only inputs &.
        This is done to match real shell behaviour in the edge case where only & is entered*/

        //Checking background or foreground execution, if we are running in the background then last token should be &, 
        int back_flag = 0; //0 represents foreground execution and 1 represents background execution. Foreground is default
        if(num_tokens > 0 && tokens[num_tokens - 1] && strcmp(tokens[num_tokens - 1], "&") == 0) // checking num_tokens > 0, this statement doesn't succeed if user only pressed enter
        {
            tokens[num_tokens - 1] = NULL;  //replacing & by NULL (excluding it from tokens array)
            back_flag = 1; //It is background execution
            num_tokens--;
        }

        //If user only presses enter (token at index 0 is set to NULL by the create_tokens function)
        if(tokens[0] == NULL)
        {
            /*if user only pressed enter, we continue to the next iteration because there is nothing to process*/
            free(line_cpy);
            free(tokens);
            free(line_orig);
            continue;
        }
        

        //Creating a variable called cmd_config from the  CommandConfig struct, that will store all the configurations related to the commands
        //such as cleaned arguments without redirection symbols, I/O file names, truncation or appending to file flags and flags for foreground and 
        //background execution 
        CommandConfig cmd_config;
        //calling the get_command_from_tokens function that sets the command configurations,
        if(get_command_from_tokens(tokens, back_flag, &cmd_config) != 0) //this function is implemented in cmd_config.c
        {
            /*If the get_command_from_tokens function returns anything else different from
            0, we know that there was either an error with getting the input file, the output 
            file, memory reallocation issues or basically no command 
            given and just symbols were entered > < >>. So we skip processing and continue to the next iteration*/
            free(line_orig);
            free(line_cpy);
            free(tokens);
            continue;
        }
        

        //Calling wrapper function run_builtin_redirect with the command configuration struct as argument 
        //This function determines if the command is internal or external. 
        //If the command is internal and does not require redirection, it executes the function normally
        //If the command is internal and requires redirection, it changes the output destinations
        //of the current shell's process and calls the corresponding internal command, after the execution of the command
        //it restores the normal stdout stream.
        int result = run_builtin_redirect(&cmd_config);      //this function is implemented in builtin_cmd_redirect.c
        
        //If run_builtin_redirect returns -2, the command is not one of the internal commands, so call run_external to execute it
        if(result == -2)                    
        {
            run_external(&cmd_config);
        }

        //If result is -1 then there was an error in the corresponding command function, exit gracefully 
        else if(result == -1)
        {
            free(line_orig);
            free(line_cpy);
            free(tokens);
            free_cmd_config(&cmd_config);
            exit(0);
        }

        //Before the next iteration, we free memory to avoid memory leaks
        free(tokens);
        free(line_cpy);
        free(line_orig);
        free_cmd_config(&cmd_config); //free memory allocated for the command configuration
    }
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/
