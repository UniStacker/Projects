#pragma once
#include <filesystem>
#include <regex>
#include <iostream>

class Env;

namespace fs = std::filesystem;
namespace utils {
  fs::path parse_path(const fs::path &input, const Env &env);
}
