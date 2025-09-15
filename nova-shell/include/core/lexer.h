#pragma once
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include "utils/types.h"
#include "utils/string.h"


struct Token;
using vec_tok = std::vector<Token>;
enum class TokenType { OPERATOR, SEPARATOR, STRING };
const vec_str TypeName { "OPERATOR", "SEPARATOR", "STRING" };

struct Token {
  TokenType type;
  std::string value;

  friend std::ostream& operator<<(std::ostream& s, const Token& t) {
    s << "[" << TypeName[(size_t)(t.type)] << " '" << t.value << "']\n";
    return s;
  }
};


class Lexer {
  std::string                   code        {};
  size_t                        lineNo      {};
  std::string                   file_path   {};
  std::unique_ptr<std::istream> stream      {};

public:
  Lexer() {};
  Lexer(const std::string &code, bool fromFile);

  static Lexer fromFile(const std::string &filepath);
  static Lexer fromString(const std::string &string);

  vec_tok tokenize_line();
  vec_tok tokenize_all();

  bool eof();
};
