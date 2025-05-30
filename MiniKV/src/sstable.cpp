#include "sstable.hpp"
#include <iostream>
#include <algorithm>
#include<vector>
#include <fstream>

SSTable::SSTable(const std::string& filename) : filename_(filename), valid_(false) {
    std::ifstream file(filename_, std::ios::binary);
    if (file.is_open()) {
        build_index();
        valid_ = true;
    }
}

bool SSTable::write(const std::map<std::string, std::string>& data) {
    std::ofstream file(filename_, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    index_.clear();
    size_t offset = 0;

    // Write data and build index
    for (const auto& [key, value] : data) {
        size_t entry_start = offset;

        // Write key length and key
        uint16_t key_len = static_cast<uint16_t>(key.length());
        file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        file.write(key.c_str(), key_len);

        // Write value length and value
        uint16_t val_len = static_cast<uint16_t>(value.length());
        file.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
        file.write(value.c_str(), val_len);

        size_t entry_size = sizeof(key_len) + key_len + sizeof(val_len) + val_len;
        index_.push_back({key, entry_start, entry_size});
        offset += entry_size;
    }

    valid_ = file.good();
    return valid_;
}

bool SSTable::get(const std::string& key, std::string& value) {
    if (!valid_) return false;

    size_t offset, size;
    if (!binary_search_key(key, offset, size)) {
        return false;
    }

    std::ifstream file(filename_, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(offset);

    // Read key length and key
    uint16_t key_len;
    file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

    std::string read_key(key_len, '\\0');
    file.read(&read_key[0], key_len);

    // Verify key matches
    if (read_key != key) {
        return false;
    }

    // Read value length and value
    uint16_t val_len;
    file.read(reinterpret_cast<char*>(&val_len), sizeof(val_len));

    value.resize(val_len);
    file.read(&value[0], val_len);

    return file.good();
}

void SSTable::build_index() {
    std::ifstream file(filename_, std::ios::binary);
    if (!file.is_open()) return;

    index_.clear();
    size_t offset = 0;

    while (file.good() && !file.eof()) {
        size_t entry_start = offset;

        // Read key length
        uint16_t key_len;
        if (!file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len))) {
            break;
        }

        // Read key
        std::string key(key_len, '\\0');
        if (!file.read(&key[0], key_len)) {
            break;
        }

        // Read value length
        uint16_t val_len;
        if (!file.read(reinterpret_cast<char*>(&val_len), sizeof(val_len))) {
            break;
        }

        // Skip value
        file.seekg(val_len, std::ios::cur);

        size_t entry_size = sizeof(key_len) + key_len + sizeof(val_len) + val_len;
        index_.push_back({key, entry_start, entry_size});
        offset += entry_size;
    }
}

bool SSTable::binary_search_key(const std::string& key, size_t& offset, size_t& size) {
    auto it = std::lower_bound(index_.begin(), index_.end(), key,
        [](const IndexEntry& entry, const std::string& k) {
            return entry.key < k;
        });

    if (it != index_.end() && it->key == key) {
        offset = it->offset;
        size = it->size;
        return true;
    }

    return false;
}

bool SSTable::is_valid() const {
    return valid_;
}
