#ifndef CMD_CONFIG_H
#define CMD_CONFIG_H


typedef struct CommandConfig CommandConfig;
//definition of struct in the .h file because the structure components have to be visible to 
//other files (simpleshell.c)
struct CommandConfig
{
    char** args;       //cleaned arguments after tokenasation (just pure arguments)
    char* in_file;     //pointer to input file I/O redirection
    char* out_file;    //pointer to output file I/O redirection
    int trunc_appn;    //0 for truncate/create (>), 1 for append/create (>>)
    int fore_back;     //0 for foreground, 1 for background
};

int get_command_from_tokens(char** tokens, int back_flag, CommandConfig* out_config);
void free_cmd_config(CommandConfig* out_config);  //to free new array allocated for args


//TESTING FUNCTION
void print_command(CommandConfig* out_config);
#endif