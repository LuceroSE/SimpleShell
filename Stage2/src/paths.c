/*--------------------Libraries needed-----------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "paths.h"

/*get_working_dir function get working directory uses built-in function 
getcwd that dynamically allocates memory because we are 
passing NULL (using malloc) and the size of the buffer as 0.
If the getcwd fails it returns null. Otherwise it returns 
a pointer to the current working directory*/
char* get_working_dir()
{
    //get the current working directory
    char* curr_path = getcwd(NULL, 0);  //dynamically allocate memory for curr_path based on the length of the path, it returns NULL if memory allocation failed so we check that MUST FREE THIS MEMORY LATER REMEMBER
    if (curr_path == NULL) {
        perror("getcwd");
        return NULL;
    }
    return curr_path;
}

/*get_shell_path function uses function realpath that 
converts the symbolic path /proc/self/exe into its fully 
resolved form. Path /proc/self/exe is a sybolic path that
points to the executable file of the current process.
If realpath failed then it returns null. Otherwise it returns
a pointer to the path. */
char* get_shell_path()
{
    char* path = realpath("/proc/self/exe", NULL);
    if(!path)
    {
        perror("realpath");
        return NULL;
    }
    return path;
}

/*get_manual_path function uses getenv to get the value of the 
shell environment variable that we set in main. 
This environment variable has the path to the executable file of the shell, 
get_manual_path function uses this path to built a full path to the manual readme.txt.
It uses function strrchr to locate where the last slash in the path is. 
It then uses pointer arithmetic to find out the length of the path up until 
the last slash. 
It sets an array with the relative path to the location of the manual. 
It then adds the length of the path up until the last slash plus 
the length of the path where the manual is + 1 (to hold '\0'), 
It dynamically allocates memory big enough to hold the 2 paths together. 
It uses memcpy to copy everything from the shell environment path up until the last slash, which has the
length_p.
Lastly it concatenates to that string the relative path of the manual and returns that entire path to the caller*/
char* get_manual_path()
{
    //get the path to the shell executable
    char* shell_exe = getenv("shell");
    if(shell_exe == NULL)
    {
        return NULL;
    }
    //find the pointer to the last / in the path
    char* last_slash = strrchr(shell_exe, '/');
    if(last_slash == NULL)
    {
        return NULL;
    }
    //use pointer arithmetic to the the length of the path to the last /
    //path should look like "/something1/something2/Stage1/bin/simpleshell"
    //but after finding the last slash we get "/something1/something2/Stage1/bin"
    size_t length_p = (size_t)(last_slash - shell_exe);

    //relative path to the manual
    char manual_p[] = "/../manual/readme.txt";

    //get the length of "/something1/something2/Stage1/bin" and "/../manual/readme.txt" + '\0'
    size_t total = length_p + strlen(manual_p) + 1;
    //allocate memory of that size, using calloc array gets 0 value in all indices by default (avoid having to add '\0' manually)
    char* complete_p = calloc(total, sizeof(char));
    if(complete_p == NULL)
    {
        return NULL;
    }
    //copy "/something1/something2/Stage1/bin" into complete_p
    memcpy(complete_p, shell_exe, length_p);
    //concatenate "/something1/something2/Stage1/bin" + "/../manual/readme.txt"
    strcat(complete_p, manual_p);
    return  complete_p;
}


/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/