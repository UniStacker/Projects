#include <iostream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <openssl/sha.h>
#include <vector>

namespace fs = std::filesystem;

// ANSI Escape Codes for Colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

// Function to compute SHA-256 hash of a file
std::string computeHash(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << RED << "Error: Unable to read file: " << filePath << RESET << "\n";
        return "";
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    SHA256_Update(&sha256, buffer, file.gcount()); // Final chunk

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::string hashStr;
    for (unsigned char i : hash) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", i);
        hashStr += buf;
    }
    return hashStr;
}

// Function to scan directory and find duplicates
std::unordered_map<std::string, std::vector<std::string>> findDuplicates(const std::string& directory) {
    std::unordered_map<std::string, std::vector<std::string>> hashToFileMap;

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_regular_file(entry.path())) {
            std::string hash = computeHash(entry.path().string());
            if (!hash.empty()) {
                hashToFileMap[hash].push_back(entry.path().string());
            }
        }
    }
    return hashToFileMap;
}

// Function to display duplicate files
void displayDuplicates(const std::unordered_map<std::string, std::vector<std::string>>& duplicates) {
    int totalDuplicates = 0;

    for (const auto& [hash, files] : duplicates) {
        if (files.size() > 1) {
            totalDuplicates += files.size() - 1;
            std::cout << BLUE << "\nDuplicate Group (Hash: " << hash << ")\n" << RESET;
            for (const auto& file : files) {
                std::cout << "  " << YELLOW << file << RESET << "\n";
            }
        }
    }
    std::cout << GREEN << "\nTotal duplicate files found: " << totalDuplicates << RESET << "\n";
}

// Function to delete duplicate files (with confirmation)
void deleteDuplicates(std::unordered_map<std::string, std::vector<std::string>>& duplicates) {
    std::cout << "\nDelete Options:\n";
    std::cout << "1 - Delete all duplicates (Keep one copy per group)\n";
    std::cout << "2 - Select files manually\n";
    std::cout << "3 - Cancel\n";

    int choice;
    std::cout << "Enter choice: ";
    std::cin >> choice;

    if (choice == 3) {
        std::cout << "Deletion canceled.\n";
        return;
    }

    for (auto& [hash, files] : duplicates) {
        if (files.size() > 1) {
            if (choice == 1) {
                for (size_t i = 1; i < files.size(); ++i) { // Keep first, delete others
                    std::cout << RED << "Deleting: " << files[i] << RESET << "\n";
                    fs::remove(files[i]);
                }
            } else if (choice == 2) {
                std::cout << "\nDuplicate Group (Hash: " << hash << ")\n";
                for (size_t i = 1; i < files.size(); ++i) { // Show duplicates except first one
                    std::cout << i << " - " << files[i] << "\n";
                }

                std::cout << "Enter numbers to delete (comma-separated) or 'all' to delete all except one: ";
                std::string input;
                std::cin >> input;

                if (input == "all") {
                    for (size_t i = 1; i < files.size(); ++i) {
                        std::cout << RED << "Deleting: " << files[i] << RESET << "\n";
                        fs::remove(files[i]);
                    }
                } else {
                    std::vector<int> indices;
                    std::stringstream ss(input);
                    std::string token;
                    while (std::getline(ss, token, ',')) {
                        indices.push_back(std::stoi(token));
                    }

                    for (int index : indices) {
                        if (index > 0 && index < files.size()) {
                            std::cout << RED << "Deleting: " << files[index] << RESET << "\n";
                            fs::remove(files[index]);
                        } else {
                            std::cout << YELLOW << "Invalid index: " << index << RESET << "\n";
                        }
                    }
                }
            }
        }
    }

    std::cout << GREEN << "Duplicate file deletion completed.\n" << RESET;
}

// Function to display help
void showHelp() {
    std::cout << BLUE << "\nUsage: vxc file duplicates <directory>\n" << RESET;
    std::cout << "Scans the specified directory and detects duplicate files based on SHA-256 hashing.\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h   Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << RED << "Usage: vxc file duplicates <directory>\n" << RESET;
        return 1;
    }

    std::string directory = argv[1];

    // Handle help option
    if (directory == "--help" || directory == "-h") {
        showHelp();
        return 0;
    }

    // Check if the directory is valid
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << RED << "Error: Invalid directory\n" << RESET;
        return 1;
    }

    std::cout << BLUE << "Scanning directory: (" << directory << ")...\n" << RESET;
    auto duplicates = findDuplicates(directory);

    if (duplicates.empty()) {
        std::cout << GREEN << "No duplicate files found.\n" << RESET;
        return 0;
    }

    displayDuplicates(duplicates);

    std::cout << "\nOptions:\n";
    std::cout << "1 - Delete duplicate files\n";
    std::cout << "2 - Exit\n";

    int choice;
    std::cout << "Enter choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            deleteDuplicates(duplicates);
            break;
        case 2:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << RED << "Invalid choice.\n" << RESET;
    }

    return 0;
}
