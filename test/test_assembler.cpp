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
#include "Assembler.hpp"


class TestAssembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

};

TEST_F(TestAssembler, test_init)
{
    Assembler test_asm;

}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

