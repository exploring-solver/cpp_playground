# MiniKV - Lightweight Key-Value Store

A educational implementation of a key-value database built in C++, following the architecture patterns of LevelDB and RocksDB.

## Features

- **In-memory MemTable** with sorted keys
- **Write-Ahead Log (WAL)** for durability
- **SSTable** format for disk storage
- **Crash recovery** via WAL replay
- **Basic compaction** support
- **Thread-safe** operations

## Building

### Prerequisites
- C++17 compatible compiler
- CMake 3.14+
- Google Test (optional, for running tests)

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make
