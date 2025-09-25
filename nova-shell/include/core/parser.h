#pragma once
#include "core/lexer.h"
#include "utils/types.h"
#include "utils/env.h"
#include "utils/path.h"
#include "core/ast.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <memory>
#include <unistd.h>


AST parse(const vec_tok &tokens, const Env &env);
bool is_builtin(const std::string &cmd);
