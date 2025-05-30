#pragma once

#include <string>
#include <vector>

namespace utils {
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool file_exists(const std::string& filename);
    size_t file_size(const std::string& filename);
}
