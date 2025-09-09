#include "core/executer.h"

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
  fs::path rpath = path;
  if (path.is_relative()) rpath = fs::current_path() / path;

  if (path.empty()) return cmd_cd(env.get("HOME"), env);
  else if (fs::exists(path) && fs::is_directory(path)) {
    if (env.get("OLDPWD") != env.get("PWD")) env.set("OLDPWD", env.get("PWD"));
    chdir(path.string().c_str());
    env.set("PWD", fs::current_path().string());
    return 0;
  } 
  else if (fs::exists(path)) {
    std::cerr << "cd: Path is not a dir '" << path.string() << "'\n";
    return 1;
  }
  else std::cerr << "cd: Path does not exist '" << path.string() << "'\n";
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
    std::clog << "Running: " << command << " '" << args << "'\n";

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

void execute(Lexer &lex, Env &env) {
  while (!lex.eof()) {
    auto tokens = lex.tokenize_line();

    if (tokens.empty()) continue;
    if (DEBUG) for (auto& t : tokens) std::cout << "[" << t << "]\n";

    map_str ast = parse(tokens, env);
    if (ast["type"] == "command") {
      fs::path command = ast["cmd"];
      if (!fs::exists(command) && !is_builtin(command)) {
        // command = "/usr/bin/";
        // if (!env.get("PREFIX").empty()) (command = env.get("PREFIX")) /= "bin";
        // command /= ast["cmd"];
        command = env.getFromPath(ast["cmd"]);
      }

      if (!fs::exists(command) && !is_builtin(command)) {
        std::cerr << "No command " << ast["cmd"] << " Found, Did you mean:" << '\n';
      } else run_command(command, ast["args"], env);
    }
  }
}

