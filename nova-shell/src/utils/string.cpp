#include "utils/string.h"


bool is_delim(const std::string &str, size_t &idx, const std::string &delim) {
  auto dpos = delim.find("|");
  auto& npos = std::string::npos;
  vec_str delims;
  if (dpos != npos && delim[dpos - 1 >= 0 ? dpos - 1 : dpos] != '\\') {
    delims = utils::split_string(delim, "\\|");
  } else delims = {delim};

  size_t si { idx }, di { 0 };
  for (auto& del : delims) {
    bool passed { true };
    for (si = idx, di = 0; si<str.length() && di<del.length(); si++, di++)
      if (str[si] != del[di]) passed = false;
    if (di < del.length()) passed = false;

    if (passed) {
      idx = si - 1;
      return true;
    }
  }

  return false;
}

vec_str utils::split_string(const std::string &s, const std::string &delim, bool skip_quotes) {
  vec_str tokens {};
  std::string current {};
  bool in_quotes = { false };
  char quote_char { '\0' };

  for (size_t i = 0; i < s.size(); i++) {
    char c = s[i];

    if (in_quotes && skip_quotes) {
      if (c == quote_char) {
        in_quotes = false; // closing quote
      }
      current.push_back(c);
    } else {
      if (c == '"' || c == '\'') {
        in_quotes = true;
        quote_char = c; // start quote
      } else if (is_delim(s, i, delim)) {
        if (!current.empty()) {
          tokens.push_back(current);
          current.clear();
          continue;
        }
      }
      current.push_back(c);
    }
  }

  if (!current.empty())
    tokens.push_back(current);

  return tokens;
}

