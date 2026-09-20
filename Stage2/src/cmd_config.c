/*--------------------Libraries needed-----------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cmd_config.h"
#include "tokens.h"


/*Function get_command_from_tokens uses the tokens array produced by the function create_tokens.
This array contains the full tokenised input. get_command_from_tokens filters any input or output redirection symbol 
and stores the results in the cmd_config struct. It iterates over the tokens and if it finds a 
redirection symbol, it checks whether there is a file name next to it. 
If there is one, then it stores the file name in the cmd_config struct.
If there is not, then it displays a corresponding message. 

It then skips the file name in the array and keeps looping until it finds another redirection symbol 
or reaches the end of the array. 
Any token in the tokens array that is not a redirection symbol and is not next to one is considered an argument or command name. 
Therefore, it is stored as an argument in the cmd_config struct */

int get_command_from_tokens(char** tokens, int back_flag, CommandConfig* cmd_config)
{
    //The function only changes the values of the variables in the configuration struct if
    //there is redirection, if there is not, then we assign default values to avoid holding garbage values
    cmd_config->args = NULL;       
    cmd_config->in_file = NULL;     
    cmd_config->out_file = NULL;    
    cmd_config->trunc_appn = 0;    
    cmd_config->fore_back = back_flag;
    
    int n_valid_t = 0; //number of valid tokens
    for(int i = 0; tokens[i] != NULL; i++)
    {
        //checking output redirection
        if(strcmp(tokens[i], ">") == 0)
        {
            //if no file name after >, return
            if(tokens[i+1] == NULL)
            {
                fprintf(stderr, "Redirection error missing output file after >\n");
                return 1;
            }

            //file name is after >
            cmd_config->out_file = tokens[i+1];
            cmd_config->trunc_appn = 0; //0 for truncate/create (>)
            i++; //skip file name (so we don't add it in the arguments array)
            
            //if we matched > then we know that the next argument is the file name which is already recorded
            //so we can jump to the next iteration to look for other matches
            continue;
        }

        //checking append output redirection
        if(strcmp(tokens[i], ">>") == 0)
        {
            //if no file name after >>, return
            if(tokens[i+1] == NULL)
            {
                fprintf(stderr, "Redirection error missing output file after >\n");
                return 1;
            }
            //file name is after >>
            cmd_config->out_file = tokens[i+1];
            cmd_config->trunc_appn = 1; //1 for append/create (>>)
            i++; //skip file name (so we don't add it in the arguments array)
            
            //if we matched >> then we know that the next argument is the file name which is already recorded
            //so we can jump to the next iteration to look for other matches
            continue;           
        }

        //checking input redirection
        if(strcmp(tokens[i], "<") == 0)
        {
            //if no file name after <, return
            if(tokens[i+1] == NULL)
            {
                fprintf(stderr, "Redirection error missing input file after <\n");
                return 1;
            }
            //file name is after <
            cmd_config->in_file = tokens[i+1];
            i++; //skip file name (so we don't add it in the arguments array)
            
            //if we matched < then we know that the next argument is the file name which is already recorded
            //so we can jump to the next iteration to look for other matches
            continue;
        }

        //iteration only gets to this piece of code if none of the above if statements match
        //meaning the current token, tokens[i], is not >, >>, or < and is therefore a regular argument
        //it must be part of the args array
        char** tmp = realloc(cmd_config->args, sizeof(char*) * (n_valid_t + 1));
        if(tmp == NULL)
        {
            free(cmd_config->args);
            cmd_config->args = NULL;
            return 1;  //not enough memory for reallocation
        }
        cmd_config->args = tmp;
        cmd_config->args[n_valid_t] = tokens[i];
        n_valid_t++;
    }

    //making the args array end with NULL to simulate argv and tokens arrays
    char** tmp = realloc(cmd_config->args, sizeof(char*) * (n_valid_t + 1));
    if(tmp == NULL)
    {
        free(cmd_config->args);
        cmd_config->args = NULL;
        return 1;  //not enough memory for reallocation
    }
    cmd_config->args = tmp;
    cmd_config->args[n_valid_t] = NULL; 
    
    
    //if in the loop above there was no argument that wasn't < or > or >>
    //which can happen if the user only types "> file" or "< file" or ">> file", or just ">"
    //then cmd_config->args is NULL and we are allocating memory for cmd_config->args
    //to hold one value, which is NULL
    if (cmd_config->args[0] == NULL) 
    {
        fprintf(stderr, "Error no command specified\n");
        free(cmd_config->args);
        cmd_config->args = NULL;
        return 1;  
    }
    return 0;
}

//free_cmd_configd function frees the arguments in the args array that is stored cmd_config.
void free_cmd_config(CommandConfig* cmd_config)
{
    free(cmd_config->args);
    cmd_config->args = NULL;
}

void print_command(CommandConfig* out_config)
{
    printf("printing the arguments\n");
    if(out_config->args)
    {
        for(int i = 0; out_config->args[i] != NULL; i++)
        {
            printf("Arg[%d] is %s\n", i, out_config->args[i]);
        }
    }

    printf("infile is: %s\n", out_config->in_file);
    printf("outfile is: %s\n", out_config->out_file);
    printf("trunc (0) or not? %d\n", out_config->trunc_appn);
    printf("fore (0) or back? %d\n", out_config->fore_back);
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/
