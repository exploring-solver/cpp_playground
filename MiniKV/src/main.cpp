#include "kvstore.hpp"
#include <iostream>
#include <string>
#include<sstream>
#include <vector>
void print_usage() {
    std::cout << "MiniKV Commands:\\n";
    std::cout << "  put <key> <value> - Store a key-value pair\\n";
    std::cout << "  get <key>         - Retrieve value for key\\n";
    std::cout << "  del <key>         - Delete key\\n";
    std::cout << "  flush             - Flush memtable to disk\\n";
    std::cout << "  snapshot          - Create snapshot\\n";
    std::cout << "  quit              - Exit\\n";
}

int main() {
    KVStore store("./minikv_data");
    std::string command;

    std::cout << "MiniKV - Lightweight Key-Value Store\\n";
    std::cout << "Type 'help' for commands\\n\\n";

    while (true) {
        std::cout << "minikv> ";
        std::getline(std::cin, command);

        if (command.empty()) continue;

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "quit" || cmd == "exit") {
            break;
        } else if (cmd == "help") {
            print_usage();
        } else if (cmd == "put") {
            std::string key, value;
            iss >> key >> value;
            if (key.empty() || value.empty()) {
                std::cout << "Usage: put <key> <value>\\n";
            } else {
                if (store.put(key, value)) {
                    std::cout << "OK\\n";
                } else {
                    std::cout << "Error: Failed to put\\n";
                }
            }
        } else if (cmd == "get") {
            std::string key;
            iss >> key;
            if (key.empty()) {
                std::cout << "Usage: get <key>\\n";
            } else {
                std::string value;
                if (store.get(key, value)) {
                    std::cout << value << "\\n";
                } else {
                    std::cout << "Key not found\\n";
                }
            }
        } else if (cmd == "del") {
            std::string key;
            iss >> key;
            if (key.empty()) {
                std::cout << "Usage: del <key>\\n";
            } else {
                if (store.remove(key)) {
                    std::cout << "OK\\n";
                } else {
                    std::cout << "Error: Failed to delete\\n";
                }
            }
        } else if (cmd == "flush") {
            store.flush_memtable();
            std::cout << "Memtable flushed\\n";
        } else if (cmd == "snapshot") {
            if (store.create_snapshot()) {
                std::cout << "Snapshot created\\n";
            } else {
                std::cout << "Error: Failed to create snapshot\\n";
            }
        } else {
            std::cout << "Unknown command. Type 'help' for available commands.\\n";
        }
    }

    std::cout << "Goodbye!\\n";
    return 0;
}
