#pragma once
#include <unistd.h>   // for fork and execve
#include <sys/wait.h> // for waitpid
#include "core/lexer.h"
#include "core/parser.h"
#include "utils/env.h"

#define DEBUG 0

void execute(Lexer &lex, Env &env);
