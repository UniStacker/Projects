#include "core/lexer.h"
#include "utils/path.h"


// --- Operator list & Match function ---
static const vec_str OPERATORS = {
    "==", "!=", "||", "&&", "!", "|", "&", "=", "<", ">", ">>", "<=", ">="
};
static vec_str sortedOperators = [] {
    auto ops = OPERATORS;
    std::sort(ops.begin(), ops.end(),
              [](const std::string &a, const std::string &b) {
                  return a.size() > b.size();
              });
    return ops;
}();
std::string matchOperator(const std::string &input, size_t pos) {
    for (const auto &op : sortedOperators) {
        if (input.compare(pos, op.size(), op) == 0) {
            return op;
        }
    }
    return {};
}

// --- Separators ---
static const std::string SEPARATORS = "(){}[];";

// --- Main tokenizer ---
vec_tok tokenize(const std::string &input) {
    vec_tok tokens;
    size_t i = 0;

    while (i < input.size()) {
        char c = input[i];

        // 1. Skip whitespace
        if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        // 2. Operators
        if (auto op = matchOperator(input, i); !op.empty()) {
            tokens.push_back({TokenType::OPERATOR, op});
            i += op.size();
            continue;
        }

        // 3. Separators (single char)
        if (SEPARATORS.find(c) != std::string::npos) {
            tokens.push_back({TokenType::SEPARATOR, std::string(1, c)});
            i++;
            continue;
        }

        // 4. Quoted string
        if (c == '"' || c == '\'') {
            char quote = c;
            i++; // size_t start = ++i; // skip starting quote
            std::string value; value += quote;

            while (i < input.size()) {
                if (input[i] == '\\' && i + 1 < input.size()) {
                    value.push_back(input[i + 1]); // unescape next char
                    i += 2;
                } else if (input[i] == quote) {
                    i++;
                    break;
                } else {
                    value.push_back(input[i++]);
                }
            }

            tokens.push_back({TokenType::STRING, value});
            continue;
        }

        // 5. Bareword (collect until space/operator/separator/quote)
        size_t start = i;
        while (i < input.size()) {
            if (std::isspace(static_cast<unsigned char>(input[i]))) break;
            if (!matchOperator(input, i).empty()) break;
            if (SEPARATORS.find(input[i]) != std::string::npos) break;
            if (input[i] == '"' || input[i] == '\'') break;
            i++;
        }
        tokens.push_back({TokenType::STRING, input.substr(start, i - start)});
    }

    return tokens;
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

vec_tok Lexer::tokenize_line() {
  this->lineNo++;
  std::string line {};
  std::getline(*this->stream, line);
  return tokenize(line);
}

vec_tok Lexer::tokenize_all() {
  vec_tok tokens {};
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
