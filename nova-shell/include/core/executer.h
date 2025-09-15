#pragma once
#include <unistd.h>   // for fork and execve
#include <sys/wait.h> // for waitpid
#include "core/lexer.h"
#include "core/parser.h"
#include "utils/env.h"
#include "utils/path.h"

#define DEBUG 1

void execute(Lexer &lex, Env &env);
