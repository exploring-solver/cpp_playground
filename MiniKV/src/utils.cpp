#include "utils.hpp"
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace utils {

std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

bool file_exists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

size_t file_size(const std::string& filename) {
    return std::filesystem::file_size(filename);
}

}
