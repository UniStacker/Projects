#include "core/lexer.h"


vec_str tokenize(const std::string& s) {
  vec_str tokens {};
  std::string current {};
  bool in_quotes = { false };
  char quote_char { '\0' };

  for (size_t i = 0; i < s.size(); i++) {
    char c = s[i];

    if (in_quotes) {
      if (c == quote_char) {
        in_quotes = false; // closing quote
      }
      current.push_back(c);
    } else {
      if (c == '"' || c == '\'') {
        in_quotes = true;
        quote_char = c; // start quote
      } else if (isspace((unsigned char)c)) {
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

Lexer::Lexer(const std::string& code, bool fromFile) {
  this->code = "";
  this->lineNo = 0;

  if (fromFile) {
    this->file_path = code;
    this->stream = std::make_unique<std::ifstream>(code);
    if (!this->stream || !this->stream->good())
      throw std::runtime_error("Error: Failed to open file for reading: "
                               + this->file_path + '\n');
  } else {
    this->file_path = "stdin";
    this->stream = std::make_unique<std::istringstream>(code);
  }
}

Lexer Lexer::fromFile(const std::string& filepath) {
  return Lexer(filepath, true);
}

Lexer Lexer::fromString(const std::string& string) {
  return Lexer(string, false);
}

vec_str Lexer::tokenize_line() {
  this->lineNo++;
  std::string line {};
  std::getline(*this->stream, line);
  return tokenize(line);
}

vec_str Lexer::tokenize_all() {
  vec_str tokens {};
  while (!this->stream->eof()) {
    this->lineNo++;
    std::string line {};
    std::getline(*this->stream, line);
    auto next = tokenize(line);
    tokens.insert(tokens.end(), next.begin(), next.end());
  }
  return tokens;
}

bool Lexer::eof() {
  return this->stream->eof();
}
