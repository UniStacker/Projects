#include "core/lexer.h"
#include "core/parser.h"
#include "utils/env.h"
#include <unistd.h>     // fork, execve
#include <sys/wait.h>   // waitpid
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#define DEBUG 0

namespace fs = std::filesystem;

struct Config {
  std::string PS1;
  Env env;
};

const std::string builtins = "cd pwd exit";

bool is_builtin(const std::string& cmd) {
  if (builtins.find(cmd) == std::string::npos) return false;
  return true;
}

std::vector<char*> make_argv(const std::string &filename,
                             const std::string &args) {
  std::vector<char*> argv;
  argv.reserve(args.size() + 2);

  argv.push_back(strdup(filename.c_str()));   // copy program name

  std::istringstream iss(args);
  std::string arg;
  while (iss >> arg) {
    argv.push_back(strdup(arg.c_str()));  // copy each arg
  }
  argv.push_back(nullptr);

  return argv;
}

int cmd_cd(const fs::path &path, Env &env) {
  if (path.empty()) return cmd_cd(env.get("HOME"), env);
  else if (fs::exists(path) && fs::is_directory(path)) {
    env.set("OLDPWD", env.get("PWD"));
    env.set("PWD", path);
    return 0;
  } 
  else if (fs::exists(path)) {
    std::cerr << "cd: Path is not a dir '" << path << "'\n";
    return 1;
  }
  else std::cerr << "cd: Path does not exist '" << path << "'\n";
  return 2;
}

void cmd_exit() {
  exit(0);
}

int run_command(const fs::path &command, 
                const std::string &args, 
                Env &env) 
{
  std::string cmd = command.filename().string();
  if (DEBUG)
    std::clog << "Running: " << cmd << " '" << args << "'\n";

  if (cmd == "cd")
    return cmd_cd(args, env);
  else if (cmd == "pwd") {
    std::cout << env.get("PWD") << '\n';
    return 0;
  }
  else if (cmd == "exit") {
    cmd_exit();
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return -1;
  }

  if (pid == 0) {
    // Child //
    // Build argv
    std::vector<char*> argv = make_argv(cmd, args);

    // Build envp
    std::vector<char*> envp = env.to_envp();

    // Exec
    chdir(env.get("PWD").c_str());
    execve(command.string().c_str(), argv.data(), envp.data());

    // If execve returns, there was an error
    perror("execve failed");
    _exit(1);
  }

  // Parent: wait for child //
  int status;
  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid failed");
    return -1;
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);  // child exit code
  } else {
    return -1;
  }
}

std::string parse_prompt(const Config &conf) {
  std::string out {};

  for (size_t i=0; i<conf.PS1.length(); i++) {
    char ch = conf.PS1[i];
    if (ch == '%') {
      char next = (i + 1 < conf.PS1.length()) ? conf.PS1[++i] : '\0';
      if (next == 'u') out += conf.env.get("USER"); else
      if (next == 'h') out += "Nova"; else
      if (next == 'c') {
        std::string cwd = conf.env.get("PWD");
        std::string home = conf.env.get("HOME");
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

void execute(Lexer &lex, Env &env) {
  while (!lex.eof()) {
    auto tokens = lex.tokenize_line();

    if (tokens.empty()) continue;
    if (DEBUG) for (auto& t : tokens) std::cout << "[" << t << "]\n";

    string_map ast = parse(tokens, env);
    if (ast["type"] == "command") {
      fs::path command = ast["cmd"];
      if (!fs::exists(command) && !is_builtin(command)) {
        command = "/usr/bin/";
        if (!env.get("PREFIX").empty()) (command = env.get("PREFIX")) /= "bin";
        command /= ast["cmd"];
      }

      if (!fs::exists(command) && !is_builtin(command)) {
        std::cerr << "No command " << ast["cmd"] << " Found, Did you mean:" << '\n';
      } else run_command(command, ast["args"], env);
    }
  }
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
  Config conf {"%u@%h %c $ ", env};

  if (argc > 2 && std::string(argv[1]) == "-c") {
    lex = Lexer::fromString(std::string(argv[2]));
    execute(lex, conf.env);
  } else
  if (argc == 2) {
    lex = Lexer::fromFile(std::string(argv[1]));
    execute(lex, conf.env);
  } else
  if (argc == 1) {
    while (true) {
      std::cout << parse_prompt(conf);
      std::string line;
      std::getline(std::cin, line);
      lex = Lexer::fromString(line);
      execute(lex, conf.env);
    }
  }

  return 0;
}
