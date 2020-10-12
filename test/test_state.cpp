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
// for assembly helper function
#include "Lexer.hpp"
#include "Assembler.hpp"


Program assem_helper(const std::string& src)
{
    Lexer lex;
    Assembler assem;

    lex.loadSource(src);
    lex.lex();
    assem.loadSource(lex.getSourceInfo());
    assem.assemble();

    return assem.getProgram();
}


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
    REQUIRE(test_state.instr == 0);
    REQUIRE(test_state.op_bits == 0);
    REQUIRE(test_state.func == 0);
    REQUIRE(test_state.rs == 0);
    REQUIRE(test_state.rt == 0);
    REQUIRE(test_state.rd == 0);
    REQUIRE(test_state.shamt == 0);
    REQUIRE(test_state.imm == 0);
    REQUIRE(test_state.tmp == 0);
    REQUIRE(test_state.hi == 0);
    REQUIRE(test_state.lo == 0);
    REQUIRE(test_state.mem_addr == 0);
    REQUIRE(test_state.mem_data == 0);

    // check registers
    for(int i = 0; i < 32; ++i)
       REQUIRE(test_state.reg[i] == 0); 
}

TEST_CASE("test_state_zero_mem", "[classic]")
{
    State test_state;
    
    // put some junk in the memory 
    std::vector<uint8_t> dummy_data = {0xDE, 0xAD, 0xBE, 0xEF};
    test_state.writeMem(dummy_data, 0);
    test_state.writeMem(dummy_data, 32);
    test_state.writeMem(dummy_data, 64);

    for(unsigned int i = 0; i < dummy_data.size(); ++i)
    {
        REQUIRE(test_state.mem[i]    == dummy_data[i]);
        REQUIRE(test_state.mem[i+32] == dummy_data[i]);
        REQUIRE(test_state.mem[i+64] == dummy_data[i]);
    }

    test_state.clearMem();
    for(unsigned int i = 0; i < test_state.mem.size(); ++i)
        REQUIRE(test_state.mem[i] == 0);
}


TEST_CASE("test_decode_j", "[classic]")
{
    State test_state;

    test_state.writeMem(j_example, 0);
    test_state.fetch();
    test_state.decode();
    REQUIRE(test_state.imm == 257);
}

// ================ INSTRUCTION UNIT TESTS ======== //
TEST_CASE("test_instr_add", "[classic]")
{
    State test_state;
    Program prog;

    const std::string src = "add $t1, $s2, $t6";
    prog = assem_helper(src);
    std::vector<uint8_t> vec = prog.toVec();

    test_state.writeMem(vec, 0);

    // check each stage in the pipeline
    REQUIRE(test_state.pc == 0);
    test_state.fetch();
    REQUIRE(test_state.pc == 4);
    REQUIRE(test_state.instr == 0x024E4820);

    test_state.decode();
    REQUIRE(test_state.rd == REG_TEMP_1);
    REQUIRE(test_state.rs == REG_SAVED_2);
    REQUIRE(test_state.rt == REG_TEMP_6);

    // setup registers for execute stage 
    test_state.reg[REG_SAVED_2] = 0xF0;
    test_state.reg[REG_TEMP_6] = 0x0F;
    test_state.reg[REG_TEMP_1] = 0xDEADBEEF;

    test_state.execute();
    REQUIRE(test_state.alu == 0xFF);

    test_state.write_back();
    REQUIRE(test_state.reg[REG_TEMP_1] == 0xFF);
}

TEST_CASE("test_instr_andi", "[classic]")
{
    State test_state;
    Program prog;

    const std::string src = "andi $t1, $t2, 255";
    prog = assem_helper(src);
    std::vector<uint8_t> vec = prog.toVec();

    test_state.writeMem(vec, 0);

    // check each stage in the pipeline
    REQUIRE(test_state.pc == 0);
    test_state.fetch();
    REQUIRE(test_state.pc == 4);
    REQUIRE(test_state.instr == 0x314900FF);

    test_state.decode();
    REQUIRE(test_state.rt == REG_TEMP_1);
    REQUIRE(test_state.rs == REG_TEMP_2);
    REQUIRE(test_state.imm == 0xFF);
    // setup registers for execute stage 
    test_state.reg[REG_TEMP_2] = 0xFFFF;
    test_state.reg[REG_TEMP_1] = 0xDEADBEEF;        // this should be overwritten 

    test_state.execute();
    REQUIRE(test_state.alu == 0xFF);
    test_state.memory();

    test_state.write_back();
    REQUIRE(test_state.reg[REG_TEMP_1] == 0xFF);
}

//TEST_CASE("test_instr_lw", "[classic]")
//{
//    State test_state;
//    Program prog;
//
//    const std::string src = "lw $t0 32($s3)";
//    prog = assem_helper(src);
//    std::vector<uint8_t> vec = prog.toVec();
//    test_state.writeMem(vec, 0);
//    
//    test_state.fetch();
//    REQUIRE(test_state.instr == 0x8E680020);
//    //REQUIRE(test_state.instr == 0x8c080000);
//    REQUIRE(test_state.pc == 4);
//    test_state.decode();
//    REQUIRE(test_state.rs == REG_SAVED_3);
//    REQUIRE(test_state.rt == REG_TEMP_0);
//    REQUIRE(test_state.imm == 32);
//    // Set the test up so that the word we want to load is at offset 64
//    // Since the offset in the example instruction is 32, we place 32
//    // in $rs and pre-load the memory. 
//    test_state.reg[test_state.rs] = 32;
//    const std::vector<uint8_t> dummy_data = {0xDE, 0xAD, 0xBE, 0xEF};
//    test_state.writeMem(dummy_data, 64);
//
//    // now execute this lw
//    test_state.execute();
//    REQUIRE(test_state.mem_addr == 64);
//    test_state.memory();
//    REQUIRE(test_state.mem_data == int32_t(0xDEADBEEF));       
//    test_state.write_back();
//    REQUIRE(test_state.reg[REG_TEMP_0] == int32_t(0xDEADBEEF));       
//    
//    for(unsigned int i = 0; i < dummy_data.size(); ++i)
//        REQUIRE(test_state.mem[test_state.mem_addr+i] == dummy_data[i]);
//}

TEST_CASE("test_instr_sll", "[classic]")
{
    State test_state;
    Program prog;

    const std::string src = "sll $t0, $t0, 2";
    prog = assem_helper(src);
    std::vector<uint8_t> vec = prog.toVec();

    test_state.writeMem(vec, 0);
    
    for(unsigned int i = 0; i < vec.size(); ++i)
        REQUIRE(test_state.mem[i] == vec[i]);

    // check each stage in the pipeline
    REQUIRE(test_state.pc == 0);
    test_state.fetch();
    REQUIRE(test_state.pc == 4);
    REQUIRE(test_state.instr == 0x00084080);

    test_state.decode();
    REQUIRE(test_state.rd == REG_TEMP_0);
    REQUIRE(test_state.rt == REG_TEMP_0);
    REQUIRE(test_state.shamt == 2);

    // set up some values for execute stage
    test_state.reg[REG_TEMP_0] = 0x80;

    test_state.execute();
    REQUIRE(test_state.alu == 0x200);

    test_state.memory();

    test_state.write_back();
    REQUIRE(test_state.reg[REG_TEMP_0] == 0x200);
}

TEST_CASE("test_instr_srl", "[classic]")
{
    State test_state;
    Program prog;

    const std::string src = "srl $t0, $s5, 2";
    prog = assem_helper(src);
    std::vector<uint8_t> vec = prog.toVec();

    test_state.writeMem(vec, 0);
    
    for(unsigned int i = 0; i < vec.size(); ++i)
        REQUIRE(test_state.mem[i] == vec[i]);

    // check each stage in the pipeline
    REQUIRE(test_state.pc == 0);
    test_state.fetch();
    REQUIRE(test_state.pc == 4);
    REQUIRE(test_state.instr == 0x00154082);

    test_state.decode();
    REQUIRE(test_state.rd == REG_TEMP_0);
    REQUIRE(test_state.rt == REG_SAVED_5);
    REQUIRE(test_state.shamt == 2);
    REQUIRE(test_state.op_bits == 0);

    // set up some values for execute stage
    test_state.reg[REG_SAVED_5] = 0x80;

    test_state.execute();
    REQUIRE(test_state.alu == 0x20);

    test_state.memory();

    test_state.write_back();
    REQUIRE(test_state.reg[REG_TEMP_0] == 0x20);
}



// ================ INSTRUCTION PIPLINES ======== //


TEST_CASE("test_lw_pipeline", "[classic]")
{
    State test_state;

    test_state.writeMem(lw_example, 0);
    // ensure all registers are zero'd at start up time 
    for(int i = 0; i < 32; ++i)
        REQUIRE(test_state.reg[i] == 0);

    test_state.fetch();
    REQUIRE(test_state.instr == 0x8E680020);
    REQUIRE(test_state.pc == 4);
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
    REQUIRE(test_state.mem_addr == 64);
    test_state.memory();
    REQUIRE(test_state.mem_data == int32_t(0xDEADBEEF));       
    test_state.write_back();
    REQUIRE(test_state.reg[test_state.rt] == int32_t(0xDEADBEEF));       
    
    for(unsigned int i = 0; i < dummy_data.size(); ++i)
        REQUIRE(test_state.mem[test_state.mem_addr+i] == dummy_data[i]);
}

TEST_CASE("test_j_pipeline", "[classic]")
{
    State test_state;

    test_state.writeMem(j_example, 0);
    // ensure all registers are zero'd at start up time 
    for(int i = 0; i < 32; ++i)
        REQUIRE(test_state.reg[i] == 0);

    test_state.fetch();
    REQUIRE(test_state.instr == 0x08000101);
    REQUIRE(test_state.pc == 4);
    test_state.decode();
    REQUIRE(test_state.imm == 257);
    REQUIRE(test_state.pc == 4);

    test_state.execute();
    REQUIRE(test_state.pc == 1028+4);
    // nothing actually happens in memory or write-back phase for jumps
}

