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

#include "Codes.hpp"
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
    line.offset[1]       = 4;
    line.type[1]         = SYM_REG_GLOBAL;
    info.add(line);

    // line 2
    // mult $t0, $t0, $t0
    line.init();
    line.line_num        = 4;
    line.addr            = 0x201;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
    line.val[0]         = 0;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_TEMP;
    line.val[2]         = 0;
    line.type[2]        = SYM_REG_TEMP;
    info.add(line);

    // line 3
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x202;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.val[0]         = 1;
    line.type[0]        = SYM_REG_TEMP;
    line.offset[1]      = 4;
    line.type[1]        = SYM_REG_GLOBAL;
    info.add(line);

    // line 4
    // ori $t2, $zero 3
    line.init();
    line.line_num        = 6;
    line.addr            = 0x203;      
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ORI";
    line.is_imm          = true;
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.offset[1]       = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 3;
    line.type[3]         = SYM_LITERAL;
    info.add(line);

    // line 5
    // mult $t1, $t1, $t2
    line.init();
    line.line_num        = 7;
    line.addr            = 0x204;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 2;
    line.type[3]         = SYM_REG_TEMP;
    info.add(line);

    // line 7
    // add $t2, $t0, $t1
    line.init();
    line.line_num        = 8;
    line.addr            = 0x205;      
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 1;
    line.type[3]         = SYM_REG_TEMP;
    info.add(line);

    // line 8
    // sw $t2, 0($gp)
    line.init();
    line.line_num        = 9;
    line.addr            = 0x206;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "SW";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.offset[1]       = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    info.add(line);

    return info;
}

SourceInfo get_for_loop_expected_source_info(void)
{
    SourceInfo info;
    LineInfo   line;

    // line 4
    // ADD $t0, $gp, $zero
    line.line_num        = 4;
    line.addr            = 0x200;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.val[0]         = 0;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_GLOBAL;
    line.type[2]        = SYM_REG_ZERO;
    info.add(line);

    // line 5
    // LW $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x201;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 4;
    line.type[1]         = SYM_REG_GLOBAL;
    info.add(line);

    // line 6
    // SLL $t1, $t1, 2
    line.init();
    line.line_num        = 6;
    line.addr            = 0x202;
    line.opcode.instr    = LEX_SLL;
    line.opcode.mnemonic = "SLL";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 2;
    line.type[2]         = SYM_LITERAL;
    info.add(line);

    // line 7
    // ADD $t1, $t1, $gp
    line.init();
    line.line_num        = 7;
    line.addr            = 0x203;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 0;
    line.type[2]         = SYM_REG_GLOBAL;
    info.add(line);

    // line 8
    // ORI $t2, $zero, 256
    line.init();
    line.line_num        = 8;
    line.addr            = 0x204;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ORI";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 256;
    line.type[2]         = SYM_LITERAL;
    info.add(line);

    // label gets attached to first following non-empty line
    // line 10
    // top: SLTU $t3, $t0, $t1
    line.init();
    line.line_num        = 10;
    line.addr            = 0x205;
    line.opcode.instr    = LEX_SLTU;
    line.opcode.mnemonic = "SLTU";
    line.val[0]         = 3;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_TEMP;
    line.val[2]         = 1;
    line.type[2]        = SYM_REG_TEMP;
    line.is_label        = true;
    line.label           = "TOP";
    info.add(line);

    // line 11
    // BEQ $t3, $zero, done
    line.init();
    line.line_num        = 11;
    line.addr            = 0x206;
    line.opcode.instr    = LEX_BEQ;
    line.opcode.mnemonic = "BEQ";
    line.val[0]          = 3;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.is_symbol       = true;
    line.symbol          = "DONE";
    info.add(line);

    // line 12
    // SW $t2, 28($t8)
    line.init();
    line.line_num        = 12;
    line.addr            = 0x207;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "SW";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.offset[1]       = 28;
    info.add(line);

    // line 13
    // ADDI $t0, $t0, 4
    line.init();
    line.line_num        = 13;
    line.addr            = 0x208;
    line.opcode.instr    = LEX_ADDI;
    line.opcode.mnemonic = "SW";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.offset[1]       = 28;
    info.add(line);

    // line 14
    // J TOP
    line.init();
    line.line_num        = 14;
    line.addr            = 0x209;
    line.opcode.instr    = LEX_J;
    line.opcode.mnemonic = "J";
    line.is_symbol       = true;
    line.symbol          = "TOP";
    info.add(line);

    // line 15 (this is just a standalone symbol)
    // DONE
    line.init();
    line.line_num        = 15;
    line.addr            = 0x20A;
    line.is_label        = true;
    line.label           = "DONE";
    info.add(line);

    return info;
}

class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_mult_add_file = "asm/mult_add.asm";
        std::string test_for_loop_file = "asm/for_loop.asm";
};


TEST_F(TestLexer, test_lex_mult_add)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(false);
    test_lexer.loadFile(this->test_mult_add_file);
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


TEST_F(TestLexer, test_for_loop)
{

    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_for_loop_file);
    test_lexer.lex();

    // get the source info
    src_out = test_lexer.getSourceInfo();

    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;

    expected_src_out = get_for_loop_expected_source_info();
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
