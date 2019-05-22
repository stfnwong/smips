/* TEST_ASSEMBLER 
 * Unit tests for SMIPS Assembler object 
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>

// unit(s) under test
#include "Lexer.hpp"
#include "Assembler.hpp"
#include "Program.hpp"


Program get_mult_add_expected_program(void)
{
    Program prog;
    Instr instr;

    // TODO: when starting address is sorted, update this

    // LW $t0, 4($gp)
    instr.ins = 0x23;
    //instr.ins = instr.ins | (
    instr.adr = 0x200;


    return prog;
}


class TestAssembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_file = "asm/mult_add.asm";

};

TEST_F(TestAssembler, test_asm_mult_add)
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;

    test_asm.setVerbose(true);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(this->test_file);
    lexer.lex();
    src_out = lexer.getSourceInfo();

    std::cout << "Source info: " << std::endl;
    std::cout << src_out.toString() << std::endl;
    
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // show the resulting program
    prog_out = test_asm.getProgram();
    std::cout << prog_out.toString() << std::endl;
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

