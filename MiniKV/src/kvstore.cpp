#include "kvstore.hpp"
#include "wal.hpp"
#include "sstable.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>

KVStore::KVStore(const std::string& data_dir)
    : data_dir_(data_dir), memtable_size_limit_(1024 * 1024), current_memtable_size_(0) {

    // Create data directory if it doesn't exist
    std::filesystem::create_directories(data_dir_);

    // Initialize WAL
    wal_ = std::make_unique<WAL>(data_dir_ + "/wal.log");

    // Recover from existing data
    recover();
}

KVStore::~KVStore() {
    close();
}

bool KVStore::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Write to WAL first for durability
    if (!wal_->write_put(key, value)) {
        return false;
    }

    // Update memtable
    auto old_size = memtable_[key].size();
    memtable_[key] = value;
    current_memtable_size_ += value.size() + key.size() - old_size;

    // Check if memtable needs flushing
    if (current_memtable_size_ > memtable_size_limit_) {
        flush_to_sstable();
    }

    return true;
}

bool KVStore::get(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Check memtable first
    auto it = memtable_.find(key);
    if (it != memtable_.end()) {
        value = it->second;
        return true;
    }

    // Check SSTables (most recent first)
    for (auto rit = sstables_.rbegin(); rit != sstables_.rend(); ++rit) {
        if ((*rit)->get(key, value)) {
            return true;
        }
    }

    return false;
}

bool KVStore::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Write to WAL
    if (!wal_->write_delete(key)) {
        return false;
    }

    // Remove from memtable
    auto it = memtable_.find(key);
    if (it != memtable_.end()) {
        current_memtable_size_ -= it->first.size() + it->second.size();
        memtable_.erase(it);
    }

    return true;
}

void KVStore::recover() {
    load_existing_sstables();
    replay_wal();
}

void KVStore::replay_wal() {
    auto entries = wal_->read_all();
    for (const auto& entry : entries) {
        if (entry.op_type == WAL::OpType::PUT) {
            memtable_[entry.key] = entry.value;
            current_memtable_size_ += entry.key.size() + entry.value.size();
        } else if (entry.op_type == WAL::OpType::DELETE) {
            auto it = memtable_.find(entry.key);
            if (it != memtable_.end()) {
                current_memtable_size_ -= it->first.size() + it->second.size();
                memtable_.erase(it);
            }
        }
    }
}

void KVStore::flush_to_sstable() {
    if (memtable_.empty()) return;

    std::string filename = generate_sstable_filename();
    auto sstable = std::make_unique<SSTable>(filename);

    if (sstable->write(memtable_)) {
        sstables_.push_back(std::move(sstable));
        memtable_.clear();
        current_memtable_size_ = 0;
        wal_->clear(); // Clear WAL after successful flush
    }
}

std::string KVStore::generate_sstable_filename() {
    static int counter = 0;
    return data_dir_ + "/sstable_" + std::to_string(++counter) + ".sst";
}

void KVStore::load_existing_sstables() {
    // Load existing SSTable files from data directory
    for (const auto& entry : std::filesystem::directory_iterator(data_dir_)) {
        if (entry.path().extension() == ".sst") {
            auto sstable = std::make_unique<SSTable>(entry.path().string());
            if (sstable->is_valid()) {
                sstables_.push_back(std::move(sstable));
            }
        }
    }
}

bool KVStore::create_snapshot() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Implementation for snapshotting
    return true;
}

void KVStore::flush_memtable() {
    std::lock_guard<std::mutex> lock(mutex_);
    flush_to_sstable();
}

void KVStore::compact() {
    // Implementation for compaction
}

void KVStore::close() {
    if (wal_) {
        wal_->close();
    }
}
