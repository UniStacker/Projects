#include "core/executer.h"


std::vector<char*> make_argv(const std::string &filename,
                             const vec_str &args) {
  std::vector<char*> argv;
  argv.reserve(args.size() + 2);

  argv.push_back(strdup(filename.c_str()));   // copy program name

  for (auto& arg : args) {
    argv.push_back(strdup(arg.c_str()));  // copy each arg
  }
  argv.push_back(nullptr);

  return argv;
}

int cmd_cd(const vec_str &args, Env &env) {
  fs::path rpath = args[0];
  if (rpath.is_relative()) rpath = fs::current_path() / rpath;

  if (rpath.empty()) return cmd_cd({env.get("HOME")}, env);
  else if (fs::exists(rpath) && fs::is_directory(rpath)) {
    if (env.get("OLDPWD") != env.get("PWD")) env.set("OLDPWD", env.get("PWD"));
    chdir(rpath.string().c_str());
    env.set("PWD", fs::current_path().string());
    return 0;
  } 
  else if (fs::exists(rpath)) {
    std::cerr << "cd: Path is not a dir '" << rpath.string() << "'\n";
    return 1;
  }
  else std::cerr << "cd: Path does not exist '" << rpath.string() << "'\n";
  return 2;
}

int cmd_pwd(const Env& env) {
  auto cwd = env.get("PWD") + '\n';
  write(STDOUT_FILENO, cwd.c_str(), cwd.length());
  return 0;
}

void cmd_exit() {
  exit(0);
}

int run_builtin(const std::string &cmd, const vec_str &args, Env &env) {
  if (cmd == "cd")
    return cmd_cd(args, env);
  else if (cmd == "pwd")
    return cmd_pwd(env);
  else if (cmd == "exit")
    cmd_exit();

  return -1;
}

bool redirect_fd(const int &src, const int &target) {
  if (dup2(src, target) < 0) {
    std::cerr << "Nova: couldn't duplicate fd: " << src << " -> " << target << '\n';
    close(src);
    return false;
  }
  close(src);
  return true;
}

bool redirect_file(const std::string &path, int target, bool append=false, bool input=false) {
  int flags = O_WRONLY | (append ? O_APPEND : O_CREAT | O_TRUNC);

  int fd;
  if (!input)
    fd = open(path.c_str(), flags, 0644);
  else
    fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    std::cerr << "Nova: couldn't open file: " << path << '\n';
    return false;
  }
  
  return redirect_fd(fd, target);
}


std::string getFullCommand(const std::string& command, const Env& env) {
  fs::path path { command };
  if (!fs::exists(path) && !is_builtin(path))
    path = env.getFromPath(path);

  if (!fs::exists(path) && !is_builtin(path)) {
    std::cerr << "Nova: No command " << path << " Found, Did you mean:\n";
    return {};
  } return path;
}

int run_command(ExecNode &node, Env &env) {
  bool isBasic = !node.pipe;
  std::vector<int> pipes;

  std::vector<pid_t> pids {};

  ExecNode cmdNode = std::move(node);
  for (size_t i = 0; ; i++) {
    auto command = getFullCommand(cmdNode.command, env);
    auto args = cmdNode.args;
    auto name = fs::path(command).filename().string();
    bool builtin = is_builtin(command);

    if (DEBUG) {
      std::clog << "Running: " << command << " ";
      for (const auto& arg : args) std::cout << arg << ' ';
      std::cout << '\n';
    }


    // Special case: single builtin (no pipes)
    if (builtin && isBasic)
      return run_builtin(command, args, env);

    pid_t pid = fork();
    if (pid == 0) {
      // --- CHILD ---

      // Setup redirections
      for (const auto& [src, target] : cmdNode.redirects) {
             if (src == "write")  redirect_file(target, STDOUT_FILENO);
        else if (src == "append") redirect_file(target, STDOUT_FILENO, true);
        else if (src == "read")   redirect_file(target, STDIN_FILENO, false, true);
        else redirect_fd(stoi(src), stoi(target));
      }

      if (cmdNode.pipe) {
        pipes.push_back(0); pipes.push_back(0);
        if (pipe(&pipes[i * 2]) == -1) {
          std::cerr << "Nova: couldn't create a pipe" << '\n';
          return 127;
        }
        if (i > 0) dup2(pipes[(i - 1) * 2], STDIN_FILENO);
        if (cmdNode.pipe) dup2(pipes[i * 2 + 1], STDOUT_FILENO);
      }
      for (int fd : pipes) close(fd);

      if (builtin)
        return run_builtin(command, args, env);
      else {
        auto argv = make_argv(command, args);
        execve(argv[0], argv.data(), env.to_envp().data());
        std::cerr << "Nova: couldn't execv command: " << command << '\n';
        return 127;
      }
    }
    pids.push_back(pid);
    if (cmdNode.pipe)
      cmdNode = std::move(*static_cast<ExecNode*>(cmdNode.pipe.get()));
    else break;
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

// =======================
//     Visitor Iterface
// =======================
struct Interpreter : Visitor {
  Env& env;
  
  Interpreter(Env &env) : env(env)
  {}
  
  // Node interpreters
  void visit(ExecNode &node) override {
    run_command(node, env);
  }
};

void execute(Lexer &lex, Env &env) {
  while (!lex.eof()) {
    vec_tok tokens = lex.tokenize_line();

    if (tokens.empty()) continue;
    if (DEBUG) for (auto& t : tokens) std::cout << t;

    auto ast = parse(tokens, env);
    Interpreter intp(env);
    ast.traverse(intp);
  }
}

