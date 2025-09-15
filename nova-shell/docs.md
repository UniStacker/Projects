# Nova Shell

Nova Shell is a modern, cross-platform command-line shell written in C++. It aims to provide a user-friendly and powerful alternative to traditional shells like bash and zsh.

## Tasks

- [x] **Core Shell Functionality**
  - [x] Interactive command-line interface (REPL).
  - [x] Execution of external commands.
  - [x] Support for command-line arguments.
  - [x] Input from files (`nova <filename>`).
  - [x] Input from string (`nova -c <command>`).

- [x] **Lexer and Parser**
  - [x] Tokenization of input into strings, operators, and separators.
  - [x] Handling of single and double quoted strings.
  - [x] Basic parsing of commands and arguments.

- [x] **Command Execution**
  - [x] Forking and executing external commands using `execve`.
  - [x] Waiting for child processes to complete.
  - [x] Handling of command pipelines (`|`).

- [x] **Built-in Commands**
  - [x] `cd`: Change directory.
  - [x] `pwd`: Print working directory.
  - [x] `exit`: Terminate the shell.

- [x] **Environment Variable Management**
  - [x] Loading environment variables from the parent process.
  - [x] `get`, `set`, and `unset` environment variables.
  - [x] Passing environment variables to child processes.

- [x] **Path Resolution**
  - [x] Expansion of `~` to the user's home directory.
  - [x] Expansion of environment variables in paths (e.g., `$HOME`).
  - [x] Searching for executables in the `PATH`.

- [x] **Prompt**
  - [x] Customizable prompt with support for `%u` (user), `%h` (host), and `%c` (current directory).

- [ ] **Job Control**
  - [ ] Implement background processes (`&`).
  - [ ] `jobs`, `fg`, `bg` commands.
  - [ ] Process suspension (Ctrl+Z).
- [ ] **Redirection**
  - [ ] Input redirection (`<`).
  - [ ] Output redirection (`>`, `>>`).
  - [ ] Error redirection (`2>`, `2>>`).
- [ ] **Globbing**
  - [ ] Wildcard expansion (`*`, `?`, `[]`).
- [ ] **History**
  - [ ] Command history (`history` command).
  - [ ] Up/down arrow to navigate history.
  - [ ] History expansion (`!!`, `!n`).
- [ ] **Tab Completion**
  - [ ] Basic tab completion for commands and file paths.

- [ ] **Scripting**
  - [ ] Support for shell scripts with shebang (`#!/usr/bin/env nova`).
  - [ ] Variables and control structures (`if`, `else`, `while`, `for`).
  - [ ] Functions.
- [ ] **Aliases**
  - [ ] `alias` and `unalias` commands.
- [ ] **More Built-ins**
  - [ ] `echo`, `export`, `unset`, `source`, `history`.
- [ ] **Signal Handling**
  - [ ] Proper handling of signals like `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z).

- [ ] **Advanced Tab Completion**
  - [ ] Context-aware completion (e.g., completing git commands).
  - [ ] Menu-based completion.
- [ ] **Plugin System**
  - [ ] Allow users to extend the shell with plugins.
- [ ] **Syntax Highlighting**
  - [ ] Real-time syntax highlighting of the command line.
- [ ] **Autosuggestions**
  - [ ] Suggest commands based on history and context (like fish).
- [ ] **Theming**
  - [ ] Customizable colors and themes.
- [ ] **Cross-Platform Support**
  - [ ] Ensure the shell works on Linux, macOS, and Windows (with WSL).
- [ ] **Performance**
  - [ ] Optimize the shell for speed and low resource usage.
- [ ] **Modern UI/UX**
  - [ ] A more user-friendly and intuitive interface.
  - [ ] Better error messages.
