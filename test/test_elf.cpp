/* TEST_ELF 
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
#include "ELF.hpp"

class TestELF : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};


TEST_F(TestELF, test_elf_file_header_get_set)
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

TEST_F(TestELF, test_elf_program_header_get_set)
{
    ELFProgramHeader test_header;

    // check that any default params are correct
    for(int i = 0; i < 0x20; ++i)
        ASSERT_EQ(0, test_header.header[i]);

    test_header.set_p_type(ELF_PT_DYNAMIC);
    ASSERT_EQ(ELF_PT_DYNAMIC, test_header.get_p_type());
    test_header.set_p_offset(0xDEADBEEF);
    ASSERT_EQ(0xDEADBEEF, test_header.get_p_offset());
    test_header.set_p_vaddr(0xDEADBEEF + 1);
    ASSERT_EQ(0xDEADBEEF + 1, test_header.get_p_vaddr());
    test_header.set_p_paddr(0xDEADBEEF + 2);
    ASSERT_EQ(0xDEADBEEF + 2, test_header.get_p_paddr());
    test_header.set_p_filesz(0xDEADBEEF + 3);
    ASSERT_EQ(0xDEADBEEF + 3, test_header.get_p_filesz());
    test_header.set_p_memsz(0xDEADBEEF + 4);
    ASSERT_EQ(0xDEADBEEF + 4, test_header.get_p_memsz());
    test_header.set_p_flags(0xDEADBEEF + 5);
    ASSERT_EQ(0xDEADBEEF + 5, test_header.get_p_flags());
    test_header.set_p_align(0xDEADBEEF + 6);
    ASSERT_EQ(0xDEADBEEF + 6, test_header.get_p_align());
}


TEST_F(TestELF, test_elf_section_header_get_set)
{
    ELFSectionHeader test_header;

    // check that any default params are correct
    for(int i = 0; i < 0x28; ++i)
        ASSERT_EQ(0, test_header.header[i]);

    test_header.set_sh_addr(0xDEADBEEF);
    ASSERT_EQ(0xDEADBEEF, test_header.get_sh_addr());
    test_header.set_sh_offset(0xDEADBEEF + 1);
    ASSERT_EQ(0xDEADBEEF + 1, test_header.get_sh_offset());
    test_header.set_sh_size(0xDEADBEEF + 2);
    ASSERT_EQ(0xDEADBEEF + 2, test_header.get_sh_size());
    test_header.set_sh_link(0xDEADBEEF + 3);
    ASSERT_EQ(0xDEADBEEF + 3, test_header.get_sh_link());
    test_header.set_sh_info(0xDEADBEEF + 4);
    ASSERT_EQ(0xDEADBEEF + 4, test_header.get_sh_info());
    test_header.set_sh_addralign(0xDEADBEEF + 5);
    ASSERT_EQ(0xDEADBEEF + 5, test_header.get_sh_addralign());
    test_header.set_sh_entsize(0xDEADBEEF + 6);
    ASSERT_EQ(0xDEADBEEF + 6, test_header.get_sh_entsize());
}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
