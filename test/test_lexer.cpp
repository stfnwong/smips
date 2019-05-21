/* TEST_LEXER 
 * Unit tests for SMIPS Lexer object 
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Source.hpp"


// TODO : adjust starting address later 
SourceInfo get_mult_add_expected_source_info(void)
{
    SourceInfo info;
    LineInfo   line;

    // line 1
    // lw $t0, 4($gp)
    line.init();
    line.line_num        = 3;
    line.addr            = 0x200;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.args[1]         = 4;
    line.types[1]        = SYM_REG_GLOBAL;
    info.add(line);

    // line 2
    // mult $t0, $t0, $t0
    line.init();
    line.line_num        = 4;
    line.addr            = 0x201;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
    line.args[0]         = 0;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 0;
    line.types[1]        = SYM_REG_TEMP;
    line.args[2]         = 0;
    line.types[2]        = SYM_REG_TEMP;
    info.add(line);

    // line 3
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x202;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.args[0]         = 1;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 4;
    line.types[1]        = SYM_REG_GLOBAL;
    info.add(line);

    // line 4
    // ori $t2, $zero 3
    line.init();
    line.line_num        = 6;
    line.addr            = 0x203;      
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ORI";
    line.is_imm          = true;
    line.args[0]         = 2;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 0;
    line.types[1]        = SYM_REG_ZERO;
    line.args[2]         = 3;
    line.types[3]        = SYM_LITERAL;
    info.add(line);

    // line 5
    // mult $t1, $t1, $t2
    line.init();
    line.line_num        = 7;
    line.addr            = 0x204;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
    line.args[0]         = 1;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 1;
    line.types[1]        = SYM_REG_TEMP;
    line.args[2]         = 2;
    line.types[3]        = SYM_REG_TEMP;
    info.add(line);

    // line 7
    // add $t2, $t0, $t1
    line.init();
    line.line_num        = 8;
    line.addr            = 0x205;      
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.args[0]         = 2;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 0;
    line.types[1]        = SYM_REG_TEMP;
    line.args[2]         = 1;
    line.types[3]        = SYM_REG_TEMP;
    info.add(line);

    // line 8
    // sw $t2, 0($gp)
    line.init();
    line.line_num        = 9;
    line.addr            = 0x206;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "SW";
    line.args[0]         = 2;
    line.types[0]        = SYM_REG_TEMP;
    line.args[1]         = 0;
    line.types[1]        = SYM_REG_GLOBAL;
    info.add(line);



    return info;
}

class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_file = "asm/mult_add.asm";

};


TEST_F(TestLexer, test_lex_mult_add)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_file);
    test_lexer.lex();

    // get the source info
    src_out = test_lexer.getSourceInfo();

    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;

    expected_src_out = get_mult_add_expected_source_info();
    ASSERT_EQ(expected_src_out.getNumLines(), src_out.getNumLines());

    // Check each line in turn
    LineInfo expected_line;
    LineInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getNumLines(); ++line)
    {
        expected_line = expected_src_out.get(line);
        output_line = src_out.get(line);
        ASSERT_EQ(expected_line, output_line);
    }
}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
