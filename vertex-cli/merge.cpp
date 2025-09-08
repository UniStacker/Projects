#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <algorithm>

namespace fs = std::filesystem;

// ANSI color codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

// Function to print a progress bar
void showProgressBar(double progress) {
    int barWidth = 40;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << GREEN << "=" << RESET;
        else if (i == pos) std::cout << GREEN << ">" << RESET;
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%\r";
    std::cout.flush();
}

// Function to display help menu
void showHelp() {
    std::cout << CYAN << "Usage:\n"
              << YELLOW << "  merge <directory>\n"
              << "  merge <prefix>\n\n"
              << CYAN << "Example:\n"
              << YELLOW << "  merge myFile.txt_parts/\n"
              << "  merge myFile.txt.part_\n" << RESET << std::endl;
}

// Extract numerical suffix from filenames
int extractPartNumber(const std::string& filename) {
    std::regex numberRegex(R"((\d+)$)");
    std::smatch match;
    if (std::regex_search(filename, match, numberRegex)) {
        return std::stoi(match.str(1));
    }
    return -1;
}

// Determine the output filename from prefix or directory
std::string determineOutputFilename(const std::string& inputPath) {
    fs::path input(inputPath);

    if (fs::is_directory(input)) {
        // Remove "_parts/" if present
        std::string name = input.filename().string();
	size_t len = name.size();
	if (len > 0 && name[len-1] == '/') name = name.substr(0, len - 1);
        if (len > 6 && name.substr(len - 6) == "_parts") {
            return name.substr(0, len - 6);
        }
        return name; // Fallback: Use the directory name
    } else {
        // Prefix-based: Remove ".part_N"
        std::string name = input.filename().string();
        std::regex partRegex(R"(^(.*?)(\.part_\d+)$)");
        std::smatch match;
        if (std::regex_match(name, match, partRegex)) {
            return match.str(1);
        }
        return name; // Fallback: Use the given prefix
    }
}

// Function to merge file parts
void mergeFiles(const std::string& inputPath) {
    std::string outputFilename = determineOutputFilename(inputPath);
    std::ofstream output(outputFilename, std::ios::binary);
    if (!output) {
        std::cerr << RED << "Error: Cannot create output file " << outputFilename << RESET << std::endl;
        return;
    }

    std::vector<std::string> partFiles;

    if (fs::is_directory(inputPath)) {
        // Case: Directory input
        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.is_regular_file()) {
                partFiles.push_back(entry.path().string());
            }
        }
    } else {
        // Case: Prefix input
        fs::path parentDir = fs::path(inputPath).parent_path().empty() ? "." : fs::path(inputPath).parent_path();
        std::string prefix = fs::path(inputPath).filename().string();

        for (const auto& entry : fs::directory_iterator(parentDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.rfind(prefix, 0) == 0) { // Check if it starts with the prefix
                    partFiles.push_back(entry.path().string());
                }
            }
        }
    }

    if (partFiles.empty()) {
        std::cerr << RED << "Error: No part files found." << RESET << std::endl;
        return;
    }

    // Sort files numerically
    std::sort(partFiles.begin(), partFiles.end(), [](const std::string& a, const std::string& b) {
        return extractPartNumber(a) < extractPartNumber(b);
    });

    size_t totalSize = 0;
    for (const auto& file : partFiles) {
        totalSize += fs::file_size(file);
    }

    size_t processedSize = 0;
    std::vector<char> buffer(4096); // 4KB buffer
    for (const auto& file : partFiles) {
        std::ifstream input(file, std::ios::binary);
        if (!input) {
            std::cerr << RED << "Error: Cannot open part file " << file << RESET << std::endl;
            return;
        }

        while (!input.eof()) {
            input.read(buffer.data(), buffer.size());
            std::streamsize bytesRead = input.gcount();
            output.write(buffer.data(), bytesRead);
            processedSize += bytesRead;
            showProgressBar(double(processedSize) / totalSize);
        }
        input.close();

        // Delete part file after merging
        fs::remove(file);
    }

    // Delete the directory if applicable
    if (fs::is_directory(inputPath)) {
        fs::remove_all(inputPath);
    }

    std::cout << std::endl << GREEN << "Merge completed! File saved as: " << outputFilename << RESET << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        showHelp();
        return 1;
    }

    std::string inputPath = argv[1];

    if (!fs::exists(inputPath)) {
        std::cerr << RED << "Error: Input path does not exist." << RESET << std::endl;
        return 1;
    }

    mergeFiles(inputPath);

    return 0;
}
