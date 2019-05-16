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


class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    std::string test_file = "asm/example_prog.asm";
    unsigned int expected_num_instrs = 9;       // update this as more instructions are implemented

};

TEST_F(TestLexer, test_init)
{
    Lexer test_lexer;

}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
