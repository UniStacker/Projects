#pragma once
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <cstdlib>
#include "utils/types.h"
#include "utils/string.h"


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

  vec_str tokenize_line();
  vec_str tokenize_all();

  bool eof();
};
