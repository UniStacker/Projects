#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <algorithm>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>

namespace fs = std::filesystem;

// ANSI color codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

// Function to print a progress bar
void showProgressBar(double progress) {
    static int lastPercentage = -1; // Store last printed percentage

    int percentage = static_cast<int>(progress * 100.0);
    if (percentage == lastPercentage) return; // Skip redundant updates
    lastPercentage = percentage;

    int barWidth = 40;
    int pos = static_cast<int>(barWidth * progress);

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << GREEN << "=" << RESET;
        else if (i == pos) std::cout << GREEN << ">" << RESET;
        else std::cout << " ";
    }
    std::cout << "] " << percentage << "%\r";
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
    std::regex partRegex(R"(.*\.part_(\d+)$)");
    std::smatch match;
    if (std::regex_match(filename, match, partRegex)) {
        return std::stoi(match[1]);
    }
    return -1; // Return -1 for files that don't match the pattern
}

// Determine the output filename from prefix or directory
std::string determineOutputFilename(const std::string& inputPath) {
    fs::path input = fs::path(inputPath).lexically_normal(); // Normalize path

    if (fs::is_directory(input)) {
        std::string name = input.filename().string();
        if (name.size() > 6 && name.substr(name.size() - 6) == "_parts") {
            return (input.parent_path() / name.substr(0, name.size() - 6)).string();
        }
        return (input.parent_path() / name).string(); // Default to directory name
    } else {
        std::string name = input.filename().string();
        if (name.size() > 6 && name.substr(name.size() - 6) == ".part_") {
            return (input.parent_path() / name.substr(0, name.size() - 6)).string();
        }
        return (input.parent_path() / name).string(); // Fallback: use given prefix
    }
}

std::string computeSHA256(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << RED << "Error: Cannot open file for hashing: " << filename << RESET << std::endl;
        return "";
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();  // Create context
    if (!ctx) {
        std::cerr << RED << "Error: Failed to create OpenSSL digest context!" << RESET << std::endl;
        return "";
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        std::cerr << RED << "Error: Failed to initialize SHA-256 digest!" << RESET << std::endl;
        EVP_MD_CTX_free(ctx);
        return "";
    }

    std::vector<char> buffer(4096); // 4KB buffer
    while (file) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            if (EVP_DigestUpdate(ctx, buffer.data(), bytesRead) != 1) {
                std::cerr << RED << "Error: Failed to update SHA-256 digest!" << RESET << std::endl;
                EVP_MD_CTX_free(ctx);
                return "";
            }
        }
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    if (EVP_DigestFinal_ex(ctx, hash, nullptr) != 1) {
        std::cerr << RED << "Error: Failed to finalize SHA-256 digest!" << RESET << std::endl;
        EVP_MD_CTX_free(ctx);
        return "";
    }

    EVP_MD_CTX_free(ctx);  // Clean up OpenSSL context

    std::ostringstream hexStream;
    for (unsigned char byte : hash) {
        hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return hexStream.str();
}

void verifyIntegrity(const std::string& mergedFilename) {
    std::string mergedHash = computeSHA256(mergedFilename);
    std::string hashFile = "." + mergedFilename + ".sha256";

    if (fs::exists(hashFile)) {
        std::ifstream hashIn(hashFile);
        std::string originalHash;
        hashIn >> originalHash;

        if (mergedHash == originalHash) {
            std::cout << GREEN << "Integrity Check Passed: Merged file matches original!" << RESET << std::endl;
        } else {
            std::cerr << RED << "Error: Integrity Check Failed! Merged file is corrupted." << RESET << std::endl;
        }
    } else {
        std::cout << YELLOW << "Warning: No original hash found. Do you want to create a hash for future checks? ('y' for yes): " << RESET;
	char ch = std::cin.get();
	if  (ch != 'y') return;
	std::ofstream hashOut(hashFile);
        hashOut << mergedHash;
	std::cout << GREEN << "Hash file created, successfully." << RESET << std::endl;
    }
}

// Function to merge file parts
void mergeFiles(const std::string& inputPath) {
    // Trim trailing slashes from inputPath
    std::string cleanInputPath = inputPath;
    while (!cleanInputPath.empty() && cleanInputPath.back() == '/') {
        cleanInputPath.pop_back();
    }

    std::string outputFilename = determineOutputFilename(cleanInputPath);

    std::ofstream output(outputFilename, std::ios::binary | std::ios::app); // Open in append mode
    if (!output) {
        std::cerr << RED << "Error: Cannot open output file for writing." << RESET << std::endl;
        return;
    }

    // Get the existing output file size (resuming merge if interrupted)
    size_t existingSize = fs::exists(outputFilename) ? fs::file_size(outputFilename) : 0;
    output.seekp(existingSize);  // Ensure appending at correct position

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

    size_t processedSize = existingSize; // Start tracking from the actual existing size
    std::vector<char> buffer(4096); // 4KB buffer
    size_t updateThreshold = 1048576; // 1MB
    size_t nextUpdate = processedSize + updateThreshold;

    for (const auto& file : partFiles) {
        if (processedSize >= totalSize) break; // Ensure we don't write extra

        if (fs::file_size(file) <= existingSize) {
            continue; // Skip parts that are already merged
        }

        std::ifstream input(file, std::ios::binary);
        if (!input) {
            std::cerr << RED << "Error: Cannot open part file " << file << RESET << std::endl;
            return;
        }

        while (input) {
            input.read(buffer.data(), buffer.size());
            std::streamsize bytesRead = input.gcount();
            if (bytesRead > 0) {
                output.write(buffer.data(), bytesRead);
                processedSize += bytesRead;

                if (processedSize >= nextUpdate) {
                    showProgressBar(double(processedSize) / totalSize);
                    nextUpdate = processedSize + updateThreshold;
                }
            }
        }
        input.close();
    }

    output.close();

    // Final size check
    size_t finalSize = fs::file_size(outputFilename);
    if (finalSize != totalSize) {
        std::cerr << RED << "Error: Merged file size mismatch! Expected: "
                  << totalSize << " bytes, Got: " << finalSize << " bytes" << RESET << std::endl;
        return;
    }

    // Delete the directory if applicable
    if (fs::is_directory(inputPath)) {
        fs::remove_all(inputPath);
    } else {
        for (const auto& file : partFiles) {
            fs::remove(file);
        }
    }

    std::cout << std::endl << GREEN << "Merge completed! File saved as: " << outputFilename << RESET << std::endl;
    verifyIntegrity(outputFilename);
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
