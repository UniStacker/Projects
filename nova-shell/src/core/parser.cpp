#include "core/parser.h"


bool valid_tokens(const vec_str &tokens) {
  for (auto &tok : tokens) {
    if (
      (tok[0] == '\'' && tok[tok.length()-1] != '\'')
      || (tok[0] == '"' && tok[tok.length()-1] != '"')
    ) {
      std::cerr << "Unterminated string: " << tok << '\n';
      return false;
    }
  }
  return true;
}

map_str parse_command(const vec_str &tokens, const Env &env) {
  std::string command { utils::parse_path(tokens[0], env) };
  std::string args {};

  for (size_t i=1; i<tokens.size(); i++) {
    if (i != 1) args += ' ';

    std::string tok = tokens[i];
    if (tok[0] != '\'') {
      if (tok[0] == '"') tok = tok.substr(1, tok.length()-2);
      tok = utils::parse_path(tok, env);
    }

    if (tok[0] == '\'') tok = tok.substr(1, tok.length()-2);
    args += tok;
  }

  return {{"type", "command"}, {"cmd", command}, {"args", args}};
}

map_str parse(const vec_str &tokens, const Env &env) {
  if (!tokens.size() || !valid_tokens(tokens)) {
    return {};
  }

  std::string tok { tokens[0] };
  if (0) { // TODO: Keyword check

  }
  else if (0) { // TODO: Assignment check
    
  }
  else
    return parse_command(tokens, env);
}
