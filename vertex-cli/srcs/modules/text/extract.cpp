// text_extract.cpp (Beast Mode)
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <unordered_set>
#include <algorithm>
#include <cctype>

//----------------------------------
// Config Struct
//----------------------------------
struct Options {
    std::string filename;
    std::optional<std::pair<int, int>> lineRange;
    std::optional<int> head;
    std::optional<int> tail;
    std::optional<std::string> matchRegex;
    std::optional<std::string> contains;
    std::optional<std::string> before;
    std::optional<std::string> after;
    std::optional<int> context;
    bool countOnly = false;
    bool numberLines = false;
    bool invert = false;
    bool ignoreCase = false;
    bool unique = false;
    bool trim = false;
};

//----------------------------------
// Utility
//----------------------------------
std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

//----------------------------------
// Input
//----------------------------------
std::vector<std::string> read_lines(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    return lines;
}

//----------------------------------
// Core Logic
//----------------------------------
bool matches(const std::string& lineRaw, const Options& opts) {
    std::string line = opts.ignoreCase ? to_lower(lineRaw) : lineRaw;
    if (opts.matchRegex) {
        try {
            std::regex r(*opts.matchRegex, opts.ignoreCase ? std::regex::icase : std::regex::ECMAScript);
            return std::regex_search(line, r);
        } catch (...) {
            return false;
        }
    }
    if (opts.contains) {
        std::string searchFor = opts.ignoreCase ? to_lower(*opts.contains) : *opts.contains;
        return line.find(searchFor) != std::string::npos;
    }
    return true;
}

std::pair<int, int> parse_line_range(const std::string& str) {
    if (str.find("..") != std::string::npos) {
        auto pos = str.find("..");
        int start = std::stoi(str.substr(0, pos));
        int end = str.substr(pos + 2).empty() ? -1 : std::stoi(str.substr(pos + 2));
        return {start, end};
    }
    return {1, std::stoi(str) - 1};
}

std::vector<std::string> filter_lines(const std::vector<std::string>& lines, const Options& opts, std::vector<int>& lineNumsOut) {
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    int total = lines.size();
    int start = 0, end = total;

    // Apply line range/head/tail
    if (opts.head) end = std::min(*opts.head, total);
    if (opts.tail) start = std::max(0, total - *opts.tail);
    if (opts.lineRange) {
        start = std::max(start, (*opts.lineRange).first - 1);
        end = (*opts.lineRange).second == -1 ? total : std::min(end, (*opts.lineRange).second);
    }

    bool afterTrigger = !opts.after;
    for (int i = start; i < end; ++i) {
        std::string line = opts.trim ? trim(lines[i]) : lines[i];
        std::string checkLine = opts.ignoreCase ? to_lower(line) : line;

        // 'after' flag
        if (!afterTrigger && opts.after && checkLine.find(*opts.after) != std::string::npos) {
            afterTrigger = true;
            continue;
        }
        if (!afterTrigger) continue;

        // 'before' flag
        if (opts.before && checkLine.find(*opts.before) != std::string::npos) break;

        bool isMatch = matches(line, opts);
        if ((isMatch && !opts.invert) || (!isMatch && opts.invert)) {
            if (opts.unique && seen.count(line)) continue;
            seen.insert(line);
            result.push_back(line);
            lineNumsOut.push_back(i + 1);
        }

        // Context capture
        if (opts.context && isMatch) {
            int ctx = *opts.context;
            for (int j = std::max(i - ctx, start); j < std::min(i + ctx + 1, end); ++j) {
                if (j != i && (opts.unique && seen.count(lines[j]))) continue;
                seen.insert(lines[j]);
                result.push_back(opts.trim ? trim(lines[j]) : lines[j]);
                lineNumsOut.push_back(j + 1);
            }
        }
    }
    return result;
}

//----------------------------------
// Output
//----------------------------------
void output_lines(const std::vector<std::string>& lines, const Options& opts, const std::vector<int>& lineNums) {
    if (opts.countOnly) {
        std::cout << lines.size() << "\n";
        return;
    }
    for (size_t i = 0; i < lines.size(); ++i) {
        if (opts.numberLines && i < lineNums.size()) {
            std::cout << lineNums[i] << ": ";
        }
        std::cout << lines[i] << "\n";
    }
}

//----------------------------------
// Argument Parser
//----------------------------------
Options parse_args(int argc, char* argv[]) {
    Options opts;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-lines" && i + 1 < argc) opts.lineRange = parse_line_range(argv[++i]);
        else if (arg == "-head" && i + 1 < argc) opts.head = std::stoi(argv[++i]);
        else if (arg == "-tail" && i + 1 < argc) opts.tail = std::stoi(argv[++i]);
        else if (arg == "-match" && i + 1 < argc) opts.matchRegex = argv[++i];
        else if (arg == "-contains" && i + 1 < argc) opts.contains = argv[++i];
        else if (arg == "-before" && i + 1 < argc) opts.before = argv[++i];
        else if (arg == "-after" && i + 1 < argc) opts.after = argv[++i];
        else if (arg == "-context" && i + 1 < argc) opts.context = std::stoi(argv[++i]);
        else if (arg == "-count") opts.countOnly = true;
        else if (arg == "-number") opts.numberLines = true;
        else if (arg == "-invert") opts.invert = true;
        else if (arg == "-ignore-case") opts.ignoreCase = true;
        else if (arg == "-unique") opts.unique = true;
        else if (arg == "-trim") opts.trim = true;
        else if (arg[0] != '-') opts.filename = arg;
    }
    return opts;
}

//----------------------------------
// Main
//----------------------------------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: vxc text extract <file> [flags]\n";
        return 1;
    }

    try {
        Options opts = parse_args(argc, argv);
        auto lines = read_lines(opts.filename);
        std::vector<int> lineNums;
        auto filtered = filter_lines(lines, opts, lineNums);
        output_lines(filtered, opts, lineNums);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
