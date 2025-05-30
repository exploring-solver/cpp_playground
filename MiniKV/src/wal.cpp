#include "wal.hpp"
#include <iostream>

WAL::WAL(const std::string& filename) : filename_(filename) {
    write_stream_.open(filename_, std::ios::binary | std::ios::app);
}

WAL::~WAL() {
    close();
}

bool WAL::write_put(const std::string& key, const std::string& value) {
    LogEntry entry{OpType::PUT, key, value};
    return write_entry(entry);
}

bool WAL::write_delete(const std::string& key) {
    LogEntry entry{OpType::DELETE, key, ""};
    return write_entry(entry);
}

bool WAL::write_entry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!write_stream_.is_open()) {
        return false;
    }

    // Write opcode
    write_stream_.write(reinterpret_cast<const char*>(&entry.op_type), sizeof(entry.op_type));

    // Write key length and key
    uint16_t key_len = static_cast<uint16_t>(entry.key.length());
    write_stream_.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    write_stream_.write(entry.key.c_str(), key_len);

    // Write value length and value (for PUT operations)
    if (entry.op_type == OpType::PUT) {
        uint16_t val_len = static_cast<uint16_t>(entry.value.length());
        write_stream_.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
        write_stream_.write(entry.value.c_str(), val_len);
    }

    write_stream_.flush();
    return write_stream_.good();
}

std::vector<WAL::LogEntry> WAL::read_all() {
    std::vector<LogEntry> entries;
    std::ifstream read_stream(filename_, std::ios::binary);

    if (!read_stream.is_open()) {
        return entries;
    }

    LogEntry entry;
    while (read_entry(read_stream, entry)) {
        entries.push_back(entry);
    }

    return entries;
}

bool WAL::read_entry(std::ifstream& stream, LogEntry& entry) {
    // Read opcode
    if (!stream.read(reinterpret_cast<char*>(&entry.op_type), sizeof(entry.op_type))) {
        return false;
    }

    // Read key length and key
    uint16_t key_len;
    if (!stream.read(reinterpret_cast<char*>(&key_len), sizeof(key_len))) {
        return false;
    }

    entry.key.resize(key_len);
    if (!stream.read(&entry.key[0], key_len)) {
        return false;
    }

    // Read value for PUT operations
    if (entry.op_type == OpType::PUT) {
        uint16_t val_len;
        if (!stream.read(reinterpret_cast<char*>(&val_len), sizeof(val_len))) {
            return false;
        }

        entry.value.resize(val_len);
        if (!stream.read(&entry.value[0], val_len)) {
            return false;
        }
    } else {
        entry.value.clear();
    }

    return true;
}

void WAL::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    write_stream_.close();
    write_stream_.open(filename_, std::ios::binary | std::ios::trunc);
}

void WAL::close() {
    if (write_stream_.is_open()) {
        write_stream_.close();
    }
}
