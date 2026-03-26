#include "../src/util/HashTable.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

void test_basic_operations() {
    std::cout << "Testing basic operations...\n";
    HashTable ht;

    // Test set and get
    ht.set("key1", "value1");
    auto result = ht.get("key1");
    assert(result.has_value());
    assert(result.value() == "value1");
    std::cout << "set/get works\n";

    // Test get non-existent key
    auto result2 = ht.get("nonexistent");
    assert(!result2.has_value());
    std::cout << "get non-existent key returns nullopt\n";

    // Test multiple keys
    ht.set("key2", "value2");
    ht.set("key3", "value3");
    assert(ht.get("key2").value() == "value2");
    assert(ht.get("key3").value() == "value3");
    std::cout << "multiple keys work\n";

    // Test erase
    bool erased = ht.erase("key2");
    assert(erased);
    assert(!ht.get("key2").has_value());
    std::cout << "erase works\n";

    // Test erase non-existent key
    bool erased2 = ht.erase("nonexistent");
    assert(!erased2);
    std::cout << "erase non-existent key returns false\n";

    // Test overwrite
    ht.set("key1", "new_value");
    assert(ht.get("key1").value() == "new_value");
    std::cout << "overwrite value works\n";
}

void test_concurrent_reads() {
    std::cout << "Testing concurrent reads...\n";
    HashTable ht;

    // Pre-fill with data
    for (int i = 0; i < 100; i++) {
        ht.set("key" + std::to_string(i), "value" + std::to_string(i));
    }

    // Multiple threads reading
    std::vector<std::thread> threads;
    bool all_success = true;

    for (int t = 0; t < 10; t++) {
        threads.emplace_back([&ht, &all_success, t]() {
            for (int i = 0; i < 100; i++) {
                auto result = ht.get("key" + std::to_string(i));
                if (!result.has_value() || result.value() != "value" + std::to_string(i)) {
                    all_success = false;
                }
            }
        });
    }

    for (auto &th : threads) {
        th.join();
    }

    assert(all_success);
    std::cout << "concurrent reads work\n";
}

void test_concurrent_writes() {
    std::cout << "Testing concurrent writes and reads...\n";
    HashTable ht;

    std::vector<std::thread> threads;

    // Mix of readers and writers
    for (int t = 0; t < 5; t++) {
        threads.emplace_back([&ht, t]() {
            for (int i = 0; i < 20; i++) {
                ht.set("key_" + std::to_string(t) + "_" + std::to_string(i),
                       "val_" + std::to_string(i));
            }
        });
    }

    for (int t = 0; t < 5; t++) {
        threads.emplace_back([&ht, t]() {
            for (int i = 0; i < 20; i++) {
                ht.get("key_" + std::to_string(t) + "_" + std::to_string(i));
            }
        });
    }

    for (auto &th : threads) {
        th.join();
    }

    std::cout << "concurrent reads and writes work (no deadlock)\n";
}

void test_large_dataset() {
    std::cout << "Testing with large dataset...\n";
    HashTable ht;

    const int COUNT = 1000;

    // Insert many items to trigger rehashing
    for (int i = 0; i < COUNT; i++) {
        ht.set("key" + std::to_string(i), "value" + std::to_string(i));
    }

    // Verify all items
    for (int i = 0; i < COUNT; i++) {
        auto result = ht.get("key" + std::to_string(i));
        assert(result.has_value());
        assert(result.value() == "value" + std::to_string(i));
    }

    std::cout << "large dataset with rehashing works\n";
}

int main(int argc, char *argv[]) {
    try {
        test_basic_operations();
        test_concurrent_reads();
        test_concurrent_writes();
        test_large_dataset();

        std::cout << "\nAll tests passed!\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}
