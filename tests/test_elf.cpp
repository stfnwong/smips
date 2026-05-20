/* TEST_ELF 
 * Unit tests for ELF Object objects
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
// unit(s) under test
#include "ELF.hpp"


TEST_CASE("test_elf_file_header_get_set", "[classic]")
{
    ELFFileHeader test_header;

    REQUIRE(0 == test_header.get_e_type());
    // ensure that just after initialization the settable params 
    // are all zeroed
    REQUIRE(0 == test_header.get_e_entry());
    REQUIRE(0 == test_header.get_e_phoff());
    REQUIRE(0 == test_header.get_e_shoff());
    REQUIRE(0 == test_header.get_e_ehsize());
    REQUIRE(0 == test_header.get_e_phentsize());
    REQUIRE(0 == test_header.get_e_phnum());
    REQUIRE(0 == test_header.get_e_shentsize());
    REQUIRE(0 == test_header.get_e_shnum());
    REQUIRE(0 == test_header.get_e_shstrndx());

    // Now set each value in turn and check
    // 4 byte fields
    test_header.set_e_entry(0xDEADBEEF);
    REQUIRE(0xDEADBEEF == test_header.get_e_entry());
    test_header.set_e_phoff(0xDEADBEEF + 1);
    REQUIRE(0xDEADBEEF + 1 == test_header.get_e_phoff());
    test_header.set_e_shoff(0xDEADBEEF + 2);
    REQUIRE(0xDEADBEEF + 2 == test_header.get_e_shoff());
    test_header.set_e_flags(0xDEADBEEF + 3);
    REQUIRE(0xDEADBEEF + 3 == test_header.get_e_flags());

    // 2 byte fields
    test_header.set_e_ehsize(0xBEEF);
    REQUIRE(0xBEEF == test_header.get_e_ehsize());
    test_header.set_e_phentsize(0xBEEF + 1);
    REQUIRE(0xBEEF + 1 == test_header.get_e_phentsize());
    test_header.set_e_phnum(0xBEEF + 2);
    REQUIRE(0xBEEF + 2 == test_header.get_e_phnum());
    test_header.set_e_shentsize(0xBEEF + 3);
    REQUIRE(0xBEEF + 3 == test_header.get_e_shentsize());
    test_header.set_e_shnum(0xBEEF + 4);
    REQUIRE(0xBEEF + 4 == test_header.get_e_shnum());
    test_header.set_e_shstrndx(0xBEEF + 5);
    REQUIRE(0xBEEF + 5 == test_header.get_e_shstrndx());
}

TEST_CASE("test_elf_program_header_get_set", "[classic]")
{
    ELFProgramHeader test_header;

    // check that any default params are correct
    for(int i = 0; i < 0x20; ++i)
        REQUIRE(0 == test_header.header[i]);

    test_header.set_p_type(ELF_PT_DYNAMIC);
    REQUIRE(ELF_PT_DYNAMIC == test_header.get_p_type());
    test_header.set_p_offset(0xDEADBEEF);
    REQUIRE(0xDEADBEEF == test_header.get_p_offset());
    test_header.set_p_vaddr(0xDEADBEEF + 1);
    REQUIRE(0xDEADBEEF + 1 == test_header.get_p_vaddr());
    test_header.set_p_paddr(0xDEADBEEF + 2);
    REQUIRE(0xDEADBEEF + 2 == test_header.get_p_paddr());
    test_header.set_p_filesz(0xDEADBEEF + 3);
    REQUIRE(0xDEADBEEF + 3 == test_header.get_p_filesz());
    test_header.set_p_memsz(0xDEADBEEF + 4);
    REQUIRE(0xDEADBEEF + 4 == test_header.get_p_memsz());
    test_header.set_p_flags(0xDEADBEEF + 5);
    REQUIRE(0xDEADBEEF + 5 == test_header.get_p_flags());
    test_header.set_p_align(0xDEADBEEF + 6);
    REQUIRE(0xDEADBEEF + 6 == test_header.get_p_align());
}


TEST_CASE("test_elf_section_header_get_set", "[classic]")
{
    ELFSectionHeader test_header;

    // check that any default params are correct
    for(int i = 0; i < 0x28; ++i)
        REQUIRE(0 == test_header.header[i]);

    test_header.set_sh_addr(0xDEADBEEF);
    REQUIRE(0xDEADBEEF == test_header.get_sh_addr());
    test_header.set_sh_offset(0xDEADBEEF + 1);
    REQUIRE(0xDEADBEEF + 1 == test_header.get_sh_offset());
    test_header.set_sh_size(0xDEADBEEF + 2);
    REQUIRE(0xDEADBEEF + 2 == test_header.get_sh_size());
    test_header.set_sh_link(0xDEADBEEF + 3);
    REQUIRE(0xDEADBEEF + 3 == test_header.get_sh_link());
    test_header.set_sh_info(0xDEADBEEF + 4);
    REQUIRE(0xDEADBEEF + 4 == test_header.get_sh_info());
    test_header.set_sh_addralign(0xDEADBEEF + 5);
    REQUIRE(0xDEADBEEF + 5 == test_header.get_sh_addralign());
    test_header.set_sh_entsize(0xDEADBEEF + 6);
    REQUIRE(0xDEADBEEF + 6 == test_header.get_sh_entsize());
}
