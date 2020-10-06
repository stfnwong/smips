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
#include "Segment.hpp"


class Object
{
    // TODO : segments..
    private:
        Segment code;
        Segment data;
        Segment zero;
        Segment bss;

    private:
        ELFFileHeader    file_header;
        ELFProgramHeader prog_header;
        ELFSectionHeader sect_header;

    private:
        std::vector <uint8_t> obj_data;

    // ensure that we don't have any copy construction
    private:
        Object(const Object&) = delete;

        void operator=(const Object&) = delete;

    public:
        Object();
        ~Object();

        // getters 
        unsigned int getNumSectionHeaders(void) const;
        unsigned int getNumProgramHeaders(void) const;
};


#endif /*__OBJECT_HPP*/
