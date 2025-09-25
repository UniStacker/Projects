#include "core/parser.h"


const std::string builtins = "cd pwd exit";

bool is_builtin(const std::string &cmd) {
  if (builtins.find(cmd) == std::string::npos) return false;
  return true;
}

bool valid_quotes(const vec_tok &tokens) {
  for (auto &tok : tokens) {
    auto value = tok.value;
    if (
      (value[0] == '\'' && value[value.length()-1] != '\'')
      || (value[0] == '"' && value[value.length()-1] != '"')
    ) {
      std::cerr << "Nova: Unterminated string: " << value << '\n';
      return false;
    }
  }
  return true;
}

bool addValidCmd(
  const std::string &cmd,
  const vec_str &args,
  const Env &env,
  ExecNode *node
) {
  std::string path = utils::parse_path(cmd, env);
  if (!fs::exists(path) && !is_builtin(path))
    path = env.getFromPath(path);

  if (!fs::exists(path) && !is_builtin(path)) {
    std::cerr << "Nova: No command " << path << " Found, Did you mean:\n";
    return false;
  } else {
    if (1) std::cout << "Adding command: " << path << '\n';
    node->command = std::move(path);
    node->args = std::move(args);
    return true;
  }
}

bool handleRedirects(
  const std::string op,
  const vec_tok &tokens,
  size_t &i,
  const Env &env,
  ExecNode* node
) {
  if (++i >= tokens.size()) {
    std::cerr << "Nova: Unexpected end of tokens\n";
    return false;
  }

  if (op == ">") {
    node->redirects.emplace("write", utils::parse_path(tokens[i].value, env));
  }
  else if (op == ">>") {
    node->redirects.emplace("append", utils::parse_path(tokens[i].value, env));
  }
  else if (op == "<") {
    node->redirects.emplace("read", utils::parse_path(tokens[i].value, env));
  }
  else return false;

  i++;
  return true;
}

std::unique_ptr<ExecNode> parse_command(
  const vec_tok &tokens,
  size_t &idx,
  const Env &env
) {
  auto node = std::make_unique<ExecNode>();
  std::string cmd { tokens[0].value };
  vec_str args {};

  std::string redOps(" > >> < ");

  for (; idx<tokens.size(); idx++) {
    auto type = tokens[idx].type;
    auto value = tokens[idx].value;
    auto isRed = redOps.find(' ' + value + ' ') != std::string::npos;

    if (type == TokenType::OPERATOR) {
      if (!addValidCmd(cmd, args, env, node.get())) return nullptr;
      if (isRed && !handleRedirects(value, tokens, idx, env, node.get())) return nullptr;
      if (value == "|") {
        std::unique_ptr<ExecNode> next;
        if (!(next = parse_command(tokens, ++idx, env))) return nullptr;
        node->pipe = std::move(next);
      }
    } else if (value[0] != '\'') {
      if (value[0] == '"') value = value.substr(1, value.length()-2);
      value = utils::parse_path(value, env);
    }

    if (value[0] == '\'') value = value.substr(1, value.length()-2);
    args.push_back(value);
  }
  // if (!cmd.empty() && addValidCmd(cmd, args, env, node.get())) return nullptr;
  return node;
}

AST parse(const vec_tok &tokens, const Env &env) {
  if (!tokens.size() || !valid_quotes(tokens)) {
    return {};
  }

  AST ast;
  size_t idx = 0;
  // TokenType type = { tokens[0].type };
  // std::string value { tokens[0].value };
  if (0) { // TODO: Keyword check

  }
  else if (0) { // TODO: Assignment check

  }
  else{
    std::unique_ptr<ExecNode> cmdNode;
    if (!(cmdNode = parse_command(tokens, idx, env))) return {};
    ast.append_node(std::move(cmdNode));
  }

  return ast;
}
