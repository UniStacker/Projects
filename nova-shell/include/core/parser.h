#pragma once
#include "core/lexer.h"
#include "utils/types.h"
#include "utils/env.h"
#include <cstdlib>
#include <iostream>
#include <regex>

string_map parse(const vec_str &tokens, const Env &env);
