#include <gtest/gtest.h>
#include "sstable.hpp"
#include <filesystem>
#include <map>

class SSTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file = "./test_sstable.sst";
        std::filesystem::remove(test_file);
    }

    void TearDown() override {
        std::filesystem::remove(test_file);
    }

    std::string test_file;
};

TEST_F(SSTableTest, WriteAndRead) {
    std::map<std::string, std::string> data = {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"}
    };

    {
        SSTable sstable(test_file);
        EXPECT_TRUE(sstable.write(data));
    }

    SSTable sstable_read(test_file);
    EXPECT_TRUE(sstable_read.is_valid());

    std::string value;
    EXPECT_TRUE(sstable_read.get("key1", value));
    EXPECT_EQ(value, "value1");

    EXPECT_TRUE(sstable_read.get("key2", value));
    EXPECT_EQ(value, "value2");

    EXPECT_FALSE(sstable_read.get("nonexistent", value));
}
