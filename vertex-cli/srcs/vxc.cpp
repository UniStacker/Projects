#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <unistd.h>  // For execv and readlink
#include <limits>    // For minimum distance comparison

namespace fs = std::filesystem;

// ANSI color codes for better terminal output
#define COLOR_RED "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

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

// Levenshtein Distance Function (Measures String Similarity)
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
        }
    }
    return dp[len1][len2];
}

// Find the closest matching command in the modules directory
std::string findClosestCommand(const fs::path& modulesDir, const std::string& category, const std::string& command) {
    int minDistance = std::numeric_limits<int>::max();
    std::string closestMatch;

    fs::path categoryPath = modulesDir / category;
    if (fs::exists(categoryPath) && fs::is_directory(categoryPath)) {
        for (const auto& entry : fs::directory_iterator(categoryPath)) {
            if (entry.is_regular_file()) {
                std::string existingCommand = entry.path().filename().string();
                int distance = levenshteinDistance(command, existingCommand);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestMatch = existingCommand;
                }
            }
        }
    }

    return (minDistance <= 2) ? closestMatch : "";  // Only suggest if it's a close match
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << COLOR_YELLOW "Usage: vxc [category] [command] [options]" COLOR_RESET << std::endl;
        return 1;
    }

    // Get the installation directory dynamically
    fs::path vxcDir = getExecutablePath();
    fs::path modulesDir = vxcDir / ".." / "modules";

    // Extract category and command
    std::string category = argv[1];
    std::string command = argv[2];
    fs::path modulePath = modulesDir / category / command;

    // Check if the command exists
    if (!fs::exists(modulePath)) {
        std::cerr << COLOR_RED "Error: Command '" << category << " " << command << "' not found." COLOR_RESET << std::endl;

        // Try to suggest a similar command
        std::string suggestion = findClosestCommand(modulesDir, category, command);
        if (!suggestion.empty()) {
            std::cerr << COLOR_GREEN "Did you mean: 'vxc " << category << " " << suggestion << "'?" COLOR_RESET << std::endl;
        }

        return 1;
    }

    // Ensure it's an executable
    if (!fs::is_regular_file(modulePath) || access(modulePath.c_str(), X_OK) != 0) {
        std::cerr << COLOR_RED "Error: '" << modulePath.filename() << "' is not executable or lacks permissions." COLOR_RESET << std::endl;
        return 1;
    }

    // Prepare command execution arguments
    std::vector<const char*> args;
    args.push_back(modulePath.c_str());  // Module executable
    for (int i = 3; i < argc; ++i) {
        args.push_back(argv[i]);  // Pass all arguments
    }
    args.push_back(nullptr);  // Null-terminate for execv

    // Execute the module
    execv(modulePath.c_str(), const_cast<char* const*>(args.data()));

    // If execv fails, handle error
    std::cerr << COLOR_RED "Error: Failed to execute module '" << modulePath << "'." COLOR_RESET << std::endl;
    perror("execv");
    return 1;
}
