/* TEST_OBJECT 
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>

// unit(s) under test
#include "Object.hpp"




// Test for overall object class
class TestObject : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_mult_add_file = "asm/mult_add.asm";
        std::string test_for_loop_file = "asm/for_loop.asm";
};


TEST_F(TestObject, test_object_init)
{
    Object test_object;

    ASSERT_EQ(0, test_object.getNumSectionHeaders());
    ASSERT_EQ(0, test_object.getNumProgramHeaders());
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
