#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

class WAL;
class SSTable;

class KVStore {
public:
    KVStore(const std::string& data_dir = "./data");
    ~KVStore();

    // Core operations
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value);
    bool remove(const std::string& key);

    // Management operations
    bool create_snapshot();
    void flush_memtable();
    void compact();
    void close();

private:
    std::string data_dir_;
    std::map<std::string, std::string> memtable_;
    std::unique_ptr<WAL> wal_;
    std::vector<std::unique_ptr<SSTable>> sstables_;
    std::mutex mutex_;

    size_t memtable_size_limit_;
    size_t current_memtable_size_;

    // Recovery
    void recover();
    void replay_wal();

    // SSTable management
    void flush_to_sstable();
    std::string generate_sstable_filename();
    void load_existing_sstables();
};
