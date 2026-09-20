/*--------------------Libraries needed-----------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "paths.h"

/*Static pointer that holds variables related to the shell state. 
These are the input and where is it coming from, this can be used by 
all the functions in the file because it is static*/
static ShellState* gshell_state = NULL;    

/*set_shell_state is a setter fuction that sets the shell state to the 
the static ShellState pointer defined above*/
void set_shell_state(ShellState* curr_state)
{
    gshell_state = curr_state;
}

/*get_shell_state is a getter function that returns 
a pointer to the state of the shell that other functions 
can use. This improves encapsulation*/
ShellState* get_shell_state()
{
    return gshell_state;
}

/*show_prompt function uses the get_working_dir function defined in path.c 
If get_working_dir function gave back an actual pointer to the current 
working directory, it sets the prompt to contain the current working directory. 
If it didn't return a valid pointer to the current working directory path, 
prompt just defaults to shell*/
void show_prompt() 
{
    char* curr_path = get_working_dir();
    if(curr_path != NULL)
    {
        printf("[%s] shell> ", curr_path);
        fflush(stdout);
        free(curr_path);
    }
    else
    {
        printf("shell> ");
        fflush(stdout); //Flushing, so standard output, meaning our print statement, can get be send from the buffer to the terminal immediately and we can print our prompt before any command or error
    }

}


/*Function get read line checks if the pointer for in_data got a null value, 
this could happen if the fopen() function in main was unable to open the 
file (in batch mode). It then signals that and returns null, which is handled in main. 
After this check it initialises a pointer called line, it sets its length to 0 and declares a variable called
char_read for the number of char read. 

We then use the function getline that dynamically allocates memory for line. 
getline saves the length of how much it allocated in the length variable, 
and it reads from the gshell_state->in_data, which could be a standard input or batch. 
If it didn't write properly, it returns -1, frees the memory and returns null. 
If it read properly, it then changes the '\n' newline character to '\0' to signal the end of the line*/
char* read_line()
{
    ShellState *st = get_shell_state();
    if(st == NULL || st->in_data == NULL)
    {
        return NULL;
    }

    char* line = NULL;     //pointer that will hold our dynamic memory 
    size_t length = 0;     //size variable that the line (command) has 
    ssize_t char_read;     //the number of the chars read, we use this to check later

    //getline function waits for input,  getline will block the loop until it receives input
    char_read = getline(&line, &length, gshell_state->in_data);   //Get acommodes to the amount of characters given in the input by default and allocates memory and saves it in line, returns -1 if errors when reading, it stops reading at the newline character

    if(char_read == -1)             //for CTRL-D that interrupts input stream
    {
        free(line);                              //  User might have input a few characters, but then pressed something in the keyboard like Ctrl-D that stopped the input stream and since they didn't press enter, there is an end-of-file error. And thus, some memory was allocated for a line that we need to free.    
        return NULL;                             // Return null bc there has be an error when reading
    }

    if(char_read > 0 && line[char_read - 1] == '\n') // -1 bc cd is saved as ['c','d','\n', '\0']
    {
        line[char_read - 1] = '\0';
    }

    return line;   //return input command
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/