#include "utils/path.h"
#include "utils/env.h"

std::string replaceWithEnv(const std::string &input, const Env &env) {
  using namespace std;

  regex envPattern(R"(\$(\w+))");
  string result {};
  auto begin = sregex_iterator(input.begin(), input.end(), envPattern);
  auto end   = sregex_iterator();

  size_t lastPos { 0 };
  for (auto it = begin; it != end; ++it) {
    smatch m { *it };
    // append everything before match
    result.append(input, lastPos, m.position() - lastPos);
    // append replacement
    result += env.get(m.str(1));
    // move past this match
    lastPos = m.position() + m.length();
  }
  // append the rest of the string
  result.append(input, lastPos, string::npos);

  return result;
}

std::string replaceWithRun(const std::string &input) {
  using namespace std;

  regex runPattern(R"(\$\(([^)]+)\))");
  string result {};
  auto begin = sregex_iterator(input.begin(), input.end(), runPattern);
  auto end   = sregex_iterator();

  size_t lastPos { 0 };
  for (auto it = begin; it != end; ++it) {
    smatch m { *it };
    result.append(input, lastPos, m.position() - lastPos);
    result += "run(\"" + m.str(1) + "\")";
    lastPos = m.position() + m.length();
  }
  result.append(input, lastPos, string::npos);

  return result;
}

fs::path utils::parse_path(const fs::path &input, const Env &env) {
  std::string result { replaceWithRun(input.string()) };
  result = replaceWithEnv(result, env);

  std::string path_str { result };
  fs::path path = { path_str };
  auto pre = path.string().substr(0, 2);

  // Replace '~' and relative paths
  if (path_str[0] == '~') path = env.get("HOME") + '/' + path_str.substr(1); else
  if (path.is_relative()) {
    auto resolved = path.string();
    if (pre == "./" || pre == "../")
      resolved = parse_path(env.get("PWD") / path, env);
    if (fs::exists(resolved)) path = resolved;
  }

  path = path.lexically_normal().string();

  // std::cout << "Resolved path: " << path.string() << '\n';
  return path;
}

