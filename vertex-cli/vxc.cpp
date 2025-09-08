#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <vector>

namespace fs = std::filesystem;

// Function to get the directory of the current executable
fs::path getExecutablePath() {
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return fs::path(buffer).parent_path();
    }
    return fs::path(".");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: vxc [category] [command] [options]" << std::endl;
        return 1;
    }

    // Get the installation directory dynamically
    fs::path vxcDir = getExecutablePath();
    fs::path modulesDir = vxcDir / ".." / "modules";

    // Construct the module path
    std::string category = argv[1];
    std::string command = argv[2];
    fs::path modulePath = modulesDir / category / command;

    if (!fs::exists(modulePath)) {
        std::cerr << "Error: Command '" << category << " " << command << "' not found." << std::endl;
        return 1;
    }

    // Prepare command execution
    std::vector<const char*> args;
    args.push_back(modulePath.c_str());  // Module executable
    for (int i = 3; i < argc; ++i) {
        args.push_back(argv[i]);  // Pass all arguments
    }
    args.push_back(nullptr);  // Null-terminate for execv

    // Execute the module
    execv(modulePath.c_str(), const_cast<char* const*>(args.data()));

    // If execv fails
    std::cerr << "Error: Failed to execute module '" << modulePath << "'." << std::endl;
    return 1;
}
