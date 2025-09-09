#pragma once
#include "core/lexer.h"
#include "utils/types.h"
#include "utils/env.h"
#include "utils/path.h"
#include <iostream>
#include <regex>

map_str parse(const vec_str &tokens, const Env &env);
