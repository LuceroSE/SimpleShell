USER MANUAL OF SIMPLE SHELL

DESCRIPTION:
    This is an implementation of a shell written in C. It supports basic
    commands as listed in Commands. It can be run in interactive
    or batch mode. The shell prompt displays the current working directory.

HOW TO COMPILE:
    To compile: make
        Description: make creates an executable file "simpleshell"
                     in the bin directory: bin/simpleshell

HOW TO RUN:
    1. Interactive mode: ./bin/simpleshell
        Description: This allows real-time interaction with the shell.
                     The prompt will wait for continuous input in the
                     current directory. Commands are executed by
                     pressing Enter.

    2. Batch mode: ./bin/simpleshell batchfile.txt
        Description: Commands are read from a file (note the file name
                     can be anything). There is no interactive prompt.
                     The "pause" command is disabled in this mode.
                     The shell exits when it reaches the end of file.

PROMPT EXPLANATION:
    Format: [current working directory] shell>
    Description: The path in brackets is the current working directory,
                 which updates when you change directories with
                 the command "cd".

INTERNAL COMMANDS:

    1. cd target
        Description: The cd command changes directory. If no target
                     directory is provided, it lists the current directory.
                     It updates the PWD environment variable and shows
                     an error statement if the directory does not exist.

        Usage examples:
                     - cd /home
                     - cd ..
                     - cd
                     - cd ~

    2. clear (clr)
        Description: The clear command clears the screen by removing
                     the display of previous commands.
        Usage examples:
                     - clr
                     - clear

    3. dir target
        Description: The dir command lists the contents of a target
                     directory. If no target directory is provided,
                     it displays the contents of the current directory.
        Usage examples:
                     - dir
                     - dir /home

    4. environ
        Description: The environ command prints all the environment
                     variables. Environment variables are editable
                     values that are shared by programs to help them
                     know where to find needed resources such as
                     temporary files and executable programs.
        Usage example:
                     - environ

    5. echo
        Description: The echo command prints text to the screen.
                     Multiple words are allowed.
        Usage examples:
                     - echo hello world
                     - echo miski     is        a       dog

    6. help
        Description: The help command displays this current user manual.
        Usage example:
                     - help

    7. pause
        Description: The pause command stops the shell (and its prompt)
                     until Enter is pressed. It is disabled in batch mode,
                     and a message is displayed to indicate this.
        Usage example:
                     - pause

    8. quit (q)
        Description: Exits the shell.
        Usage examples:
                     - quit
                     - q

EXTERNAL COMMANDS:

    Any command that is not listed in internal commands are executed as external programs using fork() and execvp().
        - fork(): creates a child process that is a copy of the current process.
        - execvp(): replaces the current process image with the process image specified in its arguments. 
    For external commands, the child process sets an environment variable called parent that stores the full path to 
    the shell executable before calling execvp(). 
    
    Usage examples:
                 - ls
                 - pwd
                 - cat file.txt
    
INPUT AND OUTPUT REDIRECTION:

    The shell supports I/O redirection. The instructions for usage are as follows:
                 - Input redirection (<)
                 
                        Usage: command < inputfile
                 
                        Description: The source of the command input is a file. It allows programs to read data from a
                        file instead of waiting for manual keyboard input. This is achieved by changing the input stream. 
                        The file must exist for the shell to read
                        from it.
                 
                 - Output redirection: Truncate (>)

                        Usage: command > outputfile
                 
                        Description: The output of the command is directed to a file instead of being shown to the screen.
                        This is achieved by changing the output stream. The destination file does not need to exist before
                        command execution. If the destination file does not exist, the file is created. If the destination 
                        file exists, the output of the command is truncated to the file. That is, contents of the file will 
                        be deleted and replaced with the output of the command.
                 
                 - Output redirection: Append (>>)
                 
                        Usage: command >> outputfile
                 
                        Description: The output of the command is directed to a file instead of being shown to the screen.
                        This is achieved by changing the output stream. The destination file does not need to exist before
                        command execution. If the destination file does not exist, the file is created. If the destination 
                        file exists, the output of the command is appended to the file. That is, contents of the file will 
                        remain and the output of the command will be added at the end.
                Usage examples:
                            - echo hello world > out.txt 
                            - ls >> out.txt
                            - wc -w < out.txt
                            - sort < out.txt > s.txt
    
    The internal commands that support output redirection are:
                - dir
                - environ
                - echo 
                - help
    The internal commands that do no support I/O redirection and will display a corresponding message indicating this are:
                - cd
                - clear
                - pause
                - quit
    
FOREGROUND AND BACKGROUND EXECUTION
    A program executed in foreground blocks the shell process from accepting new commands and further processing until that 
    program finishes.
    A program executed in background allows the shell to return to interactive mode and accept new commands even if that 
    program did not finish yet

    This shell implements both. Background execution is implemented using fork() and execvp(). Finished background child 
    processes are cleaned up using a non-blocking waitpid() in the main shell loop to avoid zombie processes.

    Foreground execution also uses fork() and execvp() but waitpid() is implemented inside the parent execution logic. 
    
    Usage example:
                 - sleep 5 &

ENVIRONMENT VARIABLES
    The shell sets the following environment variables: 
                 - shell: holds the full path to the executable of the shell.
                 - parent: For external commands, the child process sets this variable to keep track of the parent process 
                           before running execvp().

ERROR MESSAGES:
    Some commands might require memory management and interaction with environment variables that might trigger an error.
    These errors are displayed appropriately.
    I/O file redirection, fork and execvp errors are handled gracefully and corresponding informative messages are displayed.
    Incomplete commands are rejected and an error message is displayed.

LIMITATIONS:
    The shell does not support the following:
                 - pipe redirection (|)
                 - advanced parsing such as quotes or scape characters
                 - redirection depends on whitespace separation between tokens
                 - background execution is only implemented for external commands

SOURCE FILE STRUCTURE:
                 - builtin_cmd_redirect.c:
                            - int run_builtin_redirect(CommandConfig* cmd_config)
                            - int cmd_support_redirect(char* command)
                 - builtin_cmd_redirect.h

                 - cmd_config.c:
                            - int get_command_from_tokens(char** tokens, int back_flag, CommandConfig* cmd_config)
                            - void free_cmd_config(CommandConfig* cmd_config)
                 - cmd_config.h
                 
                 - commands.c:
                            - static int command_clear(char** tokens);
                            - static int command_dir(char** tokens);
                            - static int command_environ(char** tokens);
                            - static int command_quit(char** tokens);
                            - static int command_cd(char** tokens);
                            - static int command_echo(char** tokens);
                            - static int command_pause(char** tokens);
                            - static int command_help(char** tokens);
                            - int command_caller(char** tokens);
                            - int is_internal_command(char* command);
                 - commands.h
                 
                 - input.c:
                            - void set_shell_state(ShellState* curr_state)
                            - ShellState* get_shell_state()
                            - void show_prompt() 
                            - char* read_line()
                 - input.h
                 
                 - makefile

                 - paths.c:
                            - char* get_working_dir()
                            - char* get_shell_path()
                            - char* get_manual_path()
                 - paths.h

                 - run.c      
                            - int run_external(CommandConfig* cmd_config)
                            - int apply_redirection(CommandConfig* cmd_config)
                 - run.h

                 - simpleshell.c
                 
                 - tokens.c: 
                            - char** create_tokens(char* line_cpy, int* num_tokens)
                 - tokens.h

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I have adhered to DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/