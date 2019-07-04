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
    uint8_t header[0x32];

    public:
        ELFFileHeader();
        void setProgStart(const uint32_t val);
        void setProgTableStart(const uint32_t val);
        void setSectionStart(const uint32_t val);
        void setNumSectionHeaders(const uint16_t val);
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
