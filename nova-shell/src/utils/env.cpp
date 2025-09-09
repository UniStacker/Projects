#include "utils/env.h"

Env::Env(char **envp) {
    for (size_t i = 0; envp[i] != nullptr; i++) {
        std::string entry(envp[i]);
        size_t pos = entry.find('=');
        if (pos != std::string::npos) {
            this->vars[entry.substr(0, pos)] = entry.substr(pos + 1);
        }
    }
}

std::string Env::get(const std::string &key) const {
    auto it = this->vars.find(key);
    return (it != this->vars.end()) ? it->second : "";
}

void Env::set(const std::string &key, const std::string &value) {
    this->vars[key] = value;
}

void Env::unset(const std::string &key) {
    this->vars.erase(key);
}

void Env::print() const {
    for (auto &kv : this->vars) {
        std::cout << kv.first << "=" << kv.second << "\n";
    }
}

std::vector<char*> Env::to_envp() const {
    std::vector<char*> envp;
    envp.reserve(this->vars.size() + 1);

    for (auto &kv : this->vars) {
        std::string entry = kv.first + "=" + kv.second;
        char *cstr = strdup(entry.c_str()); // required for execve
        envp.push_back(cstr);
    }

    envp.push_back(nullptr);
    return envp;
}

std::string Env::getFromPath(const std::string &program) {
  auto path_var = this->get("PATH");
  auto paths = utils::split_string(path_var, ":");

  for (auto& path : paths) {
    fs::path p = utils::parse_path(path, *this) / program;
    if (fs::exists(p)) return p.string();
  }

  return "";
}
