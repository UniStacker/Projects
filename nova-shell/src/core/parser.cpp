#include "core/parser.h"


const std::string builtins = "cd pwd exit";

bool is_builtin(const std::string& cmd) {
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

std::unique_pointer<ExecNode> getValidExecNode(
  const std::string &cmd,
  const vec_str &args,
  const Env &env,

) {
  std::string path = utils::parse_path(cmd, env);
  if (!fs::exists(path) && !is_builtin(path))
    path = env.getFromPath(path);

  if (!fs::exists(path) && !is_builtin(path)) {
    std::cerr << "Nova: No command " << path << " Found, Did you mean:\n";
    return nullptr;
  } else {
    auto node = std::make_unique<ExecNode>();
    node->cmd = std::move(path);
    node->args = std::move(args);
    return std::move(node);
  }
}

bool handleRedirects(
  std::string &cmd,
  vec_str &args,
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
  if ((auto cmdNode = addValidCmd(cmd, args, env))) {
    node->cmd = cmdNode->cmd;
    node->args = cmdNode->args;
  } else return false;

  if (op == ">") {
    node->redirects.emplace("nova_builtin_writeOut", utils::parse_path(tokens[i].value, env));
  }
  else if (op == ">>") {
    node->redirects.emplace("nova_builtin_appendOut", utils::parse_path(tokens[i].value, env));
  }
  else if (op == "<") {
    node->redirects.emplace("nova_builtin_readIn", utils::parse_path(tokens[i].value, env));
  }
  else if (op == "|") {
    cmd = tokens[i].value;
  }
  else return false;

  i++;
  return true;
}

std::unique_pointer<ExecNode> parse_command(const vec_tok &tokens, const Env &env) {
  auto node = std::make_unique<ExecNode>();
  std::string cmd { tokens[0].value };
  vec_str args {};

  // std::string redOps(" > >> < | ");

  for (size_t i=1; i<tokens.size(); i++) {
    auto type = tokens[i].type;
    auto value = tokens[i].value;
    // auto isRed = redOps.find(' ' + value + ' ') != std::string::npos;

    // if (type == TokenType::OPERATOR) {
      // if (!handleRedirects(cmd, args, value, tokens, i, env, node.raw()))  return nullptr;
    // } else
    if (value[0] != '\'') {
      if (value[0] == '"') value = value.substr(1, value.length()-2);
      value = utils::parse_path(value, env);
    }

    if (value[0] == '\'') value = value.substr(1, value.length()-2);
    args.push_back(value);
  }
  if (!cmd.empty() && addValidCmd(cmd, args, env, next)) return nullptr;
}

AST parse(const vec_tok &tokens, const Env &env) {
  if (!tokens.size() || !valid_quotes(tokens)) {
    return {};
  }

  Ast ast;
  // TokenType type = { tokens[0].type };
  // std::string value { tokens[0].value };
  if (0) { // TODO: Keyword check

  }
  else if (0) { // TODO: Assignment check

  }
  else{
    ast.add_node(std::move(parse_command(tokens, env)));
  }

  return ast;
}
