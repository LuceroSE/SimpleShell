#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

typedef struct ShellState ShellState;       //introducing a shell state strcut that will signal where the shell input is coming, batch or terminal stdin

struct ShellState
{
    FILE* in_data;                 //file pointer that holds input data pointer "stdin" or batch
    int batch_stdin;               //value 1 is input from stdin or 0 if batch
};

void show_prompt();
char* read_line();
void set_shell_state(ShellState* curr_state);
ShellState* get_shell_state();

#endif


/*
-----------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952

I declare that this assignment is my own work and 
that I have adhered to DCU Academic Integrity Policy.
-----------------------------------------------
*/