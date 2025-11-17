// tests/treemap_test.cpp

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/tree.hpp"

// ------------------------------------------------------
// Correctness tests
// ------------------------------------------------------

TEST_CASE("TreeMap basic insert and get") {
    TreeMap map;

    map.insert("keyOne", "valueOne");
    map.insert("keyTwo", "valueTwo");
    map.insert("keyThree", "valueThree");

    SECTION("returns correct values for existing keys") {
        REQUIRE(map.get("keyOne")   == "valueOne");
        REQUIRE(map.get("keyTwo")   == "valueTwo");
        REQUIRE(map.get("keyThree") == "valueThree");
    }

    SECTION("returns empty string for missing keys") {
        REQUIRE(map.get("doesNotExist") == "");
        REQUIRE(map.get("anotherMissing") == "");
    }
}

TEST_CASE("TreeMap updates and deletes keys correctly") {
    TreeMap map;

    // initial insert
    map.insert("user", "Brad");

    SECTION("updating an existing key overwrites the value") {
        REQUIRE(map.get("user") == "Brad");

        map.insert("user", "Bellinder"); // same key, new value

        REQUIRE(map.get("user") == "Bellinder");
    }

    SECTION("deleting an existing key removes it") {
        REQUIRE(map.get("user") == "Brad");

        map.deleteKey("user");

        // after deletion, behaves like a missing key
        REQUIRE(map.get("user") == "");
    }

    SECTION("deleting a non-existent key does not break other keys") {
        map.deleteKey("doesNotExist");

        // original key should still be there
        REQUIRE(map.get("user") == "Brad");
    }
}

TEST_CASE("TreeMap handles multiple keys") {
    TreeMap map;

    // insert out of order to exercise tree behavior
    map.insert("mango",  "yellow");
    map.insert("apple",  "red");
    map.insert("banana", "yellow");
    map.insert("grape",  "purple");
    map.insert("cherry", "red");

    SECTION("all inserted keys are retrievable") {
        REQUIRE(map.get("apple")  == "red");
        REQUIRE(map.get("banana") == "yellow");
        REQUIRE(map.get("cherry") == "red");
        REQUIRE(map.get("grape")  == "purple");
        REQUIRE(map.get("mango")  == "yellow");
    }

    SECTION("deleting some keys leaves others intact") {
        map.deleteKey("banana");
        map.deleteKey("apple");

        REQUIRE(map.get("banana") == "");
        REQUIRE(map.get("apple")  == "");

        REQUIRE(map.get("cherry") == "red");
        REQUIRE(map.get("grape")  == "purple");
        REQUIRE(map.get("mango")  == "yellow");
    }
}

// ------------------------------------------------------
// Benchmarks (no nesting, same style as factorial example)
// ------------------------------------------------------

TEST_CASE("benchmarking TreeMap operations") {
    auto makeKey = [](int i) {
        return std::string("key_") + std::to_string(i);
    };

    BENCHMARK("insert 1,000 keys") {
        TreeMap map;
        for (int i = 0; i < 1000; ++i) {
            map.insert(makeKey(i), "value");
        }
        // return something just like factorial example
        return map.get(makeKey(500));
    };

    BENCHMARK("insert 10,000 keys") {
        TreeMap map;
        for (int i = 0; i < 10000; ++i) {
            map.insert(makeKey(i), "value");
        }
        return map.get(makeKey(5000));
    };

    BENCHMARK("get same key many times after splay") {
        TreeMap map;
        for (int i = 0; i < 2000; ++i) {
            map.insert(makeKey(i), "value_" + std::to_string(i));
        }

        // splay this key near the root
        (void)map.get(makeKey(1000));

        // repeatedly get same key
        std::string last;
        for (int i = 0; i < 1000; ++i) {
            last = map.get(makeKey(1000));
        }
        return last;
    };

    BENCHMARK("mixed access pattern") {
        TreeMap map;
        for (int i = 0; i < 5000; ++i) {
            map.insert(makeKey(i), "value_" + std::to_string(i));
        }

        int idx = 0;
        std::string last;
        for (int i = 0; i < 1000; ++i) {
            idx = (idx * 37 + 23) % 5000;   // deterministic pseudo-random
            last = map.get(makeKey(idx));
        }
        return last;
    };
}
