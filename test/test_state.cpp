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
#include "Program.hpp"


// add $s0, $s1. $s2
const std::vector<uint8_t> add_example = {
    0x02, 0x32, 0x80, 0x20
};
// lw $t0 32($s3)
const std::vector<uint8_t> lw_example = {
    0x8E, 0x68, 0x00, 0x20
};
// j 1028
const std::vector<uint8_t> j_example = {
    0x08, 0x00, 0x01, 0x01
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
}

TEST_CASE("test_fetch", "[classic]")
{
    State test_state;

    REQUIRE(test_state.pc == 0);
    // load some data into memory 
    test_state.writeMem(add_example, 0);
    test_state.writeMem(lw_example, 4);
    // fetch the two instructions
    test_state.fetch();
    REQUIRE(test_state.instr == 0x02328020);
    REQUIRE(test_state.pc == 4);
    test_state.fetch();
    REQUIRE(test_state.instr == 0x8E680020);
    REQUIRE(test_state.pc == 8);
}

TEST_CASE("test_decode", "[classic]")
{
    State test_state;

    test_state.writeMem(add_example, 0);
    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.rs == 17);
    REQUIRE(test_state.rt == 18);
    REQUIRE(test_state.rd == 16);
}


TEST_CASE("test_execute_add", "[classic]")
{
    State test_state;

    test_state.writeMem(add_example, 0);
    // ensure all registers are zero'd at start up time 
    for(int i = 0; i < 32; ++i)
        REQUIRE(test_state.reg[i] == 0);

    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.rs == 17);
    REQUIRE(test_state.rt == 18);
    REQUIRE(test_state.rd == 16);
    // lets set the values of those registers to be
    // R[rs] = 1
    // R[rt] = 2
    // so that R[rd] will be 3
    test_state.reg[test_state.rs] = 1;
    test_state.reg[test_state.rt] = 2;

    // now execute this add
    test_state.execute();
    REQUIRE(test_state.reg[test_state.rd] == 3);
}

TEST_CASE("test_execute_lw", "[classic]")
{
    State test_state;

    test_state.writeMem(lw_example, 0);
    // ensure all registers are zero'd at start up time 
    for(int i = 0; i < 32; ++i)
        REQUIRE(test_state.reg[i] == 0);

    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.rs == 19);
    REQUIRE(test_state.rt == 8);
    REQUIRE(test_state.imm == 32);
    // Set the test up so that the word we want to load is at offset 64
    // Since the offset in the example instruction is 32, we place 32
    // in $rs and pre-load the memory. 
    test_state.reg[test_state.rs] = 32;
    const std::vector<uint8_t> dummy_data = {0xDE, 0xAD, 0xBE, 0xEF};
    test_state.writeMem(dummy_data, 64);

    // now execute this lw
    test_state.execute();
    REQUIRE(test_state.reg[8] == 0xDEADBEEF);
}

TEST_CASE("test_execute_j", "[classic]")
{
    State test_state;

    test_state.writeMem(j_example, 0);
    // ensure all registers are zero'd at start up time 
    for(int i = 0; i < 32; ++i)
        REQUIRE(test_state.reg[i] == 0);

    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.imm == 257);
    REQUIRE(test_state.pc == 4);

    test_state.execute();
    REQUIRE(test_state.pc == 1028+4);
}
