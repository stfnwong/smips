/* 
 * TEST_STATE
 * Unit tests for SMIPS CPU State structure
 *
 * Stefan Wong 2020
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>

// unit(s) under test 
#include "State.hpp"



TEST_CASE("test_state_init", "[classic]")
{
    State test_state;

    REQUIRE(test_state.pc == 0);
    REQUIRE(test_state.addr == 0);
    REQUIRE(test_state.instr == 0);

    // check registers
    for(int i = 0; i < 32; ++i)
       REQUIRE(test_state.reg[i] == 0); 

    std::cout << test_state.toString() << std::endl;
}
