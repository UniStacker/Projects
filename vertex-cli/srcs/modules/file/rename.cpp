#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

// ANSI Color Codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define CYAN    "\033[36m"

// Convert shell-style wildcard pattern to a regex pattern
std::string wildcardToRegex(const std::string& pattern) {
    std::string regexPattern = std::regex_replace(pattern, std::regex(R"(\.)"), R"(\.)");  // Escape `.`
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\*)"), R"(.*)");         // Convert `*`
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\?)"), R"(.{1})");      // Convert `?`
    return regexPattern;
}

// Function to print help message
void printHelp() {
    std::cout << BOLD << "\nUsage: rename [pattern] [option] [arguments]\n\n" << RESET;
    std::cout << "Options:\n";
    std::cout << "  " << BOLD << "--prefix    (-pre)" << RESET << "   Add a prefix to matching files\n";
    std::cout << "  " << BOLD << "--suffix    (-suf)" << RESET << "   Add a suffix to matching files\n";
    std::cout << "  " << BOLD << "--replace   (-rep)" << RESET << "   Replace text in filenames\n";
    std::cout << "  " << BOLD << "--sequence  (-seq)" << RESET << "   Rename files sequentially\n";
    std::cout << "  " << BOLD << "--extension (-ext)" << RESET << "   Change file extensions\n\n";
    std::cout << "Examples:\n";
    std::cout << "  rename \"*.txt\" --prefix \"new_\"        → new_file1.txt, new_file2.txt\n";
    std::cout << "  rename \"image*.jpg\" -suf \"_backup\"  → image1_backup.jpg\n";
    std::cout << "  rename \"data_*.csv\" -rep \"data_\" \"report_\"  → report_1.csv\n";
    std::cout << "  rename \"photo_*.png\" -seq \"img_\" 3   → img_001.png, img_002.png\n";
    std::cout << "  rename \"*.log\" --extension \"bak\"    → error.bak, system.bak\n";
}

// Function to rename files with prefix
void renameWithPrefix(const std::string& pattern, const std::string& prefix) {
    std::string regexPattern = wildcardToRegex(pattern);
    std::regex regexMatch(regexPattern);
    bool renamed = false;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::string filename = entry.path().filename().string();
        if (std::regex_match(filename, regexMatch)) {
            std::string new_name = prefix + filename;
            fs::rename(entry.path(), entry.path().parent_path() / new_name);
            std::cout << GREEN << "Renamed: " << RESET << filename << " → " << CYAN << new_name << RESET << "\n";
            renamed = true;
        }
    }

    if (!renamed) std::cout << YELLOW << "No files matched the pattern.\n" << RESET;
}

// Function to rename files with suffix
void renameWithSuffix(const std::string& pattern, const std::string& suffix) {
    std::string regexPattern = wildcardToRegex(pattern);
    std::regex regexMatch(regexPattern);
    bool renamed = false;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::string filename = entry.path().filename().string();
        if (std::regex_match(filename, regexMatch)) {
            std::string new_name = filename.substr(0, filename.find_last_of('.')) + suffix + entry.path().extension().string();
            fs::rename(entry.path(), entry.path().parent_path() / new_name);
            std::cout << GREEN << "Renamed: " << RESET << filename << " → " << CYAN << new_name << RESET << "\n";
            renamed = true;
        }
    }

    if (!renamed) std::cout << YELLOW << "No files matched the pattern.\n" << RESET;
}

// Function to replace text in filenames
void renameWithReplace(const std::string& pattern, const std::string& oldText, const std::string& newText) {
    std::string regexPattern = wildcardToRegex(pattern);
    std::regex regexMatch(regexPattern);
    bool renamed = false;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::string filename = entry.path().filename().string();
        if (std::regex_match(filename, regexMatch)) {
            std::string new_name = std::regex_replace(filename, std::regex(wildcardToRegex(oldText)), newText);
            fs::rename(entry.path(), entry.path().parent_path() / new_name);
            std::cout << GREEN << "Renamed: " << RESET << filename << " → " << CYAN << new_name << RESET << "\n";
            renamed = true;
        }
    }

    if (!renamed) std::cout << YELLOW << "No files matched the pattern.\n" << RESET;
}

// Function for sequential renaming
void renameWithSequence(const std::string& pattern, const std::string& baseName, int digits) {
    std::string regexPattern = wildcardToRegex(pattern);
    std::regex regexMatch(regexPattern);
    int count = 1;
    bool renamed = false;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::string filename = entry.path().filename().string();
        if (std::regex_match(filename, regexMatch)) {
            std::ostringstream new_name;
            new_name << baseName << std::setw(digits) << std::setfill('0') << count++ << entry.path().extension().string();
            fs::rename(entry.path(), entry.path().parent_path() / new_name.str());
            std::cout << GREEN << "Renamed: " << RESET << filename << " → " << CYAN << new_name.str() << RESET << "\n";
            renamed = true;
        }
    }

    if (!renamed) std::cout << YELLOW << "No files matched the pattern.\n" << RESET;
}

// Function to change file extensions
void renameWithExtension(const std::string& pattern, const std::string& newExt) {
    std::string regexPattern = wildcardToRegex(pattern);
    std::regex regexMatch(regexPattern);
    bool renamed = false;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::string filename = entry.path().filename().string();
        if (std::regex_match(filename, regexMatch)) {
            std::string new_name = filename.substr(0, filename.find_last_of('.')) + "." + newExt;
            fs::rename(entry.path(), entry.path().parent_path() / new_name);
            std::cout << GREEN << "Renamed: " << RESET << filename << " → " << CYAN << new_name << RESET << "\n";
            renamed = true;
        }
    }

    if (!renamed) std::cout << YELLOW << "No files matched the pattern.\n" << RESET;
}

int main(int argc, char* argv[]) {
    if (argc < 3 || std::string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }

    std::string pattern = argv[1];
    std::string option = argv[2];

    if (option == "--prefix" || option == "-pre") renameWithPrefix(pattern, argv[3]);
    else if (option == "--suffix" || option == "-suf") renameWithSuffix(pattern, argv[3]);
    else if (option == "--replace" || option == "-rep") renameWithReplace(pattern, argv[3], argv[4]);
    else if (option == "--sequence" || option == "-seq") renameWithSequence(pattern, argv[3], std::stoi(argv[4]));
    else if (option == "--extension" || option == "-ext") renameWithExtension(pattern, argv[3]);
    else std::cerr << RED << "Invalid option! Use --help for usage.\n" << RESET;

    return 0;
}
