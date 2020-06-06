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


TEST_CASE("register_map", "[classic]")
{
    RegisterMap test_map;

    REQUIRE(32 ==  test_map.size());

    // Try some out 
    REQUIRE("$at" == test_map.getName(1));
    REQUIRE(1 == test_map.getIdx("$at"));

    // What about invalid registers?
    REQUIRE("$?" == test_map.getName(-1));
    REQUIRE("$?" == test_map.getName(999));

    REQUIRE(-1 == test_map.getIdx("not a register"));
}
