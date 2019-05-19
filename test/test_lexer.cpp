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

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_file);
    test_lexer.lex();

    // get the source info
}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
