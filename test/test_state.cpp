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


// add $s0, $s1. $s2
const std::vector<uint8_t> add_example = {
    0x02, 0x32, 0x80, 0x80
};
// lw $t0 32($s3)
const std::vector<uint8_t> lw_example = {
    0x8E, 0x68, 0x00, 0x20
};
// j 1028
const std::vector<uint8_t> j_example = {
    0x08, 0x00, 0x01,  0x01
};


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

TEST_CASE("test_fetch", "[classic]")
{
    State test_state;

    REQUIRE(test_state.pc == 0);
    // load some data into memory 
    test_state.loadMem(add_example, 0);
    test_state.loadMem(lw_example, 4);
    
    // fetch the two instructions
    test_state.fetch();
    REQUIRE(test_state.instr == 0x02328080);
    REQUIRE(test_state.pc == 4);
    test_state.fetch();
    REQUIRE(test_state.instr == 0x8E680020);
    REQUIRE(test_state.pc == 8);
}

TEST_CASE("test_decode", "[classic]")
{
    State test_state;

    test_state.loadMem(add_example, 0);
    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.rs == 17);
    REQUIRE(test_state.rt == 18);
    REQUIRE(test_state.rd == 16);
}
