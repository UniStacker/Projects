#include "utils/env.h"

Env::Env(char **envp) {
    for (size_t i = 0; envp[i] != nullptr; i++) {
        std::string entry(envp[i]);
        size_t pos = entry.find('=');
        if (pos != std::string::npos) {
            vars[entry.substr(0, pos)] = entry.substr(pos + 1);
        }
    }
}

std::string Env::get(const std::string &key) const {
    auto it = vars.find(key);
    return (it != vars.end()) ? it->second : "";
}

void Env::set(const std::string &key, const std::string &value) {
    vars[key] = value;
}

void Env::unset(const std::string &key) {
    vars.erase(key);
}

void Env::print() const {
    for (auto &kv : vars) {
        std::cout << kv.first << "=" << kv.second << "\n";
    }
}

std::vector<char*> Env::to_envp() const {
    std::vector<char*> envp;
    envp.reserve(vars.size() + 1);

    for (auto &kv : vars) {
        std::string entry = kv.first + "=" + kv.second;
        char *cstr = strdup(entry.c_str()); // required for execve
        envp.push_back(cstr);
    }

    envp.push_back(nullptr);
    return envp;
}
