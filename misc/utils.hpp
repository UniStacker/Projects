// AeonUtils.h
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

namespace utils {

    // === COLORS ===
    enum class Color {
        Default = 39, Black = 30, Red = 31, Green = 32,
        Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
        BrightBlack = 90, BrightRed = 91, BrightGreen = 92,
        BrightYellow = 93, BrightBlue = 94, BrightMagenta = 95,
        BrightCyan = 96, BrightWhite = 97
    };

    void setTextColor(Color color) {
        std::cout << "\033[" << static_cast<int>(color) << "m";
    }

    void resetTextColor() {
        std::cout << "\033[0m";
    }

    // === TERMINAL CONTROL ===
    void clearScreen() {
        std::cout << "\033[2J\033[H" << std::flush;
    }

    void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }

    void hideCursor() {
        std::cout << "\033[?25l" << std::flush;
    }

    void showCursor() {
        std::cout << "\033[?25h" << std::flush;
    }

    std::pair<int, int> getTerminalSize() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return {w.ws_row, w.ws_col};
    }

    // === NON-BLOCKING INPUT ===
    struct termios orig_termios;

    void enableRawMode() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    }

    void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        fcntl(STDIN_FILENO, F_SETFL, 0);
    }

    bool isKeyPressed() {
        char buf = 0;
        ssize_t n = read(STDIN_FILENO, &buf, 1);
        if (n > 0) {
            return true;
        }
        return false;
    }

    char getch() {
        char c = 0;
        read(STDIN_FILENO, &c, 1);
        return c;
    }

    // === TIMING ===
    void sleepMS(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    auto timerNow() {
        return std::chrono::high_resolution_clock::now();
    }

    double timeSince(auto start) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        return elapsed.count();
    }

    // === RANDOMNESS ===
    int randomInt(int min, int max) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    double randomFloat(double min, double max) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng);
    }

    template<typename T>
    T randomChoice(const std::vector<T>& vec) {
        if (vec.empty()) throw std::runtime_error("randomChoice: empty vector");
        return vec[randomInt(0, vec.size() - 1)];
    }

    // === FILESYSTEM ===
    bool fileExists(const std::string& path) {
        return access(path.c_str(), F_OK) != -1;
    }

    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("Cannot open file: " + path);
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    void writeFile(const std::string& path, const std::string& data) {
        std::ofstream file(path);
        if (!file) throw std::runtime_error("Cannot write file: " + path);
        file << data;
    }

    std::vector<std::string> listDirectory(const std::string& path = ".") {
        std::vector<std::string> files;
        DIR* dir = opendir(path.c_str());
        if (!dir) return files;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            files.push_back(entry->d_name);
        }
        closedir(dir);
        return files;
    }

    void createDirectory(const std::string& path) {
        mkdir(path.c_str(), 0755);
    }

    // === SYSTEM INFO ===
    std::string getUsername() {
        struct passwd* pw = getpwuid(getuid());
        return pw ? pw->pw_name : "unknown";
    }

    std::string getHostname() {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        return hostname;
    }

    // === CLI EXTRAS ===
    void drawProgressBar(int percent, int width = 50) {
        int filled = (percent * width) / 100;
        std::cout << "[";
        for (int i = 0; i < width; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << " ";
        }
        std::cout << "] " << percent << "%\r" << std::flush;
    }

    void typewriter(const std::string& text, int speedMS = 50) {
        for (char c : text) {
            std::cout << c << std::flush;
            sleepMS(speedMS);
        }
    }
}

