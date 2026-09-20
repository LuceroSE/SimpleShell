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
    curr_shell.batch_stdin = 1;   //batch_stdin is 1 for stdin and 0 for batch mode
    

    //Checking is we have argv arguments (meaning if we are in batch mode)
    if(argc == 2)
    {
        //Running on batch mode, so open file given in command line arguments
        FILE* file = fopen(argv[1], "r");
        if(file == NULL)
        {
            perror("batch mode");
            exit(1);
        }
        curr_shell.in_data = file;   //Batch mode, passing input from batch file
        curr_shell.batch_stdin = 0;  //Updating to 0 for batch mode
    }

    /*Setting and getting the shell state (standard input ot batch mode) in the struct 
    to use this for processing in the while loop. These are set in this way to ensure
    encapsulation*/
    set_shell_state(&curr_shell);
    ShellState *st = get_shell_state();

    
    while(1)
    {
        //Getting the shell state (batch mode or standard input) to determine if shell promp should be displayed
        if(st->batch_stdin == 1)
        {
            show_prompt();
        }

        //Reading the commands from input (batch or standard input)
        char* line_orig = read_line();
        

        /*Checking if there was an interruption in the input stream or 
        if show_prompt function failed (could fail through built-in 
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
        if(tokens == NULL)                     //if realloc in create_tokens fails to reallocate memory, exit graceefully and free memory
        {
            free(line_cpy);
            free(tokens);
            free(line_orig);
            exit(1);    
        }
        //If user is only pressing enter (tokenise at index 0 is set to null by the create_tokens function)
        if(tokens[0] == NULL)
        {
            /*if user only pressed enter, we jump to the next iteration because there is nothing to process*/
            free(line_cpy);
            free(tokens);
            free(line_orig);
            continue;
        }

        //Calling function in charge of corresponding command (this is a mapping defined in commands.c)
        int result = command_caller(tokens);
        //If result of command_caller is -2 then command is not defined in our commands, so call system to execute it
        if(result == -2)                    
        {
            run_external(line_orig);
        }
        //If result is -1 then there was an error in the corresponding command fuction, exit gracefully 
        else if(result == -1)
        {
            free(line_orig);
            free(line_cpy);
            free(tokens);
            exit(0);
        }

        //before the next iteration we free memory to avoid memory leaks
        free(tokens);
        free(line_cpy);
        free(line_orig);
    }
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/