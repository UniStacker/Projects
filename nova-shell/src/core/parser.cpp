#include "core/parser.h"
#include <filesystem>

namespace fs = std::filesystem;


void validate_tokens(const vec_str& tokens) {
  for (auto& tok : tokens) {
    if (
      (tok[0] == '\'' && tok[tok.length()-1] != '\'')
      || (tok[0] == '"' && tok[tok.length()-1] != '"')
    )
      throw std::runtime_error("Unterminated string: " + tok + '\n');
  }
}

std::string replaceWithEnv(const std::string& input, const Env &env) {
  using namespace std;

  regex envPattern(R"(\$(\w+))");
  string result;
  auto begin = sregex_iterator(input.begin(), input.end(), envPattern);
  auto end   = sregex_iterator();

  size_t lastPos = 0;
  for (auto it = begin; it != end; ++it) {
    smatch m = *it;
    // append everything before match
    result.append(input, lastPos, m.position() - lastPos);
    // append replacement
    result += env.get(m.str(1));
    // move past this match
    lastPos = m.position() + m.length();
  }
  // append the rest of the string
  result.append(input, lastPos, string::npos);

  return result;
}

std::string replaceWithRun(const std::string& input) {
  using namespace std;

  regex runPattern(R"(\$\(([^)]+)\))");
  string result;
  auto begin = sregex_iterator(input.begin(), input.end(), runPattern);
  auto end   = sregex_iterator();

  size_t lastPos = 0;
  for (auto it = begin; it != end; ++it) {
    smatch m = *it;
    result.append(input, lastPos, m.position() - lastPos);
    result += "run(\"" + m.str(1) + "\")";
    lastPos = m.position() + m.length();
  }
  result.append(input, lastPos, string::npos);

  return result;
}

std::string parse_string(const std::string &input, const Env &env) {
  std::string result = replaceWithRun(input);
  result = replaceWithEnv(result, env);

  std::string path_str = result;
  fs::path path = path_str;

  // Replace '~' and relative paths
  if (path_str[0] == '~') path = env.get("HOME") + '/' + path_str.substr(1); else
  if (path.is_relative()) {
    std::string resolved = parse_string(env.get("PWD") + '/' + path.string(), env);
    if (fs::exists(resolved)) path = resolved;
  }

  path_str = path.lexically_normal().string();

  // std::cout << "Resolved path: " << path_str << '\n';
  return path_str;
}

string_map parse_command(const vec_str &tokens, const Env &env) {
  std::string command = parse_string(tokens[0], env);
  std::string args = "";

  for (size_t i=1; i<tokens.size(); i++) {
    if (i != 1) args += ' ';

    std::string tok = tokens[i];
    if (tok[0] != '\'') {
      tok = parse_string(tok, env);
    }

    if (tok[0] == '\'' || tok[0] == '"') tok = tok.substr(1, tok.length()-2);
    args += tok;
  }

  return {{"type", "command"}, {"cmd", command}, {"args", args}};
}

string_map parse(const vec_str &tokens, const Env &env) {
  if (!tokens.size()) {
    return {};
  }
  validate_tokens(tokens);

  std::string tok = tokens[0];

  if (0) { // TODO: Keyword check

  }
  else if (0) { // TODO: Assignment check
    
  }
  else
    return parse_command(tokens, env);
}
