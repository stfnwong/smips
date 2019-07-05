/*
 * OBJECT
 * Represents an object file for the SMIPS assembler
 *
 * Stefan Wong 2019
 */

#ifndef __OBJECT_HPP
#define __OBJECT_HPP

#include <cstdint>
#include <vector>

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
        void setType(const uint32_t val);
        void setOffset(const uint32_t val);
        void setFileSize(const uint32_t val);
        void setMemSize(const uint32_t val);
        void setVirtAddr(const uint32_t val);
        void setPhyAddr(const uint32_t val);
};


struct ELFSectionHeader
{
    uint8_t header[0x28];

    public:
        ELFSectionHeader();
        void setNameOffset(const uint32_t val);
        void setType(const uint32_t val);
        void setFlags(const uint32_t val);
        void setVirtAddr(const uint32_t val);
        void setOffset(const uint32_t val);
        void setSize(const uint32_t val);
};


typedef enum segment_t
{
    SEG_NONE,
    SEG_TEXT,
    SEG_CODE,
    SEG_DATA,
    SEG_ZERO,
    SEG_BSS,
} segment_t;



struct Segment
{
    uint32_t offset;
    segment_t type;

    public:
        Segment();
        Segment(const uint32_t offset, const segment_t t);
};



class Object
{
    private:
        ELFFileHeader header;
        std::vector<ELFSectionHeader> section_headers;
        std::vector<ELFProgramHeader> program_headers;

    // ensure that we don't have any copy construction
    private:
        Object(const Object&) = delete;
        void operator=(const Object&) = delete;

    public:
        Object();

        // getters 
        unsigned int getNumSectionHeaders(void) const;
        unsigned int getNumProgramHeaders(void) const;
};


#endif /*__OBJECT_HPP*/
