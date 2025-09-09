#include "core/lexer.h"
#include "utils/path.h"

vec_str tokenize(const std::string &s) {
  return utils::split_string(s, " ", true);
}

Lexer::Lexer(const std::string &input, bool fromFile) {
  this->code = "";
  this->lineNo = 0;

  if (fromFile) {
    this->file_path = input;
    std::ifstream ifs(input, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
      std::cerr << "Error: Failed to open file for reading: " << this->file_path << '\n';
      exit(1);
    }
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');
    ifs.read(buffer.data(), size);

    this->stream = std::make_unique<std::istringstream>(buffer);
  } else {
    this->file_path = "stdin";
    this->stream = std::make_unique<std::istringstream>(input);
  }

  if (!this->stream->good()) {
    std::cerr << "Error: istringstream is not healthy\n";
    exit(1);
  }
}

Lexer Lexer::fromFile(const std::string &filepath) {
  return Lexer(filepath, true);
}

Lexer Lexer::fromString(const std::string &string) {
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
