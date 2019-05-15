/* TEST_OPCODE 
 * Unit tests for opcode table
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>
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


class TestOpcode : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};


TEST_F(TestOpcode, test_init)
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    ASSERT_EQ(9, test_table.size());
    // Asking for an opcode past the last index should give a null op
    out_op = test_table.getIdx(20);
    ASSERT_EQ(0, out_op.instr);
    ASSERT_EQ("\0", out_op.mnemonic);

    // if we walk over these in order, they should exactly match
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.getIdx(idx);
        ASSERT_EQ(test_instr_codes[idx].instr, out_op.instr);
        ASSERT_EQ(test_instr_codes[idx].mnemonic, out_op.mnemonic);
    }

    // now test init function (clears table)
    test_table.init();
    ASSERT_EQ(0, test_table.size());
}

TEST_F(TestOpcode, test_lookup_instr)
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    ASSERT_EQ(9, test_table.size());
    // If we ask for an Opcode by an instruction that we have not seen
    // the result should be a null op
    out_op = test_table.get(0xdeadbeef);
    ASSERT_EQ(0, out_op.instr);
    ASSERT_EQ("\0", out_op.mnemonic);

    // Lookup each of the objects by instruction
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.get(test_instr_codes[idx].instr);
        ASSERT_EQ(test_instr_codes[idx].instr, out_op.instr);
        ASSERT_EQ(test_instr_codes[idx].mnemonic, out_op.mnemonic);
    }
}

TEST_F(TestOpcode, test_lookup_mnemonic)
{
    OpcodeTable test_table;
    Opcode out_op;

    // add some instructions to the table 
    for(const Opcode& code : test_instr_codes)
        test_table.add(code);

    ASSERT_EQ(9, test_table.size());
    // If we ask for an opcode by a mnemonic that we not seen
    // before the result should be a null op
    out_op = test_table.get("JUNK_OP");
    ASSERT_EQ(0, out_op.instr);
    ASSERT_EQ("\0", out_op.mnemonic);

    // Lookup each of the objects by menmonic
    for(unsigned int idx = 0; idx < test_table.size(); ++idx)
    {
        out_op = test_table.get(test_instr_codes[idx].mnemonic);
        ASSERT_EQ(test_instr_codes[idx].instr, out_op.instr);
        ASSERT_EQ(test_instr_codes[idx].mnemonic, out_op.mnemonic);
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
