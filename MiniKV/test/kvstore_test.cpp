#include <gtest/gtest.h>
#include "kvstore.hpp"
#include <filesystem>

class KVStoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir = "./test_data";
        std::filesystem::remove_all(test_dir);
        store = std::make_unique<KVStore>(test_dir);
    }

    void TearDown() override {
        store.reset();
        std::filesystem::remove_all(test_dir);
    }

    std::string test_dir;
    std::unique_ptr<KVStore> store;
};

TEST_F(KVStoreTest, BasicPutGet) {
    EXPECT_TRUE(store->put("key1", "value1"));

    std::string value;
    EXPECT_TRUE(store->get("key1", value));
    EXPECT_EQ(value, "value1");
}

TEST_F(KVStoreTest, GetNonExistentKey) {
    std::string value;
    EXPECT_FALSE(store->get("nonexistent", value));
}

TEST_F(KVStoreTest, PutUpdateGet) {
    EXPECT_TRUE(store->put("key1", "value1"));
    EXPECT_TRUE(store->put("key1", "value2"));

    std::string value;
    EXPECT_TRUE(store->get("key1", value));
    EXPECT_EQ(value, "value2");
}

TEST_F(KVStoreTest, DeleteKey) {
    EXPECT_TRUE(store->put("key1", "value1"));
    EXPECT_TRUE(store->remove("key1"));

    std::string value;
    EXPECT_FALSE(store->get("key1", value));
}

TEST_F(KVStoreTest, Persistence) {
    // Insert data
    EXPECT_TRUE(store->put("persistent_key", "persistent_value"));
    store->flush_memtable();

    // Close and reopen
    store.reset();
    store = std::make_unique<KVStore>(test_dir);

    // Check if data persists
    std::string value;
    EXPECT_TRUE(store->get("persistent_key", value));
    EXPECT_EQ(value, "persistent_value");
}
