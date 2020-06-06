/* 
 * TEST_REGISTER 
 * Unit tests for SMIPS Assembler object 
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

// unit(s) under test
#include "Register.hpp"


TEST_CASE("init register map", "[classic]")
{
    RegisterMap test_map;

    REQUIRE(32 ==  test_map.size());
}
