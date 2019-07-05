/* TEST_OBJECT 
 * Unit tests for ELF Object objects
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


class TestELFHeader : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};



TEST_F(TestELFHeader, test_header_get_set)
{
    ELFFileHeader test_header;

    ASSERT_EQ(0, test_header.get_e_type());
    // ensure that just after initialization the settable params 
    // are all zeroed
    ASSERT_EQ(0, test_header.get_e_entry());
    ASSERT_EQ(0, test_header.get_e_phoff());
    ASSERT_EQ(0, test_header.get_e_shoff());
    ASSERT_EQ(0, test_header.get_e_ehsize());
    ASSERT_EQ(0, test_header.get_e_phentsize());
    ASSERT_EQ(0, test_header.get_e_phnum());
    ASSERT_EQ(0, test_header.get_e_shentsize());
    ASSERT_EQ(0, test_header.get_e_shnum());
    ASSERT_EQ(0, test_header.get_e_shstrndx());

    // Now set each value in turn and check
    // 4 byte fields
    test_header.set_e_entry(0xDEADBEEF);
    ASSERT_EQ(0xDEADBEEF, test_header.get_e_entry());
    test_header.set_e_phoff(0xDEADBEEF + 1);
    ASSERT_EQ(0xDEADBEEF + 1, test_header.get_e_phoff());
    test_header.set_e_shoff(0xDEADBEEF + 2);
    ASSERT_EQ(0xDEADBEEF + 2, test_header.get_e_shoff());
    test_header.set_e_flags(0xDEADBEEF + 3);
    ASSERT_EQ(0xDEADBEEF + 3, test_header.get_e_flags());

    // 2 byte fields
    test_header.set_e_ehsize(0xBEEF);
    ASSERT_EQ(0xBEEF, test_header.get_e_ehsize());
    test_header.set_e_phentsize(0xBEEF + 1);
    ASSERT_EQ(0xBEEF + 1, test_header.get_e_phentsize());
    test_header.set_e_phnum(0xBEEF + 2);
    ASSERT_EQ(0xBEEF + 2, test_header.get_e_phnum());
    test_header.set_e_shentsize(0xBEEF + 3);
    ASSERT_EQ(0xBEEF + 3, test_header.get_e_shentsize());
    test_header.set_e_shnum(0xBEEF + 4);
    ASSERT_EQ(0xBEEF + 4, test_header.get_e_shnum());
    test_header.set_e_shstrndx(0xBEEF + 5);
    ASSERT_EQ(0xBEEF + 5, test_header.get_e_shstrndx());

}


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
