#include "core/lexer.h"
#include "core/parser.h"
#include "core/executer.h"
#include "utils/env.h"
#include <unistd.h>     // fork, execve
#include <sys/wait.h>   // waitpid
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;


std::string parse_prompt(const std::string &prompt, const Env &env) {
  std::string out {};

  for (size_t i=0; i<prompt.length(); i++) {
    char ch = prompt[i];
    if (ch == '%') {
      char next = (i + 1 < prompt.length()) ? prompt[++i] : '\0';
      if (next == 'u') out += env.get("USER"); else
      if (next == 'h') out += "Nova"; else
      if (next == '~') {
        std::string cwd = env.get("PWD");
        std::string home = env.get("HOME");
        size_t pos = cwd.find(home);
        if (pos != std::string::npos)
          out += cwd.substr(0, pos) + '~' + cwd.substr(pos + home.length());
        else out += cwd;
      } else
      if (next != '\0') out += '%' + next;
    }
    else out += ch;
  }

  return out;
}

int main(int argc, char *argv[], char *envp[]) {
  if (argc == 2 && std::string(argv[1]) == "--help") {
    std::cerr << "Usage: nova <file name> | -c <command>\n";
    exit(1);
  }

  Env env(envp);
  Lexer lex;

  env.set("OLDPWD", env.get("HOME"));
  env.set("PWD", env.get("HOME"));
  chdir(env.get("PWD").c_str());
  const std::string PS1 { "╭─\033[1m\033[32m%u@%h \033[34m%~\033[0m\n╰─$ " };


  if (argc > 2 && std::string(argv[1]) == "-c") {
    lex = Lexer::fromString(std::string(argv[2]));
    execute(lex, env);
  } else
  if (argc == 2 && argv[1][0] != '-') {
    std::string file(argv[1]);
    lex = Lexer::fromFile(file);
    execute(lex, env);
  }
  else {
    while (true) {
      std::cout << parse_prompt(PS1, env);
      std::string line;
      std::getline(std::cin, line);
      lex = Lexer::fromString(line);
      auto astNode = parse(lex.tokenize_line());
      BaseNode *next = &astNode;
      std::string spaces = "";
      while (next) {
        std::cout << spaces << "[" << next->type << "]\n";
        spaces += "  ";
        next = next->next;
      }
      // execute(lex, env);
    }
  }

  return 0;
}
