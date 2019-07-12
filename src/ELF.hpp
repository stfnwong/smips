/*
 * ELF
 * Stuff for dealing with ELF Files
 * Stefan Wong 2019
 */

#ifndef __ELF_HPP
#define __ELF_HPP

#include <cstdint>


// Program table type constants
#define ELF_PT_NULL    0x00000000
#define ELF_PT_LOAD    0x00000001
#define ELF_PT_DYNAMIC 0x00000002
#define ELF_PT_INTERP  0x00000003
#define ELF_PT_NOTE    0x00000004
#define ELF_PT_SHLIB   0x00000005
#define ELF_PT_PHDR    0x00000006
#define ELF_PT_LOOS    0x60000000
#define ELF_PT_HIOS    0x6FFFFFFF
#define ELF_PT_LOPROC  0x70000000
#define ELF_PT_HIPROC  0x7FFFFFFF

// Section header type constants
#define ELF_SHT_NULL         0x00
#define ELF_SHT_PROGBITS     0x01
#define ELF_SHT_SYMTAB       0x02
#define ELF_SHT_STRTAB       0x03
#define ELF_SHT_RELA         0x04
#define ELF_SHT_HASH         0x05
#define ELF_SHT_DYNAMIC      0x06
#define ELF_SHT_NOTE         0x07
#define ELF_SHT_NOBITS       0x08
#define ELF_SHT_REL          0x09
#define ELF_SHT_SHLIB        0x0A
#define ELF_SHT_DYNSYM       0x0B
#define ELF_SHT_INIT_ARRAY   0x0E
#define ELF_SHT_FINI_ARRAY   0x0F
#define ELF_SHT_GROUP        0x10
#define ELF_SHT_SYMTAB_INDEX 0x11
#define ELF_SHT_NUM          0x12
#define ELF_SHT_LOOS         0x13

// Section header flag constants
#define ELF_SHF_WRITE            0x01
#define ELF_SHF_ALLOC            0x02
#define ELF_SHF_EXECINSTR        0x04
#define ELF_SHF_MERGE            0x10
#define ELF_SHF_STRINGS          0x20
#define ELF_SHF_INFO_LINK        0x40
#define ELF_SHF_LINK_ORDER       0x80
#define ELF_SHF_OS_NONCONFORMING 0x100
#define ELF_SHF_GROUP            0x200
#define ELF_SHF_TLS              0x400
#define ELF_SHF_MASKOS           0x0FF00000
#define ELF_SHF_MASKPROC         0xF0000000
#define ELF_SHF_ORDERED          0x40000000
#define ELF_SHF_EXCLUDE          0x80000000

// ELF header stuff
// This might go in another file later

struct ELFFileHeader
{
    uint8_t header[0x34];       

    public:
        ELFFileHeader();
        // getters
        uint8_t  get_e_type(void) const;
        uint32_t get_e_version(void) const;
        uint32_t get_e_entry(void) const;
        uint32_t get_e_phoff(void) const;
        uint32_t get_e_shoff(void) const;
        uint32_t get_e_flags(void) const;

        uint16_t get_e_ehsize(void) const;
        uint16_t get_e_phentsize(void) const;
        uint16_t get_e_phnum(void) const; 
        uint16_t get_e_shentsize(void) const;
        uint16_t get_e_shnum(void) const;
        uint16_t get_e_shstrndx(void) const;

        // setters 
        void set_e_entry(const uint32_t val);
        void set_e_phoff(const uint32_t val);
        void set_e_shoff(const uint32_t val);
        void set_e_flags(const uint32_t val);
        void set_e_ehsize(const uint16_t val);
        void set_e_phentsize(const uint16_t val);
        void set_e_phnum(const uint16_t val);
        void set_e_shentsize(const uint16_t val);
        void set_e_shnum(const uint16_t val);
        void set_e_shstrndx(const uint16_t val);
};


struct ELFProgramHeader
{
    uint8_t header[0x20];

    public:
        ELFProgramHeader();
        // getters 
        uint32_t get_p_type(void) const;
        uint32_t get_p_offset(void) const;
        uint32_t get_p_vaddr(void) const;
        uint32_t get_p_paddr(void) const;
        uint32_t get_p_filesz(void) const;
        uint32_t get_p_memsz(void) const;
        uint32_t get_p_flags(void) const;
        uint32_t get_p_align(void) const;

        // setters 
        void set_p_type(const uint32_t val);
        void set_p_offset(const uint32_t val);
        void set_p_vaddr(const uint32_t val);
        void set_p_paddr(const uint32_t val);
        void set_p_filesz(const uint32_t val);
        void set_p_memsz(const uint32_t val);
        void set_p_flags(const uint32_t val);
        void set_p_align(const uint32_t val);
};


struct ELFSectionHeader
{
    uint8_t header[0x28];

    public:
        ELFSectionHeader();

        // getters
        uint32_t get_sh_name(void) const;
        uint32_t get_sh_type(void) const;
        uint32_t get_sh_flags(void) const;
        uint32_t get_sh_addr(void) const;
        uint32_t get_sh_offset(void) const;
        uint32_t get_sh_size(void) const;
        uint32_t get_sh_link(void) const;
        uint32_t get_sh_info(void) const;
        uint32_t get_sh_addralign(void) const;
        uint32_t get_sh_entsize(void) const;

        // setters
        void set_sh_name(const uint32_t val);
        void set_sh_type(const uint32_t val);
        void set_sh_flags(const uint32_t val);
        void set_sh_addr(const uint32_t val);
        void set_sh_offset(const uint32_t val);
        void set_sh_size(const uint32_t val);
        void set_sh_link(const uint32_t val);
        void set_sh_info(const uint32_t val);
        void set_sh_addralign(const uint32_t val);
        void set_sh_entsize(const uint32_t val);
};


#endif /*__ELF_HPP*/
