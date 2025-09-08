#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>   // for strdup

class Env {
private:
  std::unordered_map<std::string, std::string> vars;

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
};
