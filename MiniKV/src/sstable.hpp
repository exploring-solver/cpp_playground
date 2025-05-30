#pragma once

#include <string>
#include <map>
#include <fstream>
#include<vector>
using namespace std;
class SSTable {
public:
    explicit SSTable(const std::string& filename);
    ~SSTable() = default;

    bool write(const std::map<std::string, std::string>& data);
    bool get(const std::string& key, std::string& value);
    bool is_valid() const;

private:
    std::string filename_;
    bool valid_;

    struct IndexEntry {
        std::string key;
        size_t offset;
        size_t size;
    };

    std::vector<IndexEntry> index_;

    void build_index();
    bool binary_search_key(const std::string& key, size_t& offset, size_t& size);
};
