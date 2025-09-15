#pragma once
#include "core/lexer.h"
#include "utils/types.h"
#include "utils/env.h"
#include "utils/path.h"
#include <iostream>
#include <algorithm>
#include <regex>

omap_str parse(const vec_tok &tokens, const Env &env);
