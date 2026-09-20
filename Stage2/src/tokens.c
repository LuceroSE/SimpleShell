/*--------------------Libraries needed-----------------------*/
#include "tokens.h"    
#include <stdlib.h>
#include <string.h>

/*create_tokens function takes a copy of the input line and a 
pointer to an integer where we're gonna store the number of 
tokens created. 
It creates a pointer to pointer variable called tokens.
It uses strtok to split the input line by tabs 
and spaces. It then starts a loop where it reallocates memory as 
it finds more tokens. 
In each iteration, it sets the index of the pointer to pointer token array, 
to the pointer word, and increases the number of tokens. 
It also calls strtok repetitively, so it can get all the tokens. 
As soon as strtok returns a null, the loop stops 
and it reallocates memory one more time for one to add one more item to the array which is 
null.*/
char** create_tokens(char* line_cpy, int* num_tokens)
{

    char** tokens = NULL;
    int n_tokens = 0;

    //read the 1st token
    char* word = strtok(line_cpy, " \t");  //splitting on spaces and tabs
    while(word)
    {
        char** tmp = realloc(tokens, sizeof(char*) * (n_tokens + 1));   //(n_tokens + 1) is the amount of memory we need allocated 
        
        if(tmp == NULL)
        {
            free(tokens);
            return NULL;  //not enough memory for reallocation
        }

        tokens = tmp;
        tokens[n_tokens] = word;
        n_tokens++; //increasing the number of tokens

        word = strtok(NULL, " \t");
    }

    //adding last element which is null to reseamble argv behaviour
    char** tmp = realloc(tokens, sizeof(char*) * (n_tokens + 1));
    if(!tmp)
    {
        free(tokens);
        return NULL;
    }

    tokens = tmp;
    tokens[n_tokens] = NULL;
    *num_tokens = n_tokens;  //we do not need to add 1 to n_tokens because null does not count as an element of the array

    return tokens;
}

/*
-----------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952

I declare that this assignment is my own work and 
that I have adhered to DCU Academic Integrity Policy.
-----------------------------------------------
*/