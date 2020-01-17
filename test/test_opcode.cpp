/* TEST_OPCODE 
 * Unit tests for opcode table
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
// unit under test
#include "Opcode.hpp"

// sample instruction codes
typedef enum instr_code 
{
    LEX_NULL,
    LEX_ADD, LEX_ADDU, LEX_ADDI, LEX_ADDIU, 
    LEX_AND, LEX_ANDI, LEX_DIV, LEX_DIVU,
} instr_code;

// sample opcodes 
const Opcode test_instr_codes[] = {
    Opcode(LEX_NULL,  "\0"),
    Opcode(LEX_ADD,   "ADD"),
    Opcode(LEX_ADDU,  "ADDU"),
    Opcode(LEX_ADDI,  "ADDI"),
    Opcode(LEX_ADDIU, "ADDIU"),
    Opcode(LEX_AND,   "AND"),
    Opcode(LEX_ANDI,  "ANDI"),
    Opcode(LEX_DIV,   "DIV"),
    Opcode(LEX_DIVU,  "DIVU"),
};


TEST_CASE("Test opcode structure init", "[classic]")
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    REQUIRE(9 == test_table.size());
    // Asking for an opcode past the last index should give a null op
    out_op = test_table.getIdx(20);
    REQUIRE(0 == out_op.instr);
    REQUIRE("\0" == out_op.mnemonic);

    // if we walk over these in order, they should exactly match
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.getIdx(idx);
        REQUIRE(test_instr_codes[idx].instr == out_op.instr);
        REQUIRE(test_instr_codes[idx].mnemonic == out_op.mnemonic);
    }

    // now test init function (clears table)
    test_table.init();
    REQUIRE(0 == test_table.size());
}


TEST_CASE("Test OpcodeTable lookup by instr", "[classic]")
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    REQUIRE(9 == test_table.size());
    // If we ask for an Opcode by an instruction that we have not seen
    // the result should be a null op
    out_op = test_table.get(0xdeadbeef);
    REQUIRE(0 == out_op.instr);
    REQUIRE("\0" == out_op.mnemonic);

    // Lookup each of the objects by instruction
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.get(test_instr_codes[idx].instr);
        REQUIRE(test_instr_codes[idx].instr == out_op.instr);
        REQUIRE(test_instr_codes[idx].mnemonic == out_op.mnemonic);
    }
}

TEST_CASE("Test OpcodeTable lookup by mnemonic", "[classic]")
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    REQUIRE(9 == test_table.size());
    // If we ask for an opcode by a mnemonic that we not seen
    // before the result should be a null op
    out_op = test_table.get("JUNK_OP");
    REQUIRE(0 == out_op.instr);
    REQUIRE("\0" == out_op.mnemonic);

    // Lookup each of the objects by menmonic
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.get(test_instr_codes[idx].mnemonic);
        REQUIRE(test_instr_codes[idx].instr == out_op.instr);
        REQUIRE(test_instr_codes[idx].mnemonic == out_op.mnemonic);
    }
}
