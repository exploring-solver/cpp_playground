#include <gtest/gtest.h>
#include "wal.hpp"
#include <filesystem>

class WALTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file = "./test_wal.log";
        std::filesystem::remove(test_file);
    }

    void TearDown() override {
        std::filesystem::remove(test_file);
    }

    std::string test_file;
};

TEST_F(WALTest, WriteAndReadPut) {
    {
        WAL wal(test_file);
        EXPECT_TRUE(wal.write_put("key1", "value1"));
        EXPECT_TRUE(wal.write_put("key2", "value2"));
    }

    WAL wal_read(test_file);
    auto entries = wal_read.read_all();

    EXPECT_EQ(entries.size(), 2);
    EXPECT_EQ(entries[0].op_type, WAL::OpType::PUT);
    EXPECT_EQ(entries[0].key, "key1");
    EXPECT_EQ(entries[0].value, "value1");
    EXPECT_EQ(entries[1].key, "key2");
    EXPECT_EQ(entries[1].value, "value2");
}

TEST_F(WALTest, WriteAndReadDelete) {
    {
        WAL wal(test_file);
        EXPECT_TRUE(wal.write_delete("key1"));
    }

    WAL wal_read(test_file);
    auto entries = wal_read.read_all();

    EXPECT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].op_type, WAL::OpType::DELETE);
    EXPECT_EQ(entries[0].key, "key1");
    EXPECT_TRUE(entries[0].value.empty());
}
