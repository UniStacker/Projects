#pragma once
#include <string>
#include "utils/types.h"

namespace utils {
  vec_str split_string(const std::string &str, const std::string &delim, bool skip_quotes = false);
}
