#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <mutex>

class WAL {
public:
    enum class OpType : uint8_t {
        PUT = 0x01,
        DELETE = 0x02
    };

    struct LogEntry {
        OpType op_type;
        std::string key;
        std::string value;
    };

    explicit WAL(const std::string& filename);
    ~WAL();

    bool write_put(const std::string& key, const std::string& value);
    bool write_delete(const std::string& key);
    std::vector<LogEntry> read_all();
    void clear();
    void close();

private:
    std::string filename_;
    std::ofstream write_stream_;
    std::mutex mutex_;

    bool write_entry(const LogEntry& entry);
    bool read_entry(std::ifstream& stream, LogEntry& entry);
};
