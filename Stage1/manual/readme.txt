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

COMMANDS:

    1. cd target
        Description: The cd command changes directory. If no target
                     directory is provided, it lists the current directory.
                     It updates the PWD environment variable and shows
                     an error statement if the directory does not exist.
        Usage examples:
                     - cd /home
                     - cd ..
                     - cd

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

ERROR MESSAGES:
    Some commands might require memory management and interaction
    with environment variables that might trigger an error.
    These errors are displayed appropriately.
    Invalid commands are passed to external execution.

LIMITATIONS:
    - Commands are tokenised using whitespace (spaces and tabs).
    - Each word separated by whitespace is treated as a separate argument.

/*
---------------------------------------------------------------------------------------------------------------------------------------------
Name: Lucero Dafne Sangueza Espinoza
Student Number: 23100952
I declare that this assignment is my own work and that I have adhered to DCU Academic Integrity Policy
---------------------------------------------------------------------------------------------------------------------------------------------
*/