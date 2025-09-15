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

int cmd_pwd(Env& env) {
  auto cwd = env.get("PWD") + '\n';
  write(STDOUT_FILENO, cwd.c_str(), cwd.length());
  return 0;
}

void cmd_exit() {
  exit(0);
}

int run_builtin(const std::string &cmd, const std::string &argv, Env &env) {
  if (cmd == "cd")
    return cmd_cd(argv, env);
  else if (cmd == "pwd")
    return cmd_pwd(env);
  else if (cmd == "exit")
    cmd_exit();

  return -1;
}

int run_pipeline(const std::vector<std::vector<std::string>> &commands, Env &env) {
  if (commands.empty()) return -1;

  size_t n = commands.size();
  std::vector<int> pipes(2 * (n - 1));

  for (size_t i = 0; i < n - 1; ++i)
    if (pipe(&pipes[i * 2]) == -1) { perror("pipe"); return 127; }

  std::vector<pid_t> pids {};

  for (size_t i = 0; i < n; ++i) {
    auto command = commands[i][0];
    auto args = commands[i][1];
    auto name = fs::path(command).filename().string();
    bool builtin = is_builtin(command);

    if (DEBUG)
      std::clog << "Running: " << command << " " << args << '\n';


    // Special case: single builtin (no pipes)
    if (builtin && n == 1)
      return run_builtin(command, args, env);

    pid_t pid = fork();
    if (pid == 0) {
      // --- CHILD ---

      // Setup redirections
      if (i > 0) dup2(pipes[(i - 1) * 2], STDIN_FILENO);
      if (i < n - 1) dup2(pipes[i * 2 + 1], STDOUT_FILENO);
      for (int fd : pipes) close(fd);

      if (builtin)
        return run_builtin(command, args, env);
      else {
        auto argv = make_argv(commands[i][0], commands[i][1]);
        execve(argv[0], argv.data(), env.to_envp().data());
        perror("execve");
        return 127;
      }
    }
    pids.push_back(pid);
  }

  for (int fd : pipes) close(fd);

  int status;
  for (pid_t pid : pids) {
    if (waitpid(pid, &status, 0) < 0) {
      perror("waitpid failed");
      return -1;
    }
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);  // last child exit code
  } else {
    return -1;
  }
}

int run_command(const fs::path &command, 
                const std::string &args, 
                Env &env)
{
  std::string name = command.filename().string();
  if (DEBUG)
    std::clog << "Running: " << command << " " << args << '\n';

  if (is_builtin(command)) return run_builtin(command, args, env);

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return -1;
  }

  if (pid == 0) {
    // Child: exec command //
    execve(command.string().c_str(), make_argv(name, args).data(), env.to_envp().data());
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

std::vector<vec_str> readyCommands(const omap_str& commands, Env& env) {
  std::vector<vec_str> outCmds {};
  outCmds.reserve(commands.size() + 1);

  for (const auto &[cmd, args] : commands) {
    fs::path path { cmd };
    if (!fs::exists(path) && !is_builtin(path))
      path = env.getFromPath(path);

    if (!fs::exists(path) && !is_builtin(path) && cmd.substr(0, 15) != "nova_internal_") {
      std::cerr << "No command " << path << " Found, Did you mean:\n";
      return {};
    } else outCmds.push_back({ path, args });
  }

  return outCmds;
}

void execute(Lexer &lex, Env &env) {
  while (!lex.eof()) {
    vec_tok tokens = lex.tokenize_line();

    if (tokens.empty()) continue;
    if (DEBUG) for (auto& t : tokens) std::cout << t;

    omap_str ast = parse(tokens, env);

    auto type = ast["type"];
    if (type == "command") {
      auto commands { readyCommands(ast.slice(1, ast.size()), env) };
      if (!commands.empty())
        run_command(commands[0][0], commands[0][1], env);
    }
    else if (type == "pipe") {
      auto commands = readyCommands(ast.slice(1, ast.size()), env);
      if (!commands.empty())
        run_pipeline(commands, env);
    }
  }
}

