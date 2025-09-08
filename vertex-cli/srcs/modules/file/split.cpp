#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iomanip>
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
              << YELLOW << "  split <filename> [--size || -s] <size>\n"
              << "  split <filename> [--parts || -p] <parts>\n\n"
              << CYAN << "Example:\n"
              << YELLOW << "  split largefile.bin --size 1M\n"
              << "  split document.pdf -p 10\n" << RESET;
}

// Function to convert size string (e.g., "1M", "512K") into bytes
size_t parseSize(const std::string& sizeStr) {
    size_t multiplier = 1;
    char unit = sizeStr.back();

    if (unit == 'K' || unit == 'k') multiplier = 1024;
    else if (unit == 'M' || unit == 'm') multiplier = 1024 * 1024;
    else if (unit == 'G' || unit == 'g') multiplier = 1024 * 1024 * 1024;
    else if (!isdigit(unit)) {
        std::cerr << RED << "Error: Invalid size format. Use K, M, or G for sizes." << RESET << std::endl;
        exit(1);
    }

    return std::stoll(sizeStr) * multiplier;
}

// Function to split file
void splitFile(const std::string& filename, size_t partSize, int numParts) {
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        std::cerr << RED << "Error: Cannot open file " << filename << RESET << std::endl;
        return;
    }

    input.seekg(0, std::ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);

    // Determine part size if splitting by number of parts
    if (numParts > 0) {
        partSize = fileSize / numParts;
        size_t remainder = fileSize % numParts;
    }

    // Validate part size
    if (partSize == 0 || partSize > fileSize) {
        std::cerr << RED << "Error: Invalid part size. File Size: " << fileSize << ", Part Size: " << partSize << RESET << std::endl;
        return;
    }

    // Create a directory for split
    std::string dirName = filename + "_parts";
    if (!fs::exists(dirName)) {
        if (!fs::create_directory(dirName)) {
            std::cerr << RED << "Error: Unable to create directory " << dirName << RESET << std::endl;
            return;
        }
    }

    size_t totalRead = 0;
    int partNum = 0;

    while (totalRead < fileSize) {
        size_t currentPartSize = (numParts > 0 && partNum == numParts - 1) ? (fileSize - totalRead) : partSize;
        std::vector<char> buffer(currentPartSize); // Allocate exact size

        input.read(buffer.data(), currentPartSize);
        std::streamsize bytesRead = input.gcount();
        totalRead += bytesRead;

        std::ofstream output(dirName + "/" + filename + ".part_" + std::to_string(partNum), std::ios::binary);
        if (!output) {
            std::cerr << RED << "Error: Cannot create part file " << partNum << RESET << std::endl;
            return;
        }
        output.write(buffer.data(), bytesRead);

        showProgressBar(double(totalRead) / fileSize);
        partNum++;
    }

    std::cout << std::endl << GREEN << "File split completed! Parts stored in: " << dirName << RESET << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        showHelp();
        return 1;
    }

    std::string filename = argv[1];
    std::string option = argv[2];

    if (option == "--size" || option == "-s") {
        size_t partSize = parseSize(argv[3]);
        splitFile(filename, partSize, 0);
    } else if (option == "--parts" || option == "-p") {
        int numParts = std::stoi(argv[3]);
        splitFile(filename, 0, numParts);
    } else {
        std::cerr << RED << "Invalid option. Use '--help' for usage info." << RESET << std::endl;
        return 1;
    }

    return 0;
}
