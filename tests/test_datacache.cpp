/* 
 * TEST_DATACACHE
 * Unit tests for SMIPS CPU cache structure
 *
 * Stefan Wong 2020
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>

#include "DataCache.hpp"

const std::vector<uint8_t> dummy_data = {0xde, 0xad, 0xbe, 0xef};


TEST_CASE("test_datacache_init", "[classic]")
{
    DataCache test_cache;
    REQUIRE(test_cache.size() == SMIPS_MEM_SIZE);
}

TEST_CASE("test_datacache_init_custom", "[classic]")
{
    DataCache test_cache(1);
    REQUIRE(test_cache.size() == 1);
}

TEST_CASE("test_datacache_read_write", "[classic]")
{
    DataCache test_cache;

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        test_cache[i] = dummy_data[i];

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        REQUIRE(test_cache[i] == dummy_data[i]);
}

TEST_CASE("test_datacache_clear", "[classic]")
{
    DataCache test_cache;

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        test_cache[i] = dummy_data[i];

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        REQUIRE(test_cache[i] == dummy_data[i]);

    test_cache.clear();
    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        REQUIRE(test_cache[i] == 0);
}

TEST_CASE("test_datacahe_read_half", "[classic]")
{
    DataCache test_cache;

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        test_cache[i] = dummy_data[i];
    REQUIRE(test_cache[0] == 0xDE);
    REQUIRE(test_cache[1] == 0xAD);

    int16_t half = test_cache.readHalf(0);
    REQUIRE(half == int16_t(0xDEAD));
}

TEST_CASE("test_datacahe_read_word", "[classic]")
{
    DataCache test_cache;

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        test_cache[i] = dummy_data[i];
    REQUIRE(test_cache[0] == 0xDE);
    REQUIRE(test_cache[1] == 0xAD);
    REQUIRE(test_cache[2] == 0xBE);
    REQUIRE(test_cache[3] == 0xEF);

    int32_t word = test_cache.readWord(0);
    REQUIRE(word == int32_t(0xDEADBEEF));
}
