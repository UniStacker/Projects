#include "core/parser.h"


bool valid_tokens(const vec_tok &tokens) {
  for (auto &tok : tokens) {
    auto value = tok.value;
    if (
      (value[0] == '\'' && value[value.length()-1] != '\'')
      || (value[0] == '"' && value[value.length()-1] != '"')
    ) {
      std::cerr << "Unterminated string: " << value << '\n';
      return false;
    }
  }
  return true;
}

omap_str parse_command(const vec_tok &tokens, const Env &env) {
  omap_str outMap { { "type", "command" } };
  std::string cmd { utils::parse_path(tokens[0].value, env) };
  std::string args {};
  bool lspace {};

  vec_str redOps = {">", ">>", "<"};

  for (size_t i=1; i<tokens.size(); i++) {
    if (lspace) args += ' ';
    else lspace = true;

    TokenType type = { tokens[i].type };
    std::string value { tokens[i].value };
    if (type == TokenType::OPERATOR
        && (value == "|" || std::find(redOps.begin(), redOps.end(), value) != redOps.end())
    ) {
      std::cout << "Parsing Op: " << value << '\n';
      outMap.emplace(cmd, args);
      lspace = false;
      if (++i < tokens.size()) {
        auto parsed = utils::parse_path(tokens[i].value, env);
        if (value == ">") outMap.emplace("nova_internal_writeStdOut", parsed);
        else if (value == ">>") outMap.emplace("nova_internal_appendStdOut", parsed);
        else if (value == "<") outMap.emplace("nova_internal_readStdIn", parsed);
        else {
          cmd = parsed;
          args = "";
          continue;
        }
        i++;
        continue;
      } else {
        std::cerr << "Nova: Unexpected end of stream\n";
        return {};
      }
    }
    else if (value[0] != '\'') {
      if (value[0] == '"') value = value.substr(1, value.length()-2);
      value = utils::parse_path(value, env);
    }

    if (value[0] == '\'') value = value.substr(1, value.length()-2);
    args += value;
  }
  if (!cmd.empty()) outMap.emplace(cmd, args);
  if (outMap.size() > 2) outMap["type"] = "pipe";

  return outMap;
}

omap_str parse(const vec_tok &tokens, const Env &env) {
  if (!tokens.size() || !valid_tokens(tokens)) {
    return {};
  }

  // TokenType type = { tokens[0].type };
  // std::string value { tokens[0].value };
  if (0) { // TODO: Keyword check

  }
  else if (0) { // TODO: Assignment check
    
  }
  else
    return parse_command(tokens, env);
}
