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
    line.init();
    line.line_num        = 3;
    line.addr            = 0x200;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.args[1]  = 4;
    line.types[1] = SYM_REG_GLOBAL;
    info.add(line);

    // line 2
    line.init();
    line.line_num        = 4;
    line.addr            = 0x201;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
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

TEST_F(TestLexer, test_init)
{
    Lexer test_lexer;
    SourceInfo src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_file);
    test_lexer.lex();

    // get the source info
    src_out = test_lexer.getSourceInfo();

    std::cout << src_out.toString() << std::endl;
}

TEST_F(TestLexer, test_lex_mult_add)
{


}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
