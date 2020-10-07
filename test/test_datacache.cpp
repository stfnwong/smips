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

    std::vector<uint8_t> dummy_data = {0xDE, 0xAD, 0xBE, 0xEF};

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        test_cache[i] = dummy_data[i];

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        REQUIRE(test_cache[i] == dummy_data[i]);
}
