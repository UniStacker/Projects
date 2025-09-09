#pragma once
#include <string>
#include <iostream>
#include <cstring>   // for strdup
#include "utils/types.h"
#include "utils/path.h"
#include "utils/string.h"

class Env {
private:
  map_str vars {};

public:
  // Construct from char** envp
  Env(char **envp);

  // Get value (returns empty string if not found)
  std::string get(const std::string &key) const;

  // Set or overwrite key=value
  void set(const std::string &key, const std::string &value);

  // Remove a variable
  void unset(const std::string &key);

  // Print all environment variables
  void print() const;

  // Export as char** for execve
  std::vector<char*> to_envp() const;

  // Get the absolute path of a program from the paths stored in $PATH
  std::string getFromPath(const std::string& program);
};
