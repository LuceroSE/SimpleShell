/*--------------------Libraries needed-----------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "paths.h"
#include "commands.h"
#include "input.h"

/*--------------------------------Function prototyping--------------------------------*/
/* Making all command function static becuase they will only be executed through the command_caller function*/
static int command_clear(char** tokens);
static int command_dir(char** tokens);
static int command_environ(char** tokens);
static int command_quit(char** tokens);
static int command_cd(char** tokens);
static int command_echo(char** tokens);
static int command_pause(char** tokens);
static int command_help(char** tokens);
int command_caller(char** tokens);

typedef struct CommandFunction CommandFunction;

/*--------------------------------Struct definition--------------------------------*/
struct CommandFunction
{
    char* command_name;                             //name of the command typed
    int (* command_func)(char** tokens);            //the pointer to the function that will handle it 
};

/*making the array of commands available in the entire programme with static*/
static CommandFunction mapping[] = {{"clr", command_clear}, 
                                 {"clear", command_clear},
                                 {"dir", command_dir},
                                 {"environ", command_environ},
                                 {"quit", command_quit},
                                 {"q", command_quit},
                                 {"cd", command_cd},
                                 {"echo", command_echo},
                                 {"pause", command_pause},
                                 {"help", command_help}}; 

extern char** environ;                               //defining access to system environment variables

/*Function in charge to compare command types in input with the coammand names in
the mapping array. When a match is found, it calls the corresponding function*/
int command_caller(char** tokens)
{
    size_t n_cmd = sizeof(mapping) / sizeof(mapping[0]);           //get the size of the command function mapping array
    for(size_t i = 0; i < n_cmd; i++)                              
    {
        if(strcmp(tokens[0], mapping[i].command_name) == 0)
        {
            return mapping[i].command_func(tokens);   //Each command function can return 0(sucessful func exec), 1(error in func execution), -1 (quit shell)
        }
    }
    return -2;  //fucntion returns -2 when we know we havent found the token in the mapping array (has to executed by system)
}

//clear command function
static int command_clear(char** tokens)
{
    system("clear");
    return 0;
}

/*The function is charge of running ls -al [directory] with the given directory as the first argument.
It creates a buffer arrau that is supposed to hold the command and the path of the directory given. 
If no path was given, then path defaults to current working directory. It then puts the command and the path 
into the buffer with snprintf(). If function snprintf failed, we return 1 to  signal this failure. 
If the size of the buffer is not big enough to hold the path, it outputs that as an standard error and 
return 1 to signal failure. At the end, it calls system on the command given.*/
static int command_dir(char** tokens)
{
    char buffer[1024];
    size_t command_size = sizeof(buffer);
    char* path = tokens[1];

    if(!path)
    {
        path = ".";
    }
    
    int size_copied = snprintf(buffer, command_size, "ls -al %s", path);
    if(size_copied < 0)                 //error when putting into buffer
    {
        return 1;                       //return 1 for failure
    }
    if((size_t)size_copied >= command_size) //changing the type of size_copied so we can compare between 2 values of the same type
    {
        fprintf(stderr, "dir: path too long\n");
        return 1;                       //return 1 for failure
    }
    
    system(buffer);
    return 0;                           //return 0 for success
}

/*Command environment uses the environ array to get access to the 
environment variables and prints them one by one with a new line character.*/
static int command_environ(char** tokens)
{
    for(int i = 0; environ[i] != NULL; i++)
    {
        printf("%s\n", environ[i]);
    }
    return 0;
}
/*Function with input returns -1 to 
let the main function exit the loop*/
static int command_quit(char** tokens)
{
    return -1; //return -1 to signal quitting
}

/* The cd command function first checks if the argument of the command is NULL
If it is, it gets the current working directory and prints it using the function 
get_working_dir (defined in paths.c). Otherwise, it retrieves the argument of cd

If the argument is a tilde (~), it gets the HOME environment variable as the target directory
It then attempts to change the directory to that target directory
If changing the directory fails, it outputs a message indicating the error

After successfully changing the directory, it gets the current working directory again using get_working_dir 
and sets the environment variable PWD to this new working directory.
If setting PWD returns -1, it outputs a message and exits the function, signaling an error.
Otherwise, the function finishes gracefully and frees the memory allocated by get_working_dir.
Note that get_working_dir uses getcwd(), which can fail.
If it fails, the directory change might have already occurred. In such cases, because of how the show_prompt() 
function is defined, the prompt will default to "shell>" in the new directory. */

static int command_cd(char** tokens)
{
    if(tokens[1] == NULL)  //no argument for cd default to print current directory
    {
        char* curr_dir = get_working_dir();
        if(curr_dir == NULL)
        {
            fprintf(stderr, "cd: could not determine current directory\n");
            return 1;
        }

        printf("%s\n", curr_dir);
        free(curr_dir);
        return 0;
    }

    char* target_dir = tokens[1];           //get teh argument for cd

    if(strcmp(tokens[1], "~") == 0)     
    {
        target_dir = getenv("HOME");
    }

    
    int result = chdir(target_dir);


    if(result == -1)
    {
        perror("Error in directory change");
        return 1;
    }

    char* new_dir = get_working_dir();
    /*if getcwd fails after chdir succeeded, shell is still in the new directory 
    but it coulndt get the path string so prompt will fall back to shell> 
    since show_prompt is defined that way.*/
    if(new_dir == NULL)
    {
        return 1;
    }

    //If an error occured when setting the PWD variable return 1
    int n = setenv("PWD", new_dir, 1);
    if(n == -1)
    {
        perror("Error in directory change");
        free(new_dir);
        return 1;
    }
    //if here, everthing went well and free new_dir allocated by get_working_dir
    free(new_dir);
    return 0;
}

/*Command echo checks if there is an argument for echo. 
If there is an argument for echo, it prints the argument 
and then starts a loop starting from index 2 and prints 
the rest of the arguments with a space in front. When the 
loop finishes, it prints a new line character. If echo doesn't 
have any argument, it just prints a new line character.*/
static int command_echo(char** tokens)
{
    /*If there is token 1, we print it and start the loop from token 2 
    because it's known token 1 exists. So as long as the index is not null, 
    starting from token 2 because token 2 is the next one, keep printing. 
    If token 2 is null, the for loop will never start.*/
    if(tokens[1] != NULL)
    {
        printf("%s", tokens[1]);
        for(int i = 2; tokens[i] != NULL; i++)
        {
            printf(" %s", tokens[i]);
        }
    }
    printf("\n");
    return 0;
}

/*Command pause gets the current state of the shell and checks if it's a 
standard input or batch mode. If it's batch mode, it outputs a corresponding 
message saying that the command is disabled for batch mode. If it's not batch 
mode, it then outputs a message letting the user know they have to press enter 
to continue. It starts a loop with getchar that reads a single character until it 
finds a newline character.*/
static int command_pause(char** tokens)
{
    ShellState* curr_state = get_shell_state();
    if(curr_state && curr_state->batch_stdin == 0)    //if we are getting imput from batch we dont pause the input stream (batch is supposed to not be interactive)
    {
        fprintf(stderr, "pause: command disabled in batch mode\n");
        return 0;
    }
    
    printf("Press Enter to continue.....\n");

    int c = getchar();   //getchar returns int of the int value of the character read or the int value of EOF (-1)
    while(c != '\n' && c != EOF)
    {
        c = getchar();
    }
    return 0;
}


/*Command help gets the path to the manual with the function 
get_manual_path defined in path.c. If the path pointer returned was a 
null pointer, it prints a statement signaling that and returns 
1 (error). 
Otherwise, it creates a buffer and it stores  the command with the path 
into this buffer. It checks the shell state to whether run less for batch mode or
more for stdin.
It then passes this buffer to system  and frees the path that was allocated in the 
get_manual_path function.*/
static int command_help(char** tokens)
{
    //get path to manual
    char* manual_path = get_manual_path();
    if(manual_path == NULL)
    {
        fprintf(stderr, "help: cannot locate manual\n");
        return 1;
    }
    //buffer
    char buffer[1024];
    size_t command_size = sizeof(buffer);

    //check shell state
    ShellState* curr_state = get_shell_state();
    //if batch use less, if stdin use more
    if(curr_state && curr_state->batch_stdin == 0) 
    {
        int size_copied = snprintf(buffer, command_size, "less %s", manual_path);
        if(size_copied < 0)                 //error when putting into buffer
        {
            free(manual_path);
            return 1;                       //return 1 for failure
        }
        if((size_t)size_copied >= command_size) //changing the type of size_copied so we can compare between 2 values of the same type
        {
            fputs("Errors in getting path to manual\n", stderr);
            free(manual_path);
            return 1;                       //return 1 for failure
        }

    }

    else
    {
        int size_copied = snprintf(buffer, command_size, "more %s", manual_path);
        if(size_copied < 0)                 //error when putting into buffer
        {
            free(manual_path);
            return 1;                       //return 1 for failure
        }
        if((size_t)size_copied >= command_size) //changing the type of size_copied so we can compare between 2 values of the same type
        {
            fputs("Errors in getting path to manual\n", stderr);
            free(manual_path);
            return 1;                       //return 1 for failure
        }
    }
    
    system(buffer);
    free(manual_path);
    return 0;
}

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/