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
#include "ELF.hpp"



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
