#ifndef BUILTIN_CMD_REDIRECT_H
#define BUILTIN_CMD_REDIRECT_H

#include "cmd_config.h"


//run builtin commands with possibility of redirection
//return values are the same as command_caller
//0 success, 1 error, -1 quit, -2 not a builtin
int run_builtin_redirect(CommandConfig* cmd_config);

#endif